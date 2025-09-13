#ifndef PANSHILAR_MAIN
#define PANSHILAR_MAIN

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
    #define PNSLR_ALIGNAS(x) __declspec(align(x))
#elif defined(__clang__) || defined(__GNUC__)
    #define PNSLR_ALIGNAS(x) __attribute__((aligned(x)))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

/** An array slice of type 'ty'. */
#define PNSLR_ArraySlice(ty) PNSLR_ArraySlice_##ty

/** Declare an array slice of type 'ty'. */
#define PNSLR_DECLARE_ARRAY_SLICE(ty) \
    typedef union PNSLR_ArraySlice(ty) { struct { ty* data; PNSLR_I64 count; }; PNSLR_RawArraySlice raw; } PNSLR_ArraySlice(ty);

typedef unsigned char       PNSLR_B8;
typedef unsigned char       PNSLR_U8;
typedef unsigned short int  PNSLR_U16;
typedef unsigned int        PNSLR_U32;
typedef unsigned long long  PNSLR_U64;
typedef signed char         PNSLR_I8;
typedef signed short int    PNSLR_I16;
typedef signed int          PNSLR_I32;
typedef signed long long    PNSLR_I64;

// #######################################################################################
// Collections
// #######################################################################################

/**
 * A raw type-unspecific array slice.
 */
typedef struct PNSLR_RawArraySlice
{
    void* data;
    PNSLR_I64 count;
} PNSLR_RawArraySlice;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_B8);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_U8);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_U16);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_U32);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_U64);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_I8);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_I16);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_I32);

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_I64);

PNSLR_DECLARE_ARRAY_SLICE(float);

PNSLR_DECLARE_ARRAY_SLICE(double);

PNSLR_DECLARE_ARRAY_SLICE(char);

/**
 * UTF-8 string type, with length info (not necessarily null-terminated).
 */
typedef PNSLR_ArraySlice(PNSLR_U8) PNSLR_UTF8STR;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_UTF8STR);

// #######################################################################################
// Environment
// #######################################################################################

/**
 * Defines the platforms supported by the library.
 */
typedef PNSLR_U8 PNSLR_Platform /* use as value */;
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
typedef PNSLR_U8 PNSLR_Architecture /* use as value */;
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
    PNSLR_UTF8STR file;
    PNSLR_I32 line;
    PNSLR_I32 column;
    PNSLR_UTF8STR function;
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
    PNSLR_U8 buffer[64];
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
PNSLR_B8 PNSLR_TryLockMutex(
    PNSLR_Mutex* mutex
);

