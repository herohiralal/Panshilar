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

// some bs internal stuff ==========================================================

#if PNSLR_WINDOWS

    static i32 GetVolumeLengthFromPathWindowsOnly(utf8str path)
    {
        #define IS_SLASH(c) ((c) == '/' || (c) == '\\')

        if (path.count < 2)
        {
            return 0; // no volume
        }

        if (path.data[1] == ':' && ((path.data[0] >= 'A' && path.data[0] <= 'Z') || (path.data[0] >= 'a' && path.data[0] <= 'z')))
        {
            return 2; // volume is present
        }

        if (path.count >= 5 && IS_SLASH(path.data[0]) && IS_SLASH(path.data[1]) && !IS_SLASH(path.data[2]) && (path.data[2] != '.'))
        {
            for (i32 i = 3; i < path.count; ++i)
            {
                if (IS_SLASH(path.data[i]))
                {
                    i += 1;
                    if (i < path.count && !IS_SLASH(path.data[i]))
                    {
                        if (path.data[i] == '.')
                        {
                            break;
                        }
                    }

                    for (; i < path.count; ++i)
                    {
                        if (IS_SLASH(path.data[i]))
                        {
                            break;
                        }
                    }

                    return i;
                }
            }
        }

        #undef IS_SLASH

        return 0;
    }

#endif // PNSLR_WINDOWS

/**
 * When appending, compares stuff to original str.
 * Creates a new buffer only if required.
 */
typedef struct LazyPathBuffer {
    PNSLR_Allocator    allocator;
    utf8str            originalString;
    ArraySlice(utf8ch) buffer;
    i32                writeIdx;
    utf8str            volAndPath;
    i32                volumeLength;
} LazyPathBuffer;

static u8 GetIndexFromLazyPathBuffer(LazyPathBuffer* buff, i32 idx)
{
    if (buff->buffer.data != nil)
    {
        return buff->buffer.data[idx];
    }

    return buff->originalString.data[idx];
}

static b8 AppendToLazyPathBuffer(LazyPathBuffer* buff, utf8ch c)
{
    if (buff->buffer.data == nil)
    {
        if (buff->writeIdx < buff->originalString.count && buff->originalString.data[buff->writeIdx] == c)
        {
            buff->writeIdx += 1;
            return true; // no need to append, just increment the index
        }

        // need to create a new buffer
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        buff->buffer = PNSLR_MakeSlice(utf8ch, (buff->originalString.count + 1), false, buff->allocator, &err);
        if (err != PNSLR_AllocatorError_None) { return false; } // allocation failed
        PNSLR_Intrinsic_MemCopy(buff->buffer.data, buff->originalString.data, buff->writeIdx);
    }

    buff->buffer.data[buff->writeIdx] = c;
    buff->writeIdx++;
    return true;
}

static utf8str StringFromLazyPathBuffer(LazyPathBuffer* buff)
{
    if (buff->buffer.data == nil)
    {
        return PNSLR_CloneString((utf8str) {.data = buff->volAndPath.data, .count = buff->volumeLength + buff->writeIdx}, buff->allocator);
    }

    utf8str x = (utf8str) {.data = buff->volAndPath.data, .count = buff->volumeLength};
    utf8str y = (utf8str) {.data = buff->buffer.data, .count = buff->writeIdx};
    utf8str z = PNSLR_MakeString((x.count + y.count), false, buff-> allocator, nil);
    if (z.data != nil)
    {
        PNSLR_Intrinsic_MemCopy(z.data,           x.data, (i32) x.count);
        PNSLR_Intrinsic_MemCopy(z.data + x.count, y.data, (i32) y.count);
    }

    return z;
}

static void DisposeLazyPathBuffer(LazyPathBuffer* buff)
{
    PNSLR_FreeSlice(buff->buffer, buff->allocator, nil);
    *buff = (LazyPathBuffer) {0}; // reset the buffer
}

