#ifndef PNSLR_MAIN_H
#define PNSLR_MAIN_H

#include "Intrinsics.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus)
    #define PNSLR_ALIGNAS(x) alignas(x)
#elif defined(_MSC_VER)
    #define PNSLR_ALIGNAS(x) __declspec(align(x))
#elif defined(__clang__) || defined(__GNUC__)
    #define PNSLR_ALIGNAS(x) __attribute__((aligned(x)))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

// #######################################################################################
// Environment
// #######################################################################################

/**
 * Defines the platforms supported by the library.
 */
typedef u8 PNSLR_Platform /* use as value */;
#define PNSLR_Platform_Unknown ((PNSLR_Platform) 0)
#define PNSLR_Platform_Windows ((PNSLR_Platform) 1)
#define PNSLR_Platform_Linux ((PNSLR_Platform) 2)
#define PNSLR_Platform_OSX ((PNSLR_Platform) 3)
#define PNSLR_Platform_Android ((PNSLR_Platform) 4)
#define PNSLR_Platform_iOS ((PNSLR_Platform) 5)
#define PNSLR_Platform_PS5 ((PNSLR_Platform) 6)
#define PNSLR_Platform_XBoxSeries ((PNSLR_Platform) 7)
#define PNSLR_Platform_Switch ((PNSLR_Platform) 8)

/**
 * Defines the architectures supported by the library.
 */
typedef u8 PNSLR_Architecture /* use as value */;
#define PNSLR_Architecture_Unknown ((PNSLR_Architecture) 0)
#define PNSLR_Architecture_X64 ((PNSLR_Architecture) 1)
#define PNSLR_Architecture_ARM64 ((PNSLR_Architecture) 2)

/**
 * Get the current platform.
 */
PNSLR_Platform PNSLR_GetPlatform(void);

/**
 * Get the current architecture.
 */
PNSLR_Architecture PNSLR_GetArchitecture(void);

// #######################################################################################
// Runtime
// #######################################################################################

/**
 * Defines the source code location for debugging purposes.
 */
typedef struct PNSLR_SourceCodeLocation
{
    utf8str file;
    i32 line;
    i32 column;
    utf8str function;
} PNSLR_SourceCodeLocation;

// #######################################################################################
// Sync
// #######################################################################################

// Mutex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * The most basic synchronization primitive.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_Mutex
{
    u8 buffer[64];
} PNSLR_Mutex;

/**
 * Creates a mutex.
 */
PNSLR_Mutex PNSLR_CreateMutex(void);

/**
 * Destroys a mutex.
 */
void PNSLR_DestroyMutex(
    PNSLR_Mutex* mutex
);

/**
 * Locks a mutex.
 */
void PNSLR_LockMutex(
    PNSLR_Mutex* mutex
);

/**
 * Unlocks a mutex.
 */
void PNSLR_UnlockMutex(
    PNSLR_Mutex* mutex
);

/**
 * Tries to lock a mutex.
 * Returns true if the mutex was successfully locked, false otherwise.
 */
b8 PNSLR_TryLockMutex(
    PNSLR_Mutex* mutex
);

// Read-Write Mutex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A read-write mutex.
 * This is a synchronization primitive that allows multiple readers or a single writer.
 * It is useful for scenarios where reads are more frequent than writes.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_RWMutex
{
    u8 buffer[200];
} PNSLR_RWMutex;

/**
 * Creates a read-write mutex.
 */
PNSLR_RWMutex PNSLR_CreateRWMutex(void);

/**
 * Destroys a read-write mutex.
 */
void PNSLR_DestroyRWMutex(
    PNSLR_RWMutex* rwmutex
);

/**
 * Locks a read-write mutex for reading.
 * Multiple threads can read simultaneously.
 */
void PNSLR_LockRWMutexShared(
    PNSLR_RWMutex* rwmutex
);

/**
 * Locks a read-write mutex for writing.
 * Only one thread can write at a time, and no other threads can read while writing.
 */
void PNSLR_LockRWMutexExclusive(
    PNSLR_RWMutex* rwmutex
);

/**
 * Unlocks a read-write mutex after reading.
 * Allows other threads to read or write.
 */
void PNSLR_UnlockRWMutexShared(
    PNSLR_RWMutex* rwmutex
);

/**
 * Unlocks a read-write mutex after writing.
 * Allows other threads to read or write.
 */
void PNSLR_UnlockRWMutexExclusive(
    PNSLR_RWMutex* rwmutex
);

/**
 * Tries to lock a read-write mutex for reading.
 * Returns true if the mutex was successfully locked for reading, false otherwise.
 */
b8 PNSLR_TryLockRWMutexShared(
    PNSLR_RWMutex* rwmutex
);

/**
 * Tries to lock a read-write mutex for writing.
 * Returns true if the mutex was successfully locked for writing, false otherwise.
 */
b8 PNSLR_TryLockRWMutexExclusive(
    PNSLR_RWMutex* rwmutex
);

// Semaphore ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A semaphore synchronization primitive.
 * It allows a certain number of threads to access a resource concurrently.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_Semaphore
{
    u8 buffer[32];
} PNSLR_Semaphore;

/**
 * Creates a semaphore.
 * The initial count specifies how many threads can access the resource concurrently.
 */
