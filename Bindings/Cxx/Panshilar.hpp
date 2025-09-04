// #pragma once is intentionally ignored.
#ifndef __cplusplus
    #error "Please use the C bibndings.";
#else
#ifndef PANSHILAR_CXX_MAIN
#define PANSHILAR_CXX_MAIN

namespace Panshilar
{
    typedef bool                b8;
    typedef unsigned char       u8;
    typedef unsigned short int  u16;
    typedef unsigned int        u32;
    typedef unsigned long long  u64;
    typedef signed char         i8;
    typedef signed short int    i16;
    typedef signed int          i32;
    typedef signed long long    i64;
    template <typename T> struct ArraySlice { T* data; i64 count; };

    // #######################################################################################
    // Intrinsics
    // #######################################################################################

    // Primitive ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A raw type-unspecific array slice.
     */
    struct RawArraySlice
    {
       void* data;
       i64 count;
    };

    /**
     * UTF-8 string type, with length info (not necessarily null-terminated).
     */
    typedef ArraySlice<u8> utf8str;

    // Memory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Allocate memory with the specified alignment and size.
     */
    void* Intrinsic_Malloc(
        i32 alignment,
        i32 size
    );

    /**
     * Free memory allocated with PNSLR_Intrinsic_Malloc.
     */
    void Intrinsic_Free(
        void* memory
    );

    /**
     * Set a block of memory to a specific value.
     */
    void Intrinsic_MemSet(
        void* memory,
        i32 value,
        i32 size
    );

    /**
     * Copy a block of memory from source to destination.
     */
    void Intrinsic_MemCopy(
        void* destination,
        void* source,
        i32 size
    );

    /**
     * Copy a block of memory from source to destination, handling overlapping regions.
     */
    void Intrinsic_MemMove(
        void* destination,
        void* source,
        i32 size
    );

    // #######################################################################################
    // Environment
    // #######################################################################################

    /**
     * Defines the platforms supported by the library.
     */
    enum class Platform : u8 /* use as value */
    {
        Unknown = 0,
        Windows = 1,
        Linux = 2,
        OSX = 3,
        Android = 4,
        iOS = 5,
        PS5 = 6,
        XBoxSeries = 7,
        Switch = 8,
    };

    /**
     * Defines the architectures supported by the library.
     */
    enum class Architecture : u8 /* use as value */
    {
        Unknown = 0,
        X64 = 1,
        ARM64 = 2,
    };

    /**
     * Get the current platform.
     */
    Platform GetPlatform();

    /**
     * Get the current architecture.
     */
    Architecture GetArchitecture();

    // #######################################################################################
    // Runtime
    // #######################################################################################

    /**
     * Defines the source code location for debugging purposes.
     */
    struct SourceCodeLocation
    {
       utf8str file;
       i32 line;
       i32 column;
       utf8str function;
    };

    // #######################################################################################
    // Sync
    // #######################################################################################

    // Mutex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * The most basic synchronization primitive.
     */
    struct alignas(8) Mutex
    {
       u8 buffer[64];
    };

    /**
     * Creates a mutex.
     */
    Mutex CreateMutex();

    /**
     * Destroys a mutex.
     */
    void DestroyMutex(
        Mutex* mutex
    );

    /**
     * Locks a mutex.
     */
    void LockMutex(
        Mutex* mutex
    );

    /**
     * Unlocks a mutex.
     */
    void UnlockMutex(
        Mutex* mutex
    );

    /**
     * Tries to lock a mutex.
     * Returns true if the mutex was successfully locked, false otherwise.
     */
    b8 TryLockMutex(
        Mutex* mutex
    );

    // Read-Write ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A read-write mutex.
     * This is a synchronization primitive that allows multiple readers or a single writer.
     * It is useful for scenarios where reads are more frequent than writes.
     */
    struct alignas(8) RWMutex
    {
       u8 buffer[200];
    };

    /**
     * Creates a read-write mutex.
     */
    RWMutex CreateRWMutex();

    /**
     * Destroys a read-write mutex.
     */
    void DestroyRWMutex(
        RWMutex* rwmutex
    );

    /**
     * Locks a read-write mutex for reading.
     * Multiple threads can read simultaneously.
     */
    void LockRWMutexShared(
        RWMutex* rwmutex
    );

    /**
     * Locks a read-write mutex for writing.
     * Only one thread can write at a time, and no other threads can read while writing.
     */
    void LockRWMutexExclusive(
        RWMutex* rwmutex
    );

    /**
     * Unlocks a read-write mutex after reading.
     * Allows other threads to read or write.
     */
    void UnlockRWMutexShared(
        RWMutex* rwmutex
    );

    /**
     * Unlocks a read-write mutex after writing.
     * Allows other threads to read or write.
     */
    void UnlockRWMutexExclusive(
        RWMutex* rwmutex
    );

    /**
     * Tries to lock a read-write mutex for reading.
     * Returns true if the mutex was successfully locked for reading, false otherwise.
     */
    b8 TryLockRWMutexShared(
        RWMutex* rwmutex
    );

    /**
     * Tries to lock a read-write mutex for writing.
     * Returns true if the mutex was successfully locked for writing, false otherwise.
     */
    b8 TryLockRWMutexExclusive(
        RWMutex* rwmutex
    );

    // Semaphore ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A semaphore synchronization primitive.
     * It allows a certain number of threads to access a resource concurrently.
     */
    struct alignas(8) Semaphore
    {
       u8 buffer[32];
    };

    /**
     * Creates a semaphore.
     * The initial count specifies how many threads can access the resource concurrently.
     */
    Semaphore CreateSemaphore(
        i32 initialCount
    );

    /**
     * Destroys a semaphore.
     */
    void DestroySemaphore(
        Semaphore* semaphore
    );

    /**
     * Waits on a semaphore.
     * The calling thread will block until the semaphore count is greater than zero.
     */
    void WaitSemaphore(
        Semaphore* semaphore
    );

    /**
     * Waits on a semaphore with a timeout.
     * The calling thread will block until the semaphore count is greater than zero or the timeout expires.
     * Returns true if the semaphore was acquired, false if the timeout expired.
     */
    b8 WaitSemaphoreTimeout(
        Semaphore* semaphore,
        i32 timeoutNs
    );

    /**
     * Signals a semaphore, incrementing its count by a specified amount.
     * If the count was zero, this will wake up one or more waiting threads.
     */
    void SignalSemaphore(
        Semaphore* semaphore,
        i32 count
    );

    // Condition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A condition variable for signaling between threads.
     * It allows threads to wait for a condition to be signaled.
     */
    struct alignas(8) ConditionVariable
    {
       u8 buffer[48];
    };

    /**
     * Creates a condition variable.
     */
    ConditionVariable CreateConditionVariable();

    /**
     * Destroys a condition variable.
     */
    void DestroyConditionVariable(
        ConditionVariable* condvar
    );

    /**
     * Waits on a condition variable.
     * The calling thread will block until the condition variable is signaled.
     * The mutex must be locked before calling this function.
     */
    void WaitConditionVariable(
        ConditionVariable* condvar,
        Mutex* mutex
    );

    /**
     * Waits on a condition variable with a timeout.
     * The calling thread will block until the condition variable is signaled or the timeout expires.
     * The mutex must be locked before calling this function.
     * Returns true if the condition variable was signaled, false if the timeout expired.
     */
    b8 WaitConditionVariableTimeout(
        ConditionVariable* condvar,
        Mutex* mutex,
        i32 timeoutNs
    );

    /**
     * Signals a condition variable, waking up one waiting thread.
     * If no threads are waiting, this has no effect.
     */
    void SignalConditionVariable(
        ConditionVariable* condvar
    );

    /**
     * Signals a condition variable, waking up all waiting threads.
     * If no threads are waiting, this has no effect.
     */
    void BroadcastConditionVariable(
        ConditionVariable* condvar
    );

    // #######################################################################################
    // Allocators
    // #######################################################################################

    // Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Defines the mode to be used when calling the allocator function.
     */
    enum class AllocatorMode : u8 /* use as value */
    {
        Allocate = 0,
        Resize = 1,
        Free = 2,
        FreeAll = 3,
        AllocateNoZero = 4,
        ResizeNoZero = 5,
        QueryCapabilities = 255,
    };

    /**
     * Defines the capabilities of an allocator.
     */
    enum class AllocatorCapability : u64 /* use as flags */
    {
        None = 0,
        ThreadSafe = 1,
        Resize = 2,
        Free = 4,
        FreeAll = 8,
        HintNil = 67108864,
        HintBump = 134217728,
        HintHeap = 268435456,
        HintTemp = 536870912,
        HintDebug = 1073741824,
    };

    /**
     * Defines the error codes that can be returned by the allocator.
     */
    enum class AllocatorError : u8 /* use as value */
    {
        None = 0,
        OutOfMemory = 1,
        InvalidAlignment = 2,
        InvalidSize = 3,
        InvalidMode = 4,
        Internal = 5,
        OutOfOrderFree = 6,
        DoubleFree = 7,
        CantFreeAll = 8,
    };