// actual function implementations =================================================

// TODO: use UTF-16 strings on Windows for better compatibility

PNSLR_NormalisedPath PNSLR_NormalisePath(utf8str path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator)
{
    if (!path.data || !path.count)
    {
        path = PNSLR_STRING_LITERAL(".");
    }

    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    #if PNSLR_WINDOWS
    {
        ArraySlice(utf16ch) p = PNSLR_UTF16FromUTF8WindowsOnly(path, internalAllocator);
        i32 n = GetFullPathNameW((LPCWSTR) p.data, 0, nil, nil);
        ArraySlice(utf16ch) buf = EMPTY_ARRAY_SLICE(utf16ch);
        utf8str tempFullPath = (utf8str) {0};
        if (n > 0)
        {
            buf = PNSLR_MakeSlice(utf16ch, n, false, internalAllocator, nil);
            n = GetFullPathNameW((LPCWSTR) p.data, (DWORD) n, (LPWSTR) buf.data, nil);
            if (n > 0)
            {
                tempFullPath = PNSLR_UTF8FromUTF16WindowsOnly(buf, internalAllocator);
            }
        }

        LazyPathBuffer outputBuffer = (LazyPathBuffer) {0};
        utf8str resultPath = (utf8str) {0};

        // clean the path
        #define IS_SEPARATOR(c) ((c) == '/' || (c) == '\\')
        if (tempFullPath.data && tempFullPath.count)
        {
            path = tempFullPath;
            utf8str originalPath = tempFullPath;
            i32 volumeLength = GetVolumeLengthFromPathWindowsOnly(path);
            path = (utf8str) {.data = path.data + volumeLength, .count = path.count - volumeLength};

            if (path.count == 0)
            {
                // path is just a volume, needs a trailing slash and then return
                resultPath = PNSLR_MakeString(volumeLength + 1, false, allocator, nil);
                if (resultPath.data)
                {
                    PNSLR_Intrinsic_MemCopy(resultPath.data, originalPath.data, volumeLength);
                    resultPath.data[volumeLength] = '/'; // add trailing slash
                }

                goto exitFunction;
            }

            b8 isRooted = IS_SEPARATOR(path.data[0]);
            n = (i32) path.count;
            outputBuffer = (LazyPathBuffer)
            {
                .originalString = path,
                .volAndPath     = originalPath,
                .volumeLength   = volumeLength,
                .allocator      = allocator,
                .buffer         = EMPTY_ARRAY_SLICE(utf8ch),
                .writeIdx       = 0,
            };

            i32 r = 0, dotDot = 0;
            if (isRooted)
            {
                if (!AppendToLazyPathBuffer(&outputBuffer, '/')) { goto exitFunction; }
                r = 1;
                dotDot = 1;
            }

            while (r < n)
            {
                if (IS_SEPARATOR(path.data[r]))
                {
                    r += 1; // skip the separator
                }
                else if (path.data[r] == '.' && (((r + 1) == n) || IS_SEPARATOR(path.data[r + 1])))
                {
                    r += 1;
                }
                else if (path.data[r] == '.' && path.data[r + 1] == '.' && (((r + 2) == n) || IS_SEPARATOR(path.data[r + 2])))
                {
                    r += 2; // skip the ".."
                    if (outputBuffer.writeIdx > dotDot)
                    {
                        outputBuffer.writeIdx -= 1;
                        while (outputBuffer.writeIdx > dotDot) {
                            u8 idx = GetIndexFromLazyPathBuffer(&outputBuffer, outputBuffer.writeIdx);
                            if (IS_SEPARATOR(idx)) { break; }
                            outputBuffer.writeIdx -= 1; // go back until we find a separator
                        }
                    }
                    else if (!isRooted)
                    {
                        if (outputBuffer.writeIdx > 0)
                        {
                            if (!AppendToLazyPathBuffer(&outputBuffer, '/')) { goto exitFunction; }
                        }

                        if (!AppendToLazyPathBuffer(&outputBuffer, '.') || !AppendToLazyPathBuffer(&outputBuffer, '.')) { goto exitFunction; }

                        dotDot = outputBuffer.writeIdx; // remember the position of the ".."
                    }
                }
                else
                {
                    if (isRooted && outputBuffer.writeIdx != 1 || !isRooted && outputBuffer.writeIdx != 0)
                    {
                        if (!AppendToLazyPathBuffer(&outputBuffer, '/')) { goto exitFunction; }
                    }

                    for (; r < n && !IS_SEPARATOR(path.data[r]); ++r)
                    {
                        if (!AppendToLazyPathBuffer(&outputBuffer, path.data[r])) { goto exitFunction; }
                    }
                }
            }

            if (outputBuffer.writeIdx == 0)
            {
                if (!AppendToLazyPathBuffer(&outputBuffer, '.')) { goto exitFunction; }
            }

            if (type == PNSLR_PathNormalisationType_Directory)
            {
                u8 lastChar = outputBuffer.writeIdx > 0 ? GetIndexFromLazyPathBuffer(&outputBuffer, outputBuffer.writeIdx - 1) : 0;
                if (!IS_SEPARATOR(lastChar))
                {
                    if (!AppendToLazyPathBuffer(&outputBuffer, '/')) { goto exitFunction; }
                }
            }

            utf8str tempResultPath = StringFromLazyPathBuffer(&outputBuffer);
            for (i32 i = 0; i < tempResultPath.count; ++i)
            {
                if (tempResultPath.data[i] == '\\') { tempResultPath.data[i] = '/'; } // normalize path separators
            }

            resultPath = tempResultPath;
        }
        #undef IS_SEPARATOR

        exitFunction:
        ;
        DisposeLazyPathBuffer(&outputBuffer);
        PNSLR_FreeString(tempFullPath, internalAllocator, nil);

        PNSLR_FreeSlice(buf, internalAllocator, nil);
        PNSLR_FreeSlice(p,   internalAllocator, nil);

        return (PNSLR_NormalisedPath) { .path = resultPath };
    }
    #elif PNSLR_UNIX
    {
        cstring relCstr = PNSLR_CStringFromString(path, internalAllocator);
        cstring pathPtr = realpath(relCstr, nil);
        PNSLR_FreeCString(relCstr, internalAllocator, nil);

        if (pathPtr == nil)
        {
            PNSLR_NormalisedPath output = (PNSLR_NormalisedPath) {0};
            return output;
        }

        utf8str tempAlias = PNSLR_StringFromCString(pathPtr);
        utf8str output = PNSLR_CloneString(tempAlias, allocator);
        PNSLR_Intrinsic_Free(pathPtr);

        return (PNSLR_NormalisedPath) { .path = output };
    }
    #endif
}

// TODO: phase out manual string->cstring conversions

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
                i32 fileNameLen = PNSLR_GetCStringLength(nextFileName);
                if (fileNameLen == 0                                                    ) { continue; } // skip empty names
                if (fileNameLen == 1 && nextFileName[0] == '.'                          ) { continue; } // skip current directory
                if (fileNameLen == 2 && nextFileName[0] == '.' && nextFileName[1] == '.') { continue; } // skip parent directory

                utf8str foundPath = PNSLR_MakeString((path.count + fileNameLen + 1 + 1), false, internalAllocator, nil);
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

b8 PNSLR_PathExists(utf8str path, PNSLR_PathExistsCheckType type)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 1), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    tempBuffer2.data[path.count] = '\0';

    b8 canBeFile      = (type == PNSLR_PathExistsCheckType_Either || type == PNSLR_PathExistsCheckType_File);
    b8 canBeDirectory = (type == PNSLR_PathExistsCheckType_Either || type == PNSLR_PathExistsCheckType_Directory);

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
