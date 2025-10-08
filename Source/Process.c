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

        size_t fullLen = 0;
        for (LPWCH var = envStringsWindows; *var; var += fullLen + 1)
        {
            fullLen = wcslen(var);
            if (fullLen == 0) { continue; }

            LPWCH equalSign = wcschr(var, L'=');
            if (!equalSign) { continue; } // malformed entry, skip
            if (equalSign == var) { continue; } // empty key, skip

            utf8str kvp = PNSLR_UTF8FromUTF16WindowsOnly((PNSLR_ArraySlice(u16)) {.data = (u16*) var, .count = (i64) fullLen}, allocator);

            i64 keyLen = (i64) (strchr((char*) kvp.data, '=') - (char*) kvp.data);
            i64 valLen = kvp.count - keyLen - 1; // -1 for '='

            envVars.data[index].kvp   = kvp;
            envVars.data[index].key   = (utf8str) {.data = kvp.data, .count = keyLen};
            envVars.data[index].value = (utf8str) {.data = kvp.data + keyLen + 1, .count = valLen};
            index++;
        }

        FreeEnvironmentStringsW(envStringsWindows); // free the original block after counting

    #elif PNSLR_UNIX

        for (cstring* varPtr = environ; *varPtr; varPtr++)
        {
            cstring var = *varPtr;

            size_t fullLen = strlen(var);

            char* equalSign = strchr(var, '=');
            if (!equalSign) { continue; } // malformed entry, skip
            if (equalSign == var) { continue; } // empty key, skip

            utf8str kvp = PNSLR_CloneString((utf8str) {.data = (u8*) var, .count = (i64) fullLen}, allocator);

            i64 keyLen = (i64) (strchr((char*) kvp.data, '=') - (char*) kvp.data);
            i64 valLen = kvp.count - keyLen - 1; // -1 for '='

            envVars.data[index].kvp   = kvp;
            envVars.data[index].key   = (utf8str) {.data = kvp.data, .count = keyLen};
            envVars.data[index].value = (utf8str) {.data = kvp.data + keyLen + 1, .count = valLen};
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

    #elif PNSLR_UNIX

        ssize_t res = write((i32) (i64) pipe.platformHandle, src.data, (size_t) src.count);
        if (res < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                res = 0; // cannot write now
            else
                success = false;
        }

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

#if PNSLR_WINDOWS
PNSLR_ArraySlice(u16) PNSLR_Internal_BuildWindowsProcessCmdLine(PNSLR_ArraySlice(utf8str) execAndArgs, PNSLR_Allocator tempAllocator)
{
    PNSLR_StringBuilder sb = {.allocator = tempAllocator};

    for (i64 i = 0; i < execAndArgs.count; i++)
    {
        utf8str arg = execAndArgs.data[i];

        if (i != 0) { PNSLR_AppendByteToStringBuilder(&sb, ' '); }

        // Escape and quote the argument as needed
        b8 needsQuotes = false;
        for (i64 j = 0; j < arg.count; j++)
        {
            if (false ||
                arg.data[j] == '('  ||
                arg.data[j] == ')'  ||
                arg.data[j] == '['  ||
                arg.data[j] == ']'  ||
                arg.data[j] == '{'  ||
                arg.data[j] == '}'  ||
                arg.data[j] == '^'  ||
                arg.data[j] == '='  ||
                arg.data[j] == ';'  ||
                arg.data[j] == '!'  ||
                arg.data[j] == '\'' ||
                arg.data[j] == '+'  ||
                arg.data[j] == ','  ||
                arg.data[j] == '`'  ||
                arg.data[j] == '~'  ||
                arg.data[j] == '\"' ||
                arg.data[j] == ' '  ||
                arg.data[j] == '\t' || // include tab as whitespace
                false)
            {
                needsQuotes = true;
                break;
            }
        }

        if (!needsQuotes)
        {
            PNSLR_AppendStringToStringBuilder(&sb, arg);
            continue;
        }

        // Argument needs quotes and possibly escaping

        PNSLR_AppendByteToStringBuilder(&sb, '\"'); // start

        i64 j = 0;
        while (j < arg.count)
        {
            i64 backslashes = 0;

            while (j < arg.count && arg.data[j] == '\\') { backslashes++; j++; }

            if (j == arg.count)
            {
                // Escape all backslashes at the end
                for (i64 k = 0; k < backslashes * 2; k++)
                    PNSLR_AppendByteToStringBuilder(&sb, '\\');
                break;
            }
            else if (arg.data[j] == '\"')
            {
                // Escape all backslashes and the quote
                for (i64 k = 0; k < (backslashes * 2) + 1; k++)
                    PNSLR_AppendByteToStringBuilder(&sb, '\\');

                PNSLR_AppendByteToStringBuilder(&sb, '\"');
            }
            else
            {
                // No special handling needed, just output the backslashes
                for (i64 k = 0; k < backslashes; k++)
                    PNSLR_AppendByteToStringBuilder(&sb, '\\');

                PNSLR_AppendByteToStringBuilder(&sb, arg.data[j]);
            }

            j++;
        }

        PNSLR_AppendByteToStringBuilder(&sb, '\"'); // end
    }

    PNSLR_AppendByteToStringBuilder(&sb, '\0');
    utf8str cmdLineUtf8 = PNSLR_StringFromStringBuilder(&sb);

    i32 n = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) cmdLineUtf8.data, -1, NULL, 0);
    if (n <= 0) { return (PNSLR_ArraySlice(u16)) {0}; } // conversion failed

    PNSLR_ArraySlice(u16) cmdLineUtf16 = PNSLR_MakeSlice(u16, (i64) n, false, tempAllocator, PNSLR_GET_LOC(), nil);
    if (!cmdLineUtf16.data) { return (PNSLR_ArraySlice(u16)) {0}; } // allocation failed

    i32 n1 = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) cmdLineUtf8.data, -1, (LPWSTR) cmdLineUtf16.data, n);
    if (n1 == 0)
    {
        PNSLR_FreeSlice(&cmdLineUtf16, tempAllocator, PNSLR_GET_LOC(), nil);
        return (PNSLR_ArraySlice(u16)) {0}; // conversion failed
    }

    return cmdLineUtf16;
}

