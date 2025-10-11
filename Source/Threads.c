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
        LPWSTR threadName = nil;
        if (GetThreadDescription((HANDLE) handle.handle, &threadName) != S_OK) goto failedToFindName;
        if (!threadName) goto failedToFindName;

        i32 len = 0;
        while (threadName[len] != 0) { ++len; }

        return PNSLR_UTF8FromUTF16WindowsOnly((PNSLR_ArraySlice(u16)) { .data = (u16*) threadName, .count = len }, allocator);
    }
    #elif PNSLR_OSX || PNSLR_IOS || (PNSLR_LINUX && defined(_GNU_SOURCE)) || (PNSLR_ANDROID && defined(_GNU_SOURCE))
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
    #if !PNSLR_APPLE
        if (name.count > PNSLR_MAX_THREAD_NAME_LEN - 1)
            name.count = PNSLR_MAX_THREAD_NAME_LEN - 1;

        #if PNSLR_WINDOWS
        {
            WCHAR nameBuffer[PNSLR_MAX_THREAD_NAME_LEN] = {0};
            MultiByteToWideChar(CP_UTF8, 0, (cstring) name.data, (i32) name.count, nameBuffer, PNSLR_MAX_THREAD_NAME_LEN);

            SetThreadDescription((HANDLE) handle.handle, nameBuffer);
        }
        #elif (PNSLR_LINUX && defined(_GNU_SOURCE)) || (PNSLR_ANDROID && defined(_GNU_SOURCE))
        {
            char nameBuffer[PNSLR_MAX_THREAD_NAME_LEN] = {0};
            PNSLR_MemCopy(nameBuffer, name.data, (i32) name.count);
            nameBuffer[name.count] = '\0';

            pthread_setname_np((pthread_t) handle.handle, nameBuffer);
        }
        #endif
    #else
        return; // setting thread name on Apple platforms is not supported
    #endif
}

utf8str PNSLR_GetCurrentThreadName(PNSLR_Allocator allocator)
{
    return PNSLR_GetThreadName(PNSLR_GetCurrentThreadHandle(), allocator);
}

void PNSLR_SetCurrentThreadName(utf8str name)
{
    #if PNSLR_APPLE
        if (name.count > PNSLR_MAX_THREAD_NAME_LEN - 1)
            name.count = PNSLR_MAX_THREAD_NAME_LEN - 1;

        char nameBuffer[PNSLR_MAX_THREAD_NAME_LEN] = {0};
        PNSLR_MemCopy(nameBuffer, name.data, (i32) name.count);
        nameBuffer[name.count] = '\0';
        pthread_setname_np(nameBuffer);
        return;
    #else
        PNSLR_SetThreadName(PNSLR_GetCurrentThreadHandle(), name);
    #endif
}

typedef struct PNSLR_Internal_ThreadProcPayload
{
    PNSLR_ThreadProcedure procedure;
    rawptr                data;
    utf8str               threadName;
} PNSLR_Internal_ThreadProcPayload;

#if PNSLR_WINDOWS
    DWORD WINAPI PNSLR_Internal_WinThreadProcWrapper(LPVOID param)
    {
        PNSLR_Internal_ThreadProcPayload* payloadPtr = (PNSLR_Internal_ThreadProcPayload*) param;
        PNSLR_Internal_ThreadProcPayload  payload = *payloadPtr;
        PNSLR_Delete(payloadPtr, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

        PNSLR_SetCurrentThreadName(payload.threadName);
        PNSLR_FreeString(payload.threadName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        payload.procedure(payload.data);
        return 0;
    }
#elif PNSLR_UNIX
    void* PNSLR_Internal_UnixThreadProcWrapper(void* param)
    {
        PNSLR_Internal_ThreadProcPayload* payloadPtr = (PNSLR_Internal_ThreadProcPayload*) param;
        PNSLR_Internal_ThreadProcPayload  payload = *payloadPtr;
        PNSLR_Delete(payloadPtr, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

        PNSLR_SetCurrentThreadName(payload.threadName);
        PNSLR_FreeString(payload.threadName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        payload.procedure(payload.data);
        return nil;
    }
#else
    #error "Unknown platform."
#endif

PNSLR_ThreadHandle PNSLR_StartThread(PNSLR_ThreadProcedure procedure, rawptr data, utf8str name)
{
    PNSLR_Internal_ThreadProcPayload* payloadPtr = PNSLR_New(PNSLR_Internal_ThreadProcPayload, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    if (!payloadPtr) { FORCE_DBG_TRAP; return (PNSLR_ThreadHandle) {0}; }

    *payloadPtr = (PNSLR_Internal_ThreadProcPayload)
    {
        .procedure  = procedure,
        .data       = data,
        .threadName = PNSLR_CloneString(name, PNSLR_GetAllocator_DefaultHeap())
    };

    b8 failed = false;
    PNSLR_ThreadHandle handle = {0};
    #if PNSLR_WINDOWS
    {
        HANDLE threadHandle = CreateThread(
            nil,                                 // default security attributes
            0,                                   // use default stack size
            PNSLR_Internal_WinThreadProcWrapper, // thread function name
            payloadPtr,                          // argument to thread function
            0,                                   // use default creation flags
            nil                                  // returns the thread identifier
        );

        if (threadHandle == nil)
        {
            failed = true;
        }
        else
        {
            handle.handle = (u64) threadHandle;
        }
    }
    #elif PNSLR_UNIX
    {
        pthread_t thread;
        if (pthread_create(&thread, nil, PNSLR_Internal_UnixThreadProcWrapper, payloadPtr) != 0)
        {
            failed = true;
        }
        else
        {
            handle.handle = (u64) thread;
        }
    }
    #else
        #error "Unknown platform."
    #endif

    if (failed)
    {
        PNSLR_FreeString(payloadPtr->threadName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        FORCE_DBG_TRAP;
        return (PNSLR_ThreadHandle) {0};
    }

    return handle;
}

void PNSLR_JoinThread(PNSLR_ThreadHandle handle)
{
    #if PNSLR_WINDOWS
        WaitForSingleObject((HANDLE) handle.handle, INFINITE);
        CloseHandle((HANDLE) handle.handle);
    #elif PNSLR_UNIX
        pthread_join((pthread_t) handle.handle, nil);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_SleepCurrentThread(u64 milliseconds)
{
    #if PNSLR_WINDOWS
        Sleep((DWORD) milliseconds);
    #elif PNSLR_UNIX
        struct timespec req, rem;
        req.tv_sec = (time_t)(milliseconds / 1000);
        req.tv_nsec = (long)((milliseconds % 1000) * 1000000);
        while (nanosleep(&req, &rem) == -1 && errno == EINTR)
        {
            req = rem;
        }
    #else
        #error "Unknown platform."
    #endif
}

#undef PNSLR_MAX_THREAD_NAME_LEN