    /**
     * Defines the delegate type for the allocator function.
     */
    typedef void* (*AllocatorProcedure)(
        void* allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        void* oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
     */
    struct Allocator
    {
       AllocatorProcedure procedure;
       void* data;
    };

    // Allocation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Allocate memory using the provided allocator.
     */
    void* Allocate(
        Allocator allocator,
        b8 zeroed,
        i32 size,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Resize memory using the provided allocator.
     */
    void* Resize(
        Allocator allocator,
        b8 zeroed,
        void* oldMemory,
        i32 oldSize,
        i32 newSize,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Fallback resize function that can be used when the allocator does not support resizing.
     */
    void* DefaultResize(
        Allocator allocator,
        b8 zeroed,
        void* oldMemory,
        i32 oldSize,
        i32 newSize,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Free memory using the provided allocator.
     */
    void Free(
        Allocator allocator,
        void* memory,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Free all memory allocated by the provided allocator.
     */
    void FreeAll(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Query the capabilities of the provided allocator.
     */
    u64 QueryAllocatorCapabilities(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // Nil ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
     * Otherwise no-ops all around.
     */
    Allocator GetAllocator_Nil();

    // Default ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Get the default heap allocator.
     */
    Allocator GetAllocator_DefaultHeap();

    /**
     * Main allocator function for the default heap allocator.
     */
    void* AllocatorFn_DefaultHeap(
        void* allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        void* oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // Arena ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A block of memory used by the arena allocator.
     */
    struct ArenaAllocatorBlock
    {
       ArenaAllocatorBlock* previous;
       Allocator allocator;
       void* memory;
       u32 capacity;
       u32 used;
    };

    /**
     * The payload used by the arena allocator.
     */
    struct ArenaAllocatorPayload
    {
       Allocator backingAllocator;
       ArenaAllocatorBlock* currentBlock;
       u32 totalUsed;
       u32 totalCapacity;
       u32 minimumBlockSize;
       u32 numSnapshots;
    };

    /**
     * Create a new arena allocator with the specified backing allocator.
     * The arena allocator will use the backing allocator to allocate its blocks.
     * The arena allocator will not free the backing allocator, so it is the caller's responsibility to
     * free the backing allocator when it is no longer needed.
     */
    Allocator NewAllocator_Arena(
        Allocator backingAllocator,
        u32 pageSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Destroy an arena allocator and free all its resources.
     * This does not free the backing allocator, only the arena allocator's own resources.
     */
    void DestroyAllocator_Arena(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Main allocator function for the arena allocator.
     */
    void* AllocatorFn_Arena(
        void* allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        void* oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    enum class ArenaSnapshotError : u8 /* use as value */
    {
        None = 0,
        InvalidData = 1,
        MemoryBlockNotOwned = 2,
        OutOfOrderRestoreUsage = 3,
        DoubleRestoreOrDiscardUsage = 4,
    };

    /**
     * A snapshot of the arena allocator, recording its state at a specific point in time.
     * Can be saved/loaded/discarded as needed.
     */
    struct ArenaAllocatorSnapshot
    {
       b8 valid;
       ArenaAllocatorPayload* payload;
       ArenaAllocatorBlock* block;
       u32 used;
    };

    /**
     * Ensures that the arena allocator has either restored/discarded all the
     * snapshots that were taken.
     */
    b8 ValidateArenaAllocatorSnapshotState(
        Allocator allocator
    );

    /**
     * Captures a snapshot of the arena allocator.
     * The returned value can be used to load back the existing state at this point.
     */
    ArenaAllocatorSnapshot CaptureArenaAllocatorSnapshot(
        Allocator allocator
    );

    /**
     * Restores the state of the arena allocator from a snapshot.
     * Upon success, the snapshot is marked as invalid.
     */
    ArenaSnapshotError RestoreArenaAllocatorSnapshot(
        ArenaAllocatorSnapshot* snapshot,
        SourceCodeLocation loc
    );

    /**
     * Discards a snapshot of the arena allocator.
     */
    ArenaSnapshotError DiscardArenaAllocatorSnapshot(
        ArenaAllocatorSnapshot* snapshot
    );

    // Stack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A page of a stack allocator.
     */
    struct alignas(8) StackAllocatorPage
    {
       StackAllocatorPage* previousPage;
       u64 usedBytes;
       u8 buffer[8192];
    };

    /**
     * The header used for every separate stack allocation.
     * This is used to store metadata about the allocation and deallocate appropriately.
     */
    struct StackAllocationHeader
    {
       StackAllocatorPage* page;
       i32 size;
       i32 alignment;
       void* lastAllocation;
       void* lastAllocationHeader;
    };

    /**
     * The payload used by the stack allocator.
     */
    struct StackAllocatorPayload
    {
       Allocator backingAllocator;
       StackAllocatorPage* currentPage;
       void* lastAllocation;
       StackAllocationHeader* lastAllocationHeader;
    };

    /**
     * Create a stack allocator with the specified backing allocator.
     * The stack allocator will use the backing allocator to allocate its pages.
     * The stack allocator will not free the backing allocator, so it is the caller's responsibility to
     * free the backing allocator when it is no longer needed.
     */
    Allocator NewAllocator_Stack(
        Allocator backingAllocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Destroy a stack allocator and free all its resources.
     * This does not free the backing allocator, only the stack allocator's own resources.
     */
    void DestroyAllocator_Stack(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Main allocator function for the stack allocator.
     */
    void* AllocatorFn_Stack(
        void* allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        void* oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // Collections ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Allocate a raw array slice of 'count' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
     */
    RawArraySlice MakeRawSlice(
        i32 tySize,
        i32 tyAlign,
        i64 count,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Free a raw array slice allocated with `PNSLR_MakeRawSlice`, using the provided allocator.
     */
    void FreeRawSlice(
        RawArraySlice* slice,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Resize a raw array slice to one with 'newCount' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
     */
    void ResizeRawSlice(
        RawArraySlice* slice,
        i32 tySize,
        i32 tyAlign,
        i64 newCount,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
     */
    utf8str MakeString(
        i64 count,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
     */
    void FreeString(
        utf8str str,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
     */
    char* MakeCString(
        i64 count,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    /**
     * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
     */
    void FreeCString(
        char* str,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // #######################################################################################
    // Chrono
    // #######################################################################################

    /**
     * Returns the current time in nanoseconds since the Unix epoch (January 1, 1970).
     */
    i64 NanosecondsSinceUnixEpoch();

    // #######################################################################################
    // Strings
    // #######################################################################################

    // Basics/Conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Returns the length of the given C-style null-terminated string, excluding the null terminator.
     */
    i32 GetCStringLength(
        char* str
    );

    /**
     * Clone a C-style string into a new allocated string.
     */
    utf8str StringFromCString(
        char* str
    );

    /**
     * Clones a UTF-8 string to a C-style null-terminated string.
     * The returned string is allocated using the specified allocator.
     */
    char* CStringFromString(
        utf8str str,
        Allocator allocator
    );

    /**
     * Clones a UTF-8 string to a new allocated UTF-8 string.
     * The returned string is allocated using the specified allocator.
     */
    utf8str CloneString(
        utf8str str,
        Allocator allocator
    );

    // Basic/Manipulation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Concatenates two UTF-8 strings into a new allocated string.
     * The returned string is allocated using the specified allocator.
     */
    utf8str ConcatenateStrings(
        utf8str str1,
        utf8str str2,
        Allocator allocator
    );

    // Casing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Converts a UTF-8 string to uppercase.
     * The returned string is allocated using the specified allocator.
     */
    utf8str UpperString(
        utf8str str,
        Allocator allocator
    );

    /**
     * Converts a UTF-8 string to lowercase.
     * The returned string is allocated using the specified allocator.
     */
    utf8str LowerString(
        utf8str str,
        Allocator allocator
    );

    // Comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Represents the type of string comparison to perform.
     */
    enum class StringComparisonType : u8 /* use as value */
    {
        CaseSensitive = 0,
        CaseInsensitive = 1,
    };

    /**
     * Checks if two UTF-8 strings contain the same data.
     * Returns true if they are equal, false otherwise.
     */
    b8 AreStringsEqual(
        utf8str str1,
        utf8str str2,
        StringComparisonType comparisonType
    );

    /**
     * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
     * Returns true if they are equal, false otherwise.
     */
    b8 AreStringAndCStringEqual(
        utf8str str1,
        char* str2,
        StringComparisonType comparisonType
    );

    /**
     * Checks if two C-style null-terminated strings are equal.
     * Returns true if they are equal, false otherwise.
     */
    b8 AreCStringsEqual(
        char* str1,
        char* str2,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a UTF-8 string starts with the specified prefix.
     * Returns true if it does, false otherwise.
     */
    b8 StringStartsWith(
        utf8str str,
        utf8str prefix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a UTF-8 string ends with the specified suffix.
     * Returns true if it does, false otherwise.
     */
    b8 StringEndsWith(
        utf8str str,
        utf8str suffix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a C-style null-terminated string starts with the specified prefix.
     */
    b8 StringStartsWithCString(
        utf8str str,
        char* prefix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified suffix.
     */
    b8 StringEndsWithCString(
        utf8str str,
        char* suffix,
        StringComparisonType comparisonType
    );

    /**
     * Returns the length of the given C-style null-terminated string, excluding the null terminator.
     */
    b8 CStringStartsWith(
        char* str,
        utf8str prefix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
    b8 CStringEndsWith(
        char* str,
        utf8str suffix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
     */
    b8 CStringStartsWithCString(
        utf8str str,
        char* prefix,
        StringComparisonType comparisonType
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
    b8 CStringEndsWithCString(
        utf8str str,
        char* suffix,
        StringComparisonType comparisonType
    );

    // Advanced ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Searches for the first occurrence of a substring within a string.
     * Returns the index of the first occurrence, or -1 if not found.
     */
    i32 SearchFirstIndexInString(
        utf8str str,
        utf8str substring,
        StringComparisonType comparisonType
    );

    /**
     * Searches for the last occurrence of a substring within a string.
     * Returns the index of the last occurrence, or -1 if not found.
     */
    i32 SearchLastIndexInString(
        utf8str str,
        utf8str substring,
        StringComparisonType comparisonType
    );

    /**
     * Replaces all occurrences of a substring within a string with a new value.
     * The returned string is allocated using the specified allocator.
     */
    utf8str ReplaceInString(
        utf8str str,
        utf8str oldValue,
        utf8str newValue,
        Allocator allocator,
        StringComparisonType comparisonType
    );

    // UTF-8 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Result structure for UTF-8 rune encoding.
     * Contains the encoded bytes and the number of bytes used.
     */
    struct EncodedRune
    {
       u8 data[4];
       i32 length;
    };

    /**
     * Result structure for UTF-8 rune decoding.
     * Contains the decoded rune and the number of bytes consumed.
     */
    struct DecodedRune
    {
       u32 rune;
       i32 length;
    };

    /**
     * Returns the number of bytes required to encode the given rune in UTF-8.
     */
    i32 GetRuneLength(
        u32 r
    );

    /**
     * Encodes a rune into UTF-8 byte sequence and returns the structure containing encoded bytes/length.
     * Invalid runes or surrogates are replaced with the error rune (U+FFFD).
     */
    EncodedRune EncodeRune(
        u32 c
    );

    /**
     * Decodes a UTF-8 byte sequence into a rune and returns the structure containing the rune/length.
     * Returns error rune (U+FFFD) for invalid sequences.
     */
    DecodedRune DecodeRune(
        ArraySlice<u8> s
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
    struct Path
    {
       utf8str path;
    };

    /**
     * Represents the type of path normalisation to perform.
     */
    enum class PathNormalisationType : u8 /* use as value */
    {
        File = 0,
        Directory = 1,
    };

    /**
     * Normalises a path to a consistent format, as specified in `PNSLR_PathNormalisationType`.
     */
    Path NormalisePath(
        utf8str path,
        PathNormalisationType type,
        Allocator allocator
    );

    /**
     * Split a path into its components. Assumes that the path is a valid normalised path in Panshilar conventions.
     */
    b8 SplitPath(
        Path path,
        Path* parent,
        utf8str* selfNameWithExtension,
        utf8str* selfName,
        utf8str* extension
    );

    /**
     * Returns a normalised path for a file inside a given directory.
     */
    Path GetPathForChildFile(
        Path dir,
        utf8str fileNameWithExtension,
        Allocator allocator
    );

    /**
     * Returns a normalised path for a subdirectory inside a given directory.
     */
    Path GetPathForSubdirectory(
        Path dir,
        utf8str dirName,
        Allocator allocator
    );

    /**
     * The signature of the delegate that's supposed to be called for iterating over a directory.
     */
    typedef b8 (*DirectoryIterationVisitorDelegate)(
        void* payload,
        Path path,
        b8 isDirectory,
        b8* exploreCurrentDirectory
    );

    /**
     * Iterates over a directory and calls the visitor function for each file/directory found.
     * If `recursive` is true, it will also iterate over subdirectories.
     */
    void IterateDirectory(
        Path path,
        b8 recursive,
        void* visitorPayload,
        DirectoryIterationVisitorDelegate visitorFunc
    );

    /**
     * Represents the type of path check to perform when checking if a path exists.
     */
    enum class PathExistsCheckType : u8 /* use as value */
    {
        Either = 0,
        File = 1,
        Directory = 2,
    };

    /**
     * Checks if a file/directory exists at the specified path.
     */
    b8 PathExists(
        Path path,
        PathExistsCheckType type
    );

    /**
     * Deletes a file/directory at a path, if it exists.
     */
    b8 DeletePath(
        Path path
    );

    /**
     * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
     */
    i64 GetFileTimestamp(
        Path path
    );

    /**
     * Gets the size of a file at the specified path in bytes.
     */
    i64 GetFileSize(
        Path path
    );

    /**
     * Creates a directory tree, if it doesn't exist.
     * Note that if the path doesn't have a trailing slash, it'll assume it's a file.
     * So, the last component of the path (if is a directory) will not be created.
     */
    b8 CreateDirectoryTree(
        Path path
    );

    /**
     * Represents an opened file.
     */
    struct File
    {
       void* handle;
    };

    /**
     * Opens a file for reading, optionally allow writing to the stream.
     * If the file does not exist, this function will fail.
     */
    File OpenFileToRead(
        Path path,
        b8 allowWrite
    );

    /**
     * Opens a file for writing (or appending), optionally allow reading from the stream.
     * If the file does not exist, it will be created.
     */
    File OpenFileToWrite(
        Path path,
        b8 append,
        b8 allowRead
    );

    /**
     * Gets the size of an opened file.
     */
    i64 GetSizeOfFile(
        File handle
    );

    /**
     * Seeks to a specific position in an opened file.
     * If not relative, it's absolute from the start.
     */
    b8 SeekPositionInFile(
        File handle,
        i64 newPos,
        b8 relative
    );

    /**
     * Reads data from an opened file at the current position.
     */
    b8 ReadFromFile(
        File handle,
        ArraySlice<u8> dst
    );

    /**
     * Writes data to an opened file at the current position.
     */
    b8 WriteToFile(
        File handle,
        ArraySlice<u8> src
    );

    /**
     * Truncates an opened file to a specific size.
     */
    b8 TruncateFile(
        File handle,
        i64 newSize
    );

    /**
     * Flushes any buffered data to the file.
     */
    b8 FlushFile(
        File handle
    );

    /**
     * Closes an opened file.
     */
    void CloseFileHandle(
        File handle
    );

    /**
     * Reads a file fully end-to-end and stores in a buffer. Won't work if dst is nil.
     * Provided allocator is used for creating the buffer.
     */
    b8 ReadAllContentsFromFile(
        Path path,
        ArraySlice<u8>* dst,
        Allocator allocator
    );

    /**
     * Dump a bunch of data into a file. Optionally append it instead of overwriting.
     */
    b8 WriteAllContentsToFile(
        Path path,
        ArraySlice<u8> src,
        b8 append
    );

    /**
     * Copies a file from src to dst. If dst exists, it will be overwritten.
     */
    b8 CopyFile(
        Path src,
        Path dst
    );

    /**
     * Moves a file from src to dst. If dst exists, it will be overwritten.
     */
    b8 MoveFile(
        Path src,
        Path dst
    );

    // #######################################################################################
    // Console
    // #######################################################################################

    /**
     * Print a message to the standard output stream.
     */
    i32 PrintToStdOut(
        utf8str message
    );

    // #######################################################################################
    // Process
    // #######################################################################################

    /**
     * Exits the current process immediately with the specified exit code.
     */
    void ExitProcess(
        i32 exitCode
    );

    /** Create a utf8str from a string literal. */
    template <u64 N> constexpr utf8str STRING_LITERAL(const char (&str)[N]) { utf8str output; output.count = (i64) (N - 1); output.data = (u8*) str; return output; }

    /** Get the current source code location. */
    #define PNSLR_GET_LOC() (PNSLR_SourceCodeLocation) {STRING_LITERAL(__FILE__), __LINE__, 0, STRING_LITERAL(__FUNCTION__)}

    /** Allocate an object of type 'ty' using the provided allocator. */
    template <typename T> T* New(Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        return (T*) Allocate(allocator, true, (i32) sizeof(T), (i32) alignof(T), loc, err);
    }

    /** Delete an object allocated with `PNSLR_New`, using the provided allocator. */
    template <typename T> void Delete(T* obj, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        if (obj) { Free(allocator, obj, loc, err); }
    }

    /** Allocate an array of 'count__' elements of type 'ty' using the provided allocator. Optionally zeroed. */
    template <typename T> ArraySlice<T> MakeSlice(i64 count, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        RawArraySlice raw = MakeRawSlice((i32) sizeof(T), (i32) alignof(T), count, zeroed, allocator, loc, err);
        return *reinterpret_cast<ArraySlice<T>*>(&raw);
}

    /** Free a 'slice' allocated with `PNSLR_MakeSlice`, using the provided allocator. Expects a reassignable variable. */
    template <typename T> void FreeSlice(ArraySlice<T>* slice, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        if (slice) FreeRawSlice(reinterpret_cast<RawArraySlice*>(slice), allocator, loc, err);
    }

    /** Resize a slice to one with 'newCount__' elements of type 'ty' using the provided allocator. Optionally zeroed. Expects a reassignable variable. */
    template <typename T> void ResizeSlice(ArraySlice<T>* slice, i64 newCount, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        if (slice) ResizeRawSlice(reinterpret_cast<RawArraySlice*>(slice), (i32) sizeof(T), (i32) alignof(T), newCount, zeroed, allocator, loc, err);
    }
}//namespace end

#endif//PANSHILAR_CXX_MAIN

#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
    static_assert(sizeof(Panshilar::b8 ) == 1, "Size mismatch.");
    static_assert(sizeof(Panshilar::u8 ) == 1, "Size mismatch.");
    static_assert(sizeof(Panshilar::u16) == 2, "Size mismatch.");
    static_assert(sizeof(Panshilar::u32) == 4, "Size mismatch.");
    static_assert(sizeof(Panshilar::u64) == 8, "Size mismatch.");
    static_assert(sizeof(Panshilar::i8 ) == 1, "Size mismatch.");
    static_assert(sizeof(Panshilar::i16) == 2, "Size mismatch.");
    static_assert(sizeof(Panshilar::i32) == 4, "Size mismatch.");
    static_assert(sizeof(Panshilar::i64) == 8, "Size mismatch.");
#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS

#ifndef PNSLR_IMPLEMENTATION
    #define PNSLR_SKIP_IMPLEMENTATION
#else //PNSLR_IMPLEMENTATION
    #undef  PNSLR_SKIP_IMPLEMENTATION
    #undef  PNSLR_IMPLEMENTATION
#endif//PNSLR_IMPLEMENTATION

#ifndef PNSLR_SKIP_IMPLEMENTATION
#define PNSLR_SKIP_IMPLEMENTATION

typedef Panshilar::b8  PNSLR_B8;
typedef Panshilar::u8  PNSLR_U8;
typedef Panshilar::u16 PNSLR_U16;
typedef Panshilar::u32 PNSLR_U32;
typedef Panshilar::u64 PNSLR_U64;
typedef Panshilar::i8  PNSLR_I8;
typedef Panshilar::i16 PNSLR_I16;
typedef Panshilar::i32 PNSLR_I32;
typedef Panshilar::i64 PNSLR_I64;

void*      PNSLR_Bindings_Convert(void*      x) { return x; }
char*      PNSLR_Bindings_Convert(char*      x) { return x; }
PNSLR_B8*  PNSLR_Bindings_Convert(PNSLR_B8*  x) { return x; }
PNSLR_U8*  PNSLR_Bindings_Convert(PNSLR_U8*  x) { return x; }
PNSLR_U16* PNSLR_Bindings_Convert(PNSLR_U16* x) { return x; }
PNSLR_U32* PNSLR_Bindings_Convert(PNSLR_U32* x) { return x; }
PNSLR_U64* PNSLR_Bindings_Convert(PNSLR_U64* x) { return x; }
PNSLR_I8*  PNSLR_Bindings_Convert(PNSLR_I8*  x) { return x; }
PNSLR_I16* PNSLR_Bindings_Convert(PNSLR_I16* x) { return x; }
PNSLR_I32* PNSLR_Bindings_Convert(PNSLR_I32* x) { return x; }
PNSLR_I64* PNSLR_Bindings_Convert(PNSLR_I64* x) { return x; }

PNSLR_B8&  PNSLR_Bindings_Convert(PNSLR_B8&  x) { return x; }
PNSLR_U8&  PNSLR_Bindings_Convert(PNSLR_U8&  x) { return x; }
PNSLR_U16& PNSLR_Bindings_Convert(PNSLR_U16& x) { return x; }
PNSLR_U32& PNSLR_Bindings_Convert(PNSLR_U32& x) { return x; }
PNSLR_U64& PNSLR_Bindings_Convert(PNSLR_U64& x) { return x; }
PNSLR_I8&  PNSLR_Bindings_Convert(PNSLR_I8&  x) { return x; }
PNSLR_I16& PNSLR_Bindings_Convert(PNSLR_I16& x) { return x; }
PNSLR_I32& PNSLR_Bindings_Convert(PNSLR_I32& x) { return x; }
PNSLR_I64& PNSLR_Bindings_Convert(PNSLR_I64& x) { return x; }

#if (_MSC_VER)
    #define PNSLR_STRUCT_OFFSET(type, member) ((PNSLR_U64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
#elif (__clang__) || (__GNUC__)
    #define PNSLR_STRUCT_OFFSET(type, member) ((PNSLR_U64) offsetof(type, member))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

struct PNSLR_RawArraySlice
{
   void* data;
   PNSLR_I64 count;
};
static_assert(sizeof(PNSLR_RawArraySlice) == sizeof(Panshilar::RawArraySlice), "size mismatch");
static_assert(alignof(PNSLR_RawArraySlice) == alignof(Panshilar::RawArraySlice), "align mismatch");
PNSLR_RawArraySlice* PNSLR_Bindings_Convert(Panshilar::RawArraySlice* x) { return reinterpret_cast<PNSLR_RawArraySlice*>(x); }
Panshilar::RawArraySlice* PNSLR_Bindings_Convert(PNSLR_RawArraySlice* x) { return reinterpret_cast<Panshilar::RawArraySlice*>(x); }
PNSLR_RawArraySlice& PNSLR_Bindings_Convert(Panshilar::RawArraySlice& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::RawArraySlice& PNSLR_Bindings_Convert(PNSLR_RawArraySlice& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_RawArraySlice, data) == PNSLR_STRUCT_OFFSET(Panshilar::RawArraySlice, data), "data offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_RawArraySlice, count) == PNSLR_STRUCT_OFFSET(Panshilar::RawArraySlice, count), "count offset mismatch");

typedef struct { PNSLR_B8* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_B8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_B8) == sizeof(Panshilar::ArraySlice<Panshilar::b8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_B8) == alignof(Panshilar::ArraySlice<Panshilar::b8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_B8* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::b8>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_B8*>(x); }
Panshilar::ArraySlice<Panshilar::b8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_B8* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::b8>*>(x); }
PNSLR_ArraySlice_PNSLR_B8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::b8>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::b8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_B8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_B8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::b8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_B8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::b8>, data), "data offset mismatch");

typedef struct { PNSLR_U8* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_U8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U8) == sizeof(Panshilar::ArraySlice<Panshilar::u8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U8) == alignof(Panshilar::ArraySlice<Panshilar::u8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U8* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u8>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_U8*>(x); }
Panshilar::ArraySlice<Panshilar::u8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U8* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::u8>*>(x); }
PNSLR_ArraySlice_PNSLR_U8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u8>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::u8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u8>, data), "data offset mismatch");

typedef struct { PNSLR_U16* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_U16;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U16) == sizeof(Panshilar::ArraySlice<Panshilar::u16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U16) == alignof(Panshilar::ArraySlice<Panshilar::u16>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U16* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u16>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_U16*>(x); }
Panshilar::ArraySlice<Panshilar::u16>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U16* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::u16>*>(x); }
PNSLR_ArraySlice_PNSLR_U16& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u16>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::u16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U16& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U16, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U16, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u16>, data), "data offset mismatch");

typedef struct { PNSLR_U32* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_U32;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U32) == sizeof(Panshilar::ArraySlice<Panshilar::u32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U32) == alignof(Panshilar::ArraySlice<Panshilar::u32>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U32* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u32>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_U32*>(x); }
Panshilar::ArraySlice<Panshilar::u32>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U32* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::u32>*>(x); }
PNSLR_ArraySlice_PNSLR_U32& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u32>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::u32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U32& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U32, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U32, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u32>, data), "data offset mismatch");

typedef struct { PNSLR_U64* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_U64;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U64) == sizeof(Panshilar::ArraySlice<Panshilar::u64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U64) == alignof(Panshilar::ArraySlice<Panshilar::u64>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U64* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u64>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_U64*>(x); }
Panshilar::ArraySlice<Panshilar::u64>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U64* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::u64>*>(x); }
PNSLR_ArraySlice_PNSLR_U64& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u64>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::u64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U64& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U64, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U64, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u64>, data), "data offset mismatch");

typedef struct { PNSLR_I8* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_I8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I8) == sizeof(Panshilar::ArraySlice<Panshilar::i8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I8) == alignof(Panshilar::ArraySlice<Panshilar::i8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I8* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i8>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_I8*>(x); }
Panshilar::ArraySlice<Panshilar::i8>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I8* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::i8>*>(x); }
PNSLR_ArraySlice_PNSLR_I8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i8>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::i8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I8& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i8>, data), "data offset mismatch");

typedef struct { PNSLR_I16* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_I16;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I16) == sizeof(Panshilar::ArraySlice<Panshilar::i16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I16) == alignof(Panshilar::ArraySlice<Panshilar::i16>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I16* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i16>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_I16*>(x); }
Panshilar::ArraySlice<Panshilar::i16>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I16* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::i16>*>(x); }
PNSLR_ArraySlice_PNSLR_I16& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i16>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::i16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I16& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I16, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I16, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i16>, data), "data offset mismatch");

typedef struct { PNSLR_I32* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_I32;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I32) == sizeof(Panshilar::ArraySlice<Panshilar::i32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I32) == alignof(Panshilar::ArraySlice<Panshilar::i32>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I32* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i32>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_I32*>(x); }
Panshilar::ArraySlice<Panshilar::i32>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I32* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::i32>*>(x); }
PNSLR_ArraySlice_PNSLR_I32& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i32>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::i32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I32& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I32, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I32, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i32>, data), "data offset mismatch");

typedef struct { PNSLR_I64* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_I64;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I64) == sizeof(Panshilar::ArraySlice<Panshilar::i64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I64) == alignof(Panshilar::ArraySlice<Panshilar::i64>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I64* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i64>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_I64*>(x); }
Panshilar::ArraySlice<Panshilar::i64>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I64* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::i64>*>(x); }
PNSLR_ArraySlice_PNSLR_I64& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i64>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::i64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I64& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I64, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I64, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i64>, data), "data offset mismatch");

typedef struct { float* data; PNSLR_I64 count; } PNSLR_ArraySlice_float;
static_assert(sizeof(PNSLR_ArraySlice_float) == sizeof(Panshilar::ArraySlice<float>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_float) == alignof(Panshilar::ArraySlice<float>), "align mismatch");
PNSLR_ArraySlice_float* PNSLR_Bindings_Convert(Panshilar::ArraySlice<float>* x) { return reinterpret_cast<PNSLR_ArraySlice_float*>(x); }
Panshilar::ArraySlice<float>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_float* x) { return reinterpret_cast<Panshilar::ArraySlice<float>*>(x); }
PNSLR_ArraySlice_float& PNSLR_Bindings_Convert(Panshilar::ArraySlice<float>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<float>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_float& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_float, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<float>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_float, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<float>, data), "data offset mismatch");

typedef struct { double* data; PNSLR_I64 count; } PNSLR_ArraySlice_double;
static_assert(sizeof(PNSLR_ArraySlice_double) == sizeof(Panshilar::ArraySlice<double>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_double) == alignof(Panshilar::ArraySlice<double>), "align mismatch");
PNSLR_ArraySlice_double* PNSLR_Bindings_Convert(Panshilar::ArraySlice<double>* x) { return reinterpret_cast<PNSLR_ArraySlice_double*>(x); }
Panshilar::ArraySlice<double>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_double* x) { return reinterpret_cast<Panshilar::ArraySlice<double>*>(x); }
PNSLR_ArraySlice_double& PNSLR_Bindings_Convert(Panshilar::ArraySlice<double>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<double>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_double& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_double, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<double>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_double, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<double>, data), "data offset mismatch");

