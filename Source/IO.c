#include "IO.h"
#include "Allocators.h"
#include "Strings.h"

// internal allocator stuff ========================================================

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(Paths, 512);

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

typedef struct
{
    PNSLR_Allocator        allocator;
    b8                     failedAtSomething;
} DeleteAllContentsWhileIteratingDirectoryPayload;

static b8 DeleteAllContentsWhileIteratingDirectory(void* payload, PNSLR_Path path, b8 isDirectory, b8* exploreCurrentDirectory)
{
    DeleteAllContentsWhileIteratingDirectoryPayload* data = (DeleteAllContentsWhileIteratingDirectoryPayload*) payload;
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) path2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, data->allocator);
    #elif PNSLR_UNIX
        cstring path2 = PNSLR_CStringFromString(path.path, data->allocator);
    #endif

    if (isDirectory)
    {
        /**
         * The reason for doing things weirdly like this is because of how the directory iterator function works
         * it performs a top-down approach where the callback is given for the directory before its contents.
         * But we cannot delete the directory before its contents. So we kind of hack it by recursing the directory
         * iterator function ourselves.
         */
        *exploreCurrentDirectory = false;

        PNSLR_IterateDirectory(path, false, payload, DeleteAllContentsWhileIteratingDirectory);
        #if PNSLR_WINDOWS
            data->failedAtSomething = (RemoveDirectoryW((LPCWSTR) path2.data) == 0);
        #elif PNSLR_UNIX
            data->failedAtSomething = (rmdir(path2)            != 0);
        #endif
    }
    else
    {
        #if PNSLR_WINDOWS
            data->failedAtSomething = (DeleteFileW((LPCWSTR) path2.data) == 0);
        #elif PNSLR_UNIX
            data->failedAtSomething = (unlink(path2)      != 0);
        #endif
    }

    return true;
}

// actual function implementations =================================================

PNSLR_Path PNSLR_NormalisePath(utf8str path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator)
{
    if (!path.data || !path.count)
    {
        path = PNSLR_STRING_LITERAL(".");
    }

    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

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
                if (tempResultPath.data[i] == '\\') { tempResultPath.data[i] = '/'; } // normalise path separators
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

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);

    return (PNSLR_Path) { .path = resultPath };
}

// TODO: replace ANSI string usage for windows with UTF-16 strings; the specific complexity here is how the code has been reused
void PNSLR_IterateDirectory(PNSLR_Path path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.path.count + 3), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.path.data, (i32) path.path.count);
    #if PNSLR_WINDOWS
    {
        u32 iterator = (u32) path.path.count - 1;
        if (tempBuffer2.data[iterator] == '/' || tempBuffer2.data[iterator] == '\\') // has trailing slash
        {
            iterator = (u32) path.path.count;
        }
        else
        {
            tempBuffer2.data[path.path.count] = '/'; // add slash at the end
            iterator = (u32) path.path.count + 1;
        }

        tempBuffer2.data[iterator    ] = '*';  // add wildcard for file matching
        tempBuffer2.data[iterator + 1] = '\0'; // null-terminate
        tempBuffer2.count = iterator + 1;
    }
    #elif PNSLR_UNIX
    {
        tempBuffer2.data[path.path.count] = '\0'; // null-terminate
        tempBuffer2.count = path.path.count;
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

                utf8str foundPath = PNSLR_MakeString((path.path.count + fileNameLen + 3), false, internalAllocator, nil);

                PNSLR_Intrinsic_MemCopy(foundPath.data, path.path.data, (i32) path.path.count);
                u32 iterator = (u32) path.path.count - 1;
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
                        if (foundPath.data[i] == '\\') { foundPath.data[i] = '/'; } // normalise path separators
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

                PNSLR_Path foundPath2 = {.path = foundPath};

                b8 exploreCurrentDirectory = recursive;
                b8 iterateFurther = visitorFunc(visitorPayload, foundPath2, isDirectory, &exploreCurrentDirectory);

                // handle recursion
                if (iterateFurther && isDirectory && exploreCurrentDirectory) { PNSLR_IterateDirectory(foundPath2, recursive, visitorPayload, visitorFunc); }

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

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
}

