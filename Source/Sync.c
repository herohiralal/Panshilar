#define PNSLR_IMPLEMENTATION
#include "Sync.h"

#if PNSLR_WINDOWS

    static_assert(sizeof  (PNSLR_Mutex) >= sizeof (CRITICAL_SECTION)      , "PNSLR_Mutex must be at least as ""large as CRITICAL_SECTION.");
    static_assert((alignof(PNSLR_Mutex) %  alignof(CRITICAL_SECTION)) == 0, "PNSLR_Mutex must be at least as aligned as CRITICAL_SECTION.");

    static_assert(sizeof  (PNSLR_RWMutex) >= sizeof (SRWLOCK)      , "PNSLR_RWMutex must be at least as ""large as SRWLOCK.");
    static_assert((alignof(PNSLR_RWMutex) %  alignof(SRWLOCK)) == 0, "PNSLR_RWMutex must be at least as aligned as SRWLOCK.");

    static_assert(sizeof  (PNSLR_Semaphore) >= sizeof (HANDLE)      , "PNSLR_Semaphore must be at least as ""large as HANDLE.");
    static_assert((alignof(PNSLR_Semaphore) %  alignof(HANDLE)) == 0, "PNSLR_Semaphore must be at least as aligned as HANDLE.");

    static_assert(sizeof  (PNSLR_ConditionVariable) >= sizeof (CONDITION_VARIABLE)      , "PNSLR_ConditionVariable must be at least as ""large as CONDITION_VARIABLE.");
    static_assert((alignof(PNSLR_ConditionVariable) %  alignof(CONDITION_VARIABLE)) == 0, "PNSLR_ConditionVariable must be at least as aligned as CONDITION_VARIABLE.");

    static_assert(sizeof  (PNSLR_DoOnce) >= sizeof (INIT_ONCE)      , "PNSLR_DoOnce must be at least as ""large as INIT_ONCE.");
    static_assert((alignof(PNSLR_DoOnce) %  alignof(INIT_ONCE)) == 0, "PNSLR_DoOnce must be at least as aligned as INIT_ONCE.");

#elif PNSLR_UNIX

    static_assert(sizeof  (PNSLR_Mutex) >= sizeof (pthread_mutex_t)      , "PNSLR_Mutex must be at least as ""large as pthread_mutex_t.");
    static_assert((alignof(PNSLR_Mutex) %  alignof(pthread_mutex_t)) == 0, "PNSLR_Mutex must be at least as aligned as pthread_mutex_t.");

    static_assert(sizeof  (PNSLR_RWMutex) >= sizeof (pthread_rwlock_t)      , "PNSLR_RWMutex must be at least as ""large as pthread_rwlock_t.");
    static_assert((alignof(PNSLR_RWMutex) %  alignof(pthread_rwlock_t)) == 0, "PNSLR_RWMutex must be at least as aligned as pthread_rwlock_t.");

    #if PNSLR_APPLE
        static_assert(sizeof  (PNSLR_Semaphore) >= sizeof (dispatch_semaphore_t)      , "PNSLR_Semaphore must be at least as ""large as dispatch_semaphore_t.");
        static_assert((alignof(PNSLR_Semaphore) %  alignof(dispatch_semaphore_t)) == 0, "PNSLR_Semaphore must be at least as aligned as dispatch_semaphore_t.");
    #else
        static_assert(sizeof  (PNSLR_Semaphore) >= sizeof (sem_t)      , "PNSLR_Semaphore must be at least as ""large as sem_t.");
        static_assert((alignof(PNSLR_Semaphore) %  alignof(sem_t)) == 0, "PNSLR_Semaphore must be at least as aligned as sem_t.");
    #endif

    static_assert(sizeof  (PNSLR_ConditionVariable) >= sizeof (pthread_cond_t)      , "PNSLR_ConditionVariable must be at least as ""large as pthread_cond_t.");
    static_assert((alignof(PNSLR_ConditionVariable) %  alignof(pthread_cond_t)) == 0, "PNSLR_ConditionVariable must be at least as aligned as pthread_cond_t.");

    static_assert(sizeof  (PNSLR_DoOnce) >= sizeof (pthread_once_t)      , "PNSLR_DoOnce must be at least as ""large as pthread_once_t.");
    static_assert((alignof(PNSLR_DoOnce) %  alignof(pthread_once_t)) == 0, "PNSLR_DoOnce must be at least as aligned as pthread_once_t.");