PNSLR_ArraySlice(u16) PNSLR_Internal_BuildWindowsProcessEnvBlock(PNSLR_ArraySlice(utf8str) envVars, PNSLR_Allocator tempAllocator)
{
    PNSLR_StringBuilder sb = {.allocator = tempAllocator};

    for (i64 currIdx = (envVars.count - 1); currIdx >= 0; --currIdx)
    {
        utf8str kv = envVars.data[currIdx];

        i64 eqIdx = -1;
        for (i64 i = 0; i < kv.count; i++)
        {
            if (kv.data[i] == '=')
            {
                eqIdx = i;
                break;
            }
        }

        if (eqIdx == -1) { continue; } // malformed, skip

        utf8str key = kv; key.count = eqIdx;

        b8 foundDuplicate = false;
        for (i64 prevIdx = (currIdx + 1); prevIdx < envVars.count; prevIdx++)
        {
            i64 eqIdxPrev = -1;
            for (i64 i = 0; i < envVars.data[prevIdx].count; i++)
            {
                if (envVars.data[prevIdx].data[i] == '=')
                {
                    eqIdxPrev = i;
                    break;
                }
            }

            if (eqIdxPrev == -1) { continue; } // malformed, skip

            utf8str keyPrev = envVars.data[prevIdx]; keyPrev.count = eqIdxPrev;
            if (PNSLR_AreStringsEqual(key, keyPrev, PNSLR_StringComparisonType_CaseInsensitive))
            {
                foundDuplicate = true;
                break;
            }
        }

        if (foundDuplicate) { continue; } // skip this one, a later one exists

        PNSLR_AppendStringToStringBuilder(&sb, kv);
        PNSLR_AppendByteToStringBuilder(&sb, '\0'); // null terminator
    }

    PNSLR_AppendByteToStringBuilder(&sb, '\0'); // final null terminator

    utf8str envBlockUtf8 = PNSLR_StringFromStringBuilder(&sb);

    i32 n = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) envBlockUtf8.data, -1, NULL, 0);
    if (n <= 0) { return (PNSLR_ArraySlice(u16)) {0}; } // conversion failed

    PNSLR_ArraySlice(u16) envBlockUtf16 = PNSLR_MakeSlice(u16, (i64) n, false, tempAllocator, PNSLR_GET_LOC(), nil);
    if (!envBlockUtf16.data) { return (PNSLR_ArraySlice(u16)) {0}; } // allocation failed

    i32 n1 = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) envBlockUtf8.data, -1, (LPWSTR) envBlockUtf16.data, (i32) n);
    if (n1 == 0)
    {
        PNSLR_FreeSlice(&envBlockUtf16, tempAllocator, PNSLR_GET_LOC(), nil);
        return (PNSLR_ArraySlice(u16)) {0}; // conversion failed
    }

    return envBlockUtf16;
}
#elif PNSLR_UNIX