PNSLR_Semaphore PNSLR_CreateSemaphore(
    i32 initialCount
);

/**
 * Destroys a semaphore.
 */
void PNSLR_DestroySemaphore(
    PNSLR_Semaphore* semaphore
);

/**
 * Waits on a semaphore.
 * The calling thread will block until the semaphore count is greater than zero.
 */
void PNSLR_WaitSemaphore(
    PNSLR_Semaphore* semaphore
);

/**
 * Waits on a semaphore with a timeout.
 * The calling thread will block until the semaphore count is greater than zero or the timeout expires.
 * Returns true if the semaphore was acquired, false if the timeout expired.
 */
b8 PNSLR_WaitSemaphoreTimeout(
    PNSLR_Semaphore* semaphore,
    i32 timeoutNs
);

/**
 * Signals a semaphore, incrementing its count by a specified amount.
 * If the count was zero, this will wake up one or more waiting threads.
 */
void PNSLR_SignalSemaphore(
    PNSLR_Semaphore* semaphore,
    i32 count
);

// Condition Variable ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A condition variable for signaling between threads.
 * It allows threads to wait for a condition to be signaled.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_ConditionVariable
{
    u8 buffer[48];
} PNSLR_ConditionVariable;

/**
 * Creates a condition variable.
 */
PNSLR_ConditionVariable PNSLR_CreateConditionVariable(void);

/**
 * Destroys a condition variable.
 */
void PNSLR_DestroyConditionVariable(
    PNSLR_ConditionVariable* condvar
);

/**
 * Waits on a condition variable.
 * The calling thread will block until the condition variable is signaled.
 * The mutex must be locked before calling this function.
 */
void PNSLR_WaitConditionVariable(
    PNSLR_ConditionVariable* condvar,
    PNSLR_Mutex* mutex
);

/**
 * Waits on a condition variable with a timeout.
 * The calling thread will block until the condition variable is signaled or the timeout expires.
 * The mutex must be locked before calling this function.
 * Returns true if the condition variable was signaled, false if the timeout expired.
 */
b8 PNSLR_WaitConditionVariableTimeout(
    PNSLR_ConditionVariable* condvar,
    PNSLR_Mutex* mutex,
    i32 timeoutNs
);

/**
 * Signals a condition variable, waking up one waiting thread.
 * If no threads are waiting, this has no effect.
 */
void PNSLR_SignalConditionVariable(
    PNSLR_ConditionVariable* condvar
);

/**
 * Signals a condition variable, waking up all waiting threads.
 * If no threads are waiting, this has no effect.
 */
void PNSLR_BroadcastConditionVariable(
    PNSLR_ConditionVariable* condvar
);

// #######################################################################################
// Memory
// #######################################################################################

/**
 * Set a block of memory to a specific value.
 */
void PNSLR_MemSet(
    rawptr memory,
    i32 value,
    i32 size
);

/**
 * Copy a block of memory from source to destination.
 */
void PNSLR_MemCopy(
    rawptr destination,
    rawptr source,
    i32 size
);

/**
 * Copy a block of memory from source to destination, handling overlapping regions.
 */
void PNSLR_MemMove(
    rawptr destination,
    rawptr source,
    i32 size
);

// #######################################################################################
// Allocators
// #######################################################################################

// Allocator Declaration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Defines the mode to be used when calling the allocator function.
 */
typedef u8 PNSLR_AllocatorMode /* use as value */;
#define PNSLR_AllocatorMode_Allocate ((PNSLR_AllocatorMode) 0)
#define PNSLR_AllocatorMode_Resize ((PNSLR_AllocatorMode) 1)
#define PNSLR_AllocatorMode_Free ((PNSLR_AllocatorMode) 2)
#define PNSLR_AllocatorMode_FreeAll ((PNSLR_AllocatorMode) 3)
#define PNSLR_AllocatorMode_AllocateNoZero ((PNSLR_AllocatorMode) 4)
#define PNSLR_AllocatorMode_ResizeNoZero ((PNSLR_AllocatorMode) 5)
#define PNSLR_AllocatorMode_QueryCapabilities ((PNSLR_AllocatorMode) 255)

/**
 * Defines the capabilities of an allocator.
 */
typedef u64 PNSLR_AllocatorCapability /* use as flags */;
#define PNSLR_AllocatorCapability_None ((PNSLR_AllocatorCapability) 0)
#define PNSLR_AllocatorCapability_ThreadSafe ((PNSLR_AllocatorCapability) 1)
#define PNSLR_AllocatorCapability_Resize ((PNSLR_AllocatorCapability) 2)
#define PNSLR_AllocatorCapability_Free ((PNSLR_AllocatorCapability) 4)
#define PNSLR_AllocatorCapability_FreeAll ((PNSLR_AllocatorCapability) 8)
#define PNSLR_AllocatorCapability_HintNil ((PNSLR_AllocatorCapability) 67108864)
#define PNSLR_AllocatorCapability_HintBump ((PNSLR_AllocatorCapability) 134217728)
#define PNSLR_AllocatorCapability_HintHeap ((PNSLR_AllocatorCapability) 268435456)
#define PNSLR_AllocatorCapability_HintTemp ((PNSLR_AllocatorCapability) 536870912)
#define PNSLR_AllocatorCapability_HintDebug ((PNSLR_AllocatorCapability) 1073741824)