b8 PNSLR_PathExists(PNSLR_Path path, PNSLR_PathExistsCheckType type)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    b8 canBeFile      = (type == PNSLR_PathExistsCheckType_Either || type == PNSLR_PathExistsCheckType_File);
    b8 canBeDirectory = (type == PNSLR_PathExistsCheckType_Either || type == PNSLR_PathExistsCheckType_Directory);

    b8 result         = false;

    #if PNSLR_WINDOWS

        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        DWORD fileAttributes = GetFileAttributesW((LPCWSTR) tempBuffer2.data);
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

        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        struct stat fileStat;
        if (stat(tempBuffer2, &fileStat) != 0)
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

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return result;
}

b8 PNSLR_DeletePath(PNSLR_Path path)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    b8 throwawayB8 = false;
    DeleteAllContentsWhileIteratingDirectoryPayload payload = {.failedAtSomething = false, .allocator = internalAllocator};
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        DWORD fileAttributes = GetFileAttributesW((LPCWSTR) tempBuffer2.data);
        if (fileAttributes != INVALID_FILE_ATTRIBUTES)
        {
            b8 isDir = (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            DeleteAllContentsWhileIteratingDirectory(&payload, path, isDir, &throwawayB8);
        }

    #elif PNSLR_UNIX
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        struct stat fileStat;
        if (stat(tempBuffer2, &fileStat) == 0)
        {
            b8 isFile = S_ISREG(fileStat.st_mode), isDir = S_ISDIR(fileStat.st_mode);
            if (isFile || isDir)
            {
                DeleteAllContentsWhileIteratingDirectory(&payload, path, isDir, &throwawayB8);
            }
        }

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);

    return !payload.failedAtSomething;
}

i64 PNSLR_GetFileTimestamp(PNSLR_Path path)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    i64 timestamp = 0;
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        HANDLE fileHandle = CreateFileW((LPCWSTR) tempBuffer2.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        struct stat fileStat;
        if (stat(tempBuffer2, &fileStat) == 0)
        {
            // Convert to nanoseconds since epoch
            timestamp = (i64) fileStat.st_mtime * 1000000000LL; // Convert seconds to nanoseconds
        }

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return timestamp;
}

i64 PNSLR_GetFileSize(PNSLR_Path path)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    i64 sizeInBytes = 0;
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        HANDLE fileHandle = CreateFileW((LPCWSTR) tempBuffer2.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            BY_HANDLE_FILE_INFORMATION fileInfo;
            if (GetFileInformationByHandle(fileHandle, &fileInfo))
            {
                sizeInBytes = ((i64) fileInfo.nFileSizeHigh << 32) | fileInfo.nFileSizeLow;
            }
            CloseHandle(fileHandle);
        }

    #elif PNSLR_UNIX
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        struct stat fileStat;
        if (stat(tempBuffer2, &fileStat) == 0)
        {
            sizeInBytes = (i64) fileStat.st_size;
        }

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);

    return sizeInBytes;
}

b8 PNSLR_CreateDirectoryTree(PNSLR_Path path)
{
    if (!path.path.data || !path.path.count) { return false; }

    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    #if PNSLR_WINDOWS
        WCHAR* tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator).data;
    #elif PNSLR_UNIX
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);
    #endif

    b8 success = true;
    for (i32 i = 1; success && i < (i32) path.path.count; i++)
    {
        if (tempBuffer2[i] == '/')
        {
            tempBuffer2[i] = '\0';
            #if PNSLR_WINDOWS
                if (!CreateDirectoryW(tempBuffer2, NULL))
                {
                    DWORD err = GetLastError();
                    if (err != ERROR_ALREADY_EXISTS) { success = false; }
                }
            #elif PNSLR_UNIX
                if (mkdir(tempBuffer2, 0777) != 0)
                {
                    if (errno != EEXIST) { success = false; }
                }
            #endif
            tempBuffer2[i] = '/';
        }
    }

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return success;
}