PNSLR_ArraySlice(utf8str) PNSLR_Internal_SplitUnixPathList(utf8str pathStr, PNSLR_Allocator allocator)
{
    if (!pathStr.data || !pathStr.count) return (PNSLR_ArraySlice(utf8str)) {0};

    i64 start = 0, count = 0;
    b8 quote = false;

    for (i64 i = 0; i < pathStr.count; i++)
    {
        u8 c = pathStr.data[i];
        if (c == '\"') { quote = !quote; }
        else if (c == ':' && !quote) { count++; }
    }

    start = 0; quote = false;

    PNSLR_ArraySlice(utf8str) paths = PNSLR_MakeSlice(utf8str, count + 1, false, allocator, PNSLR_GET_LOC(), nil);

    i64 index = 0;
    for (i64 i = 0; i < pathStr.count; i++)
    {
        u8 c = pathStr.data[i];
        if (c == '\"') { quote = !quote; }
        else if (c == ':' && !quote)
        {
            paths.data[index] = pathStr; paths.data[index].data += start; paths.data[index].count = i - start;
            index++;
            start = i + 1;
        }
    }

    if (index != count) FORCE_DBG_TRAP;

    paths.data[index] = pathStr; paths.data[index].data += start; paths.data[index].count = pathStr.count - start;

    for (i64 i = 0; i < paths.count; i++)
    {
        utf8str s0 = paths.data[i];

        utf8str s = PNSLR_ReplaceInString(s0, PNSLR_StringLiteral("\""), PNSLR_StringLiteral(""), allocator, PNSLR_StringComparisonType_CaseSensitive);
        paths.data[i] = s;
    }

    return paths;
}

#endif