#else

    #error "Unsupported platform for PNSLR_Mutex."

#endif

PNSLR_Mutex PNSLR_CreateMutex(void)
{
    PNSLR_Mutex  output;
    PNSLR_Mutex* mutex = &output;

    #if PNSLR_WINDOWS
        InitializeCriticalSection((CRITICAL_SECTION*) mutex);
        SetCriticalSectionSpinCount((CRITICAL_SECTION*) mutex, 4000); // good balance between performance and responsiveness
    #elif PNSLR_UNIX
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init((pthread_mutex_t*) mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    #else
        #error "Unknown platform."
    #endif

    return output;
}

void PNSLR_DestroyMutex(PNSLR_Mutex* mutex)
{
    #if PNSLR_WINDOWS
        DeleteCriticalSection((CRITICAL_SECTION*) mutex);
    #elif PNSLR_UNIX
        pthread_mutex_destroy((pthread_mutex_t*) mutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_LockMutex(PNSLR_Mutex* mutex)
{
    #if PNSLR_WINDOWS
        EnterCriticalSection((CRITICAL_SECTION*) mutex);
    #elif PNSLR_UNIX
        pthread_mutex_lock((pthread_mutex_t*) mutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_UnlockMutex(PNSLR_Mutex* mutex)
{
    #if PNSLR_WINDOWS
        LeaveCriticalSection((CRITICAL_SECTION*) mutex);
    #elif PNSLR_UNIX
        pthread_mutex_unlock((pthread_mutex_t*) mutex);
    #else
        #error "Unknown platform."
    #endif
}

b8 PNSLR_TryLockMutex(PNSLR_Mutex* mutex)
{
    #if PNSLR_WINDOWS
        return !!TryEnterCriticalSection((CRITICAL_SECTION*) mutex);
    #elif PNSLR_UNIX
        return 0 == pthread_mutex_trylock((pthread_mutex_t*) mutex);
    #else
        #error "Unknown platform."
    #endif
}

PNSLR_RWMutex PNSLR_CreateRWMutex(void)
{
    PNSLR_RWMutex  output;
    PNSLR_RWMutex* rwmutex = &output;

    #if PNSLR_WINDOWS
        InitializeSRWLock((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        pthread_rwlock_init((pthread_rwlock_t*) rwmutex, nil);
    #else
        #error "Unknown platform."
    #endif

    return output;
}

void PNSLR_DestroyRWMutex(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        (void)rwmutex; // SRWLOCK has no destruction API
    #elif PNSLR_UNIX
        pthread_rwlock_destroy((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_LockRWMutexShared(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        AcquireSRWLockShared((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        pthread_rwlock_rdlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_LockRWMutexExclusive(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        AcquireSRWLockExclusive((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        pthread_rwlock_wrlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_UnlockRWMutexShared(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        ReleaseSRWLockShared((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        pthread_rwlock_unlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_UnlockRWMutexExclusive(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        ReleaseSRWLockExclusive((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        pthread_rwlock_unlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

b8 PNSLR_TryLockRWMutexShared(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        return !!TryAcquireSRWLockShared((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        return 0 == pthread_rwlock_tryrdlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

b8 PNSLR_TryLockRWMutexExclusive(PNSLR_RWMutex* rwmutex)
{
    #if PNSLR_WINDOWS
        return !!TryAcquireSRWLockExclusive((SRWLOCK*) rwmutex);
    #elif PNSLR_UNIX
        return 0 == pthread_rwlock_trywrlock((pthread_rwlock_t*) rwmutex);
    #else
        #error "Unknown platform."
    #endif
}

PNSLR_Semaphore PNSLR_CreateSemaphore(i32 initialCount)
{
    PNSLR_Semaphore  output;
    PNSLR_Semaphore* semaphore = &output;

    #if PNSLR_WINDOWS
        HANDLE tempSemaphore = CreateSemaphoreExW(nil, initialCount, I32_MAX, nil, 0, SEMAPHORE_ALL_ACCESS);
        *((HANDLE*) semaphore) = tempSemaphore;
        if (tempSemaphore == nil) {
            // Handle error
        }
    #elif PNSLR_APPLE
        dispatch_semaphore_t tempSemaphore = dispatch_semaphore_create(initialCount);
        *((dispatch_semaphore_t*) semaphore) = tempSemaphore;
        if (tempSemaphore == nil) {
            // Handle error
        }
    #elif PNSLR_UNIX
        sem_init((sem_t*) semaphore, 0, (u32) initialCount);
    #else
        #error "Unknown platform."
    #endif

    return output;
}

void PNSLR_DestroySemaphore(PNSLR_Semaphore* semaphore)
{
    #if PNSLR_WINDOWS
        if (!CloseHandle(*(HANDLE*) semaphore)) {
            // Handle error
        }
    #elif PNSLR_APPLE
        dispatch_release(*(dispatch_semaphore_t*) semaphore);
    #elif PNSLR_UNIX
        sem_destroy((sem_t*) semaphore);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_WaitSemaphore(PNSLR_Semaphore* semaphore)
{
    #if PNSLR_WINDOWS
        DWORD result = WaitForSingleObject(*(HANDLE*) semaphore, INFINITE);
        if (result != WAIT_OBJECT_0) {
            // Handle error
        }
    #elif PNSLR_APPLE
        dispatch_semaphore_wait(*(dispatch_semaphore_t*) semaphore, DISPATCH_TIME_FOREVER);
    #elif PNSLR_UNIX
        if (sem_wait((sem_t*) semaphore) != 0) {
            // Handle error
        }
    #else
        #error "Unknown platform."
    #endif
}

b8 PNSLR_WaitSemaphoreTimeout(PNSLR_Semaphore* semaphore, i32 timeoutNs)
{
    #if PNSLR_WINDOWS
        DWORD result = WaitForSingleObject(*(HANDLE*) semaphore, timeoutNs / 1000000);
        if (result == WAIT_OBJECT_0) {
            return true; // Semaphore acquired
        } else if (result == WAIT_TIMEOUT) {
            return false; // Timeout expired
        } else {
            // Handle error
            return false;
        }
    #elif PNSLR_APPLE
        dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, timeoutNs);
        if (dispatch_semaphore_wait(*(dispatch_semaphore_t*) semaphore, timeout) == 0) {
            return true; // Semaphore acquired
        } else {
            return false; // Timeout expired
        }
    #elif PNSLR_UNIX
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeoutNs / 1000000000;
        ts.tv_nsec += timeoutNs % 1000000000;
        if (sem_timedwait((sem_t*) semaphore, &ts) == 0) {
            return true; // Semaphore acquired
        } else if (errno == ETIMEDOUT) {
            return false; // Timeout expired
        } else {
            // Handle error
            return false;
        }
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_SignalSemaphore(PNSLR_Semaphore* semaphore, i32 count)
{
    #if PNSLR_WINDOWS
        LONG previousCount;
        if (!ReleaseSemaphore(*(HANDLE*) semaphore, count, &previousCount)) {
            // Handle error
        }
    #elif PNSLR_APPLE
        for (i32 i = 0; i < count; ++i) {
            dispatch_semaphore_signal(*(dispatch_semaphore_t*) semaphore);
        }
    #elif PNSLR_UNIX
        for (i32 i = 0; i < count; ++i) {
            if (sem_post((sem_t*) semaphore) != 0) {
                // Handle error
            }
        }
    #else
        #error "Unknown platform."
    #endif
}

PNSLR_ConditionVariable PNSLR_CreateConditionVariable(void)
{
    PNSLR_ConditionVariable  output;
    PNSLR_ConditionVariable* condVar = &output;

    #if PNSLR_WINDOWS
        InitializeConditionVariable((CONDITION_VARIABLE*) condVar);
    #elif PNSLR_UNIX
        pthread_cond_init((pthread_cond_t*) condVar, nil);
    #else
        #error "Unknown platform."
    #endif

    return output;
}

void PNSLR_DestroyConditionVariable(PNSLR_ConditionVariable* condVar)
{
    #if PNSLR_WINDOWS
        // CONDITION_VARIABLE has no destruction API, so nothing to do here.
    #elif PNSLR_UNIX
        pthread_cond_destroy((pthread_cond_t*) condVar);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_WaitConditionVariable(PNSLR_ConditionVariable* condVar, PNSLR_Mutex* mutex)
{
    #if PNSLR_WINDOWS
        if (!SleepConditionVariableCS((CONDITION_VARIABLE*) condVar, (CRITICAL_SECTION*) mutex, INFINITE)) {
            // Handle error
        }
    #elif PNSLR_UNIX
        if (pthread_cond_wait((pthread_cond_t*) condVar, (pthread_mutex_t*) mutex) != 0) {
            // Handle error
        }
    #else
        #error "Unknown platform."
    #endif
}

b8 PNSLR_WaitConditionVariableTimeout(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex, i32 timeoutNs)
{
    #if PNSLR_WINDOWS
        DWORD result = SleepConditionVariableCS((CONDITION_VARIABLE*) condvar, (CRITICAL_SECTION*) mutex, timeoutNs / 1000000);
        if (result) {
            return true; // Condition variable signaled
        } else if (GetLastError() == ERROR_TIMEOUT) {
            return false; // Timeout expired
        } else {
            // Handle error
            return false;
        }
    #elif PNSLR_UNIX
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeoutNs / 1000000000;
        ts.tv_nsec += timeoutNs % 1000000000;
        if (pthread_cond_timedwait((pthread_cond_t*) condvar, (pthread_mutex_t*) mutex, &ts) == 0) {
            return true; // Condition variable signaled
        } else if (errno == ETIMEDOUT) {
            return false; // Timeout expired
        } else {
            // Handle error
            return false;
        }
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_SignalConditionVariable(PNSLR_ConditionVariable* condVar)
{
    #if PNSLR_WINDOWS
        WakeConditionVariable((CONDITION_VARIABLE*) condVar);
    #elif PNSLR_UNIX
        pthread_cond_signal((pthread_cond_t*) condVar);
    #else
        #error "Unknown platform."
    #endif
}

void PNSLR_BroadcastConditionVariable(PNSLR_ConditionVariable* condVar)
{
    #if PNSLR_WINDOWS
        WakeAllConditionVariable((CONDITION_VARIABLE*) condVar);
    #elif PNSLR_UNIX
        pthread_cond_broadcast((pthread_cond_t*) condVar);
    #else
        #error "Unknown platform."
    #endif
}

#if PNSLR_WINDOWS
    static BOOL CALLBACK PNSLR_Internal_InitOnceImpl(PINIT_ONCE initOnce, PVOID parameter, PVOID* lpCtx)
    {
        PNSLR_DoOnceCallback callback = (PNSLR_DoOnceCallback) parameter;
        callback();
        return TRUE;
    }
#endif

void PNSLR_ExecuteDoOnce(PNSLR_DoOnce* once, PNSLR_DoOnceCallback callback)
{
    #if PNSLR_WINDOWS
        PINIT_ONCE_FN fn = PNSLR_Internal_InitOnceImpl;
        InitOnceExecuteOnce((PINIT_ONCE) once, fn, (PVOID) callback, nil);
    #elif PNSLR_UNIX
        pthread_once((pthread_once_t*) once, callback);
    #else
        #error "Unknown platform."
    #endif
}