// Read-Write ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A read-write mutex.
 * This is a synchronization primitive that allows multiple readers or a single writer.
 * It is useful for scenarios where reads are more frequent than writes.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_RWMutex
{
    PNSLR_U8 buffer[200];
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
PNSLR_B8 PNSLR_TryLockRWMutexShared(
    PNSLR_RWMutex* rwmutex
);

/**
 * Tries to lock a read-write mutex for writing.
 * Returns true if the mutex was successfully locked for writing, false otherwise.
 */
PNSLR_B8 PNSLR_TryLockRWMutexExclusive(
    PNSLR_RWMutex* rwmutex
);

// Semaphore ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A semaphore synchronization primitive.
 * It allows a certain number of threads to access a resource concurrently.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_Semaphore
{
    PNSLR_U8 buffer[32];
} PNSLR_Semaphore;

/**
 * Creates a semaphore.
 * The initial count specifies how many threads can access the resource concurrently.
 */
PNSLR_Semaphore PNSLR_CreateSemaphore(
    PNSLR_I32 initialCount
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
PNSLR_B8 PNSLR_WaitSemaphoreTimeout(
    PNSLR_Semaphore* semaphore,
    PNSLR_I32 timeoutNs
);

/**
 * Signals a semaphore, incrementing its count by a specified amount.
 * If the count was zero, this will wake up one or more waiting threads.
 */
void PNSLR_SignalSemaphore(
    PNSLR_Semaphore* semaphore,
    PNSLR_I32 count
);

// Condition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A condition variable for signaling between threads.
 * It allows threads to wait for a condition to be signaled.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_ConditionVariable
{
    PNSLR_U8 buffer[48];
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
PNSLR_B8 PNSLR_WaitConditionVariableTimeout(
    PNSLR_ConditionVariable* condvar,
    PNSLR_Mutex* mutex,
    PNSLR_I32 timeoutNs
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
    void* memory,
    PNSLR_I32 value,
    PNSLR_I32 size
);

/**
 * Copy a block of memory from source to destination.
 */
void PNSLR_MemCopy(
    void* destination,
    void* source,
    PNSLR_I32 size
);

/**
 * Copy a block of memory from source to destination, handling overlapping regions.
 */
void PNSLR_MemMove(
    void* destination,
    void* source,
    PNSLR_I32 size
);

// #######################################################################################
// Allocators
// #######################################################################################

// Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Defines the mode to be used when calling the allocator function.
 */
typedef PNSLR_U8 PNSLR_AllocatorMode /* use as value */;
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
typedef PNSLR_U64 PNSLR_AllocatorCapability /* use as flags */;
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
typedef PNSLR_U8 PNSLR_AllocatorError /* use as value */;
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
typedef void* (*PNSLR_AllocatorProcedure)(
    void* allocatorData,
    PNSLR_AllocatorMode mode,
    PNSLR_I32 size,
    PNSLR_I32 alignment,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
 */
typedef struct PNSLR_Allocator
{
    PNSLR_AllocatorProcedure procedure;
    void* data;
} PNSLR_Allocator;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_Allocator);

// Allocation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Allocate memory using the provided allocator.
 */
void* PNSLR_Allocate(
    PNSLR_Allocator allocator,
    PNSLR_B8 zeroed,
    PNSLR_I32 size,
    PNSLR_I32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Resize memory using the provided allocator.
 */
void* PNSLR_Resize(
    PNSLR_Allocator allocator,
    PNSLR_B8 zeroed,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_I32 newSize,
    PNSLR_I32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Fallback resize function that can be used when the allocator does not support resizing.
 */
void* PNSLR_DefaultResize(
    PNSLR_Allocator allocator,
    PNSLR_B8 zeroed,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_I32 newSize,
    PNSLR_I32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(
    PNSLR_Allocator allocator,
    void* memory,
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
PNSLR_U64 PNSLR_QueryAllocatorCapabilities(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Nil ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
 * Otherwise no-ops all around.
 */
PNSLR_Allocator PNSLR_GetAllocator_Nil(void);

// Default ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Get the default heap allocator.
 */
PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap(void);

/**
 * Main allocator function for the default heap allocator.
 */
void* PNSLR_AllocatorFn_DefaultHeap(
    void* allocatorData,
    PNSLR_AllocatorMode mode,
    PNSLR_I32 size,
    PNSLR_I32 alignment,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Arena ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A block of memory used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorBlock
{
    struct PNSLR_ArenaAllocatorBlock* previous;
    PNSLR_Allocator allocator;
    void* memory;
    PNSLR_U32 capacity;
    PNSLR_U32 used;
} PNSLR_ArenaAllocatorBlock;

/**
 * The payload used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorPayload
{
    PNSLR_Allocator backingAllocator;
    PNSLR_ArenaAllocatorBlock* currentBlock;
    PNSLR_U32 totalUsed;
    PNSLR_U32 totalCapacity;
    PNSLR_U32 minimumBlockSize;
    PNSLR_U32 numSnapshots;
} PNSLR_ArenaAllocatorPayload;

/**
 * Create a new arena allocator with the specified backing allocator.
 * The arena allocator will use the backing allocator to allocate its blocks.
 * The arena allocator will not free the backing allocator, so it is the caller's responsibility to
 * free the backing allocator when it is no longer needed.
 */
PNSLR_Allocator PNSLR_NewAllocator_Arena(
    PNSLR_Allocator backingAllocator,
    PNSLR_U32 pageSize,
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
void* PNSLR_AllocatorFn_Arena(
    void* allocatorData,
    PNSLR_AllocatorMode mode,
    PNSLR_I32 size,
    PNSLR_I32 alignment,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

typedef PNSLR_U8 PNSLR_ArenaSnapshotError /* use as value */;
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
    PNSLR_B8 valid;
    PNSLR_ArenaAllocatorPayload* payload;
    PNSLR_ArenaAllocatorBlock* block;
    PNSLR_U32 used;
} PNSLR_ArenaAllocatorSnapshot;

/**
 * Ensures that the arena allocator has either restored/discarded all the
 * snapshots that were taken.
 */
PNSLR_B8 PNSLR_ValidateArenaAllocatorSnapshotState(
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

// Stack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A page of a stack allocator.
 */
typedef struct PNSLR_ALIGNAS(8) PNSLR_StackAllocatorPage
{
    struct PNSLR_StackAllocatorPage* previousPage;
    PNSLR_U64 usedBytes;
    PNSLR_U8 buffer[8192];
} PNSLR_StackAllocatorPage;

/**
 * The header used for every separate stack allocation.
 * This is used to store metadata about the allocation and deallocate appropriately.
 */
typedef struct PNSLR_StackAllocationHeader
{
    PNSLR_StackAllocatorPage* page;
    PNSLR_I32 size;
    PNSLR_I32 alignment;
    void* lastAllocation;
    void* lastAllocationHeader;
} PNSLR_StackAllocationHeader;

/**
 * The payload used by the stack allocator.
 */
typedef struct PNSLR_StackAllocatorPayload
{
    PNSLR_Allocator backingAllocator;
    PNSLR_StackAllocatorPage* currentPage;
    void* lastAllocation;
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
void* PNSLR_AllocatorFn_Stack(
    void* allocatorData,
    PNSLR_AllocatorMode mode,
    PNSLR_I32 size,
    PNSLR_I32 alignment,
    void* oldMemory,
    PNSLR_I32 oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

// Collections ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Allocate a raw array slice of 'count' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
 */
PNSLR_RawArraySlice PNSLR_MakeRawSlice(
    PNSLR_I32 tySize,
    PNSLR_I32 tyAlign,
    PNSLR_I64 count,
    PNSLR_B8 zeroed,
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
    PNSLR_I32 tySize,
    PNSLR_I32 tyAlign,
    PNSLR_I64 newCount,
    PNSLR_B8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
 */
PNSLR_UTF8STR PNSLR_MakeString(
    PNSLR_I64 count,
    PNSLR_B8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
 */
void PNSLR_FreeString(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
 */
char* PNSLR_MakeCString(
    PNSLR_I64 count,
    PNSLR_B8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error
);

/**
 * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
 */
void PNSLR_FreeCString(
    char* str,
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
PNSLR_I64 PNSLR_NanosecondsSinceUnixEpoch(void);

// #######################################################################################
// Strings
// #######################################################################################

// Basics/Conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
PNSLR_I32 PNSLR_GetCStringLength(
    char* str
);

/**
 * Clone a C-style string into a new allocated string.
 */
PNSLR_UTF8STR PNSLR_StringFromCString(
    char* str
);

/**
 * Clones a UTF-8 string to a C-style null-terminated string.
 * The returned string is allocated using the specified allocator.
 */
char* PNSLR_CStringFromString(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator
);

/**
 * Clones a UTF-8 string to a new allocated UTF-8 string.
 * The returned string is allocated using the specified allocator.
 */
PNSLR_UTF8STR PNSLR_CloneString(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator
);

// Basic/Manipulation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Concatenates two UTF-8 strings into a new allocated string.
 * The returned string is allocated using the specified allocator.
 */
PNSLR_UTF8STR PNSLR_ConcatenateStrings(
    PNSLR_UTF8STR str1,
    PNSLR_UTF8STR str2,
    PNSLR_Allocator allocator
);

// Casing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Converts a UTF-8 string to uppercase.
 * The returned string is allocated using the specified allocator.
 */
PNSLR_UTF8STR PNSLR_UpperString(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator
);

/**
 * Converts a UTF-8 string to lowercase.
 * The returned string is allocated using the specified allocator.
 */
PNSLR_UTF8STR PNSLR_LowerString(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator
);

// Comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Represents the type of string comparison to perform.
 */
typedef PNSLR_U8 PNSLR_StringComparisonType /* use as value */;
#define PNSLR_StringComparisonType_CaseSensitive ((PNSLR_StringComparisonType) 0)
#define PNSLR_StringComparisonType_CaseInsensitive ((PNSLR_StringComparisonType) 1)

/**
 * Checks if two UTF-8 strings contain the same data.
 * Returns true if they are equal, false otherwise.
 */
PNSLR_B8 PNSLR_AreStringsEqual(
    PNSLR_UTF8STR str1,
    PNSLR_UTF8STR str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
 * Returns true if they are equal, false otherwise.
 */
PNSLR_B8 PNSLR_AreStringAndCStringEqual(
    PNSLR_UTF8STR str1,
    char* str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if two C-style null-terminated strings are equal.
 * Returns true if they are equal, false otherwise.
 */
PNSLR_B8 PNSLR_AreCStringsEqual(
    char* str1,
    char* str2,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a UTF-8 string starts with the specified prefix.
 * Returns true if it does, false otherwise.
 */
PNSLR_B8 PNSLR_StringStartsWith(
    PNSLR_UTF8STR str,
    PNSLR_UTF8STR prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a UTF-8 string ends with the specified suffix.
 * Returns true if it does, false otherwise.
 */
PNSLR_B8 PNSLR_StringEndsWith(
    PNSLR_UTF8STR str,
    PNSLR_UTF8STR suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string starts with the specified prefix.
 */
PNSLR_B8 PNSLR_StringStartsWithCString(
    PNSLR_UTF8STR str,
    char* prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified suffix.
 */
PNSLR_B8 PNSLR_StringEndsWithCString(
    PNSLR_UTF8STR str,
    char* suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
PNSLR_B8 PNSLR_CStringStartsWith(
    char* str,
    PNSLR_UTF8STR prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
PNSLR_B8 PNSLR_CStringEndsWith(
    char* str,
    PNSLR_UTF8STR suffix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
 */
PNSLR_B8 PNSLR_CStringStartsWithCString(
    PNSLR_UTF8STR str,
    char* prefix,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
PNSLR_B8 PNSLR_CStringEndsWithCString(
    PNSLR_UTF8STR str,
    char* suffix,
    PNSLR_StringComparisonType comparisonType
);

// Advanced ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Searches for the first occurrence of a substring within a string.
 * Returns the index of the first occurrence, or -1 if not found.
 */
PNSLR_I32 PNSLR_SearchFirstIndexInString(
    PNSLR_UTF8STR str,
    PNSLR_UTF8STR substring,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Searches for the last occurrence of a substring within a string.
 * Returns the index of the last occurrence, or -1 if not found.
 */
PNSLR_I32 PNSLR_SearchLastIndexInString(
    PNSLR_UTF8STR str,
    PNSLR_UTF8STR substring,
    PNSLR_StringComparisonType comparisonType
);

/**
 * Replaces all occurrences of a substring within a string with a new value.
 * The returned string is allocated using the specified allocator.
 */
PNSLR_UTF8STR PNSLR_ReplaceInString(
    PNSLR_UTF8STR str,
    PNSLR_UTF8STR oldValue,
    PNSLR_UTF8STR newValue,
    PNSLR_Allocator allocator,
    PNSLR_StringComparisonType comparisonType
);

// UTF-8 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Result structure for UTF-8 rune encoding.
 * Contains the encoded bytes and the number of bytes used.
 */
typedef struct PNSLR_EncodedRune
{
    PNSLR_U8 data[4];
    PNSLR_I32 length;
} PNSLR_EncodedRune;

/**
 * Result structure for UTF-8 rune decoding.
 * Contains the decoded rune and the number of bytes consumed.
 */
typedef struct PNSLR_DecodedRune
{
    PNSLR_U32 rune;
    PNSLR_I32 length;
} PNSLR_DecodedRune;

/**
 * Returns the number of bytes required to encode the given rune in UTF-8.
 */
PNSLR_I32 PNSLR_GetRuneLength(
    PNSLR_U32 r
);

/**
 * Encodes a rune into UTF-8 byte sequence and returns the structure containing encoded bytes/length.
 * Invalid runes or surrogates are replaced with the error rune (U+FFFD).
 */
PNSLR_EncodedRune PNSLR_EncodeRune(
    PNSLR_U32 c
);

/**
 * Decodes a UTF-8 byte sequence into a rune and returns the structure containing the rune/length.
 * Returns error rune (U+FFFD) for invalid sequences.
 */
PNSLR_DecodedRune PNSLR_DecodeRune(
    PNSLR_ArraySlice(PNSLR_U8) s
);

// Windows-specific ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Converts a UTF-8 string to a UTF-16 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
PNSLR_ArraySlice(PNSLR_U16) PNSLR_UTF16FromUTF8WindowsOnly(
    PNSLR_UTF8STR str,
    PNSLR_Allocator allocator
);

/**
 * Converts a UTF-16 string to a UTF-8 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
PNSLR_UTF8STR PNSLR_UTF8FromUTF16WindowsOnly(
    PNSLR_ArraySlice(PNSLR_U16) utf16str,
    PNSLR_Allocator allocator
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
    PNSLR_UTF8STR path;
} PNSLR_Path;

/**
 * Represents the type of path normalisation to perform.
 */
typedef PNSLR_U8 PNSLR_PathNormalisationType /* use as value */;
#define PNSLR_PathNormalisationType_File ((PNSLR_PathNormalisationType) 0)
#define PNSLR_PathNormalisationType_Directory ((PNSLR_PathNormalisationType) 1)

/**
 * Normalises a path to a consistent format, as specified in `PNSLR_PathNormalisationType`.
 */
PNSLR_Path PNSLR_NormalisePath(
    PNSLR_UTF8STR path,
    PNSLR_PathNormalisationType type,
    PNSLR_Allocator allocator
);

/**
 * Split a path into its components. Assumes that the path is a valid normalised path in Panshilar conventions.
 */
PNSLR_B8 PNSLR_SplitPath(
    PNSLR_Path path,
    PNSLR_Path* parent,
    PNSLR_UTF8STR* selfNameWithExtension,
    PNSLR_UTF8STR* selfName,
    PNSLR_UTF8STR* extension
);

/**
 * Returns a normalised path for a file inside a given directory.
 */
PNSLR_Path PNSLR_GetPathForChildFile(
    PNSLR_Path dir,
    PNSLR_UTF8STR fileNameWithExtension,
    PNSLR_Allocator allocator
);

/**
 * Returns a normalised path for a subdirectory inside a given directory.
 */
PNSLR_Path PNSLR_GetPathForSubdirectory(
    PNSLR_Path dir,
    PNSLR_UTF8STR dirName,
    PNSLR_Allocator allocator
);

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
typedef PNSLR_B8 (*PNSLR_DirectoryIterationVisitorDelegate)(
    void* payload,
    PNSLR_Path path,
    PNSLR_B8 isDirectory,
    PNSLR_B8* exploreCurrentDirectory
);

/**
 * Iterates over a directory and calls the visitor function for each file/directory found.
 * If `recursive` is true, it will also iterate over subdirectories.
 */
void PNSLR_IterateDirectory(
    PNSLR_Path path,
    PNSLR_B8 recursive,
    void* visitorPayload,
    PNSLR_DirectoryIterationVisitorDelegate visitorFunc
);

/**
 * Represents the type of path check to perform when checking if a path exists.
 */
typedef PNSLR_U8 PNSLR_PathExistsCheckType /* use as value */;
#define PNSLR_PathExistsCheckType_Either ((PNSLR_PathExistsCheckType) 0)
#define PNSLR_PathExistsCheckType_File ((PNSLR_PathExistsCheckType) 1)
#define PNSLR_PathExistsCheckType_Directory ((PNSLR_PathExistsCheckType) 2)

/**
 * Checks if a file/directory exists at the specified path.
 */
PNSLR_B8 PNSLR_PathExists(
    PNSLR_Path path,
    PNSLR_PathExistsCheckType type
);

/**
 * Deletes a file/directory at a path, if it exists.
 */
PNSLR_B8 PNSLR_DeletePath(
    PNSLR_Path path
);

/**
 * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
 */
PNSLR_I64 PNSLR_GetFileTimestamp(
    PNSLR_Path path
);

/**
 * Gets the size of a file at the specified path in bytes.
 */
PNSLR_I64 PNSLR_GetFileSize(
    PNSLR_Path path
);

/**
 * Creates a directory tree, if it doesn't exist.
 * Note that if the path doesn't have a trailing slash, it'll assume it's a file.
 * So, the last component of the path (if is a directory) will not be created.
 */
PNSLR_B8 PNSLR_CreateDirectoryTree(
    PNSLR_Path path
);

/**
 * Represents an opened file.
 */
typedef struct PNSLR_File
{
    void* handle;
} PNSLR_File;

/**
 * Opens a file for reading, optionally allow writing to the stream.
 * If the file does not exist, this function will fail.
 */
PNSLR_File PNSLR_OpenFileToRead(
    PNSLR_Path path,
    PNSLR_B8 allowWrite
);

/**
 * Opens a file for writing (or appending), optionally allow reading from the stream.
 * If the file does not exist, it will be created.
 */
PNSLR_File PNSLR_OpenFileToWrite(
    PNSLR_Path path,
    PNSLR_B8 append,
    PNSLR_B8 allowRead
);

/**
 * Gets the size of an opened file.
 */
PNSLR_I64 PNSLR_GetSizeOfFile(
    PNSLR_File handle
);

/**
 * Seeks to a specific position in an opened file.
 * If not relative, it's absolute from the start.
 */
PNSLR_B8 PNSLR_SeekPositionInFile(
    PNSLR_File handle,
    PNSLR_I64 newPos,
    PNSLR_B8 relative
);

/**
 * Reads data from an opened file at the current position.
 */
PNSLR_B8 PNSLR_ReadFromFile(
    PNSLR_File handle,
    PNSLR_ArraySlice(PNSLR_U8) dst
);

/**
 * Writes data to an opened file at the current position.
 */
PNSLR_B8 PNSLR_WriteToFile(
    PNSLR_File handle,
    PNSLR_ArraySlice(PNSLR_U8) src
);

/**
 * Truncates an opened file to a specific size.
 */
PNSLR_B8 PNSLR_TruncateFile(
    PNSLR_File handle,
    PNSLR_I64 newSize
);

/**
 * Flushes any buffered data to the file.
 */
PNSLR_B8 PNSLR_FlushFile(
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
 */
PNSLR_B8 PNSLR_ReadAllContentsFromFile(
    PNSLR_Path path,
    PNSLR_ArraySlice(PNSLR_U8)* dst,
    PNSLR_Allocator allocator
);

/**
 * Dump a bunch of data into a file. Optionally append it instead of overwriting.
 */
PNSLR_B8 PNSLR_WriteAllContentsToFile(
    PNSLR_Path path,
    PNSLR_ArraySlice(PNSLR_U8) src,
    PNSLR_B8 append
);

/**
 * Copies a file from src to dst. If dst exists, it will be overwritten.
 */
PNSLR_B8 PNSLR_CopyFile(
    PNSLR_Path src,
    PNSLR_Path dst
);

/**
 * Moves a file from src to dst. If dst exists, it will be overwritten.
 */
PNSLR_B8 PNSLR_MoveFile(
    PNSLR_Path src,
    PNSLR_Path dst
);

// #######################################################################################
// Console
// #######################################################################################

/**
 * Print a message to the standard output stream.
 */
PNSLR_I32 PNSLR_PrintToStdOut(
    PNSLR_UTF8STR message
);

// #######################################################################################
// Process
// #######################################################################################

/**
 * Exits the current process immediately with the specified exit code.
 */
void PNSLR_ExitProcess(
    PNSLR_I32 exitCode
);

// #######################################################################################
// Network
// #######################################################################################

/**
 * Represents an IP address in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(PNSLR_U8) PNSLR_IPAddress;

/**
 * Represents a subnet mask in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
typedef PNSLR_ArraySlice(PNSLR_U8) PNSLR_IPMask;

/**
 * Represents an IP network, consisting of an IP address and a subnet mask.
 */
typedef struct PNSLR_IPNetwork
{
    PNSLR_IPAddress address;
    PNSLR_IPMask mask;
} PNSLR_IPNetwork;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_IPNetwork);

PNSLR_B8 PNSLR_GetInterfaceIPAddresses(
    PNSLR_ArraySlice(PNSLR_IPNetwork)* networks,
    PNSLR_Allocator allocator
);

#undef PNSLR_ALIGNAS

/** Create a utf8str from a string literal. */
#define PNSLR_StringLiteral(str) (PNSLR_UTF8STR) {.count = sizeof(str) - 1, .data = (PNSLR_U8*) str}

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
    (PNSLR_ArraySlice_##ty) {.raw = PNSLR_MakeRawSlice((PNSLR_I32) sizeof(ty), (PNSLR_I32) alignof(ty), (PNSLR_I64) count, zeroed, allocator, loc, error__)}

/** Free a 'slice' (passed by ptr) allocated with `PNSLR_MakeSlice`, using the provided allocator. */
#define PNSLR_FreeSlice(slice, allocator, loc, error__) \
    do { if (slice) PNSLR_FreeRawSlice(&((slice)->raw), allocator, loc, error__); } while(0)

/** Resize a 'slice' (passed by ptr) to one with 'newCount' elements of type 'ty' using the provided allocator. Optionally zeroed. */
#define PNSLR_ResizeSlice(ty, slice, newCount, zeroed, allocator, loc, error__) \
    do { if (slice) PNSLR_ResizeRawSlice(&((slice)->raw), (PNSLR_I32) sizeof(ty), (PNSLR_I32) alignof(ty), (PNSLR_I64) newCount, zeroed, allocator, loc, error__); } while(0)

#ifdef __cplusplus
} // extern c
#endif

#endif//PANSHILAR_MAIN

#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
    #ifndef __cplusplus
        _Static_assert(sizeof(PNSLR_B8 ) == 1, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_U8 ) == 1, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_U16) == 2, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_U32) == 4, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_U64) == 8, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_I8 ) == 1, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_I16) == 2, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_I32) == 4, "Size mismatch.");
        _Static_assert(sizeof(PNSLR_I64) == 8, "Size mismatch.");
    #else //__cplusplus
        static_assert(sizeof(PNSLR_B8 ) == 1, "Size mismatch.");
        static_assert(sizeof(PNSLR_U8 ) == 1, "Size mismatch.");
        static_assert(sizeof(PNSLR_U16) == 2, "Size mismatch.");
        static_assert(sizeof(PNSLR_U32) == 4, "Size mismatch.");
        static_assert(sizeof(PNSLR_U64) == 8, "Size mismatch.");
        static_assert(sizeof(PNSLR_I8 ) == 1, "Size mismatch.");
        static_assert(sizeof(PNSLR_I16) == 2, "Size mismatch.");
        static_assert(sizeof(PNSLR_I32) == 4, "Size mismatch.");
        static_assert(sizeof(PNSLR_I64) == 8, "Size mismatch.");
    #endif//__cplusplus
#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