b8 PNSLR_RunProcess(PNSLR_ProcessHandle* outProcessHandle, PNSLR_ArraySlice(utf8str) execAndArgs, PNSLR_ArraySlice(utf8str) environmentVariables, PNSLR_Path workingDirectory, PNSLR_PipeHandle* stdOutPipe, PNSLR_PipeHandle* stdErrPipe)
{
    if (!outProcessHandle) return false;
    *outProcessHandle = (PNSLR_ProcessHandle) {0};

    if (execAndArgs.count < 1 || !execAndArgs.data) return false;

    PNSLR_Allocator tempAllocator = PNSLR_NewAllocator_Arena(PNSLR_GetAllocator_DefaultHeap(), 8192, PNSLR_GET_LOC(), nil);
    if (!tempAllocator.procedure) return false;

    b8 success = false;
    #if PNSLR_WINDOWS
    {
        PNSLR_ArraySlice(u16) cmdLine = PNSLR_Internal_BuildWindowsProcessCmdLine(execAndArgs, tempAllocator);

        // Build localEnvVars if environmentVariables is empty; otherwise use provided env list.
        PNSLR_ArraySlice(utf8str) localEnvVars = environmentVariables;
        if (!environmentVariables.count || !environmentVariables.data)
        {
            PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) kvps = PNSLR_GetEnvironmentVariables(tempAllocator);
            localEnvVars = PNSLR_MakeSlice(utf8str, kvps.count, false, tempAllocator, PNSLR_GET_LOC(), nil);
            for (i64 i = 0; i < kvps.count; i++)
                localEnvVars.data[i] = kvps.data[i].kvp;
        }

        PNSLR_ArraySlice(u16) envBlock = {0};
        if (localEnvVars.count && localEnvVars.data)
            envBlock = PNSLR_Internal_BuildWindowsProcessEnvBlock(localEnvVars, tempAllocator);

        HANDLE nullHandle = NULL;
        if (!stdOutPipe || !stdErrPipe)
        {
            SECURITY_ATTRIBUTES sa = {.nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = true};
            nullHandle = CreateFileW(L"NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nil);
            if (nullHandle == INVALID_HANDLE_VALUE) { FORCE_DBG_TRAP; } // should not happen
        }

        HANDLE stdOutHandle = stdOutPipe ? (HANDLE) (rawptr) stdOutPipe->platformHandle : nullHandle;
        HANDLE stdErrHandle = stdErrPipe ? (HANDLE) (rawptr) stdErrPipe->platformHandle : nullHandle;
        HANDLE stdInHandle  = nullHandle; // we do not support stdin for now

        PNSLR_ArraySlice(u16) workingDirUtf16 = {0};
        if (workingDirectory.path.data && workingDirectory.path.count) workingDirUtf16 = PNSLR_UTF16FromUTF8WindowsOnly(workingDirectory.path, tempAllocator);

        STARTUPINFOW si =
        {
            .cb = sizeof(STARTUPINFOW),
            .hStdError = stdErrHandle,
            .hStdOutput = stdOutHandle,
            .hStdInput = stdInHandle,
            .dwFlags = STARTF_USESTDHANDLES
        };

        PROCESS_INFORMATION pi = {0};
        BOOL ok = CreateProcessW(
            NULL,
            (LPWSTR) cmdLine.data,
            NULL,
            NULL,
            true,
            CREATE_UNICODE_ENVIRONMENT | NORMAL_PRIORITY_CLASS,
            envBlock.data ? envBlock.data : NULL,
            workingDirUtf16.data ? workingDirUtf16.data : NULL,
            &si,
            &pi
        );

        if (nullHandle != NULL)
        {
            CloseHandle(nullHandle);
            nullHandle = NULL;
        }

        success = (b8) ok;

        if (ok)
        {
            *outProcessHandle = (PNSLR_ProcessHandle)
            {
                .pid = (i64) pi.dwProcessId,
                .handle = (u64) (rawptr) pi.hProcess
            };
            // We don't need the initial thread handle here — close to avoid leak.
            if (pi.hThread) { CloseHandle(pi.hThread); pi.hThread = NULL; }
        }
        else
        {
            // Ensure we don't leak any handles if CreateProcessW failed and returned handles.
            if (pi.hProcess) { CloseHandle(pi.hProcess); pi.hProcess = NULL; }
            if (pi.hThread)  { CloseHandle(pi.hThread);  pi.hThread = NULL; }
            goto exitFn;
        }
    }
    #elif PNSLR_UNIX
    {
        PNSLR_StringBuilder exeBuilder = {.allocator = tempAllocator};
        utf8str exePath = execAndArgs.data[0];

        b8 isSimpleExePath = true;
        for (i64 i = 0; i < exePath.count; i++)
        {
            if (exePath.data[i] == '/' || exePath.data[i] == '\\')
            {
                isSimpleExePath = false;
                break;
            }
        }

        PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) currentEnvVars = {0};
        if (!isSimpleExePath)
        {
            PNSLR_ResetStringBuilder(&exeBuilder);
            PNSLR_AppendStringToStringBuilder(&exeBuilder, exePath);
            PNSLR_AppendByteToStringBuilder(&exeBuilder, '\0');

            utf8str exePathWithNull = PNSLR_StringFromStringBuilder(&exeBuilder);
            cstring exePathCStr = (cstring) exePathWithNull.data;

            // check if path is executable
            if (access(exePathCStr, X_OK) != 0)
            {
                goto exitFn; // not executable or does not exist
            }
        }
        else
        {
            currentEnvVars = PNSLR_GetEnvironmentVariables(tempAllocator); // to ensure PATH is loaded
            utf8str pathVar = {0};
            for (i64 i = 0; i < currentEnvVars.count; i++)
            {
                if (PNSLR_AreStringsEqual(currentEnvVars.data[i].key, PNSLR_StringLiteral("PATH"), PNSLR_StringComparisonType_CaseSensitive))
                {
                    pathVar = currentEnvVars.data[i].value;
                    break;
                }
            }

            PNSLR_ArraySlice(utf8str) pathDirs = PNSLR_Internal_SplitUnixPathList(pathVar, tempAllocator);

            b8 found = false;
            for (i64 i = 0; i < pathDirs.count; i++)
            {
                utf8str dir = pathDirs.data[i];
                PNSLR_ResetStringBuilder(&exeBuilder);
                PNSLR_AppendStringToStringBuilder(&exeBuilder, dir);
                PNSLR_AppendByteToStringBuilder(&exeBuilder, '/');
                PNSLR_AppendStringToStringBuilder(&exeBuilder, exePath);
                PNSLR_AppendByteToStringBuilder(&exeBuilder, '\0');

                utf8str fullPath = PNSLR_StringFromStringBuilder(&exeBuilder);
                cstring fullPathCStr = (cstring) fullPath.data;

                if (access(fullPathCStr, X_OK) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found) // check in cwd
            {
                PNSLR_ResetStringBuilder(&exeBuilder);
                PNSLR_AppendStringToStringBuilder(&exeBuilder, workingDirectory.path);
                if (workingDirectory.path.count > 0 && workingDirectory.path.data[workingDirectory.path.count - 1] != '/')
                    PNSLR_AppendByteToStringBuilder(&exeBuilder, '/');
                PNSLR_AppendStringToStringBuilder(&exeBuilder, PNSLR_StringLiteral("./"));
                PNSLR_AppendStringToStringBuilder(&exeBuilder, exePath);
                PNSLR_AppendByteToStringBuilder(&exeBuilder, '\0');

                utf8str fullPath = PNSLR_StringFromStringBuilder(&exeBuilder);
                cstring fullPathCStr = (cstring) fullPath.data;

                if (access(fullPathCStr, X_OK) == 0)
                {
                    found = true;
                }
            }

            if (!found) { goto exitFn; } // not found in PATH
        }

        cstring cwd = nil;
        if (workingDirectory.path.data && workingDirectory.path.count)
            cwd = PNSLR_CStringFromString(workingDirectory.path, tempAllocator);


        cstring* cmd = PNSLR_Allocate(tempAllocator, false, (i32) ((i64) sizeof(cstring) * (execAndArgs.count + 1)), (i32) alignof(cstring), PNSLR_GET_LOC(), nil);
        if (!cmd) goto exitFn;

        for (i64 i = 0; i < execAndArgs.count; i++)
            cmd[i] = PNSLR_CStringFromString(execAndArgs.data[i], tempAllocator);
        cmd[execAndArgs.count] = NULL; // NULL-terminate argv

        cstring* env;
        cstring* cenv = NULL;
        if (!environmentVariables.count || !environmentVariables.data)
        {
            env = environ; // inherit from current process
        }
        else
        {
            cenv = PNSLR_Allocate(tempAllocator, false, (i32) ((i64) sizeof(cstring) * (environmentVariables.count + 1)), (i32) alignof(cstring), PNSLR_GET_LOC(), nil);
            if (!cenv) goto exitFn;
            for (i64 i = 0; i < environmentVariables.count; i++)
                cenv[i] = PNSLR_CStringFromString(environmentVariables.data[i], tempAllocator);
            cenv[environmentVariables.count] = NULL; // NULL-terminate envp

            env = cenv;
        }

        static const i32 READ = 0;
        static const i32 WRITE = 1;

        int pipeVal[2];
        if (pipe(pipeVal) != 0) { goto exitFn; }

        // make read end close-on-exec
        if (fcntl(pipeVal[READ], F_SETFD, FD_CLOEXEC) == -1)
        {
            close(pipeVal[READ]);
            close(pipeVal[WRITE]);
            goto exitFn;
        }

        // make write end close-on-exec
        if (fcntl(pipeVal[WRITE], F_SETFD, FD_CLOEXEC) == -1)
        {
            close(pipeVal[READ]);
            close(pipeVal[WRITE]);
            goto exitFn;
        }

        __pid_t pid = fork();
        switch (pid)
        {
            case -1: // fork failed
            {
                close(pipeVal[WRITE]);
                close(pipeVal[READ]);
                goto exitFn;
            }

            case 0: // child
            {
                // Close read end in child; child will write a single byte on failure.
                close(pipeVal[READ]);

                int nullFile = open("/dev/null", O_RDWR);
                if (nullFile == -1)
                {
                    int errNoVal = errno;
                    // write 4 bytes of errno for safety
                    (void)write(pipeVal[WRITE], &errNoVal, sizeof(errNoVal));
                    _exit(126);
                }

                int stdoutFile = stdOutPipe ? (i32) (i64) stdOutPipe->platformHandle : nullFile;
                int stderrFile = stdErrPipe ? (i32) (i64) stdErrPipe->platformHandle : nullFile;
                int stdinFile  = nullFile; // we do not support stdin for now

                if (dup2(stdoutFile, STDOUT_FILENO) == -1 ||
                    dup2(stderrFile, STDERR_FILENO) == -1 ||
                    dup2(stdinFile,  STDIN_FILENO)  == -1)
                {
                    int errNoVal = errno;
                    (void)write(pipeVal[WRITE], &errNoVal, sizeof(errNoVal));
                    _exit(126);
                }

                if (cwd != nil && chdir(cwd) != 0)
                {
                    int errNoVal = errno;
                    (void)write(pipeVal[WRITE], &errNoVal, sizeof(errNoVal));
                    _exit(126);
                }

                // exeBuilder contains the full executable path (we ensured '\0' was appended earlier).
                utf8str fullExePath = PNSLR_StringFromStringBuilder(&exeBuilder);
                cstring fullExePathCStr = (cstring) fullExePath.data;

                execve(fullExePathCStr, cmd, env);
                // If execve returns, it's an error
                {
                    int errNoVal = errno;
                    (void)write(pipeVal[WRITE], &errNoVal, sizeof(errNoVal));
                    _exit(126);
                }

                break;
            }

            default: // parent
            {
                // Close write end in parent — child writes to it on error.
                close(pipeVal[WRITE]);

                int errNoVal = 0;

                // Read error info from child (child writes errno if it failed before exec).
                // Child writes sizeof(int). Read that (or EOF).
                ssize_t totalRead = 0;
                while (totalRead < (ssize_t)sizeof(int))
                {
                    ssize_t r = read(pipeVal[READ], ((u8*)&errNoVal) + totalRead, (size_t) ((ssize_t) sizeof(int) - totalRead));
                    if (r > 0) { totalRead += r; continue; }
                    if (r == 0) { /* EOF: child closed without writing: treat as no-error */ break; }
                    if (r == -1)
                    {
                        if (errno == EINTR) continue;
                        // read failed; set errNoVal to errno and break
                        errNoVal = errno;
                        break;
                    }
                }

                if (errNoVal != 0)
                {
                    // reported error — wait for the child to avoid zombies (use local pid)
                    while (true)
                    {
                        siginfo_t info;
                        int wpid = waitid(P_PID, (id_t) pid, &info, WEXITED);
                        if (wpid == -1 && errno == EINTR)
                            continue; // interrupted, try again
                        break;
                    }

                    close(pipeVal[READ]);
                    goto exitFn;
                }

                // No error reported — child successfully exec'd (or at least didn't fail early).
                *outProcessHandle = (PNSLR_ProcessHandle) {.pid = (i64) pid, .handle = 0};
                success = true;
                break;
            }
        }

        close(pipeVal[READ]);
    }
    #else
        #error "Process creation not implemented on this platform"
    #endif

exitFn:
    PNSLR_DestroyAllocator_Arena(tempAllocator, PNSLR_GET_LOC(), nil);

    return success;
}

