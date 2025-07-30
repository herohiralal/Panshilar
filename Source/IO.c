#include "IO.h"
#include "Allocators.h"
#include "Strings.h"

// internal allocator stuff ========================================================

/**
 * The way all of this internal allocator stuff works is that we have some thread-local
 * variables that store some state for the allocators. Initially, it's set to uninitialised.
 * In this state, the system will attempt to create a new stack allocator using the default heap allocator.
 * Upon failure, it will mark itself as cleared, and fall back to the default heap allocator.
 * Any subsequent calls will return the default heap allocator.
 * If the stack allocator instead is created, it'll mark itself as ready and return the stack allocator.
 * When the thread exits, the cleanup function will be called, which will destroy the stack allocator
 * and mark the state as cleared.
 */

ENUM_START(PNSLR_PathsInternalAllocatorState, u8)
    #define PNSLR_PathsInternalAllocatorState_Uninitialised ((PNSLR_PathsInternalAllocatorState) 0)
    #define PNSLR_PathsInternalAllocatorState_Ready         ((PNSLR_PathsInternalAllocatorState) 1)
    #define PNSLR_PathsInternalAllocatorState_Cleared       ((PNSLR_PathsInternalAllocatorState) 2)
ENUM_END

static thread_local PNSLR_PathsInternalAllocatorState G_PathsInternalAllocatorState = PNSLR_PathsInternalAllocatorState_Uninitialised;
static thread_local PNSLR_Allocator                   G_PathsInternalAllocator      = {0};

static void ReleasePathsInternalAllocator(void)
{
    if (G_PathsInternalAllocatorState == PNSLR_PathsInternalAllocatorState_Ready)
    {
        PNSLR_DestroyAllocator_Stack(G_PathsInternalAllocator, CURRENT_LOC(), nil);
        G_PathsInternalAllocator = (PNSLR_Allocator) {0};
        G_PathsInternalAllocatorState = PNSLR_PathsInternalAllocatorState_Cleared;
    }
}

static PNSLR_Allocator AcquirePathsInternalAllocator(void)
{
    switch (G_PathsInternalAllocatorState)
    {
        case PNSLR_PathsInternalAllocatorState_Cleared:
            return PNSLR_DEFAULT_HEAP_ALLOCATOR; // if cleared, fall back to default heap allocator

        case PNSLR_PathsInternalAllocatorState_Ready:
            return G_PathsInternalAllocator;

        case PNSLR_PathsInternalAllocatorState_Uninitialised:
            ; // fall through to initialisation
            PNSLR_AllocatorError err;
            G_PathsInternalAllocator   = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), &err);
            if (err != PNSLR_AllocatorError_None)
            {
                G_PathsInternalAllocatorState = PNSLR_PathsInternalAllocatorState_Cleared;
                return PNSLR_DEFAULT_HEAP_ALLOCATOR; // if initialisation fails, fall back to default heap allocator
            }

            G_PathsInternalAllocatorState = PNSLR_PathsInternalAllocatorState_Ready;
            PNSLR_Intrinsic_RegisterThreadCleanup(ReleasePathsInternalAllocator);
            return G_PathsInternalAllocator;

        default:
            // should never reach here, but just in case
            return PNSLR_DEFAULT_HEAP_ALLOCATOR;
    }
}

// actual function implementations =================================================

// TODO: use UTF-16 strings on Windows for better compatibility