PNSLR_File PNSLR_OpenFileToRead(PNSLR_Path path, b8 allowWrite)
{
    if (!path.path.data || !path.path.count) { return (PNSLR_File) {0}; }

    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    PNSLR_File output = {0};
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        HANDLE fileHandle = CreateFileW((LPCWSTR) tempBuffer2.data,
                                   GENERIC_READ | (allowWrite ? GENERIC_WRITE : 0),
                                   FILE_SHARE_READ | (allowWrite ? FILE_SHARE_WRITE : 0),
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);

        if (INVALID_HANDLE_VALUE != fileHandle) { output.handle = fileHandle; }

    #elif PNSLR_UNIX
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        i32 fd = open(tempBuffer2, allowWrite ? O_RDWR : O_RDONLY);
        if (fd != -1) { output.handle = (rawptr) (i64) fd; }

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return output;
}

PNSLR_File PNSLR_OpenFileToWrite(PNSLR_Path path, b8 append, b8 allowRead)
{
    if (!path.path.data || !path.path.count) { return (PNSLR_File) {0}; }

    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    PNSLR_File output = {0};
    #if PNSLR_WINDOWS
        ArraySlice(utf16ch) tempBuffer2 = PNSLR_UTF16FromUTF8WindowsOnly(path.path, internalAllocator);

        HANDLE fileHandle = CreateFileW((LPCWSTR) tempBuffer2.data,
                                   GENERIC_WRITE | (allowRead ? GENERIC_READ : 0),
                                   FILE_SHARE_READ | (allowRead ? FILE_SHARE_WRITE : 0),
                                   NULL,
                                   append ? OPEN_ALWAYS : CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);

        if (INVALID_HANDLE_VALUE != fileHandle) { output.handle = fileHandle; }

    #elif PNSLR_UNIX
        cstring tempBuffer2 = PNSLR_CStringFromString(path.path, internalAllocator);

        i32 flags = allowRead ? O_RDWR : O_WRONLY;
        if (append) {
            flags |= O_CREAT | O_APPEND;
        } else {
            flags |= O_CREAT | O_TRUNC;
        }
        i32 fd = open(tempBuffer2, flags, 0666);
        if (fd != -1) { output.handle = (rawptr) (i64) fd; }

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return output;
}

i64 PNSLR_GetSizeOfFile(PNSLR_File handle)
{
    if (!handle.handle) { return 0; }

    i64 size = 0;
    #if PNSLR_WINDOWS

        LARGE_INTEGER fileSize;
        if (GetFileSizeEx((HANDLE) handle.handle, &fileSize)) {
            size = fileSize.QuadPart;
        }

    #elif PNSLR_UNIX

        struct stat st;
        if (fstat((i32) (i64) handle.handle, &st) == 0) {
            size = (i64) st.st_size;
        }

    #endif

    return size;
}

b8 PNSLR_SeekPositionInFile(PNSLR_File handle, i64 newPos, b8 relative)
{
    if (!handle.handle) { return false; }

    PNSLR_INTERNAL_ALLOCATOR_INIT(Paths, internalAllocator);

    b8 success = true;
    #if PNSLR_WINDOWS

        LARGE_INTEGER li;
        li.QuadPart = newPos;
        success = SetFilePointerEx((HANDLE) handle.handle, li, NULL, relative ? FILE_CURRENT : FILE_BEGIN);

    #elif PNSLR_UNIX

        success = (lseek((i32) (i64) handle.handle, newPos, relative ? SEEK_CUR : SEEK_SET) != -1);

    #endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(Paths, internalAllocator);
    return success;
}

