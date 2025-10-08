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

// Do Once ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A "do once" primitive.
 * It ensures that a specified initialization function is executed only once, even
 * if called from multiple threads.
 * This is useful for one-time initialization of shared resources.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_DoOnce
{
    u8 buffer[8];
} PNSLR_DoOnce;

/**
 * The callback function type for the "do once" primitive.
 */
typedef void (*PNSLR_DoOnceCallback)(void);

/*
 * Executing the specified callback function only once.
 * If multiple threads call this function simultaneously, only one will execute.
 */
void PNSLR_ExecuteDoOnce(
    PNSLR_DoOnce* once,
    PNSLR_DoOnceCallback callback
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

/**
 * Breaks down the given nanoseconds since the Unix epoch into its
 * date and time components.
 */
b8 PNSLR_ConvertNanosecondsSinceUnixEpochToDateTime(
    i64 ns,
    i16* outYear,
    u8* outMonth,
    u8* outDay,
    u8* outHour,
    u8* outMinute,
    u8* outSecond
);

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
    i64 writtenSize;
    i64 cursorPos;
} PNSLR_StringBuilder;

/**
 * Ensure that the string builder has enough space to accommodate additionalSize bytes.
 */
b8 PNSLR_ReserveSpaceInStringBuilder(
    PNSLR_StringBuilder* builder,
    i64 additionalSize
);

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
 * Append an 8-bit boolean value to the string builder.
 */