void PNSLR_IterateDirectory(utf8str path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 3), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    #if PNSLR_WINDOWS
    {
        tempBuffer2.data[path.count] = '/'; // use backslash for Windows paths
        tempBuffer2.data[path.count + 1] = '*'; // add wildcard for file matching
        tempBuffer2.data[path.count + 2] = '\0'; // null-terminate
        tempBuffer2.count = path.count + 2;
    }
    #elif PNSLR_UNIX
    {
        tempBuffer2.data[path.count] = '\0'; // null-terminate
        tempBuffer2.count = path.count;
    }
    #endif

    #if PNSLR_WINDOWS

        WIN32_FIND_DATAA findData;
        HANDLE findHandle = FindFirstFileA(tempBuffer2.data, &findData);

        if (findHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                char* nextFileName = findData.cFileName;

    #elif PNSLR_UNIX

        DIR *dir = opendir(tempBuffer2.data);
        if (dir != NULL)
        {
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL)
            {
                char* nextFileName = entry->d_name;

    #endif
                i32 fileNameLen = PNSLR_GetStringLength(nextFileName);
                if (fileNameLen == 0                                                    ) { continue; } // skip empty names
                if (fileNameLen == 1 && nextFileName[0] == '.'                          ) { continue; } // skip current directory
                if (fileNameLen == 2 && nextFileName[0] == '.' && nextFileName[1] == '.') { continue; } // skip parent directory

                utf8str foundPath = PNSLR_MakeSlice(utf8ch, (path.count + fileNameLen + 1 + 1), false, internalAllocator, nil);
                PNSLR_Intrinsic_MemCopy(foundPath.data, path.data, (i32) path.count);
                foundPath.data[path.count] = '/'; // add path separator
                PNSLR_Intrinsic_MemCopy(foundPath.data + path.count + 1, nextFileName, fileNameLen);
                foundPath.data[path.count + 1 + fileNameLen] = '\0'; // null-terminate the string, just in case
                foundPath.count = path.count + fileNameLen + 1; // update count

                #if PNSLR_WINDOWS
                {
                    for (i32 i = 0; i < foundPath.count; ++i)
                    {
                        if (foundPath.data[i] == '\\') { foundPath.data[i] = '/'; } // normalize path separators
                    }
                }
                #endif

                b8 isDirectory = false;

                #if PNSLR_WINDOWS
                {
                    isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                }
                #elif PNSLR_UNIX
                {
                    struct stat statBuf;
                    if (stat((char*) foundPath.data, &statBuf) == 0)
                    {
                        isDirectory = S_ISDIR(statBuf.st_mode);
                    }
                }
                #endif

                b8 iterateFurther = visitorFunc(visitorPayload, foundPath, isDirectory);

                // handle recursion
                if (iterateFurther && recursive && isDirectory) { PNSLR_IterateDirectory(foundPath, recursive, visitorPayload, visitorFunc); }

                PNSLR_FreeSlice(foundPath, internalAllocator, nil);

                if (!iterateFurther) { break; } // stop iteration if the visitor function returns false

    #if PNSLR_WINDOWS

            } while (FindNextFileA(findHandle, &findData));

            FindClose(findHandle);
        }

    #elif PNSLR_UNIX

            }

            closedir(dir);
        }

    #endif

    PNSLR_FreeSlice(tempBuffer2, internalAllocator, nil);
}

b8 PNSLR_PathExists(utf8str path, PNSLR_PathCheckType type)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 1), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    tempBuffer2.data[path.count] = '\0';

    b8 canBeFile      = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_File);
    b8 canBeDirectory = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_Directory);

    b8 result         = false;

    #if PNSLR_WINDOWS

        DWORD fileAttributes = GetFileAttributesA(tempBuffer2.data); // TODO: switch to UTF-16 strings for better compatibility
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
        {
            result = false; // File does not exist
        }
        else if (canBeFile && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            result = true; // It's a file
        }
        else if (canBeDirectory && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            result = true; // It's a directory
        }

    #elif PNSLR_UNIX

        struct stat fileStat;
        if (stat(tempBuffer2.data, &fileStat) != 0)
        {
            result = false; // File does not exist
        }
        else if (canBeFile && S_ISREG(fileStat.st_mode))
        {
            result = true; // It's a file
        }
        else if (canBeDirectory && S_ISDIR(fileStat.st_mode))
        {
            result = true; // It's a directory
        }

    #endif

    PNSLR_FreeSlice(tempBuffer2, internalAllocator, nil);
    return result;
}

i64 PNSLR_GetFileTimestamp(utf8str path)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 1), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    tempBuffer2.data[path.count] = '\0';

    i64 timestamp = 0;
    #if PNSLR_WINDOWS

        HANDLE fileHandle = CreateFileA(tempBuffer2.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            FILETIME fileTime;
            if (GetFileTime(fileHandle, NULL, NULL, &fileTime))
            {
                // Convert FILETIME to nanoseconds since epoch
                ULARGE_INTEGER ull;
                ull.LowPart = fileTime.dwLowDateTime;
                ull.HighPart = fileTime.dwHighDateTime;
                timestamp = (i64)(ull.QuadPart - 116444736000000000ULL) * 100; // Convert to nanoseconds
            }
            CloseHandle(fileHandle);
        }

    #elif PNSLR_UNIX

        struct stat fileStat;
        if (stat(tempBuffer2.data, &fileStat) == 0)
        {
            // Convert to nanoseconds since epoch
            timestamp = (i64)fileStat.st_mtime * 1000000000LL; // Convert seconds to nanoseconds
        }

    #endif

    PNSLR_FreeSlice(tempBuffer2, internalAllocator, nil);
    return timestamp;
}