typedef struct { char* data; PNSLR_I64 count; } PNSLR_ArraySlice_char;
static_assert(sizeof(PNSLR_ArraySlice_char) == sizeof(Panshilar::ArraySlice<char>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_char) == alignof(Panshilar::ArraySlice<char>), "align mismatch");
PNSLR_ArraySlice_char* PNSLR_Bindings_Convert(Panshilar::ArraySlice<char>* x) { return reinterpret_cast<PNSLR_ArraySlice_char*>(x); }
Panshilar::ArraySlice<char>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_char* x) { return reinterpret_cast<Panshilar::ArraySlice<char>*>(x); }
PNSLR_ArraySlice_char& PNSLR_Bindings_Convert(Panshilar::ArraySlice<char>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<char>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_char& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<char>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<char>, data), "data offset mismatch");

typedef PNSLR_ArraySlice_PNSLR_U8 PNSLR_UTF8STR;

typedef struct { PNSLR_UTF8STR* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_UTF8STR;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_UTF8STR) == sizeof(Panshilar::ArraySlice<Panshilar::utf8str>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_UTF8STR) == alignof(Panshilar::ArraySlice<Panshilar::utf8str>), "align mismatch");
PNSLR_ArraySlice_PNSLR_UTF8STR* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::utf8str>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_UTF8STR*>(x); }
Panshilar::ArraySlice<Panshilar::utf8str>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_UTF8STR* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::utf8str>*>(x); }
PNSLR_ArraySlice_PNSLR_UTF8STR& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::utf8str>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::utf8str>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_UTF8STR& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::utf8str>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::utf8str>, data), "data offset mismatch");