b8 PNSLR_WaitForProcess(PNSLR_ProcessHandle* process, i32* outExitCode)
{
    if (!process) return false;

#if PNSLR_WINDOWS
    HANDLE hProc = (HANDLE)(uintptr_t)process->handle;
    if (!hProc) return false;

    DWORD waitResult = WaitForSingleObject(hProc, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
        return false;

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(hProc, &exitCode))
        return false;

    if (outExitCode)
        *outExitCode = (i32)exitCode;

    return true;
#else
    int status = 0;
    pid_t pidResult = waitpid((pid_t)process->pid, &status, 0);
    if (pidResult == -1)
        return false;

    if (outExitCode)
    {
        if (WIFEXITED(status))
            *outExitCode = (i32) WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            *outExitCode = (i32) -WTERMSIG(status);
        else
            *outExitCode = (i32) -1;
    }

    return true;
#endif
}

b8 PNSLR_KillProcess(PNSLR_ProcessHandle* process)
{
    if (!process) return false;

#if PNSLR_WINDOWS
    HANDLE hProc = (HANDLE)(uintptr_t)process->handle;
    if (!hProc) return false;

    BOOL ok = TerminateProcess(hProc, 1);
    return ok ? true : false;
#else
    if (process->pid <= 0)
        return false;

    int result = kill((pid_t)process->pid, SIGKILL);
    return (result == 0);
#endif
}

void PNSLR_CloseProcess(PNSLR_ProcessHandle* process)
{
    if (!process) return;

#if PNSLR_WINDOWS
    HANDLE hProc = (HANDLE)(uintptr_t)process->handle;
    if (hProc)
        CloseHandle(hProc);
    process->handle = 0;
#else
    // If using pidfds (Linux 5.3+), close here:
    // close((int)process->handle);
    process->handle = 0;
#endif

    process->pid = 0;
}
