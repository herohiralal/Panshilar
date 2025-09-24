#define PNSLR_IMPLEMENTATION
#include "Threads.h"
#include "Strings.h"

#if PNSLR_WINDOWS || PNSLR_OSX || PNSLR_IOS
    #define PNSLR_MAX_THREAD_NAME_LEN 64
#elif PNSLR_LINUX || PNSLR_ANDROID
    #define PNSLR_MAX_THREAD_NAME_LEN 16
#else
    #error "Unimplemented."
#endif

#if PNSLR_WINDOWS
    static_assert(sizeof(PNSLR_ThreadHandle) == sizeof(HANDLE), "HANDLE size mismatch");
#elif PNSLR_UNIX
    static_assert(sizeof(PNSLR_ThreadHandle) == sizeof(pthread_t), "pthread_t size mismatch");
#endif

b8 PNSLR_IsThreadHandleValid(PNSLR_ThreadHandle handle)
{
    #if PNSLR_WINDOWS
        DWORD exitCode = 0;
        return (b8) GetExitCodeThread((HANDLE) handle.handle, &exitCode);
    #elif PNSLR_UNIX
        return (0 == pthread_kill((pthread_t) handle.handle, 0));
    #endif
}

PNSLR_ThreadHandle PNSLR_GetCurrentThreadHandle(void)
{
    #if PNSLR_WINDOWS
        return (PNSLR_ThreadHandle) { .handle = (u64) GetCurrentThread() };
    #elif PNSLR_UNIX
        return (PNSLR_ThreadHandle) { .handle = (u64) pthread_self() };
    #endif
}

utf8str PNSLR_GetThreadName(PNSLR_ThreadHandle handle, PNSLR_Allocator allocator)
{
    #if PNSLR_WINDOWS
    {
        WCHAR nameBuffer[PNSLR_MAX_THREAD_NAME_LEN] = {0};
        if (GetThreadDescription((HANDLE) handle.handle, &nameBuffer[0]) != S_OK) goto failedToFindName;
        i64 len = 0;
        while (len < PNSLR_MAX_THREAD_NAME_LEN && nameBuffer[len] != 0) { ++len; }
        if (len == 0) goto failedToFindName; // no name set

        return PNSLR_UTF8FromUTF16WindowsOnly((PNSLR_ArraySlice(u16)) { .data = (u16*) nameBuffer, .count = len }, allocator);
    }
    #elif PNSLR_UNIX
    {
        char nameBuffer[PNSLR_MAX_THREAD_NAME_LEN] = {0};
        if (pthread_getname_np((pthread_t) handle.handle, nameBuffer, PNSLR_MAX_THREAD_NAME_LEN) != 0)
        {
            return (utf8str) {0};
        }

        i32 len = 0;
        while (len < PNSLR_MAX_THREAD_NAME_LEN && nameBuffer[len] != 0) { ++len; }

        if (len == 0) goto failedToFindName; // no name set

        return PNSLR_CloneString(PNSLR_StringFromCString(nameBuffer), allocator);
    }
    #endif

    failedToFindName:
    {
        u8 tempBuffer[32];
        PNSLR_StringBuilder sb = {.buffer.data = &(tempBuffer[0]), .buffer.count = 32};
        if (!PNSLR_AppendU64ToStringBuilder(&sb, handle.handle, PNSLR_IntegerBase_Decimal))
        {
            FORCE_DBG_TRAP; return (utf8str) {0};
        }

        utf8str sbStr = PNSLR_StringFromStringBuilder(&sb);

        utf8str prefix = PNSLR_StringLiteral("Thread#");
        utf8str str = PNSLR_MakeString(prefix.count + sbStr.count, false, allocator, PNSLR_GET_LOC(), nil);
        if (!str.data || !str.count) { FORCE_DBG_TRAP; return (utf8str) {0}; }

        PNSLR_MemCopy(str.data, prefix.data, (i32) prefix.count);
        PNSLR_MemCopy(str.data + prefix.count, sbStr.data, (i32) sbStr.count);
        return str;
    }
}

void PNSLR_SetThreadName(PNSLR_ThreadHandle handle, utf8str name)
{
    #if PNSLR_WINDOWS
    #elif PNSLR_UNIX
    #endif
}

utf8str PNSLR_GetCurrentThreadName(PNSLR_Allocator allocator)
{
    return PNSLR_GetThreadName(PNSLR_GetCurrentThreadHandle(), allocator);
}

void PNSLR_SetCurrentThreadName(utf8str name)
{
    PNSLR_SetThreadName(PNSLR_GetCurrentThreadHandle(), name);
}

#undef PNSLR_MAX_THREAD_NAME_LEN
