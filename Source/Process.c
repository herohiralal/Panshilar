#define PNSLR_IMPLEMENTATION
#include "Process.h"
#include "Strings.h"

void PNSLR_ExitProcess(i32 exitCode)
{
    #if PNSLR_WINDOWS
    {
        ExitProcess((UINT) exitCode);
    }
    #elif PNSLR_UNIX
    {
        // On Unix-like systems, we can use the exit system call directly.
        _exit(exitCode);
    }
    #endif
}

PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) PNSLR_GetEnvironmentVariables(PNSLR_Allocator allocator)
{
    PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) envVars = {0};

    i64 envVarsCount = 0;
    #if PNSLR_WINDOWS

        LPWCH envStringsWindows = GetEnvironmentStringsW();
        if (!envStringsWindows) { return envVars; }

        for (LPWCH var = envStringsWindows; *var; var += wcslen(var) + 1)
            envVarsCount++;

    #elif PNSLR_UNIX

        for (cstring* var = environ; *var; var++)
            envVarsCount++;

    #endif

    envVars = PNSLR_MakeSlice(PNSLR_EnvVarKeyValuePair, envVarsCount, false, allocator, PNSLR_GET_LOC(), nil);
    if (!envVars.data || !envVars.count) { return envVars; }

    i64 index = 0;
    #if PNSLR_WINDOWS

        for (LPWCH var = envStringsWindows; *var; var += wcslen(var) + 1)
        {
            LPWCH equalSign = wcschr(var, L'=');
            if (!equalSign) { continue; } // malformed entry, skip
            if (equalSign == var) { continue; } // empty key, skip

            envVars.data[index].key   = PNSLR_UTF8FromUTF16WindowsOnly((PNSLR_ArraySlice(u16)) {.data = (u16*) (var),           .count = (i64) (equalSign - var)    }, allocator);
            envVars.data[index].value = PNSLR_UTF8FromUTF16WindowsOnly((PNSLR_ArraySlice(u16)) {.data = (u16*) (equalSign + 1), .count = (i64) wcslen(equalSign + 1)}, allocator);
            index++;
        }

        FreeEnvironmentStringsW(envStringsWindows); // free the original block after counting

    #elif PNSLR_UNIX

        for (cstring* varPtr = environ; *varPtr; varPtr++)
        {
            cstring var = *varPtr;
            char* equalSign = strchr(var, '=');
            if (!equalSign) { continue; } // malformed entry, skip
            if (equalSign == var) { continue; } // empty key, skip

            envVars.data[index].key   = PNSLR_CloneString((utf8str) {.data = (u8*) (var),           .count = (i64) (equalSign - var)    }, allocator);
            envVars.data[index].value = PNSLR_CloneString((utf8str) {.data = (u8*) (equalSign + 1), .count = (i64) strlen(equalSign + 1)}, allocator);
            index++;
        }

    #endif

    envVars.count = index; // in case of malformed entries
    return envVars;
}

b8 PNSLR_CreatePipe(PNSLR_PipeHandle* outR, PNSLR_PipeHandle* outW)
{
    if (outR) *outR = (PNSLR_PipeHandle) {0};
    if (outW) *outW = (PNSLR_PipeHandle) {0};
    if (!outR || !outW) return false;

    #if PNSLR_WINDOWS

        HANDLE readHandle, writeHandle;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        if (!CreatePipe(&readHandle, &writeHandle, &sa, 0)) { return false; }

        outR->platformHandle = (u64) (rawptr) readHandle;
        outW->platformHandle = (u64) (rawptr) writeHandle;
        return true;

    #elif PNSLR_UNIX

        int fds[2];
        if (pipe(fds) != 0) { return false; }

        outR->platformHandle = (u64) (i64) fds[0];
        outW->platformHandle = (u64) (i64) fds[1];
        return true;

    #else

        #error "Pipe creation not implemented on this platform"
        return false;

    #endif
}

b8 PNSLR_GetRemainingPipeReadSize(PNSLR_PipeHandle pipe, i64* outSize)
{
    if (!outSize)
        return true;

    #if PNSLR_WINDOWS

        DWORD bytesAvailable = 0;
        if (!PeekNamedPipe((HANDLE) (rawptr) pipe.platformHandle, NULL, 0, NULL, &bytesAvailable, NULL))
            return false;

        *outSize = (i64) bytesAvailable;
        return true;

    #elif PNSLR_UNIX

        int bytesAvailable = 0;
        if (ioctl((i32) (i64) pipe.platformHandle, FIONREAD, &bytesAvailable) != 0)
            return false;

        *outSize = (i64) bytesAvailable;
        return true;

    #else

        #error "Getting remaining pipe read size not implemented on this platform"
        return false;

    #endif
}

b8 PNSLR_ReadFromPipe(PNSLR_PipeHandle pipe, PNSLR_ArraySlice(u8) dst, i64* readSize OPT_ARG)
{
    if (readSize) *readSize = 0;
    if (!dst.data || !dst.count) { return false; }

    b8 success = true;
    i64 bytesReadTotal = 0;

    #if PNSLR_WINDOWS

        DWORD bytesRead = 0;
        success = ReadFile((HANDLE) (rawptr) pipe.platformHandle, dst.data, (DWORD) dst.count, &bytesRead, NULL);
        if (!success)
        {
            DWORD err = GetLastError();
            if (err == ERROR_NO_DATA || err == ERROR_PIPE_NOT_CONNECTED)
                success = true; // no data yet
            else
                success = false;
            bytesRead = 0;
        }
        bytesReadTotal = (i64) bytesRead;

    #elif PNSLR_UNIX

        ssize_t res = read((i32) (i64) pipe.platformHandle, dst.data, (size_t) dst.count);
        if (res < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                res = 0; // no data available right now
            else
                success = false;
        }

        bytesReadTotal = (i64) res;

    #endif

    if (readSize) *readSize = bytesReadTotal;
    return success;
}

b8 PNSLR_WriteToPipe(PNSLR_PipeHandle pipe, PNSLR_ArraySlice(u8) src)
{
    if (!src.data || !src.count) { return false; }

    i64 bytesWrittenTotal = 0;
    b8 success = true;

    #if PNSLR_WINDOWS

        DWORD bytesWritten = 0;
        success = WriteFile((HANDLE) (rawptr) pipe.platformHandle, src.data, (DWORD) src.count, &bytesWritten, NULL);
        if (!success)
        {
            DWORD err = GetLastError();
            if (err == ERROR_NO_DATA || err == ERROR_PIPE_NOT_CONNECTED)
                success = true; // cannot write now
            else
                success = false;
            bytesWritten = 0;
        }
        bytesWrittenTotal = (i64) bytesWritten;

    #elif PNSLR_UNIX

        ssize_t res = write((i32) (i64) pipe.platformHandle, src.data, (size_t) src.count);
        if (res < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                res = 0; // cannot write now
            else
                success = false;
        }

        bytesWrittenTotal = (i64) res;

    #endif

    return success;
}


b8 PNSLR_ClosePipe(PNSLR_PipeHandle pipe)
{
    b8 success = true;
    #if PNSLR_WINDOWS

        success = CloseHandle((HANDLE) (rawptr) pipe.platformHandle);

    #elif PNSLR_UNIX

        success = (close((i32) (i64) pipe.platformHandle) == 0);

    #endif

    return success;
}