extern "C" void* PNSLR_Intrinsic_Malloc(PNSLR_I32 alignment, PNSLR_I32 size);
void* Panshilar::Intrinsic_Malloc(Panshilar::i32 alignment, Panshilar::i32 size)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_Intrinsic_Malloc(PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(size)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_Intrinsic_Free(void* memory);
void Panshilar::Intrinsic_Free(void* memory)
{
    PNSLR_Intrinsic_Free(PNSLR_Bindings_Convert(memory));
}

extern "C" void PNSLR_Intrinsic_MemSet(void* memory, PNSLR_I32 value, PNSLR_I32 size);
void Panshilar::Intrinsic_MemSet(void* memory, Panshilar::i32 value, Panshilar::i32 size)
{
    PNSLR_Intrinsic_MemSet(PNSLR_Bindings_Convert(memory), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(size));
}

extern "C" void PNSLR_Intrinsic_MemCopy(void* destination, void* source, PNSLR_I32 size);
void Panshilar::Intrinsic_MemCopy(void* destination, void* source, Panshilar::i32 size)
{
    PNSLR_Intrinsic_MemCopy(PNSLR_Bindings_Convert(destination), PNSLR_Bindings_Convert(source), PNSLR_Bindings_Convert(size));
}

extern "C" void PNSLR_Intrinsic_MemMove(void* destination, void* source, PNSLR_I32 size);
void Panshilar::Intrinsic_MemMove(void* destination, void* source, Panshilar::i32 size)
{
    PNSLR_Intrinsic_MemMove(PNSLR_Bindings_Convert(destination), PNSLR_Bindings_Convert(source), PNSLR_Bindings_Convert(size));
}

enum class PNSLR_Platform : Panshilar::u8 { };
static_assert(sizeof(PNSLR_Platform) == sizeof(Panshilar::Platform), "size mismatch");
static_assert(alignof(PNSLR_Platform) == alignof(Panshilar::Platform), "align mismatch");
PNSLR_Platform* PNSLR_Bindings_Convert(Panshilar::Platform* x) { return reinterpret_cast<PNSLR_Platform*>(x); }
Panshilar::Platform* PNSLR_Bindings_Convert(PNSLR_Platform* x) { return reinterpret_cast<Panshilar::Platform*>(x); }
PNSLR_Platform& PNSLR_Bindings_Convert(Panshilar::Platform& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Platform& PNSLR_Bindings_Convert(PNSLR_Platform& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_Architecture : Panshilar::u8 { };
static_assert(sizeof(PNSLR_Architecture) == sizeof(Panshilar::Architecture), "size mismatch");
static_assert(alignof(PNSLR_Architecture) == alignof(Panshilar::Architecture), "align mismatch");
PNSLR_Architecture* PNSLR_Bindings_Convert(Panshilar::Architecture* x) { return reinterpret_cast<PNSLR_Architecture*>(x); }
Panshilar::Architecture* PNSLR_Bindings_Convert(PNSLR_Architecture* x) { return reinterpret_cast<Panshilar::Architecture*>(x); }
PNSLR_Architecture& PNSLR_Bindings_Convert(Panshilar::Architecture& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Architecture& PNSLR_Bindings_Convert(PNSLR_Architecture& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" PNSLR_Platform PNSLR_GetPlatform();
Panshilar::Platform Panshilar::GetPlatform()
{
    PNSLR_Platform zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetPlatform(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Architecture PNSLR_GetArchitecture();
Panshilar::Architecture Panshilar::GetArchitecture()
{
    PNSLR_Architecture zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetArchitecture(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_SourceCodeLocation
{
   PNSLR_UTF8STR file;
   PNSLR_I32 line;
   PNSLR_I32 column;
   PNSLR_UTF8STR function;
};
static_assert(sizeof(PNSLR_SourceCodeLocation) == sizeof(Panshilar::SourceCodeLocation), "size mismatch");
static_assert(alignof(PNSLR_SourceCodeLocation) == alignof(Panshilar::SourceCodeLocation), "align mismatch");
PNSLR_SourceCodeLocation* PNSLR_Bindings_Convert(Panshilar::SourceCodeLocation* x) { return reinterpret_cast<PNSLR_SourceCodeLocation*>(x); }
Panshilar::SourceCodeLocation* PNSLR_Bindings_Convert(PNSLR_SourceCodeLocation* x) { return reinterpret_cast<Panshilar::SourceCodeLocation*>(x); }
PNSLR_SourceCodeLocation& PNSLR_Bindings_Convert(Panshilar::SourceCodeLocation& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::SourceCodeLocation& PNSLR_Bindings_Convert(PNSLR_SourceCodeLocation& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_SourceCodeLocation, file) == PNSLR_STRUCT_OFFSET(Panshilar::SourceCodeLocation, file), "file offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_SourceCodeLocation, line) == PNSLR_STRUCT_OFFSET(Panshilar::SourceCodeLocation, line), "line offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_SourceCodeLocation, column) == PNSLR_STRUCT_OFFSET(Panshilar::SourceCodeLocation, column), "column offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_SourceCodeLocation, function) == PNSLR_STRUCT_OFFSET(Panshilar::SourceCodeLocation, function), "function offset mismatch");

struct alignas(8) PNSLR_Mutex
{
   PNSLR_U8 buffer[64];
};
static_assert(sizeof(PNSLR_Mutex) == sizeof(Panshilar::Mutex), "size mismatch");
static_assert(alignof(PNSLR_Mutex) == alignof(Panshilar::Mutex), "align mismatch");
PNSLR_Mutex* PNSLR_Bindings_Convert(Panshilar::Mutex* x) { return reinterpret_cast<PNSLR_Mutex*>(x); }
Panshilar::Mutex* PNSLR_Bindings_Convert(PNSLR_Mutex* x) { return reinterpret_cast<Panshilar::Mutex*>(x); }
PNSLR_Mutex& PNSLR_Bindings_Convert(Panshilar::Mutex& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Mutex& PNSLR_Bindings_Convert(PNSLR_Mutex& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Mutex, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::Mutex, buffer), "buffer offset mismatch");

extern "C" PNSLR_Mutex PNSLR_CreateMutex();
Panshilar::Mutex Panshilar::CreateMutex()
{
    PNSLR_Mutex zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateMutex(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyMutex(PNSLR_Mutex* mutex);
void Panshilar::DestroyMutex(Panshilar::Mutex* mutex)
{
    PNSLR_DestroyMutex(PNSLR_Bindings_Convert(mutex));
}

extern "C" void PNSLR_LockMutex(PNSLR_Mutex* mutex);
void Panshilar::LockMutex(Panshilar::Mutex* mutex)
{
    PNSLR_LockMutex(PNSLR_Bindings_Convert(mutex));
}

extern "C" void PNSLR_UnlockMutex(PNSLR_Mutex* mutex);
void Panshilar::UnlockMutex(Panshilar::Mutex* mutex)
{
    PNSLR_UnlockMutex(PNSLR_Bindings_Convert(mutex));
}

extern "C" PNSLR_B8 PNSLR_TryLockMutex(PNSLR_Mutex* mutex);
Panshilar::b8 Panshilar::TryLockMutex(Panshilar::Mutex* mutex)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockMutex(PNSLR_Bindings_Convert(mutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct alignas(8) PNSLR_RWMutex
{
   PNSLR_U8 buffer[200];
};
static_assert(sizeof(PNSLR_RWMutex) == sizeof(Panshilar::RWMutex), "size mismatch");
static_assert(alignof(PNSLR_RWMutex) == alignof(Panshilar::RWMutex), "align mismatch");
PNSLR_RWMutex* PNSLR_Bindings_Convert(Panshilar::RWMutex* x) { return reinterpret_cast<PNSLR_RWMutex*>(x); }
Panshilar::RWMutex* PNSLR_Bindings_Convert(PNSLR_RWMutex* x) { return reinterpret_cast<Panshilar::RWMutex*>(x); }
PNSLR_RWMutex& PNSLR_Bindings_Convert(Panshilar::RWMutex& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::RWMutex& PNSLR_Bindings_Convert(PNSLR_RWMutex& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_RWMutex, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::RWMutex, buffer), "buffer offset mismatch");

extern "C" PNSLR_RWMutex PNSLR_CreateRWMutex();
Panshilar::RWMutex Panshilar::CreateRWMutex()
{
    PNSLR_RWMutex zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateRWMutex(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyRWMutex(PNSLR_RWMutex* rwmutex);
void Panshilar::DestroyRWMutex(Panshilar::RWMutex* rwmutex)
{
    PNSLR_DestroyRWMutex(PNSLR_Bindings_Convert(rwmutex));
}

extern "C" void PNSLR_LockRWMutexShared(PNSLR_RWMutex* rwmutex);
void Panshilar::LockRWMutexShared(Panshilar::RWMutex* rwmutex)
{
    PNSLR_LockRWMutexShared(PNSLR_Bindings_Convert(rwmutex));
}

extern "C" void PNSLR_LockRWMutexExclusive(PNSLR_RWMutex* rwmutex);
void Panshilar::LockRWMutexExclusive(Panshilar::RWMutex* rwmutex)
{
    PNSLR_LockRWMutexExclusive(PNSLR_Bindings_Convert(rwmutex));
}

extern "C" void PNSLR_UnlockRWMutexShared(PNSLR_RWMutex* rwmutex);
void Panshilar::UnlockRWMutexShared(Panshilar::RWMutex* rwmutex)
{
    PNSLR_UnlockRWMutexShared(PNSLR_Bindings_Convert(rwmutex));
}

extern "C" void PNSLR_UnlockRWMutexExclusive(PNSLR_RWMutex* rwmutex);
void Panshilar::UnlockRWMutexExclusive(Panshilar::RWMutex* rwmutex)
{
    PNSLR_UnlockRWMutexExclusive(PNSLR_Bindings_Convert(rwmutex));
}

extern "C" PNSLR_B8 PNSLR_TryLockRWMutexShared(PNSLR_RWMutex* rwmutex);
Panshilar::b8 Panshilar::TryLockRWMutexShared(Panshilar::RWMutex* rwmutex)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockRWMutexShared(PNSLR_Bindings_Convert(rwmutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_TryLockRWMutexExclusive(PNSLR_RWMutex* rwmutex);
Panshilar::b8 Panshilar::TryLockRWMutexExclusive(Panshilar::RWMutex* rwmutex)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockRWMutexExclusive(PNSLR_Bindings_Convert(rwmutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct alignas(8) PNSLR_Semaphore
{
   PNSLR_U8 buffer[32];
};
static_assert(sizeof(PNSLR_Semaphore) == sizeof(Panshilar::Semaphore), "size mismatch");
static_assert(alignof(PNSLR_Semaphore) == alignof(Panshilar::Semaphore), "align mismatch");
PNSLR_Semaphore* PNSLR_Bindings_Convert(Panshilar::Semaphore* x) { return reinterpret_cast<PNSLR_Semaphore*>(x); }
Panshilar::Semaphore* PNSLR_Bindings_Convert(PNSLR_Semaphore* x) { return reinterpret_cast<Panshilar::Semaphore*>(x); }
PNSLR_Semaphore& PNSLR_Bindings_Convert(Panshilar::Semaphore& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Semaphore& PNSLR_Bindings_Convert(PNSLR_Semaphore& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Semaphore, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::Semaphore, buffer), "buffer offset mismatch");

extern "C" PNSLR_Semaphore PNSLR_CreateSemaphore(PNSLR_I32 initialCount);
Panshilar::Semaphore Panshilar::CreateSemaphore(Panshilar::i32 initialCount)
{
    PNSLR_Semaphore zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateSemaphore(PNSLR_Bindings_Convert(initialCount)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroySemaphore(PNSLR_Semaphore* semaphore);
void Panshilar::DestroySemaphore(Panshilar::Semaphore* semaphore)
{
    PNSLR_DestroySemaphore(PNSLR_Bindings_Convert(semaphore));
}

extern "C" void PNSLR_WaitSemaphore(PNSLR_Semaphore* semaphore);
void Panshilar::WaitSemaphore(Panshilar::Semaphore* semaphore)
{
    PNSLR_WaitSemaphore(PNSLR_Bindings_Convert(semaphore));
}

extern "C" PNSLR_B8 PNSLR_WaitSemaphoreTimeout(PNSLR_Semaphore* semaphore, PNSLR_I32 timeoutNs);
Panshilar::b8 Panshilar::WaitSemaphoreTimeout(Panshilar::Semaphore* semaphore, Panshilar::i32 timeoutNs)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WaitSemaphoreTimeout(PNSLR_Bindings_Convert(semaphore), PNSLR_Bindings_Convert(timeoutNs)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_SignalSemaphore(PNSLR_Semaphore* semaphore, PNSLR_I32 count);
void Panshilar::SignalSemaphore(Panshilar::Semaphore* semaphore, Panshilar::i32 count)
{
    PNSLR_SignalSemaphore(PNSLR_Bindings_Convert(semaphore), PNSLR_Bindings_Convert(count));
}

struct alignas(8) PNSLR_ConditionVariable
{
   PNSLR_U8 buffer[48];
};
static_assert(sizeof(PNSLR_ConditionVariable) == sizeof(Panshilar::ConditionVariable), "size mismatch");
static_assert(alignof(PNSLR_ConditionVariable) == alignof(Panshilar::ConditionVariable), "align mismatch");
PNSLR_ConditionVariable* PNSLR_Bindings_Convert(Panshilar::ConditionVariable* x) { return reinterpret_cast<PNSLR_ConditionVariable*>(x); }
Panshilar::ConditionVariable* PNSLR_Bindings_Convert(PNSLR_ConditionVariable* x) { return reinterpret_cast<Panshilar::ConditionVariable*>(x); }
PNSLR_ConditionVariable& PNSLR_Bindings_Convert(Panshilar::ConditionVariable& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ConditionVariable& PNSLR_Bindings_Convert(PNSLR_ConditionVariable& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ConditionVariable, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::ConditionVariable, buffer), "buffer offset mismatch");

extern "C" PNSLR_ConditionVariable PNSLR_CreateConditionVariable();
Panshilar::ConditionVariable Panshilar::CreateConditionVariable()
{
    PNSLR_ConditionVariable zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateConditionVariable(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyConditionVariable(PNSLR_ConditionVariable* condvar);
void Panshilar::DestroyConditionVariable(Panshilar::ConditionVariable* condvar)
{
    PNSLR_DestroyConditionVariable(PNSLR_Bindings_Convert(condvar));
}

extern "C" void PNSLR_WaitConditionVariable(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex);
void Panshilar::WaitConditionVariable(Panshilar::ConditionVariable* condvar, Panshilar::Mutex* mutex)
{
    PNSLR_WaitConditionVariable(PNSLR_Bindings_Convert(condvar), PNSLR_Bindings_Convert(mutex));
}

extern "C" PNSLR_B8 PNSLR_WaitConditionVariableTimeout(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex, PNSLR_I32 timeoutNs);
Panshilar::b8 Panshilar::WaitConditionVariableTimeout(Panshilar::ConditionVariable* condvar, Panshilar::Mutex* mutex, Panshilar::i32 timeoutNs)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WaitConditionVariableTimeout(PNSLR_Bindings_Convert(condvar), PNSLR_Bindings_Convert(mutex), PNSLR_Bindings_Convert(timeoutNs)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_SignalConditionVariable(PNSLR_ConditionVariable* condvar);
void Panshilar::SignalConditionVariable(Panshilar::ConditionVariable* condvar)
{
    PNSLR_SignalConditionVariable(PNSLR_Bindings_Convert(condvar));
}

extern "C" void PNSLR_BroadcastConditionVariable(PNSLR_ConditionVariable* condvar);
void Panshilar::BroadcastConditionVariable(Panshilar::ConditionVariable* condvar)
{
    PNSLR_BroadcastConditionVariable(PNSLR_Bindings_Convert(condvar));
}

enum class PNSLR_AllocatorMode : Panshilar::u8 { };
static_assert(sizeof(PNSLR_AllocatorMode) == sizeof(Panshilar::AllocatorMode), "size mismatch");
static_assert(alignof(PNSLR_AllocatorMode) == alignof(Panshilar::AllocatorMode), "align mismatch");
PNSLR_AllocatorMode* PNSLR_Bindings_Convert(Panshilar::AllocatorMode* x) { return reinterpret_cast<PNSLR_AllocatorMode*>(x); }
Panshilar::AllocatorMode* PNSLR_Bindings_Convert(PNSLR_AllocatorMode* x) { return reinterpret_cast<Panshilar::AllocatorMode*>(x); }
PNSLR_AllocatorMode& PNSLR_Bindings_Convert(Panshilar::AllocatorMode& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorMode& PNSLR_Bindings_Convert(PNSLR_AllocatorMode& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_AllocatorCapability : Panshilar::u64 { };
static_assert(sizeof(PNSLR_AllocatorCapability) == sizeof(Panshilar::AllocatorCapability), "size mismatch");
static_assert(alignof(PNSLR_AllocatorCapability) == alignof(Panshilar::AllocatorCapability), "align mismatch");
PNSLR_AllocatorCapability* PNSLR_Bindings_Convert(Panshilar::AllocatorCapability* x) { return reinterpret_cast<PNSLR_AllocatorCapability*>(x); }
Panshilar::AllocatorCapability* PNSLR_Bindings_Convert(PNSLR_AllocatorCapability* x) { return reinterpret_cast<Panshilar::AllocatorCapability*>(x); }
PNSLR_AllocatorCapability& PNSLR_Bindings_Convert(Panshilar::AllocatorCapability& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorCapability& PNSLR_Bindings_Convert(PNSLR_AllocatorCapability& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_AllocatorError : Panshilar::u8 { };
static_assert(sizeof(PNSLR_AllocatorError) == sizeof(Panshilar::AllocatorError), "size mismatch");
static_assert(alignof(PNSLR_AllocatorError) == alignof(Panshilar::AllocatorError), "align mismatch");
PNSLR_AllocatorError* PNSLR_Bindings_Convert(Panshilar::AllocatorError* x) { return reinterpret_cast<PNSLR_AllocatorError*>(x); }
Panshilar::AllocatorError* PNSLR_Bindings_Convert(PNSLR_AllocatorError* x) { return reinterpret_cast<Panshilar::AllocatorError*>(x); }
PNSLR_AllocatorError& PNSLR_Bindings_Convert(Panshilar::AllocatorError& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorError& PNSLR_Bindings_Convert(PNSLR_AllocatorError& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" typedef void* (*PNSLR_AllocatorProcedure)(void* allocatorData, PNSLR_AllocatorMode mode, PNSLR_I32 size, PNSLR_I32 alignment, void* oldMemory, PNSLR_I32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
static_assert(sizeof(PNSLR_AllocatorProcedure) == sizeof(Panshilar::AllocatorProcedure), "size mismatch");
static_assert(alignof(PNSLR_AllocatorProcedure) == alignof(Panshilar::AllocatorProcedure), "align mismatch");
PNSLR_AllocatorProcedure* PNSLR_Bindings_Convert(Panshilar::AllocatorProcedure* x) { return reinterpret_cast<PNSLR_AllocatorProcedure*>(x); }
Panshilar::AllocatorProcedure* PNSLR_Bindings_Convert(PNSLR_AllocatorProcedure* x) { return reinterpret_cast<Panshilar::AllocatorProcedure*>(x); }
PNSLR_AllocatorProcedure& PNSLR_Bindings_Convert(Panshilar::AllocatorProcedure& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorProcedure& PNSLR_Bindings_Convert(PNSLR_AllocatorProcedure& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_Allocator
{
   PNSLR_AllocatorProcedure procedure;
   void* data;
};
static_assert(sizeof(PNSLR_Allocator) == sizeof(Panshilar::Allocator), "size mismatch");
static_assert(alignof(PNSLR_Allocator) == alignof(Panshilar::Allocator), "align mismatch");
PNSLR_Allocator* PNSLR_Bindings_Convert(Panshilar::Allocator* x) { return reinterpret_cast<PNSLR_Allocator*>(x); }
Panshilar::Allocator* PNSLR_Bindings_Convert(PNSLR_Allocator* x) { return reinterpret_cast<Panshilar::Allocator*>(x); }
PNSLR_Allocator& PNSLR_Bindings_Convert(Panshilar::Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Allocator& PNSLR_Bindings_Convert(PNSLR_Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Allocator, procedure) == PNSLR_STRUCT_OFFSET(Panshilar::Allocator, procedure), "procedure offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Allocator, data) == PNSLR_STRUCT_OFFSET(Panshilar::Allocator, data), "data offset mismatch");

typedef struct { PNSLR_Allocator* data; PNSLR_I64 count; } PNSLR_ArraySlice_PNSLR_Allocator;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_Allocator) == sizeof(Panshilar::ArraySlice<Panshilar::Allocator>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_Allocator) == alignof(Panshilar::ArraySlice<Panshilar::Allocator>), "align mismatch");
PNSLR_ArraySlice_PNSLR_Allocator* PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::Allocator>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_Allocator*>(x); }
Panshilar::ArraySlice<Panshilar::Allocator>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Allocator* x) { return reinterpret_cast<Panshilar::ArraySlice<Panshilar::Allocator>*>(x); }
PNSLR_ArraySlice_PNSLR_Allocator& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::Allocator>& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArraySlice<Panshilar::Allocator>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::Allocator>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::Allocator>, data), "data offset mismatch");

extern "C" void* PNSLR_Allocate(PNSLR_Allocator allocator, PNSLR_B8 zeroed, PNSLR_I32 size, PNSLR_I32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::Allocate(Panshilar::Allocator allocator, Panshilar::b8 zeroed, Panshilar::i32 size, Panshilar::i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_Allocate(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void* PNSLR_Resize(PNSLR_Allocator allocator, PNSLR_B8 zeroed, void* oldMemory, PNSLR_I32 oldSize, PNSLR_I32 newSize, PNSLR_I32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::Resize(Panshilar::Allocator allocator, Panshilar::b8 zeroed, void* oldMemory, Panshilar::i32 oldSize, Panshilar::i32 newSize, Panshilar::i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_Resize(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(newSize), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void* PNSLR_DefaultResize(PNSLR_Allocator allocator, PNSLR_B8 zeroed, void* oldMemory, PNSLR_I32 oldSize, PNSLR_I32 newSize, PNSLR_I32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::DefaultResize(Panshilar::Allocator allocator, Panshilar::b8 zeroed, void* oldMemory, Panshilar::i32 oldSize, Panshilar::i32 newSize, Panshilar::i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DefaultResize(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(newSize), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_Free(PNSLR_Allocator allocator, void* memory, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::Free(Panshilar::Allocator allocator, void* memory, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_Free(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(memory), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void PNSLR_FreeAll(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeAll(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeAll(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" PNSLR_U64 PNSLR_QueryAllocatorCapabilities(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::u64 Panshilar::QueryAllocatorCapabilities(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_U64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_QueryAllocatorCapabilities(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Allocator PNSLR_GetAllocator_Nil();
Panshilar::Allocator Panshilar::GetAllocator_Nil()
{
    PNSLR_Allocator zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetAllocator_Nil(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap();
Panshilar::Allocator Panshilar::GetAllocator_DefaultHeap()
{
    PNSLR_Allocator zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetAllocator_DefaultHeap(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void* PNSLR_AllocatorFn_DefaultHeap(void* allocatorData, PNSLR_AllocatorMode mode, PNSLR_I32 size, PNSLR_I32 alignment, void* oldMemory, PNSLR_I32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::AllocatorFn_DefaultHeap(void* allocatorData, Panshilar::AllocatorMode mode, Panshilar::i32 size, Panshilar::i32 alignment, void* oldMemory, Panshilar::i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_DefaultHeap(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_ArenaAllocatorBlock
{
   PNSLR_ArenaAllocatorBlock* previous;
   PNSLR_Allocator allocator;
   void* memory;
   PNSLR_U32 capacity;
   PNSLR_U32 used;
};
static_assert(sizeof(PNSLR_ArenaAllocatorBlock) == sizeof(Panshilar::ArenaAllocatorBlock), "size mismatch");
static_assert(alignof(PNSLR_ArenaAllocatorBlock) == alignof(Panshilar::ArenaAllocatorBlock), "align mismatch");
PNSLR_ArenaAllocatorBlock* PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorBlock* x) { return reinterpret_cast<PNSLR_ArenaAllocatorBlock*>(x); }
Panshilar::ArenaAllocatorBlock* PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorBlock* x) { return reinterpret_cast<Panshilar::ArenaAllocatorBlock*>(x); }
PNSLR_ArenaAllocatorBlock& PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorBlock& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArenaAllocatorBlock& PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorBlock& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorBlock, previous) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorBlock, previous), "previous offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorBlock, allocator) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorBlock, allocator), "allocator offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorBlock, memory) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorBlock, memory), "memory offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorBlock, capacity) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorBlock, capacity), "capacity offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorBlock, used) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorBlock, used), "used offset mismatch");

struct PNSLR_ArenaAllocatorPayload
{
   PNSLR_Allocator backingAllocator;
   PNSLR_ArenaAllocatorBlock* currentBlock;
   PNSLR_U32 totalUsed;
   PNSLR_U32 totalCapacity;
   PNSLR_U32 minimumBlockSize;
   PNSLR_U32 numSnapshots;
};
static_assert(sizeof(PNSLR_ArenaAllocatorPayload) == sizeof(Panshilar::ArenaAllocatorPayload), "size mismatch");
static_assert(alignof(PNSLR_ArenaAllocatorPayload) == alignof(Panshilar::ArenaAllocatorPayload), "align mismatch");
PNSLR_ArenaAllocatorPayload* PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorPayload* x) { return reinterpret_cast<PNSLR_ArenaAllocatorPayload*>(x); }
Panshilar::ArenaAllocatorPayload* PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorPayload* x) { return reinterpret_cast<Panshilar::ArenaAllocatorPayload*>(x); }
PNSLR_ArenaAllocatorPayload& PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorPayload& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArenaAllocatorPayload& PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorPayload& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, backingAllocator) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, backingAllocator), "backingAllocator offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, currentBlock) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, currentBlock), "currentBlock offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, totalUsed) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, totalUsed), "totalUsed offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, totalCapacity) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, totalCapacity), "totalCapacity offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, minimumBlockSize) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, minimumBlockSize), "minimumBlockSize offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorPayload, numSnapshots) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorPayload, numSnapshots), "numSnapshots offset mismatch");

extern "C" PNSLR_Allocator PNSLR_NewAllocator_Arena(PNSLR_Allocator backingAllocator, PNSLR_U32 pageSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::Allocator Panshilar::NewAllocator_Arena(Panshilar::Allocator backingAllocator, Panshilar::u32 pageSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_Allocator zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NewAllocator_Arena(PNSLR_Bindings_Convert(backingAllocator), PNSLR_Bindings_Convert(pageSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyAllocator_Arena(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::DestroyAllocator_Arena(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_DestroyAllocator_Arena(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void* PNSLR_AllocatorFn_Arena(void* allocatorData, PNSLR_AllocatorMode mode, PNSLR_I32 size, PNSLR_I32 alignment, void* oldMemory, PNSLR_I32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::AllocatorFn_Arena(void* allocatorData, Panshilar::AllocatorMode mode, Panshilar::i32 size, Panshilar::i32 alignment, void* oldMemory, Panshilar::i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_Arena(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_ArenaSnapshotError : Panshilar::u8 { };
static_assert(sizeof(PNSLR_ArenaSnapshotError) == sizeof(Panshilar::ArenaSnapshotError), "size mismatch");
static_assert(alignof(PNSLR_ArenaSnapshotError) == alignof(Panshilar::ArenaSnapshotError), "align mismatch");
PNSLR_ArenaSnapshotError* PNSLR_Bindings_Convert(Panshilar::ArenaSnapshotError* x) { return reinterpret_cast<PNSLR_ArenaSnapshotError*>(x); }
Panshilar::ArenaSnapshotError* PNSLR_Bindings_Convert(PNSLR_ArenaSnapshotError* x) { return reinterpret_cast<Panshilar::ArenaSnapshotError*>(x); }
PNSLR_ArenaSnapshotError& PNSLR_Bindings_Convert(Panshilar::ArenaSnapshotError& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArenaSnapshotError& PNSLR_Bindings_Convert(PNSLR_ArenaSnapshotError& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_ArenaAllocatorSnapshot
{
   PNSLR_B8 valid;
   PNSLR_ArenaAllocatorPayload* payload;
   PNSLR_ArenaAllocatorBlock* block;
   PNSLR_U32 used;
};
static_assert(sizeof(PNSLR_ArenaAllocatorSnapshot) == sizeof(Panshilar::ArenaAllocatorSnapshot), "size mismatch");
static_assert(alignof(PNSLR_ArenaAllocatorSnapshot) == alignof(Panshilar::ArenaAllocatorSnapshot), "align mismatch");
PNSLR_ArenaAllocatorSnapshot* PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorSnapshot* x) { return reinterpret_cast<PNSLR_ArenaAllocatorSnapshot*>(x); }
Panshilar::ArenaAllocatorSnapshot* PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorSnapshot* x) { return reinterpret_cast<Panshilar::ArenaAllocatorSnapshot*>(x); }
PNSLR_ArenaAllocatorSnapshot& PNSLR_Bindings_Convert(Panshilar::ArenaAllocatorSnapshot& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArenaAllocatorSnapshot& PNSLR_Bindings_Convert(PNSLR_ArenaAllocatorSnapshot& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorSnapshot, valid) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorSnapshot, valid), "valid offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorSnapshot, payload) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorSnapshot, payload), "payload offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorSnapshot, block) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorSnapshot, block), "block offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArenaAllocatorSnapshot, used) == PNSLR_STRUCT_OFFSET(Panshilar::ArenaAllocatorSnapshot, used), "used offset mismatch");

extern "C" PNSLR_B8 PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Allocator allocator);
Panshilar::b8 Panshilar::ValidateArenaAllocatorSnapshotState(Panshilar::Allocator allocator)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_ArenaAllocatorSnapshot PNSLR_CaptureArenaAllocatorSnapshot(PNSLR_Allocator allocator);
Panshilar::ArenaAllocatorSnapshot Panshilar::CaptureArenaAllocatorSnapshot(Panshilar::Allocator allocator)
{
    PNSLR_ArenaAllocatorSnapshot zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CaptureArenaAllocatorSnapshot(PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_ArenaSnapshotError PNSLR_RestoreArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot, PNSLR_SourceCodeLocation loc);
Panshilar::ArenaSnapshotError Panshilar::RestoreArenaAllocatorSnapshot(Panshilar::ArenaAllocatorSnapshot* snapshot, Panshilar::SourceCodeLocation loc)
{
    PNSLR_ArenaSnapshotError zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_RestoreArenaAllocatorSnapshot(PNSLR_Bindings_Convert(snapshot), PNSLR_Bindings_Convert(loc)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_ArenaSnapshotError PNSLR_DiscardArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot);
Panshilar::ArenaSnapshotError Panshilar::DiscardArenaAllocatorSnapshot(Panshilar::ArenaAllocatorSnapshot* snapshot)
{
    PNSLR_ArenaSnapshotError zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DiscardArenaAllocatorSnapshot(PNSLR_Bindings_Convert(snapshot)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct alignas(8) PNSLR_StackAllocatorPage
{
   PNSLR_StackAllocatorPage* previousPage;
   PNSLR_U64 usedBytes;
   PNSLR_U8 buffer[8192];
};
static_assert(sizeof(PNSLR_StackAllocatorPage) == sizeof(Panshilar::StackAllocatorPage), "size mismatch");
static_assert(alignof(PNSLR_StackAllocatorPage) == alignof(Panshilar::StackAllocatorPage), "align mismatch");
PNSLR_StackAllocatorPage* PNSLR_Bindings_Convert(Panshilar::StackAllocatorPage* x) { return reinterpret_cast<PNSLR_StackAllocatorPage*>(x); }
Panshilar::StackAllocatorPage* PNSLR_Bindings_Convert(PNSLR_StackAllocatorPage* x) { return reinterpret_cast<Panshilar::StackAllocatorPage*>(x); }
PNSLR_StackAllocatorPage& PNSLR_Bindings_Convert(Panshilar::StackAllocatorPage& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StackAllocatorPage& PNSLR_Bindings_Convert(PNSLR_StackAllocatorPage& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPage, previousPage) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPage, previousPage), "previousPage offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPage, usedBytes) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPage, usedBytes), "usedBytes offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPage, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPage, buffer), "buffer offset mismatch");

struct PNSLR_StackAllocationHeader
{
   PNSLR_StackAllocatorPage* page;
   PNSLR_I32 size;
   PNSLR_I32 alignment;
   void* lastAllocation;
   void* lastAllocationHeader;
};
static_assert(sizeof(PNSLR_StackAllocationHeader) == sizeof(Panshilar::StackAllocationHeader), "size mismatch");
static_assert(alignof(PNSLR_StackAllocationHeader) == alignof(Panshilar::StackAllocationHeader), "align mismatch");
PNSLR_StackAllocationHeader* PNSLR_Bindings_Convert(Panshilar::StackAllocationHeader* x) { return reinterpret_cast<PNSLR_StackAllocationHeader*>(x); }
Panshilar::StackAllocationHeader* PNSLR_Bindings_Convert(PNSLR_StackAllocationHeader* x) { return reinterpret_cast<Panshilar::StackAllocationHeader*>(x); }
PNSLR_StackAllocationHeader& PNSLR_Bindings_Convert(Panshilar::StackAllocationHeader& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StackAllocationHeader& PNSLR_Bindings_Convert(PNSLR_StackAllocationHeader& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocationHeader, page) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocationHeader, page), "page offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocationHeader, size) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocationHeader, size), "size offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocationHeader, alignment) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocationHeader, alignment), "alignment offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocationHeader, lastAllocation) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocationHeader, lastAllocation), "lastAllocation offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocationHeader, lastAllocationHeader) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocationHeader, lastAllocationHeader), "lastAllocationHeader offset mismatch");

struct PNSLR_StackAllocatorPayload
{
   PNSLR_Allocator backingAllocator;
   PNSLR_StackAllocatorPage* currentPage;
   void* lastAllocation;
   PNSLR_StackAllocationHeader* lastAllocationHeader;
};
static_assert(sizeof(PNSLR_StackAllocatorPayload) == sizeof(Panshilar::StackAllocatorPayload), "size mismatch");
static_assert(alignof(PNSLR_StackAllocatorPayload) == alignof(Panshilar::StackAllocatorPayload), "align mismatch");
PNSLR_StackAllocatorPayload* PNSLR_Bindings_Convert(Panshilar::StackAllocatorPayload* x) { return reinterpret_cast<PNSLR_StackAllocatorPayload*>(x); }
Panshilar::StackAllocatorPayload* PNSLR_Bindings_Convert(PNSLR_StackAllocatorPayload* x) { return reinterpret_cast<Panshilar::StackAllocatorPayload*>(x); }
PNSLR_StackAllocatorPayload& PNSLR_Bindings_Convert(Panshilar::StackAllocatorPayload& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StackAllocatorPayload& PNSLR_Bindings_Convert(PNSLR_StackAllocatorPayload& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPayload, backingAllocator) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPayload, backingAllocator), "backingAllocator offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPayload, currentPage) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPayload, currentPage), "currentPage offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPayload, lastAllocation) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPayload, lastAllocation), "lastAllocation offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StackAllocatorPayload, lastAllocationHeader) == PNSLR_STRUCT_OFFSET(Panshilar::StackAllocatorPayload, lastAllocationHeader), "lastAllocationHeader offset mismatch");

extern "C" PNSLR_Allocator PNSLR_NewAllocator_Stack(PNSLR_Allocator backingAllocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::Allocator Panshilar::NewAllocator_Stack(Panshilar::Allocator backingAllocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_Allocator zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NewAllocator_Stack(PNSLR_Bindings_Convert(backingAllocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyAllocator_Stack(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::DestroyAllocator_Stack(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_DestroyAllocator_Stack(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void* PNSLR_AllocatorFn_Stack(void* allocatorData, PNSLR_AllocatorMode mode, PNSLR_I32 size, PNSLR_I32 alignment, void* oldMemory, PNSLR_I32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void* Panshilar::AllocatorFn_Stack(void* allocatorData, Panshilar::AllocatorMode mode, Panshilar::i32 size, Panshilar::i32 alignment, void* oldMemory, Panshilar::i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    void* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_Stack(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_RawArraySlice PNSLR_MakeRawSlice(PNSLR_I32 tySize, PNSLR_I32 tyAlign, PNSLR_I64 count, PNSLR_B8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::RawArraySlice Panshilar::MakeRawSlice(Panshilar::i32 tySize, Panshilar::i32 tyAlign, Panshilar::i64 count, Panshilar::b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_RawArraySlice zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeRawSlice(PNSLR_Bindings_Convert(tySize), PNSLR_Bindings_Convert(tyAlign), PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeRawSlice(PNSLR_RawArraySlice* slice, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeRawSlice(Panshilar::RawArraySlice* slice, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeRawSlice(PNSLR_Bindings_Convert(slice), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void PNSLR_ResizeRawSlice(PNSLR_RawArraySlice* slice, PNSLR_I32 tySize, PNSLR_I32 tyAlign, PNSLR_I64 newCount, PNSLR_B8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::ResizeRawSlice(Panshilar::RawArraySlice* slice, Panshilar::i32 tySize, Panshilar::i32 tyAlign, Panshilar::i64 newCount, Panshilar::b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_ResizeRawSlice(PNSLR_Bindings_Convert(slice), PNSLR_Bindings_Convert(tySize), PNSLR_Bindings_Convert(tyAlign), PNSLR_Bindings_Convert(newCount), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" PNSLR_UTF8STR PNSLR_MakeString(PNSLR_I64 count, PNSLR_B8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::utf8str Panshilar::MakeString(Panshilar::i64 count, Panshilar::b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeString(PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeString(PNSLR_UTF8STR str, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeString(Panshilar::utf8str str, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" char* PNSLR_MakeCString(PNSLR_I64 count, PNSLR_B8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
char* Panshilar::MakeCString(Panshilar::i64 count, Panshilar::b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    char* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeCString(PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeCString(char* str, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeCString(char* str, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" PNSLR_I64 PNSLR_NanosecondsSinceUnixEpoch();
Panshilar::i64 Panshilar::NanosecondsSinceUnixEpoch()
{
    PNSLR_I64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NanosecondsSinceUnixEpoch(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I32 PNSLR_GetCStringLength(char* str);
Panshilar::i32 Panshilar::GetCStringLength(char* str)
{
    PNSLR_I32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetCStringLength(PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromCString(char* str);
Panshilar::utf8str Panshilar::StringFromCString(char* str)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromCString(PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" char* PNSLR_CStringFromString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
char* Panshilar::CStringFromString(Panshilar::utf8str str, Panshilar::Allocator allocator)
{
    char* zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringFromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_CloneString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
Panshilar::utf8str Panshilar::CloneString(Panshilar::utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CloneString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_ConcatenateStrings(PNSLR_UTF8STR str1, PNSLR_UTF8STR str2, PNSLR_Allocator allocator);
Panshilar::utf8str Panshilar::ConcatenateStrings(Panshilar::utf8str str1, Panshilar::utf8str str2, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ConcatenateStrings(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_UpperString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
Panshilar::utf8str Panshilar::UpperString(Panshilar::utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_UpperString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_LowerString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
Panshilar::utf8str Panshilar::LowerString(Panshilar::utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_LowerString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_StringComparisonType : Panshilar::u8 { };
static_assert(sizeof(PNSLR_StringComparisonType) == sizeof(Panshilar::StringComparisonType), "size mismatch");
static_assert(alignof(PNSLR_StringComparisonType) == alignof(Panshilar::StringComparisonType), "align mismatch");
PNSLR_StringComparisonType* PNSLR_Bindings_Convert(Panshilar::StringComparisonType* x) { return reinterpret_cast<PNSLR_StringComparisonType*>(x); }
Panshilar::StringComparisonType* PNSLR_Bindings_Convert(PNSLR_StringComparisonType* x) { return reinterpret_cast<Panshilar::StringComparisonType*>(x); }
PNSLR_StringComparisonType& PNSLR_Bindings_Convert(Panshilar::StringComparisonType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StringComparisonType& PNSLR_Bindings_Convert(PNSLR_StringComparisonType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" PNSLR_B8 PNSLR_AreStringsEqual(PNSLR_UTF8STR str1, PNSLR_UTF8STR str2, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::AreStringsEqual(Panshilar::utf8str str1, Panshilar::utf8str str2, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreStringsEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_AreStringAndCStringEqual(PNSLR_UTF8STR str1, char* str2, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::AreStringAndCStringEqual(Panshilar::utf8str str1, char* str2, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreStringAndCStringEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_AreCStringsEqual(char* str1, char* str2, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::AreCStringsEqual(char* str1, char* str2, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreCStringsEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_StringStartsWith(PNSLR_UTF8STR str, PNSLR_UTF8STR prefix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::StringStartsWith(Panshilar::utf8str str, Panshilar::utf8str prefix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringStartsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_StringEndsWith(PNSLR_UTF8STR str, PNSLR_UTF8STR suffix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::StringEndsWith(Panshilar::utf8str str, Panshilar::utf8str suffix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringEndsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_StringStartsWithCString(PNSLR_UTF8STR str, char* prefix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::StringStartsWithCString(Panshilar::utf8str str, char* prefix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringStartsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_StringEndsWithCString(PNSLR_UTF8STR str, char* suffix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::StringEndsWithCString(Panshilar::utf8str str, char* suffix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringEndsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CStringStartsWith(char* str, PNSLR_UTF8STR prefix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::CStringStartsWith(char* str, Panshilar::utf8str prefix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringStartsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CStringEndsWith(char* str, PNSLR_UTF8STR suffix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::CStringEndsWith(char* str, Panshilar::utf8str suffix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringEndsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CStringStartsWithCString(PNSLR_UTF8STR str, char* prefix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::CStringStartsWithCString(Panshilar::utf8str str, char* prefix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringStartsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CStringEndsWithCString(PNSLR_UTF8STR str, char* suffix, PNSLR_StringComparisonType comparisonType);
Panshilar::b8 Panshilar::CStringEndsWithCString(Panshilar::utf8str str, char* suffix, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringEndsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I32 PNSLR_SearchFirstIndexInString(PNSLR_UTF8STR str, PNSLR_UTF8STR substring, PNSLR_StringComparisonType comparisonType);
Panshilar::i32 Panshilar::SearchFirstIndexInString(Panshilar::utf8str str, Panshilar::utf8str substring, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_I32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SearchFirstIndexInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(substring), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I32 PNSLR_SearchLastIndexInString(PNSLR_UTF8STR str, PNSLR_UTF8STR substring, PNSLR_StringComparisonType comparisonType);
Panshilar::i32 Panshilar::SearchLastIndexInString(Panshilar::utf8str str, Panshilar::utf8str substring, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_I32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SearchLastIndexInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(substring), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_ReplaceInString(PNSLR_UTF8STR str, PNSLR_UTF8STR oldValue, PNSLR_UTF8STR newValue, PNSLR_Allocator allocator, PNSLR_StringComparisonType comparisonType);
Panshilar::utf8str Panshilar::ReplaceInString(Panshilar::utf8str str, Panshilar::utf8str oldValue, Panshilar::utf8str newValue, Panshilar::Allocator allocator, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReplaceInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(oldValue), PNSLR_Bindings_Convert(newValue), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_EncodedRune
{
   PNSLR_U8 data[4];
   PNSLR_I32 length;
};
static_assert(sizeof(PNSLR_EncodedRune) == sizeof(Panshilar::EncodedRune), "size mismatch");
static_assert(alignof(PNSLR_EncodedRune) == alignof(Panshilar::EncodedRune), "align mismatch");
PNSLR_EncodedRune* PNSLR_Bindings_Convert(Panshilar::EncodedRune* x) { return reinterpret_cast<PNSLR_EncodedRune*>(x); }
Panshilar::EncodedRune* PNSLR_Bindings_Convert(PNSLR_EncodedRune* x) { return reinterpret_cast<Panshilar::EncodedRune*>(x); }
PNSLR_EncodedRune& PNSLR_Bindings_Convert(Panshilar::EncodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::EncodedRune& PNSLR_Bindings_Convert(PNSLR_EncodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_EncodedRune, data) == PNSLR_STRUCT_OFFSET(Panshilar::EncodedRune, data), "data offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_EncodedRune, length) == PNSLR_STRUCT_OFFSET(Panshilar::EncodedRune, length), "length offset mismatch");

struct PNSLR_DecodedRune
{
   PNSLR_U32 rune;
   PNSLR_I32 length;
};
static_assert(sizeof(PNSLR_DecodedRune) == sizeof(Panshilar::DecodedRune), "size mismatch");
static_assert(alignof(PNSLR_DecodedRune) == alignof(Panshilar::DecodedRune), "align mismatch");
PNSLR_DecodedRune* PNSLR_Bindings_Convert(Panshilar::DecodedRune* x) { return reinterpret_cast<PNSLR_DecodedRune*>(x); }
Panshilar::DecodedRune* PNSLR_Bindings_Convert(PNSLR_DecodedRune* x) { return reinterpret_cast<Panshilar::DecodedRune*>(x); }
PNSLR_DecodedRune& PNSLR_Bindings_Convert(Panshilar::DecodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::DecodedRune& PNSLR_Bindings_Convert(PNSLR_DecodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_DecodedRune, rune) == PNSLR_STRUCT_OFFSET(Panshilar::DecodedRune, rune), "rune offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_DecodedRune, length) == PNSLR_STRUCT_OFFSET(Panshilar::DecodedRune, length), "length offset mismatch");

extern "C" PNSLR_I32 PNSLR_GetRuneLength(PNSLR_U32 r);
Panshilar::i32 Panshilar::GetRuneLength(Panshilar::u32 r)
{
    PNSLR_I32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetRuneLength(PNSLR_Bindings_Convert(r)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_EncodedRune PNSLR_EncodeRune(PNSLR_U32 c);
Panshilar::EncodedRune Panshilar::EncodeRune(Panshilar::u32 c)
{
    PNSLR_EncodedRune zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_EncodeRune(PNSLR_Bindings_Convert(c)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_DecodedRune PNSLR_DecodeRune(PNSLR_ArraySlice_PNSLR_U8 s);
Panshilar::DecodedRune Panshilar::DecodeRune(Panshilar::ArraySlice<Panshilar::u8> s)
{
    PNSLR_DecodedRune zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DecodeRune(PNSLR_Bindings_Convert(s)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_Path
{
   PNSLR_UTF8STR path;
};
static_assert(sizeof(PNSLR_Path) == sizeof(Panshilar::Path), "size mismatch");
static_assert(alignof(PNSLR_Path) == alignof(Panshilar::Path), "align mismatch");
PNSLR_Path* PNSLR_Bindings_Convert(Panshilar::Path* x) { return reinterpret_cast<PNSLR_Path*>(x); }
Panshilar::Path* PNSLR_Bindings_Convert(PNSLR_Path* x) { return reinterpret_cast<Panshilar::Path*>(x); }
PNSLR_Path& PNSLR_Bindings_Convert(Panshilar::Path& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Path& PNSLR_Bindings_Convert(PNSLR_Path& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Path, path) == PNSLR_STRUCT_OFFSET(Panshilar::Path, path), "path offset mismatch");

enum class PNSLR_PathNormalisationType : Panshilar::u8 { };
static_assert(sizeof(PNSLR_PathNormalisationType) == sizeof(Panshilar::PathNormalisationType), "size mismatch");
static_assert(alignof(PNSLR_PathNormalisationType) == alignof(Panshilar::PathNormalisationType), "align mismatch");
PNSLR_PathNormalisationType* PNSLR_Bindings_Convert(Panshilar::PathNormalisationType* x) { return reinterpret_cast<PNSLR_PathNormalisationType*>(x); }
Panshilar::PathNormalisationType* PNSLR_Bindings_Convert(PNSLR_PathNormalisationType* x) { return reinterpret_cast<Panshilar::PathNormalisationType*>(x); }
PNSLR_PathNormalisationType& PNSLR_Bindings_Convert(Panshilar::PathNormalisationType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PathNormalisationType& PNSLR_Bindings_Convert(PNSLR_PathNormalisationType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" PNSLR_Path PNSLR_NormalisePath(PNSLR_UTF8STR path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::NormalisePath(Panshilar::utf8str path, Panshilar::PathNormalisationType type, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NormalisePath(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(type), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_SplitPath(PNSLR_Path path, PNSLR_Path* parent, PNSLR_UTF8STR* selfNameWithExtension, PNSLR_UTF8STR* selfName, PNSLR_UTF8STR* extension);
Panshilar::b8 Panshilar::SplitPath(Panshilar::Path path, Panshilar::Path* parent, Panshilar::utf8str* selfNameWithExtension, Panshilar::utf8str* selfName, Panshilar::utf8str* extension)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SplitPath(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(parent), PNSLR_Bindings_Convert(selfNameWithExtension), PNSLR_Bindings_Convert(selfName), PNSLR_Bindings_Convert(extension)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Path PNSLR_GetPathForChildFile(PNSLR_Path dir, PNSLR_UTF8STR fileNameWithExtension, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::GetPathForChildFile(Panshilar::Path dir, Panshilar::utf8str fileNameWithExtension, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetPathForChildFile(PNSLR_Bindings_Convert(dir), PNSLR_Bindings_Convert(fileNameWithExtension), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Path PNSLR_GetPathForSubdirectory(PNSLR_Path dir, PNSLR_UTF8STR dirName, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::GetPathForSubdirectory(Panshilar::Path dir, Panshilar::utf8str dirName, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetPathForSubdirectory(PNSLR_Bindings_Convert(dir), PNSLR_Bindings_Convert(dirName), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" typedef PNSLR_B8 (*PNSLR_DirectoryIterationVisitorDelegate)(void* payload, PNSLR_Path path, PNSLR_B8 isDirectory, PNSLR_B8* exploreCurrentDirectory);
static_assert(sizeof(PNSLR_DirectoryIterationVisitorDelegate) == sizeof(Panshilar::DirectoryIterationVisitorDelegate), "size mismatch");
static_assert(alignof(PNSLR_DirectoryIterationVisitorDelegate) == alignof(Panshilar::DirectoryIterationVisitorDelegate), "align mismatch");
PNSLR_DirectoryIterationVisitorDelegate* PNSLR_Bindings_Convert(Panshilar::DirectoryIterationVisitorDelegate* x) { return reinterpret_cast<PNSLR_DirectoryIterationVisitorDelegate*>(x); }
Panshilar::DirectoryIterationVisitorDelegate* PNSLR_Bindings_Convert(PNSLR_DirectoryIterationVisitorDelegate* x) { return reinterpret_cast<Panshilar::DirectoryIterationVisitorDelegate*>(x); }
PNSLR_DirectoryIterationVisitorDelegate& PNSLR_Bindings_Convert(Panshilar::DirectoryIterationVisitorDelegate& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::DirectoryIterationVisitorDelegate& PNSLR_Bindings_Convert(PNSLR_DirectoryIterationVisitorDelegate& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" void PNSLR_IterateDirectory(PNSLR_Path path, PNSLR_B8 recursive, void* visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc);
void Panshilar::IterateDirectory(Panshilar::Path path, Panshilar::b8 recursive, void* visitorPayload, Panshilar::DirectoryIterationVisitorDelegate visitorFunc)
{
    PNSLR_IterateDirectory(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(recursive), PNSLR_Bindings_Convert(visitorPayload), PNSLR_Bindings_Convert(visitorFunc));
}

enum class PNSLR_PathExistsCheckType : Panshilar::u8 { };
static_assert(sizeof(PNSLR_PathExistsCheckType) == sizeof(Panshilar::PathExistsCheckType), "size mismatch");
static_assert(alignof(PNSLR_PathExistsCheckType) == alignof(Panshilar::PathExistsCheckType), "align mismatch");
PNSLR_PathExistsCheckType* PNSLR_Bindings_Convert(Panshilar::PathExistsCheckType* x) { return reinterpret_cast<PNSLR_PathExistsCheckType*>(x); }
Panshilar::PathExistsCheckType* PNSLR_Bindings_Convert(PNSLR_PathExistsCheckType* x) { return reinterpret_cast<Panshilar::PathExistsCheckType*>(x); }
PNSLR_PathExistsCheckType& PNSLR_Bindings_Convert(Panshilar::PathExistsCheckType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PathExistsCheckType& PNSLR_Bindings_Convert(PNSLR_PathExistsCheckType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" PNSLR_B8 PNSLR_PathExists(PNSLR_Path path, PNSLR_PathExistsCheckType type);
Panshilar::b8 Panshilar::PathExists(Panshilar::Path path, Panshilar::PathExistsCheckType type)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_PathExists(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(type)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_DeletePath(PNSLR_Path path);
Panshilar::b8 Panshilar::DeletePath(Panshilar::Path path)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DeletePath(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I64 PNSLR_GetFileTimestamp(PNSLR_Path path);
Panshilar::i64 Panshilar::GetFileTimestamp(Panshilar::Path path)
{
    PNSLR_I64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetFileTimestamp(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I64 PNSLR_GetFileSize(PNSLR_Path path);
Panshilar::i64 Panshilar::GetFileSize(Panshilar::Path path)
{
    PNSLR_I64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetFileSize(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CreateDirectoryTree(PNSLR_Path path);
Panshilar::b8 Panshilar::CreateDirectoryTree(Panshilar::Path path)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateDirectoryTree(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_File
{
   void* handle;
};
static_assert(sizeof(PNSLR_File) == sizeof(Panshilar::File), "size mismatch");
static_assert(alignof(PNSLR_File) == alignof(Panshilar::File), "align mismatch");
PNSLR_File* PNSLR_Bindings_Convert(Panshilar::File* x) { return reinterpret_cast<PNSLR_File*>(x); }
Panshilar::File* PNSLR_Bindings_Convert(PNSLR_File* x) { return reinterpret_cast<Panshilar::File*>(x); }
PNSLR_File& PNSLR_Bindings_Convert(Panshilar::File& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::File& PNSLR_Bindings_Convert(PNSLR_File& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_File, handle) == PNSLR_STRUCT_OFFSET(Panshilar::File, handle), "handle offset mismatch");

extern "C" PNSLR_File PNSLR_OpenFileToRead(PNSLR_Path path, PNSLR_B8 allowWrite);
Panshilar::File Panshilar::OpenFileToRead(Panshilar::Path path, Panshilar::b8 allowWrite)
{
    PNSLR_File zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_OpenFileToRead(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(allowWrite)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_File PNSLR_OpenFileToWrite(PNSLR_Path path, PNSLR_B8 append, PNSLR_B8 allowRead);
Panshilar::File Panshilar::OpenFileToWrite(Panshilar::Path path, Panshilar::b8 append, Panshilar::b8 allowRead)
{
    PNSLR_File zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_OpenFileToWrite(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(append), PNSLR_Bindings_Convert(allowRead)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I64 PNSLR_GetSizeOfFile(PNSLR_File handle);
Panshilar::i64 Panshilar::GetSizeOfFile(Panshilar::File handle)
{
    PNSLR_I64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetSizeOfFile(PNSLR_Bindings_Convert(handle)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_SeekPositionInFile(PNSLR_File handle, PNSLR_I64 newPos, PNSLR_B8 relative);
Panshilar::b8 Panshilar::SeekPositionInFile(Panshilar::File handle, Panshilar::i64 newPos, Panshilar::b8 relative)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SeekPositionInFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(newPos), PNSLR_Bindings_Convert(relative)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_ReadFromFile(PNSLR_File handle, PNSLR_ArraySlice_PNSLR_U8 dst);
Panshilar::b8 Panshilar::ReadFromFile(Panshilar::File handle, Panshilar::ArraySlice<Panshilar::u8> dst)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReadFromFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(dst)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_WriteToFile(PNSLR_File handle, PNSLR_ArraySlice_PNSLR_U8 src);
Panshilar::b8 Panshilar::WriteToFile(Panshilar::File handle, Panshilar::ArraySlice<Panshilar::u8> src)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WriteToFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(src)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_TruncateFile(PNSLR_File handle, PNSLR_I64 newSize);
Panshilar::b8 Panshilar::TruncateFile(Panshilar::File handle, Panshilar::i64 newSize)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TruncateFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(newSize)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_FlushFile(PNSLR_File handle);
Panshilar::b8 Panshilar::FlushFile(Panshilar::File handle)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FlushFile(PNSLR_Bindings_Convert(handle)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_CloseFileHandle(PNSLR_File handle);
void Panshilar::CloseFileHandle(Panshilar::File handle)
{
    PNSLR_CloseFileHandle(PNSLR_Bindings_Convert(handle));
}

extern "C" PNSLR_B8 PNSLR_ReadAllContentsFromFile(PNSLR_Path path, PNSLR_ArraySlice_PNSLR_U8* dst, PNSLR_Allocator allocator);
Panshilar::b8 Panshilar::ReadAllContentsFromFile(Panshilar::Path path, Panshilar::ArraySlice<Panshilar::u8>* dst, Panshilar::Allocator allocator)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReadAllContentsFromFile(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(dst), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_WriteAllContentsToFile(PNSLR_Path path, PNSLR_ArraySlice_PNSLR_U8 src, PNSLR_B8 append);
Panshilar::b8 Panshilar::WriteAllContentsToFile(Panshilar::Path path, Panshilar::ArraySlice<Panshilar::u8> src, Panshilar::b8 append)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WriteAllContentsToFile(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(append)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_CopyFile(PNSLR_Path src, PNSLR_Path dst);
Panshilar::b8 Panshilar::CopyFile(Panshilar::Path src, Panshilar::Path dst)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CopyFile(PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(dst)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_B8 PNSLR_MoveFile(PNSLR_Path src, PNSLR_Path dst);
Panshilar::b8 Panshilar::MoveFile(Panshilar::Path src, Panshilar::Path dst)
{
    PNSLR_B8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MoveFile(PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(dst)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_I32 PNSLR_PrintToStdOut(PNSLR_UTF8STR message);
Panshilar::i32 Panshilar::PrintToStdOut(Panshilar::utf8str message)
{
    PNSLR_I32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_PrintToStdOut(PNSLR_Bindings_Convert(message)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_ExitProcess(PNSLR_I32 exitCode);
void Panshilar::ExitProcess(Panshilar::i32 exitCode)
{
    PNSLR_ExitProcess(PNSLR_Bindings_Convert(exitCode));
}


#undef PNSLR_STRUCT_OFFSET

#endif//PNSLR_SKIP_IMPLEMENTATION
#endif//__cplusplus
