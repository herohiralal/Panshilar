#ifndef PNSLR_SYNC_PRIMITIVES // ===================================================
#define PNSLR_SYNC_PRIMITIVES

#include "__Prelude.h"

/**
 * The most basic synchronization primitive.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_Mutex {
    u8 buffer[5 * 8];
}  PNSLR_Mutex;

/**
 * Creates a mutex.
 */
void PNSLR_CreateMutex(PNSLR_Mutex* mutex);

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

/**
 * A read-write mutex.
 * This is a synchronization primitive that allows multiple readers or a single writer.
 * It is useful for scenarios where reads are more frequent than writes.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_RWMutex {
    u8 buffer[7 * 8];
} PNSLR_RWMutex;

/**
 * Creates a read-write mutex.
 */
void PNSLR_CreateRWMutex(PNSLR_RWMutex* rwmutex);

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

#endif // PNSLR_SYNC_PRIMITIVES ====================================================
