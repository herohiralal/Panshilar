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
