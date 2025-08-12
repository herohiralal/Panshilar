#include "IO.h"
#include "Allocators.h"
#include "Strings.h"

// internal allocator stuff ========================================================

typedef struct alignas(16) PNSLR_PathsInternalAllocatorBuffer
{
    u8 data[512 * 1024]; // 0.5 MiB
} PNSLR_PathsInternalAllocatorBuffer;

typedef struct PNSLR_PathsInternalAllocatorInfo
{
    b8                                 initialised;
    PNSLR_ArenaAllocatorPayload        arenaPayload;
    PNSLR_ArenaAllocatorBlock          arenaBlock;
    PNSLR_PathsInternalAllocatorBuffer buffer;
} PNSLR_PathsInternalAllocatorInfo;

static thread_local PNSLR_PathsInternalAllocatorInfo G_PathsInternalAllocatorInfo = {0};

static PNSLR_Allocator AcquirePathsInternalAllocator(void)
{
    if (!G_PathsInternalAllocatorInfo.initialised)
    {
        G_PathsInternalAllocatorInfo.initialised = true;

        G_PathsInternalAllocatorInfo.buffer = (PNSLR_PathsInternalAllocatorBuffer) {0};

        G_PathsInternalAllocatorInfo.arenaBlock = (PNSLR_ArenaAllocatorBlock)
        {
            .previous  = nil,
            .allocator = PNSLR_NIL_ALLOCATOR,
            .memory    = (rawptr) &G_PathsInternalAllocatorInfo.buffer.data,
            .capacity  = sizeof(G_PathsInternalAllocatorInfo.buffer.data),
            .used      = 0,
        };

        G_PathsInternalAllocatorInfo.arenaPayload = (PNSLR_ArenaAllocatorPayload)
        {
            .backingAllocator    = PNSLR_NIL_ALLOCATOR,
            .currentBlock        = &G_PathsInternalAllocatorInfo.arenaBlock,
            .totalUsed           = 0,
            .totalCapacity       = sizeof(G_PathsInternalAllocatorInfo.buffer.data),
            .minimumBlockSize    = 0,
            .numSnapshots        = 0,
        };
    }

    return (PNSLR_Allocator) {.data = &G_PathsInternalAllocatorInfo.arenaPayload, .procedure = PNSLR_AllocatorFn_Arena};
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

    /**
     * When appending, compares stuff to original str.
     * Creates a new buffer only if required.
     */
    typedef struct LazyPathBuffer
    {
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

#endif // PNSLR_WINDOWS

// actual function implementations =================================================

#define PATHS_INTERNAL_ALLOCATOR_INIT \
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator(); \
    PNSLR_ArenaAllocatorSnapshot internalAllocSnapshot = PNSLR_CaptureArenaAllocatorSnapshot(internalAllocator);

#define PATHS_INTERNAL_ALLOCATOR_RESET \
    PNSLR_ArenaSnapshotError snapshotError = PNSLR_RestoreArenaAllocatorSnapshot(&internalAllocSnapshot, CURRENT_LOC()); \
    if (PNSLR_ArenaSnapshotError_None != snapshotError) { FORCE_DBG_TRAP; }

// TODO: use UTF-16 strings on Windows for better compatibility

PNSLR_NormalisedPath PNSLR_NormalisePath(utf8str path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator)
{
    if (!path.data || !path.count)
    {
        path = PNSLR_STRING_LITERAL(".");
    }

    PATHS_INTERNAL_ALLOCATOR_INIT

    utf8str resultPath = (utf8str) {0};

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
    }
    #elif PNSLR_UNIX
    {
        cstring relCstr = PNSLR_CStringFromString(path, internalAllocator);
        cstring pathPtr = realpath(relCstr, nil);

        if (pathPtr != nil)
        {
           utf8str tempAlias = PNSLR_StringFromCString(pathPtr);
           i32 tgtCount = (i32) tempAlias.count + ((type == PNSLR_PathNormalisationType_Directory) ? 1 : 0);
           utf8str output = PNSLR_MakeString(tgtCount, false, allocator, nil);

           if (output.data != nil)
           {
               PNSLR_Intrinsic_MemCopy(output.data, tempAlias.data, (i32) tempAlias.count);
               if (type == PNSLR_PathNormalisationType_Directory) { output.data[tempAlias.count] = '/'; }
               output.count = tgtCount;
           }

           PNSLR_Intrinsic_Free(pathPtr);

           resultPath = output;
        }
    }
    #endif

    PATHS_INTERNAL_ALLOCATOR_RESET

    return (PNSLR_NormalisedPath) { .path = resultPath };
}

// TODO: phase out manual string->cstring conversions

void PNSLR_IterateDirectory(utf8str path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc)
{
    PATHS_INTERNAL_ALLOCATOR_INIT

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 3), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    #if PNSLR_WINDOWS
    {
        u32 iterator = (u32) path.count - 1;
        if (tempBuffer2.data[iterator] == '/' || tempBuffer2.data[iterator] == '\\') // has trailing slash
        {
            iterator = (u32) path.count;
        }
        else
        {
            tempBuffer2.data[path.count] = '/'; // add slash at the end
            iterator = (u32) path.count + 1;
        }

        tempBuffer2.data[iterator    ] = '*';  // add wildcard for file matching
        tempBuffer2.data[iterator + 1] = '\0'; // null-terminate
        tempBuffer2.count = iterator + 1;
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
                cstring nextFileName = findData.cFileName;

    #elif PNSLR_UNIX

        DIR* dir = opendir(tempBuffer2.data);
        if (dir != NULL)
        {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL)
            {
                cstring nextFileName = entry->d_name;

    #endif
                i32 fileNameLen = PNSLR_GetCStringLength(nextFileName);
                if (fileNameLen == 0                                                    ) { continue; } // skip empty names
                if (fileNameLen == 1 && nextFileName[0] == '.'                          ) { continue; } // skip current directory
                if (fileNameLen == 2 && nextFileName[0] == '.' && nextFileName[1] == '.') { continue; } // skip parent directory

                PNSLR_ArenaAllocatorSnapshot currentIterSnapshot = PNSLR_CaptureArenaAllocatorSnapshot(internalAllocator);

                utf8str foundPath = PNSLR_MakeString((path.count + fileNameLen + 3), false, internalAllocator, nil);

                PNSLR_Intrinsic_MemCopy(foundPath.data, path.data, (i32) path.count);
                u32 iterator = (u32) path.count - 1;
                if (foundPath.data[iterator] == '/' || foundPath.data[iterator] == '\\')
                {
                    foundPath.data[iterator] = '/';
                    iterator++;
                }
                else
                {
                    iterator++;
                    foundPath.data[iterator] = '/';
                    iterator++;
                }

                PNSLR_Intrinsic_MemCopy(foundPath.data + iterator, nextFileName, fileNameLen);
                iterator += (u32) fileNameLen;
                foundPath.data[iterator] = '\0'; // null-terminate the string, just in case

                foundPath.count = iterator; // update count

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
                    if (stat((cstring) foundPath.data, &statBuf) == 0)
                    {
                        isDirectory = S_ISDIR(statBuf.st_mode);
                    }
                }
                #endif

                if (isDirectory)
                {
                    foundPath.data[iterator    ] = '/';  // ensure directory paths end with a slash
                    foundPath.data[iterator + 1] = '\0'; // null-terminate the string, just in case

                    foundPath.count = iterator + 1; // update count

                    iterator++;
                }

                b8 exploreCurrentDirectory = recursive;
                b8 iterateFurther = visitorFunc(visitorPayload, foundPath, isDirectory, &exploreCurrentDirectory);

                // handle recursion
                if (iterateFurther && isDirectory && exploreCurrentDirectory) { PNSLR_IterateDirectory(foundPath, recursive, visitorPayload, visitorFunc); }

                PNSLR_ArenaSnapshotError restoreError = PNSLR_RestoreArenaAllocatorSnapshot(&currentIterSnapshot, CURRENT_LOC());
                if (PNSLR_ArenaSnapshotError_None != restoreError) { FORCE_TRAP; }

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

    PATHS_INTERNAL_ALLOCATOR_RESET
}

b8 PNSLR_PathExists(utf8str path, PNSLR_PathExistsCheckType type)
{
    PATHS_INTERNAL_ALLOCATOR_INIT

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

    PATHS_INTERNAL_ALLOCATOR_RESET
    return result;
}

i64 PNSLR_GetFileTimestamp(utf8str path)
{
    PATHS_INTERNAL_ALLOCATOR_INIT

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

    PATHS_INTERNAL_ALLOCATOR_RESET
    return timestamp;
}

#undef PATHS_INTERNAL_ALLOCATOR_RESET
#undef PATHS_INTERNAL_ALLOCATOR_INIT