b8 PNSLR_AppendB8ToStringBuilder(
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

// String Formatting ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * The possible primitive types that can be formatted.
 */
typedef u8 PNSLR_PrimitiveFmtType /* use as value */;
#define PNSLR_PrimitiveFmtType_B8 ((PNSLR_PrimitiveFmtType) 0)
#define PNSLR_PrimitiveFmtType_F32 ((PNSLR_PrimitiveFmtType) 1)
#define PNSLR_PrimitiveFmtType_F64 ((PNSLR_PrimitiveFmtType) 2)
#define PNSLR_PrimitiveFmtType_U8 ((PNSLR_PrimitiveFmtType) 3)
#define PNSLR_PrimitiveFmtType_U16 ((PNSLR_PrimitiveFmtType) 4)
#define PNSLR_PrimitiveFmtType_U32 ((PNSLR_PrimitiveFmtType) 5)
#define PNSLR_PrimitiveFmtType_U64 ((PNSLR_PrimitiveFmtType) 6)
#define PNSLR_PrimitiveFmtType_I8 ((PNSLR_PrimitiveFmtType) 7)
#define PNSLR_PrimitiveFmtType_I16 ((PNSLR_PrimitiveFmtType) 8)
#define PNSLR_PrimitiveFmtType_I32 ((PNSLR_PrimitiveFmtType) 9)
#define PNSLR_PrimitiveFmtType_I64 ((PNSLR_PrimitiveFmtType) 10)
#define PNSLR_PrimitiveFmtType_Rune ((PNSLR_PrimitiveFmtType) 11)
#define PNSLR_PrimitiveFmtType_CString ((PNSLR_PrimitiveFmtType) 12)
#define PNSLR_PrimitiveFmtType_String ((PNSLR_PrimitiveFmtType) 13)

/**
 * The internal encoding of a type-unspecific format specifier.
 * For booleans, valueBufferA is 0 or 1.
 * For floats, valueBufferA is the float value (reinterpret as relevant),
 *     and valueBufferB is the number of decimal places (cast to i32).
 * For integers, valueBufferA is the integer value (reinterpret as relevant),
 *     and the first half of valueBufferB is the base (cast to PNSLR_IntegerBase).
 * For runes, valueBufferA is the rune value (reinterpret as u32).
 * For C-style strings, valueBufferA is the pointer to the string.
 * For UTF-8 strings, valueBufferA is the pointer to the string,
 *     and valueBufferB is the length (reinterpret as i64).
 */
typedef struct PNSLR_PrimitiveFmtOptions
{
    PNSLR_PrimitiveFmtType type;
    u64 valueBufferA;
    u64 valueBufferB;
} PNSLR_PrimitiveFmtOptions;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_PrimitiveFmtOptions);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtB8(
    b8 value
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtF32(
    f32 value,
    i32 decimalPlaces
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtF64(
    f64 value,
    i32 decimalPlaces
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU8(
    u8 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU16(
    u16 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU32(
    u32 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU64(
    u64 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI8(
    i8 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI16(
    i16 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI32(
    i32 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI64(
    i64 value,
    PNSLR_IntegerBase base
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtRune(
    u32 value
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtCString(
    cstring value
);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtString(
    utf8str value
);

/**
 * Format a string with the given format and arguments, appending the result
 * to the string builder.
 */
b8 PNSLR_FormatAndAppendToStringBuilder(
    PNSLR_StringBuilder* builder,
    utf8str fmtStr,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args
);

/**
 * Format a string with the given format and arguments, returning the result
 * as a new allocated string using the specified allocator.
 */
utf8str PNSLR_FormatString(
    utf8str fmtStr,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_Allocator allocator
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
    PNSLR_Allocator allocator,
    i32 decimalPlaces
);

/**
 * Convert a 64-bit floating-point number to a string with specified decimal places.
 */
utf8str PNSLR_StringFromF64(
    f64 value,
    PNSLR_Allocator allocator,
    i32 decimalPlaces
);

/**
 * Convert an unsigned 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU8(
    u8 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert an unsigned 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU16(
    u16 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert an unsigned 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU32(
    u32 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert an unsigned 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU64(
    u64 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert a signed 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI8(
    i8 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert a signed 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI16(
    i16 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert a signed 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI32(
    i32 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
);

/**
 * Convert a signed 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI64(
    i64 value,
    PNSLR_Allocator allocator,
    PNSLR_IntegerBase base
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
 * Returns 0 on error.
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
 * Optionally stores the number of bytes read.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromFile(
    PNSLR_File handle,
    PNSLR_ArraySlice(u8) dst,
    i64* readSize
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
 * Formats a string with the given format and arguments, writing the
 * result to the file.
 * Returns true on success, false on failure.
 */
b8 PNSLR_FormatAndWriteToFile(
    PNSLR_File handle,
    utf8str fmtStr,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args
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
// Process
// #######################################################################################

/**
 * Exits the current process immediately with the specified exit code.
 */
void PNSLR_ExitProcess(
    i32 exitCode
);

/**
 * A key-value pair representing an environment variable.
 * The `kvp` field contains the full "KEY=VALUE" string.
 * The 'key' field contains the key part.
 * The 'value' field contains the value part.
 */
typedef struct PNSLR_EnvVarKeyValuePair
{
    utf8str kvp;
    utf8str key;
    utf8str value;
} PNSLR_EnvVarKeyValuePair;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_EnvVarKeyValuePair);

/**
 * Retrieves all environment variables as an array slice of key-value pairs.
 * The returned array slice is allocated using the provided allocator.
 * The individual strings within the key-value pairs are also allocated using the same allocator.
 * For the key-value pairs, the `kvp` field contains the full "KEY=VALUE" string,
 * while the `key` and `value` fields are just 'views' into that string.
 */
PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) PNSLR_GetEnvironmentVariables(
    PNSLR_Allocator allocator
);

/**
 * Opaque handle to a pipe.
 * On Windows, this is a HANDLE.
 * On Unix-like systems, this is a file descriptor (int).
 * This is used for inter-process communication (IPC).
 */
typedef struct PNSLR_PipeHandle
{
    u64 platformHandle;
} PNSLR_PipeHandle;

/**
 * Creates a pipe and returns the read and write ends.
 * The read end is used for reading data from the pipe.
 * The write end is used for writing data to the pipe.
 */
b8 PNSLR_CreatePipe(
    PNSLR_PipeHandle* outR,
    PNSLR_PipeHandle* outW
);

/**
 * Gets the number of bytes available to read from the read end of the pipe.
 * The size is stored in `outSize`.
 *
 * Note that this function does not block, and the size may change after
 * this function returns.
 */
b8 PNSLR_GetRemainingPipeReadSize(
    PNSLR_PipeHandle pipe,
    i64* outSize
);

/**
 * Reads data from the read end of the pipe into the provided buffer.
 * The number of bytes read is stored in `readSize` if it's not null.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromPipe(
    PNSLR_PipeHandle pipe,
    PNSLR_ArraySlice(u8) dst,
    i64* readSize
);

/**
 * Writes data to the write end of the pipe from the provided buffer.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteToPipe(
    PNSLR_PipeHandle pipe,
    PNSLR_ArraySlice(u8) src
);

/**
 * Closes the pipe handle, releasing any associated resources.
 * After calling this function, the pipe handle should not be used again.
 */
b8 PNSLR_ClosePipe(
    PNSLR_PipeHandle pipe
);

/**
 * A handle to a process.
 * The `pid` field is the process ID.
 * On Windows, this is `dwProcessId`.
 * On Unix-like systems, this is the PID.
 * The `handle` field is a platform-specific handle to the process.
 * On Windows, this is a HANDLE.
 * On Unix-like systems, this is pidfd.
 */
typedef struct PNSLR_ProcessHandle
{
    i64 pid;
    u64 handle;
} PNSLR_ProcessHandle;

/**
 * Starts a new process with the specified executable and arguments.
 * Optionally, environment variables, working directory, and pipes for
 * standard output and error can be provided.
 *
 * If not provided, environment variables and working directory are inherited
 * from the current process. If provided, they must be in a 'KEY=VALUE' format.
 *
 * The pipe handles provided must be read ends for stdout and stderr respectively.
 * If null, the respective output is discarded.
 */
b8 PNSLR_RunProcess(
    PNSLR_ProcessHandle* outProcessHandle,
    PNSLR_ArraySlice(utf8str) execAndArgs,
    PNSLR_ArraySlice(utf8str) environmentVariables,
    PNSLR_Path workingDirectory,
    PNSLR_PipeHandle* stdOutPipe,
    PNSLR_PipeHandle* stdErrPipe
);

/**
 * Waits for the given process to exit and retrieves its exit code.
 *
 * Returns true if the process exited cleanly or false on failure.
 * The exit code is stored in *outExitCode if provided.
 */
b8 PNSLR_WaitForProcess(
    PNSLR_ProcessHandle* process,
    i32* outExitCode
);

/**
 * Kills the given process immediately.
 *
 * Returns true if the signal/termination request succeeded.
 */
b8 PNSLR_KillProcess(
    PNSLR_ProcessHandle* process
);

/**
 * Closes the handle associated with the process.
 * Does not terminate or wait for the process.
 */
void PNSLR_CloseProcess(
    PNSLR_ProcessHandle* process
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

// #######################################################################################
// Stream
// #######################################################################################

// Stream Declaration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Defines the mode to be used when calling the stream function.
 */
typedef u8 PNSLR_StreamMode /* use as value */;
#define PNSLR_StreamMode_GetSize ((PNSLR_StreamMode) 0)
#define PNSLR_StreamMode_GetCurrentPos ((PNSLR_StreamMode) 1)
#define PNSLR_StreamMode_SeekAbsolute ((PNSLR_StreamMode) 2)
#define PNSLR_StreamMode_SeekRelative ((PNSLR_StreamMode) 3)
#define PNSLR_StreamMode_Read ((PNSLR_StreamMode) 4)
#define PNSLR_StreamMode_Write ((PNSLR_StreamMode) 5)
#define PNSLR_StreamMode_Truncate ((PNSLR_StreamMode) 6)
#define PNSLR_StreamMode_Flush ((PNSLR_StreamMode) 7)
#define PNSLR_StreamMode_Close ((PNSLR_StreamMode) 8)

/**
 * Defines the delegate type for the stream function
 */
typedef b8 (*PNSLR_StreamProcedure)(
    rawptr streamData,
    PNSLR_StreamMode mode,
    PNSLR_ArraySlice(u8) data,
    i64 offset,
    i64* extraRet
);

/**
 * Defines a generic stream, that can be used for reading/writing data.
 */
typedef struct PNSLR_Stream
{
    PNSLR_StreamProcedure procedure;
    rawptr data;
} PNSLR_Stream;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_Stream);

// Stream ease-of-use functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Gets the size of the stream.
 * Returns 0 on error.
 */
i64 PNSLR_GetSizeOfStream(
    PNSLR_Stream stream
);

/**
 * Gets the current position in the stream.
 * Returns -1 on error.
 */
i64 PNSLR_GetCurrentPositionInStream(
    PNSLR_Stream stream
);

/**
 * Seeks to a new position in the stream.
 * If 'relative' is true, the new position is relative to the current position.
 * If 'relative' is false, the new position is absolute from the start.
 * Returns true on success, false on failure.
 */
b8 PNSLR_SeekPositionInStream(
    PNSLR_Stream stream,
    i64 newPos,
    b8 relative
);

/**
 * Reads data from the stream into the provided buffer.
 * Optionally stores the number of bytes read.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromStream(
    PNSLR_Stream stream,
    PNSLR_ArraySlice(u8) dst,
    i64* readSize
);

/**
 * Writes data from the provided buffer into the stream.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteToStream(
    PNSLR_Stream stream,
    PNSLR_ArraySlice(u8) src
);

/**
 * Formats a string and writes it to the stream.
 * Only supports primitives, for obvious reasons.
 * Use with `PNSLR_FmtB8`, `PNSLR_FmtI32`, etc.
 * Returns true on success, false on failure.
 */
b8 PNSLR_FormatAndWriteToStream(
    PNSLR_Stream stream,
    utf8str fmtStr,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args
);

/**
 * Truncates the stream to the specified size.
 * Returns true on success, false on failure.
 */
b8 PNSLR_TruncateStream(
    PNSLR_Stream stream,
    i64 newSize
);

/**
 * Flushes any buffered data to the stream.
 * Returns true on success, false on failure.
 */
b8 PNSLR_FlushStream(
    PNSLR_Stream stream
);

/**
 * Closes the stream and frees any associated resources.
 */
void PNSLR_CloseStream(
    PNSLR_Stream stream
);

// Stream casts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Creates a stream from a file handle.
 */
PNSLR_Stream PNSLR_StreamFromFile(
    PNSLR_File file
);

/**
 * Creates a stream from a string builder.
 */
PNSLR_Stream PNSLR_StreamFromStringBuilder(
    PNSLR_StringBuilder* builder
);

/**
 * Creates a stream from the standard output pipe.
 */
PNSLR_Stream PNSLR_StreamFromStdOut(
    b8 disableBuffering
);

/**
 * Creates a stream from the standard error pipe.
 */
PNSLR_Stream PNSLR_StreamFromStdErr(
    b8 disableBuffering
);

// #######################################################################################
// Logger
// #######################################################################################

// Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Defines the logging levels.
 */
typedef u8 PNSLR_LoggerLevel /* use as value */;
#define PNSLR_LoggerLevel_Debug ((PNSLR_LoggerLevel) 0)
#define PNSLR_LoggerLevel_Info ((PNSLR_LoggerLevel) 1)
#define PNSLR_LoggerLevel_Warn ((PNSLR_LoggerLevel) 2)
#define PNSLR_LoggerLevel_Error ((PNSLR_LoggerLevel) 3)
#define PNSLR_LoggerLevel_Critical ((PNSLR_LoggerLevel) 4)

/**
 * Defines options for logging output.
 */
typedef u8 PNSLR_LogOption /* use as flags */;
#define PNSLR_LogOption_None ((PNSLR_LogOption) 0)
#define PNSLR_LogOption_IncludeLevel ((PNSLR_LogOption) 1)
#define PNSLR_LogOption_IncludeDate ((PNSLR_LogOption) 2)
#define PNSLR_LogOption_IncludeTime ((PNSLR_LogOption) 4)
#define PNSLR_LogOption_IncludeFile ((PNSLR_LogOption) 8)
#define PNSLR_LogOption_IncludeFn ((PNSLR_LogOption) 16)
#define PNSLR_LogOption_IncludeColours ((PNSLR_LogOption) 32)

/**
 * Defines the delegate type for the logger function.
 */
typedef void (*PNSLR_LoggerProcedure)(
    rawptr loggerData,
    PNSLR_LoggerLevel level,
    utf8str data,
    PNSLR_LogOption options,
    PNSLR_SourceCodeLocation location
);

/**
 * Defines a generic logger structure that can be used to log messages.
 */
typedef struct PNSLR_Logger
{
    PNSLR_LoggerProcedure procedure;
    rawptr data;
    PNSLR_LoggerLevel minAllowedLvl;
    PNSLR_LogOption options;
} PNSLR_Logger;

// Default Logger Control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Sets the default logger FOR THE CURRENT THREAD.
 * By default, every thread gets a thread-safe default logger that:
 * - logs to stdout on desktop platforms
 * - logs to logcat on Android
 */
void PNSLR_SetDefaultLogger(
    PNSLR_Logger logger
);

/**
 * Disables the default logger FOR THE CURRENT THREAD.
 */
void PNSLR_DisableDefaultLogger(void);

// Default Logger Non-Format Log Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void PNSLR_LogD(
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogI(
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogW(
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogE(
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogC(
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

// Default Logger Formatted Log Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void PNSLR_LogDf(
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogIf(
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogWf(
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogEf(
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogCf(
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLD(
    PNSLR_Logger logger,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLI(
    PNSLR_Logger logger,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLW(
    PNSLR_Logger logger,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLE(
    PNSLR_Logger logger,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLC(
    PNSLR_Logger logger,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLDf(
    PNSLR_Logger logger,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLIf(
    PNSLR_Logger logger,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLWf(
    PNSLR_Logger logger,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLEf(
    PNSLR_Logger logger,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLCf(
    PNSLR_Logger logger,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

// Logger functions with explicit level ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void PNSLR_Log(
    PNSLR_LoggerLevel level,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_Logf(
    PNSLR_LoggerLevel level,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogL(
    PNSLR_Logger logger,
    PNSLR_LoggerLevel level,
    utf8str msg,
    PNSLR_SourceCodeLocation loc
);

void PNSLR_LogLf(
    PNSLR_Logger logger,
    PNSLR_LoggerLevel level,
    utf8str fmtMsg,
    PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args,
    PNSLR_SourceCodeLocation loc
);

// Logger Casts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Creates a logger that writes to the given file.
 * The file must be opened and valid.
 */
PNSLR_Logger PNSLR_LoggerFromFile(
    PNSLR_File f,
    PNSLR_LoggerLevel minAllowedLevel,
    PNSLR_LogOption options
);

/**
 * Creates a logger that uses the default outputs (see `PNSLR_SetDefaultLogger()`).
 * The returned logger is thread-safe and can be used from any thread.
 * This can be used along with `PNSLR_SetDefaultLogger()` to customize
 * the behaviour of the default in-built logger.
 */
PNSLR_Logger PNSLR_GetDefaultLoggerWithOptions(
    PNSLR_LoggerLevel minAllowedLevel,
    PNSLR_LogOption options
);

/**
 * Creates a nil logger that does nothing.
 * This can be used to disable logging in certain parts of the code.
 */
PNSLR_Logger PNSLR_GetNilLogger(void);

// #######################################################################################
// Threads
// #######################################################################################

/**
 * An opaque handle to a thread.
 */
typedef struct PNSLR_ThreadHandle
{
    u64 handle;
} PNSLR_ThreadHandle;

/**
 * Checks if the handle to a thread is valid.
 */
b8 PNSLR_IsThreadHandleValid(
    PNSLR_ThreadHandle handle
);

/**
 * Gets a handle to the current thread.
 */
PNSLR_ThreadHandle PNSLR_GetCurrentThreadHandle(void);

/**
 * Gets the name of a thread.
 * The returned string is allocated using the provided allocator.
 * If the thread has no name, an empty string is returned.
 */
utf8str PNSLR_GetThreadName(
    PNSLR_ThreadHandle handle,
    PNSLR_Allocator allocator
);

/**
 * Sets the name of a thread.
 * The name is copied, so the provided string does not need to be valid after this call.
 * On some platforms, thread names may be truncated to a certain length.
 *
 * Thread lengths on platforms (excluding null terminator):
 *     Windows/OSX/iOS - 63 characters
 *     Linux/Android   - 15 characters
 */
void PNSLR_SetThreadName(
    PNSLR_ThreadHandle handle,
    utf8str name
);

/**
 * Gets the name of the current thread.
 * Read more about `PNSLR_GetThreadName`.
 */
utf8str PNSLR_GetCurrentThreadName(
    PNSLR_Allocator allocator
);

/**
 * Sets the name of the current thread.
 * Read more about `PNSLR_SetThreadName`.
 */
void PNSLR_SetCurrentThreadName(
    utf8str name
);

/**
 * A procedure that can be run on a thread.
 * The `data` parameter is optional user data that can be passed to the thread.
 */
typedef void (*PNSLR_ThreadProcedure)(
    rawptr data
);

/**
 * Start a new thread with the specified procedure and user data.
 */
PNSLR_ThreadHandle PNSLR_StartThread(
    PNSLR_ThreadProcedure procedure,
    rawptr data,
    utf8str name
);

/**
 * Joins a thread, blocking the calling thread until the specified thread has finished.
 */
void PNSLR_JoinThread(
    PNSLR_ThreadHandle handle
);

/**
 * Sleeps the current thread for the specified number of milliseconds.
 */
void PNSLR_SleepCurrentThread(
    u64 milliseconds
);

// #######################################################################################
// SharedMemoryChannel
// #######################################################################################

// Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Opaque handle for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelHandle
{
    i64 handle;
} PNSLR_SharedMemoryChannelHandle;

/**
 * Platform-specific header for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelPlatformHeader
{
    u32 magicNum;
} PNSLR_SharedMemoryChannelPlatformHeader;

/**
 * Represents the status of a shared memory channel endpoint (reader or writer).
 */
typedef u8 PNSLR_SharedMemoryChannelStatus /* use as value */;
#define PNSLR_SharedMemoryChannelStatus_Disconnected ((PNSLR_SharedMemoryChannelStatus) 0)
#define PNSLR_SharedMemoryChannelStatus_Paused ((PNSLR_SharedMemoryChannelStatus) 1)
#define PNSLR_SharedMemoryChannelStatus_Active ((PNSLR_SharedMemoryChannelStatus) 2)

/**
 * Header for a shared memory channel, containing metadata about the channel.
 */
typedef struct PNSLR_SharedMemoryChannelHeader
{
    u32 magicNum;
    u32 version;
    PNSLR_SharedMemoryChannelStatus readerStatus;
    PNSLR_SharedMemoryChannelStatus writerStatus;
    u32 offsetToOsSpecificHeader;
    u32 offsetToMsgQueueHeader;
    u32 offsetToMsgData;
    i64 fullMemRegionSize;
    i64 dataSize;
} PNSLR_SharedMemoryChannelHeader;

/**
 * Header for the message queue within a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelMessageQueueHeader
{
    i64 readCursor;
    u8 padding[56];
    i64 writeCursor;
} PNSLR_SharedMemoryChannelMessageQueueHeader;

/**
 * Represents a reader endpoint for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelReader
{
    PNSLR_SharedMemoryChannelHeader* header;
    PNSLR_SharedMemoryChannelHandle handle;
} PNSLR_SharedMemoryChannelReader;

/**
 * Represents a writer endpoint for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelWriter
{
    PNSLR_SharedMemoryChannelHeader* header;
    PNSLR_SharedMemoryChannelHandle handle;
} PNSLR_SharedMemoryChannelWriter;

/**
 * Represents a reserved message slot for writing to a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelReservedMessage
{
    PNSLR_SharedMemoryChannelWriter* channel;
    i64 offset;
    i64 size;
    u8* writePtr;
} PNSLR_SharedMemoryChannelReservedMessage;

/**
 * Represents a message that has been read from a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelMessage
{
    PNSLR_SharedMemoryChannelReader* channel;
    i64 offset;
    i64 size;
    u8* readPtr;
    i64 readSize;
} PNSLR_SharedMemoryChannelMessage;

// Reader Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Creates a shared memory channel reader with the specified name and size.
 * The reader owns the shared memory segment and other processes can connect as writers.
 */
b8 PNSLR_CreateSharedMemoryChannelReader(
    utf8str name,
    i64 size,
    PNSLR_SharedMemoryChannelReader* reader
);

/**
 * Polls for a message from the shared memory channel.
 * Returns true if a message was found, false otherwise.
 * Sets fatalError to true if an unrecoverable error occurred.
 */
b8 PNSLR_ReadSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelReader* reader,
    PNSLR_SharedMemoryChannelMessage* message,
    b8* fatalError
);

/**
 * Acknowledges that a message has been processed and advances the read cursor.
 */
b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelMessage* message
);

/**
 * Destroys a shared memory channel reader and releases all associated resources.
 */
b8 PNSLR_DestroySharedMemoryChannelReader(
    PNSLR_SharedMemoryChannelReader* reader
);

// Writer Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Attempts to connect to an existing shared memory channel as a writer.
 * Returns true if successful, false if the channel doesn't exist or connection failed.
 */
b8 PNSLR_TryConnectSharedMemoryChannelWriter(
    utf8str name,
    PNSLR_SharedMemoryChannelWriter* writer
);

/**
 * Reserves space for a message in the shared memory channel.
 * Returns true if space was available, false otherwise.
 */
b8 PNSLR_PrepareSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelWriter* writer,
    i64 size,
    PNSLR_SharedMemoryChannelReservedMessage* reservedMessage
);

/**
 * Commits a previously reserved message to the shared memory channel.
 */
b8 PNSLR_CommitSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelWriter* writer,
    PNSLR_SharedMemoryChannelReservedMessage reservedMessage
);

/**
 * Disconnects from a shared memory channel and releases writer resources.
 */
b8 PNSLR_DisconnectSharedMemoryChannelWriter(
    PNSLR_SharedMemoryChannelWriter* writer
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

/** Easier way to pass format arguments. */
#define PNSLR_FmtArgs(...) (PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions)) \
    { \
        .data = (PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__}, \
        .count = sizeof((PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__})/sizeof(PNSLR_PrimitiveFmtOptions) \
    }

#endif//PNSLR_MAIN_H