b8 PNSLR_ReadFromFile(PNSLR_File handle, ArraySlice(u8) dst)
{
    if (!handle.handle || !dst.data || !dst.count) { return false; }

    b8 success = true;
    #if PNSLR_WINDOWS

        DWORD bytesRead = 0;
        success = ReadFile((HANDLE) handle.handle, dst.data, (DWORD) dst.count, &bytesRead, NULL);

    #elif PNSLR_UNIX

        ssize_t res = read((i32) (i64) handle.handle, dst.data, dst.count);
        success = (res == (ssize_t) dst.count);

    #endif

    return success;
}

b8 PNSLR_WriteToFile(PNSLR_File handle, ArraySlice(u8) src)
{
    if (!handle.handle || !src.data || !src.count) { return false; }

    b8 success = true;
    #if PNSLR_WINDOWS

        DWORD bytesWritten = 0;
        success = WriteFile((HANDLE) handle.handle, src.data, (DWORD) src.count, &bytesWritten, NULL);

    #elif PNSLR_UNIX

        ssize_t res = write((i32) (i64) handle.handle, src.data, src.count);
        success = (res == (ssize_t) src.count);

    #endif

    return success;
}

b8 PNSLR_TruncateFile(PNSLR_File handle, i64 newSize)
{
    if (!handle.handle || newSize < 0) { return false; }

    b8 success = true;
    #if PNSLR_WINDOWS

        LARGE_INTEGER li;
        li.QuadPart = newSize;
        if (SetFilePointerEx((HANDLE) handle.handle, li, NULL, FILE_BEGIN)) {
            success = SetEndOfFile((HANDLE) handle.handle);
        } else {
            success = false;
        }

    #elif PNSLR_UNIX

        success = (ftruncate((i32) (i64) handle.handle, newSize) == 0);

    #endif

    return success;
}

b8 PNSLR_FlushFile(PNSLR_File handle)
{
    if (!handle.handle) { return false; }

    b8 success = true;
    #if PNSLR_WINDOWS

        success = FlushFileBuffers((HANDLE) handle.handle);

    #elif PNSLR_UNIX

        success = (fsync((i32) (i64) handle.handle) == 0);

    #endif

    return success;
}

void PNSLR_CloseFileHandle(PNSLR_File handle)
{
    if (!handle.handle) { return; }

    #if PNSLR_WINDOWS

        CloseHandle((HANDLE) handle.handle);

    #elif PNSLR_UNIX

        close((i32) (i64) handle.handle);

    #endif
}

b8 PNSLR_ReadFromFile(PNSLR_Path path, ArraySlice(u8)* dst, PNSLR_Allocator allocator)
{
    if (!path.path.data || !path.path.count || !dst) { return false; }
    *dst = (ArraySlice(u8)) {0};

    PNSLR_File file = PNSLR_OpenFileToRead(path, false);
    if (!file.handle) { return false; }

    i64 size = PNSLR_GetSizeOfFile(file);
    if (size <= 0) { PNSLR_CloseFileHandle(file); return false; }

    *dst = PNSLR_MakeSlice(u8, size, false, allocator, nil);
    if (!dst->data || !dst->count) { PNSLR_CloseFileHandle(file); return false; }

    b8 success = PNSLR_ReadFromFile(file, *dst);
    PNSLR_CloseFileHandle(file);
    return success;
}

b8 PNSLR_WriteToFile(PNSLR_Path path, ArraySlice(u8) src, b8 append)
{
    if (!path.path.data || !path.path.count || !src.data || !src.count) { return false; }

    PNSLR_File file = PNSLR_OpenFileToWrite(path, append, false);
    if (!file.handle) { return false; }

    b8 success = PNSLR_WriteToFile(file, src);
    success = success && PNSLR_TruncateFile(file, src.count); // truncate to the size of the written data
    PNSLR_CloseFileHandle(file);
    return success;
}