/**
 * Defines the error codes that can be returned by the allocator.
 */
typedef u8 PNSLR_AllocatorError /* use as value */;
#define PNSLR_AllocatorError_None ((PNSLR_AllocatorError) 0)
#define PNSLR_AllocatorError_OutOfMemory ((PNSLR_AllocatorError) 1)
#define PNSLR_AllocatorError_InvalidAlignment ((PNSLR_AllocatorError) 2)
#define PNSLR_AllocatorError_InvalidSize ((PNSLR_AllocatorError) 3)
#define PNSLR_AllocatorError_InvalidMode ((PNSLR_AllocatorError) 4)
#define PNSLR_AllocatorError_Internal ((PNSLR_AllocatorError) 5)
#define PNSLR_AllocatorError_OutOfOrderFree ((PNSLR_AllocatorError) 6)
#define PNSLR_AllocatorError_DoubleFree ((PNSLR_AllocatorError) 7)
#define PNSLR_AllocatorError_CantFreeAll ((PNSLR_AllocatorError) 8)

/**
 * Defines the delegate type for the allocator function.
 */
typedef rawptr (*PNSLR_AllocatorProcedure)(
    rawptr allocatorData,
    PNSLR_AllocatorMode mode,
    i32 size,
    i32 alignment,
    rawptr oldMemory,
    i32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
 */
typedef struct PNSLR_Allocator
{
    PNSLR_AllocatorProcedure procedure;
    rawptr data;
} PNSLR_Allocator;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_Allocator);

// Allocation ease-of-use functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Allocate memory using the provided allocator.
 */
