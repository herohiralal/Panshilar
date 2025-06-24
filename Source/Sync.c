#include "Sync.h"

#if PNSLR_WINDOWS

    static_assert(sizeof  (PNSLR_Mutex) >= sizeof (CRITICAL_SECTION)      , "PNSLR_Mutex must be at least as ""large as CRITICAL_SECTION.");
    static_assert((alignof(PNSLR_Mutex) %  alignof(CRITICAL_SECTION)) == 0, "PNSLR_Mutex must be at least as aligned as CRITICAL_SECTION.");

    static_assert(sizeof  (PNSLR_RWMutex) >= sizeof (SRWLOCK)      , "PNSLR_RWMutex must be at least as ""large as SRWLOCK.");
    static_assert((alignof(PNSLR_RWMutex) %  alignof(SRWLOCK)) == 0, "PNSLR_RWMutex must be at least as aligned as SRWLOCK.");

#elif PNSLR_UNIX

    static_assert(sizeof  (PNSLR_Mutex) >= sizeof (pthread_mutex_t)      , "PNSLR_Mutex must be at least as ""large as pthread_mutex_t.");
    static_assert((alignof(PNSLR_Mutex) %  alignof(pthread_mutex_t)) == 0, "PNSLR_Mutex must be at least as aligned as pthread_mutex_t.");

    static_assert(sizeof  (PNSLR_RWMutex) >= sizeof (pthread_rwlock_t)      , "PNSLR_RWMutex must be at least as ""large as pthread_rwlock_t.");
    static_assert((alignof(PNSLR_RWMutex) %  alignof(pthread_rwlock_t)) == 0, "PNSLR_RWMutex must be at least as aligned as pthread_rwlock_t.");

#else

    #error "Unsupported platform for PNSLR_Mutex."

#endif

void PNSLR_CreateMutex(PNSLR_Mutex* mutex)
{
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

void PNSLR_CreateRWMutex(PNSLR_RWMutex* rwmutex)
{
#if PNSLR_WINDOWS
    InitializeSRWLock((SRWLOCK*) rwmutex);
#elif PNSLR_UNIX
    pthread_rwlock_init((pthread_rwlock_t*) rwmutex, nil);
#else
    #error "Unknown platform."
#endif
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
    return pthread_rwlock_tryrdlock((pthread_rwlock_t*) rwmutex) == 0;
#else
    #error "Unknown platform."
#endif
}

b8 PNSLR_TryLockRWMutexExclusive(PNSLR_RWMutex* rwmutex)
{
#if PNSLR_WINDOWS
    return !!TryAcquireSRWLockExclusive((SRWLOCK*) rwmutex);
#elif PNSLR_UNIX
    return pthread_rwlock_trywrlock((pthread_rwlock_t*) rwmutex) == 0;
#else
    #error "Unknown platform."
#endif
}
