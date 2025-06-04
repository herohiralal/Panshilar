#include "IO.h"

b8 PNSLR_PathExists(utf8str path, PNSLR_PathCheckType type)
{
    // copy the filename to a temporary buffer
    char tempBuffer[path.count + 1];
    PNSLR_Intrinsic_MemCopy(tempBuffer, path.data, (i32) path.count);
    tempBuffer[path.count] = '\0';

    b8 canBeFile      = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_File);
    b8 canBeDirectory = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_Directory);

    #if PNSLR_WINDOWS

        DWORD fileAttributes = GetFileAttributesA(tempBuffer);
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
        {
            return false; // File does not exist
        }

        if (canBeFile && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            return true; // It's a file
        }

        if (canBeDirectory && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            return true; // It's a directory
        }

    #elif PNSLR_UNIX

        struct stat fileStat;
        if (stat(tempBuffer, &fileStat) != 0)
        {
            return false; // File does not exist
        }

        if (canBeFile && S_ISREG(fileStat.st_mode))
        {
            return true; // It's a file
        }

        if (canBeDirectory && S_ISDIR(fileStat.st_mode))
        {
            return true; // It's a directory
        }

    #endif

    return false;
}

i64 PNSLR_GetFileTimestamp(utf8str path)
{
    char tempBuffer[path.count + 1];
    PNSLR_Intrinsic_MemCopy(tempBuffer, path.data, (i32) path.count);
    tempBuffer[path.count] = '\0';

    i64 timestamp = 0;
    #if PNSLR_WINDOWS

        HANDLE fileHandle = CreateFileA(tempBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
        if (stat(tempBuffer, &fileStat) == 0)
        {
            // Convert to nanoseconds since epoch
            timestamp = (i64)fileStat.st_mtime * 1000000000LL; // Convert seconds to nanoseconds
        }

    #endif

    return timestamp;
}
