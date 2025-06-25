#include "IO.h"
#include "Allocators.h"

static PNSLR_Allocator GetPathsInternalAllocator(void)
{
    // TODO: the allocations made by stack allocator will leak when the thread exits
    //       we should figure out a way to let the stack allocator know when the thread exits
    //       so it can free the memory

    static thread_local b8              initialised       = false;
    static thread_local PNSLR_Allocator internalAllocator = {.data = nil, .procedure = nil};

    if (!initialised)
    {
        PNSLR_AllocatorError error = PNSLR_AllocatorError_None;
        internalAllocator          = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), &error);
        initialised                = true;

        if (error != PNSLR_AllocatorError_None)
        {
            // as a fallback, just using the default heap allocator
            internalAllocator = PNSLR_DEFAULT_HEAP_ALLOCATOR;
        }
        else
        {
            // TODO: maybe this is where some kind of thread-cleanup code can go?
        }
    }

    return internalAllocator;
}

b8 PNSLR_PathExists(utf8str path, PNSLR_PathCheckType type)
{
    PNSLR_Allocator internalAllocator = GetPathsInternalAllocator();

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
    PNSLR_Allocator internalAllocator = GetPathsInternalAllocator();

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
