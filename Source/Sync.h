#ifndef PNSLR_SYNC_PRIMITIVES_H // =================================================
#define PNSLR_SYNC_PRIMITIVES_H
#include "__Prelude.h"
EXTERN_C_BEGIN

// Mutex ===========================================================================

/**
 * The most basic synchronization primitive.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_Mutex
{
    u8 buffer[8 * PNSLR_PTR_SIZE];
}  PNSLR_Mutex;

/**
 * Creates a mutex.
 */
PNSLR_Mutex PNSLR_CreateMutex(void);

/**
 * Destroys a mutex.
 */
void PNSLR_DestroyMutex(PNSLR_Mutex* mutex);

/**
 * Locks a mutex.
 */
void PNSLR_LockMutex(PNSLR_Mutex* mutex);

/**
 * Unlocks a mutex.
 */
void PNSLR_UnlockMutex(PNSLR_Mutex* mutex);

/**
 * Tries to lock a mutex.
 * Returns true if the mutex was successfully locked, false otherwise.
 */
b8 PNSLR_TryLockMutex(PNSLR_Mutex* mutex);

// Read-Write Mutex ================================================================

/**
 * A read-write mutex.
 * This is a synchronization primitive that allows multiple readers or a single writer.
 * It is useful for scenarios where reads are more frequent than writes.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_RWMutex
{
    u8 buffer[25 * PNSLR_PTR_SIZE];
} PNSLR_RWMutex;

/**
 * Creates a read-write mutex.
 */
PNSLR_RWMutex PNSLR_CreateRWMutex(void);

/**
 * Destroys a read-write mutex.
 */
void PNSLR_DestroyRWMutex(PNSLR_RWMutex* rwmutex);

/**
 * Locks a read-write mutex for reading.
 * Multiple threads can read simultaneously.
 */
void PNSLR_LockRWMutexShared(PNSLR_RWMutex* rwmutex);

/**
 * Locks a read-write mutex for writing.
 * Only one thread can write at a time, and no other threads can read while writing.
 */
void PNSLR_LockRWMutexExclusive(PNSLR_RWMutex* rwmutex);

/**
 * Unlocks a read-write mutex after reading.
 * Allows other threads to read or write.
 */
void PNSLR_UnlockRWMutexShared(PNSLR_RWMutex* rwmutex);

/**
 * Unlocks a read-write mutex after writing.
 * Allows other threads to read or write.
 */
void PNSLR_UnlockRWMutexExclusive(PNSLR_RWMutex* rwmutex);

/**
 * Tries to lock a read-write mutex for reading.
 * Returns true if the mutex was successfully locked for reading, false otherwise.
 */
b8 PNSLR_TryLockRWMutexShared(PNSLR_RWMutex* rwmutex);

/**
 * Tries to lock a read-write mutex for writing.
 * Returns true if the mutex was successfully locked for writing, false otherwise.
 */
b8 PNSLR_TryLockRWMutexExclusive(PNSLR_RWMutex* rwmutex);

// Semaphore =======================================================================

/**
 * A semaphore synchronization primitive.
 * It allows a certain number of threads to access a resource concurrently.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_Semaphore
{
    u8 buffer[4 * PNSLR_PTR_SIZE];
} PNSLR_Semaphore;

/**
 * Creates a semaphore.
 * The initial count specifies how many threads can access the resource concurrently.
 */
PNSLR_Semaphore PNSLR_CreateSemaphore(i32 initialCount);

/**
 * Destroys a semaphore.
 */
void PNSLR_DestroySemaphore(PNSLR_Semaphore* semaphore);

/**
 * Waits on a semaphore.
 * The calling thread will block until the semaphore count is greater than zero.
 */
void PNSLR_WaitSemaphore(PNSLR_Semaphore* semaphore);

/**
 * Waits on a semaphore with a timeout.
 * The calling thread will block until the semaphore count is greater than zero or the timeout expires.
 * Returns true if the semaphore was acquired, false if the timeout expired.
 */
b8 PNSLR_WaitSemaphoreTimeout(PNSLR_Semaphore* semaphore, i32 timeoutNs);

/**
 * Signals a semaphore, incrementing its count by a specified amount.
 * If the count was zero, this will wake up one or more waiting threads.
 */
void PNSLR_SignalSemaphore(PNSLR_Semaphore* semaphore, i32 count);

// Condition Variable ==============================================================

/**
 * A condition variable for signaling between threads.
 * It allows threads to wait for a condition to be signaled.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_ConditionVariable
{
    u8 buffer[6 * PNSLR_PTR_SIZE];
} PNSLR_ConditionVariable;

/**
 * Creates a condition variable.
 */
PNSLR_ConditionVariable PNSLR_CreateConditionVariable(void);

/**
 * Destroys a condition variable.
 */
void PNSLR_DestroyConditionVariable(PNSLR_ConditionVariable* condvar);

/**
 * Waits on a condition variable.
 * The calling thread will block until the condition variable is signaled.
 * The mutex must be locked before calling this function.
 */
void PNSLR_WaitConditionVariable(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex);

/**
 * Waits on a condition variable with a timeout.
 * The calling thread will block until the condition variable is signaled or the timeout expires.
 * The mutex must be locked before calling this function.
 * Returns true if the condition variable was signaled, false if the timeout expired.
 */
b8 PNSLR_WaitConditionVariableTimeout(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex, i32 timeoutNs);

/**
 * Signals a condition variable, waking up one waiting thread.
 * If no threads are waiting, this has no effect.
 */
void PNSLR_SignalConditionVariable(PNSLR_ConditionVariable* condvar);

/**
 * Signals a condition variable, waking up all waiting threads.
 * If no threads are waiting, this has no effect.
 */
void PNSLR_BroadcastConditionVariable(PNSLR_ConditionVariable* condvar);

// Do Once =========================================================================

/**
 * A "do once" primitive.
 * It ensures that a specified initialization function is executed only once, even
 * if called from multiple threads.
 * This is useful for one-time initialization of shared resources.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_DoOnce
{
    u8 buffer[1 * PNSLR_PTR_SIZE];
} PNSLR_DoOnce;

/**
 * The callback function type for the "do once" primitive.
 */
typedef void (*PNSLR_DoOnceCallback)(void);

/*
 * Executing the specified callback function only once.
 * If multiple threads call this function simultaneously, only one will execute.
 */
void PNSLR_ExecuteDoOnce(PNSLR_DoOnce* once, PNSLR_DoOnceCallback callback);

EXTERN_C_END
#endif // PNSLR_SYNC_PRIMITIVES_H ==================================================