rawptr PNSLR_Allocate(
    PNSLR_Allocator allocator,
    b8 zeroed,
    i32 size,
    i32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Resize memory using the provided allocator.
 */
rawptr PNSLR_Resize(
    PNSLR_Allocator allocator,
    b8 zeroed,
    rawptr oldMemory,
    i32 oldSize,
    i32 newSize,
    i32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Fallback resize function that can be used when the allocator does not support resizing.
 */
rawptr PNSLR_DefaultResize(
    PNSLR_Allocator allocator,
    b8 zeroed,
    rawptr oldMemory,
    i32 oldSize,
    i32 newSize,
    i32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(
    PNSLR_Allocator allocator,
    rawptr memory,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free all memory allocated by the provided allocator.
 */
void PNSLR_FreeAll(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Query the capabilities of the provided allocator.
 */
u64 PNSLR_QueryAllocatorCapabilities(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Nil allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
 * Otherwise no-ops all around.
 */
PNSLR_Allocator PNSLR_GetAllocator_Nil(void);

// Default Heap Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Get the default heap allocator.
 */
PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap(void);

/**
 * Main allocator function for the default heap allocator.
 */
rawptr PNSLR_AllocatorFn_DefaultHeap(
    rawptr allocatorData,
    PNSLR_AllocatorMode mode,
    i32 size,
    i32 alignment,
    rawptr oldMemory,
    i32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Arena Alloator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A block of memory used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorBlock
{
    struct PNSLR_ArenaAllocatorBlock* previous;
    PNSLR_Allocator allocator;
    rawptr memory;
    u32 capacity;
    u32 used;
} PNSLR_ArenaAllocatorBlock;

/**
 * The payload used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorPayload
{
    PNSLR_Allocator backingAllocator;
    PNSLR_ArenaAllocatorBlock* currentBlock;
    u32 totalUsed;
    u32 totalCapacity;
    u32 minimumBlockSize;
    u32 numSnapshots;
} PNSLR_ArenaAllocatorPayload;

/**
 * Create a new arena allocator with the specified backing allocator.
 * The arena allocator will use the backing allocator to allocate its blocks.
 * The arena allocator will not free the backing allocator, so it is the caller's responsibility to
 * free the backing allocator when it is no longer needed.
 */
PNSLR_Allocator PNSLR_NewAllocator_Arena(
    PNSLR_Allocator backingAllocator,
    u32 pageSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Destroy an arena allocator and free all its resources.
 * This does not free the backing allocator, only the arena allocator's own resources.
 */
void PNSLR_DestroyAllocator_Arena(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Main allocator function for the arena allocator.
 */
rawptr PNSLR_AllocatorFn_Arena(
    rawptr allocatorData,
    PNSLR_AllocatorMode mode,
    i32 size,
    i32 alignment,
    rawptr oldMemory,
    i32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

typedef u8 PNSLR_ArenaSnapshotError /* use as value */;
#define PNSLR_ArenaSnapshotError_None ((PNSLR_ArenaSnapshotError) 0)
#define PNSLR_ArenaSnapshotError_InvalidData ((PNSLR_ArenaSnapshotError) 1)
#define PNSLR_ArenaSnapshotError_MemoryBlockNotOwned ((PNSLR_ArenaSnapshotError) 2)
#define PNSLR_ArenaSnapshotError_OutOfOrderRestoreUsage ((PNSLR_ArenaSnapshotError) 3)
#define PNSLR_ArenaSnapshotError_DoubleRestoreOrDiscardUsage ((PNSLR_ArenaSnapshotError) 4)

/**
 * A snapshot of the arena allocator, recording its state at a specific point in time.
 * Can be saved/loaded/discarded as needed.
 */
typedef struct PNSLR_ArenaAllocatorSnapshot
{
    b8 valid;
    PNSLR_ArenaAllocatorPayload* payload;
    PNSLR_ArenaAllocatorBlock* block;
    u32 used;
} PNSLR_ArenaAllocatorSnapshot;

/**
 * Ensures that the arena allocator has either restored/discarded all the
 * snapshots that were taken.
 */
b8 PNSLR_ValidateArenaAllocatorSnapshotState(
    PNSLR_Allocator allocator
);

/**
 * Captures a snapshot of the arena allocator.
 * The returned value can be used to load back the existing state at this point.
 */
PNSLR_ArenaAllocatorSnapshot PNSLR_CaptureArenaAllocatorSnapshot(
    PNSLR_Allocator allocator
);

/**
 * Restores the state of the arena allocator from a snapshot.
 * Upon success, the snapshot is marked as invalid.
 */
PNSLR_ArenaSnapshotError PNSLR_RestoreArenaAllocatorSnapshot(
    PNSLR_ArenaAllocatorSnapshot* snapshot,
    PNSLR_SourceCodeLocation loc
);

/**
 * Discards a snapshot of the arena allocator.
 */
PNSLR_ArenaSnapshotError PNSLR_DiscardArenaAllocatorSnapshot(
    PNSLR_ArenaAllocatorSnapshot* snapshot
);

// Stack Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A page of a stack allocator.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_StackAllocatorPage
{
    struct PNSLR_StackAllocatorPage* previousPage;
    u64 usedBytes;
    u8 buffer[8192];
} PNSLR_StackAllocatorPage;

/**
 * The header used for every separate stack allocation.
 * This is used to store metadata about the allocation and deallocate appropriately.
 */
typedef struct PNSLR_StackAllocationHeader
{
    PNSLR_StackAllocatorPage* page;
    i32 size;
    i32 alignment;
    rawptr lastAllocation;
    rawptr lastAllocationHeader;
} PNSLR_StackAllocationHeader;

/**
 * The payload used by the stack allocator.
 */
typedef struct PNSLR_StackAllocatorPayload
{
    PNSLR_Allocator backingAllocator;
    PNSLR_StackAllocatorPage* currentPage;
    rawptr lastAllocation;
    PNSLR_StackAllocationHeader* lastAllocationHeader;
} PNSLR_StackAllocatorPayload;

/**
 * Create a stack allocator with the specified backing allocator.
 * The stack allocator will use the backing allocator to allocate its pages.
 * The stack allocator will not free the backing allocator, so it is the caller's responsibility to
 * free the backing allocator when it is no longer needed.
 */
PNSLR_Allocator PNSLR_NewAllocator_Stack(
    PNSLR_Allocator backingAllocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Destroy a stack allocator and free all its resources.
 * This does not free the backing allocator, only the stack allocator's own resources.
 */
void PNSLR_DestroyAllocator_Stack(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Main allocator function for the stack allocator.
 */
rawptr PNSLR_AllocatorFn_Stack(
    rawptr allocatorData,
    PNSLR_AllocatorMode mode,
    i32 size,
    i32 alignment,
    rawptr oldMemory,
    i32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Collections make/free functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Allocate a raw array slice of 'count' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
 */
PNSLR_RawArraySlice PNSLR_MakeRawSlice(
    i32 tySize,
    i32 tyAlign,
    i64 count,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free a raw array slice allocated with `PNSLR_MakeRawSlice`, using the provided allocator.
 */
void PNSLR_FreeRawSlice(
    PNSLR_RawArraySlice* slice,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Resize a raw array slice to one with 'newCount' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
 */
void PNSLR_ResizeRawSlice(
    PNSLR_RawArraySlice* slice,
    i32 tySize,
    i32 tyAlign,
    i64 newCount,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
 */
utf8str PNSLR_MakeString(
    i64 count,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
 */
void PNSLR_FreeString(
    utf8str str,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
 */
cstring PNSLR_MakeCString(
    i64 count,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
 */
void PNSLR_FreeCString(
    cstring str,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// #######################################################################################
// Chrono
// #######################################################################################

/**
 * Returns the current time in nanoseconds since the Unix epoch (January 1, 1970).
 */
i64 PNSLR_NanosecondsSinceUnixEpoch(void);

// #######################################################################################
// Strings
// #######################################################################################

// Basics/Conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
i32 PNSLR_GetCStringLength(
    cstring str
);

/**
 * Clone a C-style string into a new allocated string.
 */
utf8str PNSLR_StringFromCString(
    cstring str
);

/**
 * Clones a UTF-8 string to a C-style null-terminated string.
 * The returned string is allocated using the specified allocator.
 */
cstring PNSLR_CStringFromString(
    utf8str str,
    PNSLR_Allocator allocator
);

/**
 * Clones a UTF-8 string to a new allocated UTF-8 string.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_CloneString(
    utf8str str,
    PNSLR_Allocator allocator
);

// Basic/Manipulation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Concatenates two UTF-8 strings into a new allocated string.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_ConcatenateStrings(
    utf8str str1,
    utf8str str2,
    PNSLR_Allocator allocator
);

// Casing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Converts a UTF-8 string to uppercase.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_UpperString(
    utf8str str,
    PNSLR_Allocator allocator
);

/**
 * Converts a UTF-8 string to lowercase.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_LowerString(
    utf8str str,
    PNSLR_Allocator allocator
);

// Comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Represents the type of string comparison to perform.
 */
typedef u8 PNSLR_StringComparisonType /* use as value */;
#define PNSLR_StringComparisonType_CaseSensitive ((PNSLR_StringComparisonType) 0)
#define PNSLR_StringComparisonType_CaseInsensitive ((PNSLR_StringComparisonType) 1)

/**
 * Checks if two UTF-8 strings contain the same data.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreStringsEqual(
    utf8str str1,
    utf8str str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreStringAndCStringEqual(
    utf8str str1,
    cstring str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if two C-style null-terminated strings are equal.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreCStringsEqual(
    cstring str1,
    cstring str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a UTF-8 string starts with the specified prefix.
 * Returns true if it does, false otherwise.
 */
b8 PNSLR_StringStartsWith(
    utf8str str,
    utf8str prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a UTF-8 string ends with the specified suffix.
 * Returns true if it does, false otherwise.
 */
b8 PNSLR_StringEndsWith(
    utf8str str,
    utf8str suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string starts with the specified prefix.
 */
b8 PNSLR_StringStartsWithCString(
    utf8str str,
    cstring prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified suffix.
 */
b8 PNSLR_StringEndsWithCString(
    utf8str str,
    cstring suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
b8 PNSLR_CStringStartsWith(
    cstring str,
    utf8str prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
b8 PNSLR_CStringEndsWith(
    cstring str,
    utf8str suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
 */
b8 PNSLR_CStringStartsWithCString(
    utf8str str,
    cstring prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
b8 PNSLR_CStringEndsWithCString(
    utf8str str,
    cstring suffix,
    PNSLR_StringComparisonType comparisonType
);

// Advanced comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Searches for the first occurrence of a substring within a string.
 * Returns the index of the first occurrence, or -1 if not found.
 */
i32 PNSLR_SearchFirstIndexInString(
    utf8str str,
    utf8str substring,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Searches for the last occurrence of a substring within a string.
 * Returns the index of the last occurrence, or -1 if not found.
 */
i32 PNSLR_SearchLastIndexInString(
    utf8str str,
    utf8str substring,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Replaces all occurrences of a substring within a string with a new value.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_ReplaceInString(
    utf8str str,
    utf8str oldValue,
    utf8str newValue,
    PNSLR_Allocator allocator,
    PNSLR_StringComparisonType comparisonType
);

// UTF-8 functionalities ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Result structure for UTF-8 rune encoding.
 * Contains the encoded bytes and the number of bytes used.
 */
typedef struct PNSLR_EncodedRune
{
    u8 data[4];
    i32 length;
} PNSLR_EncodedRune;

/**
 * Result structure for UTF-8 rune decoding.
 * Contains the decoded rune and the number of bytes consumed.
 */
typedef struct PNSLR_DecodedRune
{
    u32 rune;
    i32 length;
} PNSLR_DecodedRune;

/**
 * Returns the number of bytes required to encode the given rune in UTF-8.
 */
i32 PNSLR_GetRuneLength(
    u32 r
);

/**
 * Encodes a rune into UTF-8 byte sequence and returns the structure containing encoded bytes/length.
 * Invalid runes or surrogates are replaced with the error rune (U+FFFD).
 */
PNSLR_EncodedRune PNSLR_EncodeRune(
    u32 c
);

/**
 * Decodes a UTF-8 byte sequence into a rune and returns the structure containing the rune/length.
 * Returns error rune (U+FFFD) for invalid sequences.
 */
PNSLR_DecodedRune PNSLR_DecodeRune(
    PNSLR_ArraySlice(u8) s
);

// Windows-specific bs for UTF-16 conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Converts a UTF-8 string to a UTF-16 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
PNSLR_ArraySlice(u16) PNSLR_UTF16FromUTF8WindowsOnly(
    utf8str str,
    PNSLR_Allocator allocator
);

/**
 * Converts a UTF-16 string to a UTF-8 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
utf8str PNSLR_UTF8FromUTF16WindowsOnly(
    PNSLR_ArraySlice(u16) utf16str,
    PNSLR_Allocator allocator
);

// String Builder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A basic string builder. Can accept strings and characters,
 * and build a single string from them.
 *
 * Create by setting the allocator and zeroing the rest of the fields.
 */
typedef struct PNSLR_StringBuilder
{
    PNSLR_Allocator allocator;
    PNSLR_ArraySlice(u8) buffer;
    i64 length;
} PNSLR_StringBuilder;

/**
 * Append a single byte to the string builder. Could be an ANSI/ASCII character,
 * or not. The function does not check for validity.
 */
b8 PNSLR_AppendByteToStringBuilder(
    PNSLR_StringBuilder* builder,
    u8 byte
);

/**
 * Append a UTF-8 string to the string builder.
 */
b8 PNSLR_AppendStringToStringBuilder(
    PNSLR_StringBuilder* builder,
    utf8str str
);

/**
 * Append a C-style null-terminated string to the string builder.
 */
b8 PNSLR_AppendCStringToStringBuilder(
    PNSLR_StringBuilder* builder,
    cstring str
);

/**
 * Append a single character (rune) to the string builder.
 */
b8 PNSLR_AppendRuneToStringBuilder(
    PNSLR_StringBuilder* builder,
    u32 rune
);

/**
 * Append a boolean value to the string builder.
 */
b8 PNSLR_AppendBooleanToStringBuilder(
    PNSLR_StringBuilder* builder,
    b8 value
);

/**
 * Append a 32-bit floating-point number to the string builder.
 */
b8 PNSLR_AppendF32ToStringBuilder(
    PNSLR_StringBuilder* builder,
    f32 value,
    i32 decimalPlaces
);

/**
 * Append a 64-bit floating point number to the string builder.
 */
b8 PNSLR_AppendF64ToStringBuilder(
    PNSLR_StringBuilder* builder,
    f64 value,
    i32 decimalPlaces
);

/**
 * The base to use when appending integer numbers to the string builder.
 */
typedef u8 PNSLR_IntegerBase /* use as value */;
#define PNSLR_IntegerBase_Decimal ((PNSLR_IntegerBase) 0)
#define PNSLR_IntegerBase_Binary ((PNSLR_IntegerBase) 1)
#define PNSLR_IntegerBase_HexaDecimal ((PNSLR_IntegerBase) 2)
#define PNSLR_IntegerBase_Octal ((PNSLR_IntegerBase) 3)

/**
 * Append an unsigned 8-bit integer to the string builder.
 */
b8 PNSLR_AppendU8ToStringBuilder(
    PNSLR_StringBuilder* builder,
    u8 value,
    PNSLR_IntegerBase base
);

/**
 * Append an unsigned 16-bit integer to the string builder.
 */
b8 PNSLR_AppendU16ToStringBuilder(
    PNSLR_StringBuilder* builder,
    u16 value,
    PNSLR_IntegerBase base
);

/**
 * Append an unsigned 32-bit integer number to the string builder.
 */
b8 PNSLR_AppendU32ToStringBuilder(
    PNSLR_StringBuilder* builder,
    u32 value,
    PNSLR_IntegerBase base
);

/**
 * Append an unsigned 64-bit integer to the string builder.
 */
b8 PNSLR_AppendU64ToStringBuilder(
    PNSLR_StringBuilder* builder,
    u64 value,
    PNSLR_IntegerBase base
);

/**
 * Append a signed 8-bit integer to the string builder.
 */
b8 PNSLR_AppendI8ToStringBuilder(
    PNSLR_StringBuilder* builder,
    i8 value,
    PNSLR_IntegerBase base
);

/**
 * Append a signed 16-bit integer to the string builder.
 */
b8 PNSLR_AppendI16ToStringBuilder(
    PNSLR_StringBuilder* builder,
    i16 value,
    PNSLR_IntegerBase base
);

/**
 * Append a signed 32-bit integer number to the string builder.
 */
b8 PNSLR_AppendI32ToStringBuilder(
    PNSLR_StringBuilder* builder,
    i32 value,
    PNSLR_IntegerBase base
);

/**
 * Append a signed 64-bit integer to the string builder.
 */
b8 PNSLR_AppendI64ToStringBuilder(
    PNSLR_StringBuilder* builder,
    i64 value,
    PNSLR_IntegerBase base
);

/**
 * Return the string from the string builder.
 */
utf8str PNSLR_StringFromStringBuilder(
    PNSLR_StringBuilder* builder
);

/**
 * Reset the string builder, clearing its contents but keeping the allocated buffer.
 */
void PNSLR_ResetStringBuilder(
    PNSLR_StringBuilder* builder
);

/**
 * Free the resources used by the string builder.
 */
void PNSLR_FreeStringBuilder(
    PNSLR_StringBuilder* builder
);

// Conversions to strings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Convert a boolean value to a string ("true" or "false").
 */
utf8str PNSLR_StringFromBoolean(
    b8 value,
    PNSLR_Allocator allocator
);

/**
 * Convert a 32-bit floating-point number to a string with specified decimal places.
 */
utf8str PNSLR_StringFromF32(
    f32 value,
    i32 decimalPlaces,
    PNSLR_Allocator allocator
);

/**
 * Convert a 64-bit floating-point number to a string with specified decimal places.
 */
utf8str PNSLR_StringFromF64(
    f64 value,
    i32 decimalPlaces,
    PNSLR_Allocator allocator
);

/**
 * Convert an unsigned 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU8(
    u8 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert an unsigned 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU16(
    u16 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert an unsigned 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU32(
    u32 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert an unsigned 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU64(
    u64 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert a signed 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI8(
    i8 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert a signed 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI16(
    i16 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert a signed 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI32(
    i32 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

/**
 * Convert a signed 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI64(
    i64 value,
    PNSLR_IntegerBase base,
    PNSLR_Allocator allocator
);

// Conversions from strings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Convert a validstring (case-insensitive "true" or "false", or "1" or "0") to a boolean.
 */
b8 PNSLR_BooleanFromString(
    utf8str str,
    b8* value
);

/**
 * Convert a valid string (numbers-only, with zero or one decimal points,
 * optional -/+ sign at the start) to a 32-bit floating-point number.
 */
b8 PNSLR_F32FromString(
    utf8str str,
    f32* value
);

/**
 * Convert a valid string (numbers-only, with zero or one decimal points,
 * optional -/+ sign at the start) to a 64-bit floating-point number.
 */
b8 PNSLR_F64FromString(
    utf8str str,
    f64* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 8-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U8FromString(
    utf8str str,
    u8* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 16-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U16FromString(
    utf8str str,
    u16* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 32-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U32FromString(
    utf8str str,
    u32* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 64-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U64FromString(
    utf8str str,
    u64* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 8-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I8FromString(
    utf8str str,
    i8* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 16-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I16FromString(
    utf8str str,
    i16* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 32-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I32FromString(
    utf8str str,
    i32* value
);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 64-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I64FromString(
    utf8str str,
    i64* value
);

// #######################################################################################
// IO
// #######################################################################################

/**
 * Represents a normalised path.
 * This is used to ensure that paths are in a consistent format across different platforms.
 * It is a simple wrapper around a UTF-8 string.
 * It's always an absolute path, and the path separators are always forward slashes ('/').
 * For directories, it always ends with a trailing slash.
 */
typedef struct PNSLR_Path
{
    utf8str path;
} PNSLR_Path;

/**
 * Represents the type of path normalisation to perform.
 */
typedef u8 PNSLR_PathNormalisationType /* use as value */;
#define PNSLR_PathNormalisationType_File ((PNSLR_PathNormalisationType) 0)
#define PNSLR_PathNormalisationType_Directory ((PNSLR_PathNormalisationType) 1)

/**
 * Normalises a path to a consistent format, as specified in `PNSLR_PathNormalisationType`.
 */
PNSLR_Path PNSLR_NormalisePath(
    utf8str path,
    PNSLR_PathNormalisationType type,
    PNSLR_Allocator allocator
);

/**
 * Split a path into its components. Assumes that the path is a valid normalised path in Panshilar conventions.
 */
b8 PNSLR_SplitPath(
    PNSLR_Path path,
    PNSLR_Path* parent,
    utf8str* selfNameWithExtension,
    utf8str* selfName,
    utf8str* extension
);

/**
 * Returns a normalised path for a file inside a given directory.
 */
PNSLR_Path PNSLR_GetPathForChildFile(
    PNSLR_Path dir,
    utf8str fileNameWithExtension,
    PNSLR_Allocator allocator
);

/**
 * Returns a normalised path for a subdirectory inside a given directory.
 */
PNSLR_Path PNSLR_GetPathForSubdirectory(
    PNSLR_Path dir,
    utf8str dirName,
    PNSLR_Allocator allocator
);

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
typedef b8 (*PNSLR_DirectoryIterationVisitorDelegate)(
    rawptr payload,
    PNSLR_Path path,
    b8 isDirectory,
    b8* exploreCurrentDirectory
);

/**
 * Iterates over a directory and calls the visitor function for each file/directory found.
 * If `recursive` is true, it will also iterate over subdirectories.
 */
void PNSLR_IterateDirectory(
    PNSLR_Path path,
    b8 recursive,
    rawptr visitorPayload,
    PNSLR_DirectoryIterationVisitorDelegate visitorFunc
);

/**
 * Represents the type of path check to perform when checking if a path exists.
 */
typedef u8 PNSLR_PathExistsCheckType /* use as value */;
#define PNSLR_PathExistsCheckType_Either ((PNSLR_PathExistsCheckType) 0)
#define PNSLR_PathExistsCheckType_File ((PNSLR_PathExistsCheckType) 1)
#define PNSLR_PathExistsCheckType_Directory ((PNSLR_PathExistsCheckType) 2)

/**
 * Checks if a file/directory exists at the specified path.
 */
b8 PNSLR_PathExists(
    PNSLR_Path path,
    PNSLR_PathExistsCheckType type
);

/**
 * Deletes a file/directory at a path, if it exists.
 */
b8 PNSLR_DeletePath(
    PNSLR_Path path
);

/**
 * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
 */
i64 PNSLR_GetFileTimestamp(
    PNSLR_Path path
);

/**
 * Gets the size of a file at the specified path in bytes.
 */
i64 PNSLR_GetFileSize(
    PNSLR_Path path
);

/**
 * Creates a directory tree, if it doesn't exist.
 * Note that if the path doesn't have a trailing slash, it'll assume it's a file.
 * So, the last component of the path (if is a directory) will not be created.
 */
b8 PNSLR_CreateDirectoryTree(
    PNSLR_Path path
);

/**
 * Represents an opened file.
 */
typedef struct PNSLR_File
{
    rawptr handle;
} PNSLR_File;

/**
 * Opens a file for reading, optionally allow writing to the stream.
 * If the file does not exist, this function will fail.
 */
PNSLR_File PNSLR_OpenFileToRead(
    PNSLR_Path path,
    b8 allowWrite
);

/**
 * Opens a file for writing (or appending), optionally allow reading from the stream.
 * If the file does not exist, it will be created.
 */
PNSLR_File PNSLR_OpenFileToWrite(
    PNSLR_Path path,
    b8 append,
    b8 allowRead
);

/**
 * Gets the size of an opened file.
 */
i64 PNSLR_GetSizeOfFile(
    PNSLR_File handle
);

/**
 * Gets the current position in an opened file.
 * Returns -1 on error.
 */
i64 PNSLR_GetCurrentPositionInFile(
    PNSLR_File handle
);

/**
 * Seeks to a specific position in an opened file.
 * If not relative, it's absolute from the start.
 * Returns true on success, false on failure.
 */
b8 PNSLR_SeekPositionInFile(
    PNSLR_File handle,
    i64 newPos,
    b8 relative
);

/**
 * Reads data from an opened file at the current position.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromFile(
    PNSLR_File handle,
    PNSLR_ArraySlice(u8) dst
);

/**
 * Writes data to an opened file at the current position.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteToFile(
    PNSLR_File handle,
    PNSLR_ArraySlice(u8) src
);

/**
 * Truncates an opened file to a specific size.
 * Returns true on success, false on failure.
 */
b8 PNSLR_TruncateFile(
    PNSLR_File handle,
    i64 newSize
);

/**
 * Flushes any buffered data to the file.
 * Returns true on success, false on failure.
 */
b8 PNSLR_FlushFile(
    PNSLR_File handle
);

/**
 * Closes an opened file.
 */
void PNSLR_CloseFileHandle(
    PNSLR_File handle
);

/**
 * Reads a file fully end-to-end and stores in a buffer. Won't work if dst is nil.
 * Provided allocator is used for creating the buffer.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadAllContentsFromFile(
    PNSLR_Path path,
    PNSLR_ArraySlice(u8)* dst,
    PNSLR_Allocator allocator
);

/**
 * Dump a bunch of data into a file. Optionally append it instead of overwriting.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteAllContentsToFile(
    PNSLR_Path path,
    PNSLR_ArraySlice(u8) src,
    b8 append
);

/**
 * Copies a file from src to dst. If dst exists, it will be overwritten.
 * Returns true on success, false on failure.
 */
b8 PNSLR_CopyFile(
    PNSLR_Path src,
    PNSLR_Path dst
);

/**
 * Moves a file from src to dst. If dst exists, it will be overwritten.
 * Returns true on success, false on failure.
 */
b8 PNSLR_MoveFile(
    PNSLR_Path src,
    PNSLR_Path dst
);

// #######################################################################################
// Console
// #######################################################################################

/**
 * Print a message to the standard output stream.
 */
i32 PNSLR_PrintToStdOut(
    utf8str message
);

// #######################################################################################
// Process
// #######################################################################################

/**
 * Exits the current process immediately with the specified exit code.
 */
void PNSLR_ExitProcess(
    i32 exitCode
);

// #######################################################################################
// Network
// #######################################################################################

/**
 * Represents an IP address in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(u8) PNSLR_IPAddress;

/**
 * Represents a subnet mask in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(u8) PNSLR_IPMask;

/**
 * Represents an IP network, consisting of an IP address and a subnet mask.
 */
typedef struct PNSLR_IPNetwork
{
    PNSLR_IPAddress address;
    PNSLR_IPMask mask;
} PNSLR_IPNetwork;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_IPNetwork);

b8 PNSLR_GetInterfaceIPAddresses(
    PNSLR_ArraySlice(PNSLR_IPNetwork)* networks,
    PNSLR_Allocator allocator
);

#undef PNSLR_ALIGNAS

#ifdef __cplusplus
} // extern c
#endif

/** Create a utf8str from a string literal. */
#define PNSLR_StringLiteral(str) (utf8str) {.count = sizeof(str) - 1, .data = (u8*) str}

/** Get the current source code location. */
#define PNSLR_GET_LOC() (PNSLR_SourceCodeLocation) \
    { \
        .file = PNSLR_StringLiteral(__FILE__), \
        .line = __LINE__, \
        .function = PNSLR_StringLiteral(__FUNCTION__) \
    }

/** Allocate an object of type 'ty' using the provided allocator. */
#define PNSLR_New(ty, allocator, loc, error__) \
    ((ty*) PNSLR_Allocate(allocator, sizeof(ty), alignof(ty), loc, error__))

/** Delete an object allocated with `PNSLR_New`, using the provided allocator. */
#define PNSLR_Delete(obj, allocator, loc, error__) \
    do { if (obj) PNSLR_Free(allocator, obj, loc, error__); } while(0)

/** Allocate an array of 'count' elements of type 'ty' using the provided allocator. Optionally zeroed. */
#define PNSLR_MakeSlice(ty, count, zeroed, allocator, loc, error__) \
    (PNSLR_ArraySlice_##ty) {.raw = PNSLR_MakeRawSlice((i32) sizeof(ty), (i32) alignof(ty), (i64) count, zeroed, allocator, loc, error__)}

/** Free a 'slice' (passed by ptr) allocated with `PNSLR_MakeSlice`, using the provided allocator. */
#define PNSLR_FreeSlice(slice, allocator, loc, error__) \
    do { if (slice) PNSLR_FreeRawSlice(&((slice)->raw), allocator, loc, error__); } while(0)

/** Resize a 'slice' (passed by ptr) to one with 'newCount' elements of type 'ty' using the provided allocator. Optionally zeroed. */
#define PNSLR_ResizeSlice(ty, slice, newCount, zeroed, allocator, loc, error__) \
    do { if (slice) PNSLR_ResizeRawSlice(&((slice)->raw), (i32) sizeof(ty), (i32) alignof(ty), (i64) newCount, zeroed, allocator, loc, error__); } while(0)

#endif//PNSLR_MAIN_H
