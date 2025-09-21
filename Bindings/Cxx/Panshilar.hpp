#ifndef __cplusplus
    #error "Please use the C bindings.";
#endif

#ifndef PNSLR_CXX_MAIN_H
#define PNSLR_CXX_MAIN_H

#include "Intrinsics.hpp"

namespace Panshilar
{
    using namespace Intrinsics;

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

    // Read-Write Mutex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

    // Condition Variable ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    // Memory
    // #######################################################################################

    /**
     * Set a block of memory to a specific value.
     */
    void MemSet(
        rawptr memory,
        i32 value,
        i32 size
    );

    /**
     * Copy a block of memory from source to destination.
     */
    void MemCopy(
        rawptr destination,
        rawptr source,
        i32 size
    );

    /**
     * Copy a block of memory from source to destination, handling overlapping regions.
     */
    void MemMove(
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
    typedef rawptr (*AllocatorProcedure)(
        rawptr allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        rawptr oldMemory,
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
       rawptr data;
    };

    // Allocation ease-of-use functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Allocate memory using the provided allocator.
     */
    rawptr Allocate(
        Allocator allocator,
        b8 zeroed,
        i32 size,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Resize memory using the provided allocator.
     */
    rawptr Resize(
        Allocator allocator,
        b8 zeroed,
        rawptr oldMemory,
        i32 oldSize,
        i32 newSize,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Fallback resize function that can be used when the allocator does not support resizing.
     */
    rawptr DefaultResize(
        Allocator allocator,
        b8 zeroed,
        rawptr oldMemory,
        i32 oldSize,
        i32 newSize,
        i32 alignment,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Free memory using the provided allocator.
     */
    void Free(
        Allocator allocator,
        rawptr memory,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Free all memory allocated by the provided allocator.
     */
    void FreeAll(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Query the capabilities of the provided allocator.
     */
    u64 QueryAllocatorCapabilities(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    // Nil allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
     * Otherwise no-ops all around.
     */
    Allocator GetAllocator_Nil();

    // Default Heap Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Get the default heap allocator.
     */
    Allocator GetAllocator_DefaultHeap();

    /**
     * Main allocator function for the default heap allocator.
     */
    rawptr AllocatorFn_DefaultHeap(
        rawptr allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        rawptr oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // Arena Alloator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A block of memory used by the arena allocator.
     */
    struct ArenaAllocatorBlock
    {
       ArenaAllocatorBlock* previous;
       Allocator allocator;
       rawptr memory;
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
        AllocatorError* error = { }
    );

    /**
     * Destroy an arena allocator and free all its resources.
     * This does not free the backing allocator, only the arena allocator's own resources.
     */
    void DestroyAllocator_Arena(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Main allocator function for the arena allocator.
     */
    rawptr AllocatorFn_Arena(
        rawptr allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        rawptr oldMemory,
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

    // Stack Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
       rawptr lastAllocation;
       rawptr lastAllocationHeader;
    };

    /**
     * The payload used by the stack allocator.
     */
    struct StackAllocatorPayload
    {
       Allocator backingAllocator;
       StackAllocatorPage* currentPage;
       rawptr lastAllocation;
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
        AllocatorError* error = { }
    );

    /**
     * Destroy a stack allocator and free all its resources.
     * This does not free the backing allocator, only the stack allocator's own resources.
     */
    void DestroyAllocator_Stack(
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Main allocator function for the stack allocator.
     */
    rawptr AllocatorFn_Stack(
        rawptr allocatorData,
        AllocatorMode mode,
        i32 size,
        i32 alignment,
        rawptr oldMemory,
        i32 oldSize,
        SourceCodeLocation location,
        AllocatorError* error
    );

    // Collections make/free functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
        AllocatorError* error = { }
    );

    /**
     * Free a raw array slice allocated with `PNSLR_MakeRawSlice`, using the provided allocator.
     */
    void FreeRawSlice(
        RawArraySlice* slice,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
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
        AllocatorError* error = { }
    );

    /**
     * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
     */
    utf8str MakeString(
        i64 count,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
     */
    void FreeString(
        utf8str str,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
     */
    cstring MakeCString(
        i64 count,
        b8 zeroed,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
    );

    /**
     * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
     */
    void FreeCString(
        cstring str,
        Allocator allocator,
        SourceCodeLocation location,
        AllocatorError* error = { }
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
        cstring str
    );

    /**
     * Clone a C-style string into a new allocated string.
     */
    utf8str StringFromCString(
        cstring str
    );

    /**
     * Clones a UTF-8 string to a C-style null-terminated string.
     * The returned string is allocated using the specified allocator.
     */
    cstring CStringFromString(
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
        StringComparisonType comparisonType = { }
    );

    /**
     * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
     * Returns true if they are equal, false otherwise.
     */
    b8 AreStringAndCStringEqual(
        utf8str str1,
        cstring str2,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if two C-style null-terminated strings are equal.
     * Returns true if they are equal, false otherwise.
     */
    b8 AreCStringsEqual(
        cstring str1,
        cstring str2,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a UTF-8 string starts with the specified prefix.
     * Returns true if it does, false otherwise.
     */
    b8 StringStartsWith(
        utf8str str,
        utf8str prefix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a UTF-8 string ends with the specified suffix.
     * Returns true if it does, false otherwise.
     */
    b8 StringEndsWith(
        utf8str str,
        utf8str suffix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a C-style null-terminated string starts with the specified prefix.
     */
    b8 StringStartsWithCString(
        utf8str str,
        cstring prefix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified suffix.
     */
    b8 StringEndsWithCString(
        utf8str str,
        cstring suffix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Returns the length of the given C-style null-terminated string, excluding the null terminator.
     */
    b8 CStringStartsWith(
        cstring str,
        utf8str prefix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
    b8 CStringEndsWith(
        cstring str,
        utf8str suffix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
     */
    b8 CStringStartsWithCString(
        utf8str str,
        cstring prefix,
        StringComparisonType comparisonType = { }
    );

    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
    b8 CStringEndsWithCString(
        utf8str str,
        cstring suffix,
        StringComparisonType comparisonType = { }
    );

    // Advanced comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Searches for the first occurrence of a substring within a string.
     * Returns the index of the first occurrence, or -1 if not found.
     */
    i32 SearchFirstIndexInString(
        utf8str str,
        utf8str substring,
        StringComparisonType comparisonType = { }
    );

    /**
     * Searches for the last occurrence of a substring within a string.
     * Returns the index of the last occurrence, or -1 if not found.
     */
    i32 SearchLastIndexInString(
        utf8str str,
        utf8str substring,
        StringComparisonType comparisonType = { }
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
        StringComparisonType comparisonType = { }
    );

    // UTF-8 functionalities ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

    // Windows-specific bs for UTF-16 conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Converts a UTF-8 string to a UTF-16 string.
     * The returned string is allocated using the specified allocator.
     * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
     */
    ArraySlice<u16> UTF16FromUTF8WindowsOnly(
        utf8str str,
        Allocator allocator
    );

    /**
     * Converts a UTF-16 string to a UTF-8 string.
     * The returned string is allocated using the specified allocator.
     * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
     */
    utf8str UTF8FromUTF16WindowsOnly(
        ArraySlice<u16> utf16str,
        Allocator allocator
    );

    // String Builder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * A basic string builder. Can accept strings and characters,
     * and build a single string from them.
     *
     * Create by setting the allocator and zeroing the rest of the fields.
     */
    struct StringBuilder
    {
       Allocator allocator;
       ArraySlice<u8> buffer;
       i64 writtenSize;
       i64 cursorPos;
    };

    /**
     * Append a single byte to the string builder. Could be an ANSI/ASCII character,
     * or not. The function does not check for validity.
     */
    b8 AppendByteToStringBuilder(
        StringBuilder* builder,
        u8 byte
    );

    /**
     * Append a UTF-8 string to the string builder.
     */
    b8 AppendStringToStringBuilder(
        StringBuilder* builder,
        utf8str str
    );

    /**
     * Append a C-style null-terminated string to the string builder.
     */
    b8 AppendCStringToStringBuilder(
        StringBuilder* builder,
        cstring str
    );

    /**
     * Append a single character (rune) to the string builder.
     */
    b8 AppendRuneToStringBuilder(
        StringBuilder* builder,
        u32 rune
    );

    /**
     * Append an 8-bit boolean value to the string builder.
     */
    b8 AppendB8ToStringBuilder(
        StringBuilder* builder,
        b8 value
    );

    /**
     * Append a 32-bit floating-point number to the string builder.
     */
    b8 AppendF32ToStringBuilder(
        StringBuilder* builder,
        f32 value,
        i32 decimalPlaces
    );

    /**
     * Append a 64-bit floating point number to the string builder.
     */
    b8 AppendF64ToStringBuilder(
        StringBuilder* builder,
        f64 value,
        i32 decimalPlaces
    );

    /**
     * The base to use when appending integer numbers to the string builder.
     */
    enum class IntegerBase : u8 /* use as value */
    {
        Decimal = 0,
        Binary = 1,
        HexaDecimal = 2,
        Octal = 3,
    };

    /**
     * Append an unsigned 8-bit integer to the string builder.
     */
    b8 AppendU8ToStringBuilder(
        StringBuilder* builder,
        u8 value,
        IntegerBase base = { }
    );

    /**
     * Append an unsigned 16-bit integer to the string builder.
     */
    b8 AppendU16ToStringBuilder(
        StringBuilder* builder,
        u16 value,
        IntegerBase base = { }
    );

    /**
     * Append an unsigned 32-bit integer number to the string builder.
     */
    b8 AppendU32ToStringBuilder(
        StringBuilder* builder,
        u32 value,
        IntegerBase base = { }
    );

    /**
     * Append an unsigned 64-bit integer to the string builder.
     */
    b8 AppendU64ToStringBuilder(
        StringBuilder* builder,
        u64 value,
        IntegerBase base = { }
    );

    /**
     * Append a signed 8-bit integer to the string builder.
     */
    b8 AppendI8ToStringBuilder(
        StringBuilder* builder,
        i8 value,
        IntegerBase base = { }
    );

    /**
     * Append a signed 16-bit integer to the string builder.
     */
    b8 AppendI16ToStringBuilder(
        StringBuilder* builder,
        i16 value,
        IntegerBase base = { }
    );

    /**
     * Append a signed 32-bit integer number to the string builder.
     */
    b8 AppendI32ToStringBuilder(
        StringBuilder* builder,
        i32 value,
        IntegerBase base = { }
    );

    /**
     * Append a signed 64-bit integer to the string builder.
     */
    b8 AppendI64ToStringBuilder(
        StringBuilder* builder,
        i64 value,
        IntegerBase base = { }
    );

    /**
     * Return the string from the string builder.
     */
    utf8str StringFromStringBuilder(
        StringBuilder* builder
    );

    /**
     * Reset the string builder, clearing its contents but keeping the allocated buffer.
     */
    void ResetStringBuilder(
        StringBuilder* builder
    );

    /**
     * Free the resources used by the string builder.
     */
    void FreeStringBuilder(
        StringBuilder* builder
    );

    // String Formatting ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * The possible primitive types that can be formatted.
     */
    enum class PrimitiveFmtType : u8 /* use as value */
    {
        B8 = 0,
        F32 = 1,
        F64 = 2,
        U8 = 3,
        U16 = 4,
        U32 = 5,
        U64 = 6,
        I8 = 7,
        I16 = 8,
        I32 = 9,
        I64 = 10,
        Rune = 11,
        CString = 12,
        String = 13,
    };

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
    struct PrimitiveFmtOptions
    {
       PrimitiveFmtType type;
       u64 valueBufferA;
       u64 valueBufferB;
    };

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtB8(
        b8 value
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtF32(
        f32 value,
        i32 decimalPlaces
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtF64(
        f64 value,
        i32 decimalPlaces
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtU8(
        u8 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtU16(
        u16 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtU32(
        u32 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtU64(
        u64 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtI8(
        i8 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtI16(
        i16 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtI32(
        i32 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtI64(
        i64 value,
        IntegerBase base = { }
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtRune(
        u32 value
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtCString(
        cstring value
    );

    /**
     * Use when formatting a string. Pass as one of the varargs.
     */
    PrimitiveFmtOptions FmtString(
        utf8str value
    );

    /**
     * Format a string with the given format and arguments, appending the result
     * to the string builder.
     */
    b8 FormatAndAppendToStringBuilder(
        StringBuilder* builder,
        utf8str fmtStr,
        ArraySlice<PrimitiveFmtOptions> args
    );

    // Conversions to strings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Convert a boolean value to a string ("true" or "false").
     */
    utf8str StringFromBoolean(
        b8 value,
        Allocator allocator
    );

    /**
     * Convert a 32-bit floating-point number to a string with specified decimal places.
     */
    utf8str StringFromF32(
        f32 value,
        Allocator allocator,
        i32 decimalPlaces = { }
    );

    /**
     * Convert a 64-bit floating-point number to a string with specified decimal places.
     */
    utf8str StringFromF64(
        f64 value,
        Allocator allocator,
        i32 decimalPlaces = { }
    );

    /**
     * Convert an unsigned 8-bit integer to a string in the specified base.
     */
    utf8str StringFromU8(
        u8 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert an unsigned 16-bit integer to a string in the specified base.
     */
    utf8str StringFromU16(
        u16 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert an unsigned 32-bit integer to a string in the specified base.
     */
    utf8str StringFromU32(
        u32 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert an unsigned 64-bit integer to a string in the specified base.
     */
    utf8str StringFromU64(
        u64 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert a signed 8-bit integer to a string in the specified base.
     */
    utf8str StringFromI8(
        i8 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert a signed 16-bit integer to a string in the specified base.
     */
    utf8str StringFromI16(
        i16 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert a signed 32-bit integer to a string in the specified base.
     */
    utf8str StringFromI32(
        i32 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    /**
     * Convert a signed 64-bit integer to a string in the specified base.
     */
    utf8str StringFromI64(
        i64 value,
        Allocator allocator,
        IntegerBase base = { }
    );

    // Conversions from strings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Convert a validstring (case-insensitive "true" or "false", or "1" or "0") to a boolean.
     */
    b8 BooleanFromString(
        utf8str str,
        b8* value
    );

    /**
     * Convert a valid string (numbers-only, with zero or one decimal points,
     * optional -/+ sign at the start) to a 32-bit floating-point number.
     */
    b8 F32FromString(
        utf8str str,
        f32* value
    );

    /**
     * Convert a valid string (numbers-only, with zero or one decimal points,
     * optional -/+ sign at the start) to a 64-bit floating-point number.
     */
    b8 F64FromString(
        utf8str str,
        f64* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optionally
     * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 8-bit integer.
     * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
     * By default (no prefix), decimal base is assumed.
     */
    b8 U8FromString(
        utf8str str,
        u8* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optionally
     * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 16-bit integer.
     * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
     * By default (no prefix), decimal base is assumed.
     */
    b8 U16FromString(
        utf8str str,
        u16* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optionally
     * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 32-bit integer.
     * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
     * By default (no prefix), decimal base is assumed.
     */
    b8 U32FromString(
        utf8str str,
        u32* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optionally
     * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 64-bit integer.
     * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
     * By default (no prefix), decimal base is assumed.
     */
    b8 U64FromString(
        utf8str str,
        u64* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
     * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
     * a signed 8-bit integer. Will be assumed to be hexadecimal if it contains A-F
     * characters but no prefix. By default (no prefix), decimal base is assumed.
     */
    b8 I8FromString(
        utf8str str,
        i8* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
     * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
     * a signed 16-bit integer. Will be assumed to be hexadecimal if it contains A-F
     * characters but no prefix. By default (no prefix), decimal base is assumed.
     */
    b8 I16FromString(
        utf8str str,
        i16* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
     * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
     * a signed 32-bit integer. Will be assumed to be hexadecimal if it contains A-F
     * characters but no prefix. By default (no prefix), decimal base is assumed.
     */
    b8 I32FromString(
        utf8str str,
        i32* value
    );

    /**
     * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
     * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
     * a signed 64-bit integer. Will be assumed to be hexadecimal if it contains A-F
     * characters but no prefix. By default (no prefix), decimal base is assumed.
     */
    b8 I64FromString(
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
        utf8str* selfNameWithExtension = { },
        utf8str* selfName = { },
        utf8str* extension = { }
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
        rawptr payload,
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
        rawptr visitorPayload,
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
       rawptr handle;
    };

    /**
     * Opens a file for reading, optionally allow writing to the stream.
     * If the file does not exist, this function will fail.
     */
    File OpenFileToRead(
        Path path,
        b8 allowWrite = { }
    );

    /**
     * Opens a file for writing (or appending), optionally allow reading from the stream.
     * If the file does not exist, it will be created.
     */
    File OpenFileToWrite(
        Path path,
        b8 append = { },
        b8 allowRead = { }
    );

    /**
     * Gets the size of an opened file.
     * Returns 0 on error.
     */
    i64 GetSizeOfFile(
        File handle
    );

    /**
     * Gets the current position in an opened file.
     * Returns -1 on error.
     */
    i64 GetCurrentPositionInFile(
        File handle
    );

    /**
     * Seeks to a specific position in an opened file.
     * If not relative, it's absolute from the start.
     * Returns true on success, false on failure.
     */
    b8 SeekPositionInFile(
        File handle,
        i64 newPos,
        b8 relative = { }
    );

    /**
     * Reads data from an opened file at the current position.
     * Optionally stores the number of bytes read.
     * Returns true on success, false on failure.
     */
    b8 ReadFromFile(
        File handle,
        ArraySlice<u8> dst,
        i64* readSize = { }
    );

    /**
     * Writes data to an opened file at the current position.
     * Returns true on success, false on failure.
     */
    b8 WriteToFile(
        File handle,
        ArraySlice<u8> src
    );

    /**
     * Formats a string with the given format and arguments, writing the
     * result to the file.
     * Returns true on success, false on failure.
     */
    b8 FormatAndWriteToFile(
        File handle,
        utf8str fmtStr,
        ArraySlice<PrimitiveFmtOptions> args
    );

    /**
     * Truncates an opened file to a specific size.
     * Returns true on success, false on failure.
     */
    b8 TruncateFile(
        File handle,
        i64 newSize
    );

    /**
     * Flushes any buffered data to the file.
     * Returns true on success, false on failure.
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
     * Returns true on success, false on failure.
     */
    b8 ReadAllContentsFromFile(
        Path path,
        ArraySlice<u8>* dst,
        Allocator allocator
    );

    /**
     * Dump a bunch of data into a file. Optionally append it instead of overwriting.
     * Returns true on success, false on failure.
     */
    b8 WriteAllContentsToFile(
        Path path,
        ArraySlice<u8> src,
        b8 append = { }
    );

    /**
     * Copies a file from src to dst. If dst exists, it will be overwritten.
     * Returns true on success, false on failure.
     */
    b8 CopyFile(
        Path src,
        Path dst
    );

    /**
     * Moves a file from src to dst. If dst exists, it will be overwritten.
     * Returns true on success, false on failure.
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

    // #######################################################################################
    // Network
    // #######################################################################################

    /**
     * Represents an IP address in binary form.
     * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
     */
    typedef ArraySlice<u8> IPAddress;

    /**
     * Represents a subnet mask in binary form.
     * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
     */
    typedef ArraySlice<u8> IPMask;

    /**
     * Represents an IP network, consisting of an IP address and a subnet mask.
     */
    struct IPNetwork
    {
       IPAddress address;
       IPMask mask;
    };

    b8 GetInterfaceIPAddresses(
        ArraySlice<IPNetwork>* networks,
        Allocator allocator
    );

    // #######################################################################################
    // Stream
    // #######################################################################################

    // Stream Declaration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Defines the mode to be used when calling the stream function.
     */
    enum class StreamMode : u8 /* use as value */
    {
        GetSize = 0,
        GetCurrentPos = 1,
        SeekAbsolute = 2,
        SeekRelative = 3,
        Read = 4,
        Write = 5,
        Truncate = 6,
        Flush = 7,
        Close = 8,
    };

    /**
     * Defines the delegate type for the stream function
     */
    typedef b8 (*StreamProcedure)(
        rawptr streamData,
        StreamMode mode,
        ArraySlice<u8> data,
        i64 offset,
        i64* extraRet
    );

    /**
     * Defines a generic stream, that can be used for reading/writing data.
     */
    struct Stream
    {
       StreamProcedure procedure;
       rawptr data;
    };

    // Stream ease-of-use functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Gets the size of the stream.
     * Returns 0 on error.
     */
    i64 GetSizeOfStream(
        Stream stream
    );

    /**
     * Gets the current position in the stream.
     * Returns -1 on error.
     */
    i64 GetCurrentPositionInStream(
        Stream stream
    );

    /**
     * Seeks to a new position in the stream.
     * If 'relative' is true, the new position is relative to the current position.
     * If 'relative' is false, the new position is absolute from the start.
     * Returns true on success, false on failure.
     */
    b8 SeekPositionInStream(
        Stream stream,
        i64 newPos = { },
        b8 relative = { }
    );

    /**
     * Reads data from the stream into the provided buffer.
     * Optionally stores the number of bytes read.
     * Returns true on success, false on failure.
     */
    b8 ReadFromStream(
        Stream stream,
        ArraySlice<u8> dst,
        i64* readSize = { }
    );

    /**
     * Writes data from the provided buffer into the stream.
     * Returns true on success, false on failure.
     */
    b8 WriteToStream(
        Stream stream,
        ArraySlice<u8> src
    );

    /**
     * Formats a string and writes it to the stream.
     * Only supports primitives, for obvious reasons.
     * Use with `PNSLR_FmtB8`, `PNSLR_FmtI32`, etc.
     * Returns true on success, false on failure.
     */
    b8 FormatAndWriteToStream(
        Stream stream,
        utf8str fmtStr,
        ArraySlice<PrimitiveFmtOptions> args
    );

    /**
     * Truncates the stream to the specified size.
     * Returns true on success, false on failure.
     */
    b8 TruncateStream(
        Stream stream,
        i64 newSize
    );

    /**
     * Flushes any buffered data to the stream.
     * Returns true on success, false on failure.
     */
    b8 FlushStream(
        Stream stream
    );

    /**
     * Closes the stream and frees any associated resources.
     */
    void CloseStream(
        Stream stream
    );

    // Stream casts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Creates a stream from a file handle.
     */
    Stream StreamFromFile(
        File file
    );

    /**
     * Creates a stream from a string builder.
     */
    Stream StreamFromStringBuilder(
        StringBuilder* builder
    );

} // namespace end

namespace Panshilar
{
    /** Create a utf8str from a string literal. */
    template <u64 N> constexpr utf8str StringLiteral(const char (&str)[N]) { utf8str output; output.count = (i64) (N - 1); output.data = (u8*) str; return output; }

    /** Get the current source code location. */
    #define PNSLR_GET_LOC() Panshilar::SourceCodeLocation{Panshilar::StringLiteral(__FILE__), __LINE__, 0, Panshilar::StringLiteral(__FUNCTION__)}

    /** Allocate an object of type 'ty' using the provided allocator. */
    template <typename T> T* New(Allocator allocator, SourceCodeLocation loc, AllocatorError* err = { })
    {
        return (T*) Allocate(allocator, true, (i32) sizeof(T), (i32) alignof(T), loc, err);
    }

    /** Delete an object allocated with `PNSLR_New`, using the provided allocator. */
    template <typename T> void Delete(T* obj, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)
    {
        if (obj) { Free(allocator, obj, loc, err); }
    }

    /** Allocate an array of 'count__' elements of type 'ty' using the provided allocator. Optionally zeroed. */
    template <typename T> ArraySlice<T> MakeSlice(i64 count, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err = { })
    {
        RawArraySlice raw = MakeRawSlice((i32) sizeof(T), (i32) alignof(T), count, zeroed, allocator, loc, err);
        return *reinterpret_cast<ArraySlice<T>*>(&raw);
    }

    /** Free a 'slice' allocated with `PNSLR_MakeSlice`, using the provided allocator. Expects a reassignable variable. */
    template <typename T> void FreeSlice(ArraySlice<T>* slice, Allocator allocator, SourceCodeLocation loc, AllocatorError* err = { })
    {
        if (slice) FreeRawSlice(reinterpret_cast<RawArraySlice*>(slice), allocator, loc, err);
    }

    /** Resize a slice to one with 'newCount__' elements of type 'ty' using the provided allocator. Optionally zeroed. Expects a reassignable variable. */
    template <typename T> void ResizeSlice(ArraySlice<T>* slice, i64 newCount, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err = { })
    {
        if (slice) ResizeRawSlice(reinterpret_cast<RawArraySlice*>(slice), (i32) sizeof(T), (i32) alignof(T), newCount, zeroed, allocator, loc, err);
    }

    /** Easier way to pass format arguments. */
    #define PNSLR_FmtArgs(...) (PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions)) \
        { \
            .data = (PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__}, \
            .count = sizeof((PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__})/sizeof(PNSLR_PrimitiveFmtOptions) \
        }
}

#ifdef PNSLR_INTRINSICS_CXX_MAIN_H
    b8 utf8str::operator==(const utf8str& other) const { return Panshilar::AreStringsEqual(*this, other); }
    b8 utf8str::operator!=(const utf8str& other) const { return !(*this == other); }
#endif

#endif//PNSLR_CXX_MAIN_H
#ifdef PNSLR_CXX_IMPL

#if (_MSC_VER)
    #define PNSLR_STRUCT_OFFSET(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
#elif (__clang__) || (__GNUC__)
    #define PNSLR_STRUCT_OFFSET(type, member) ((u64) offsetof(type, member))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

enum class PNSLR_Platform : u8 { };
static_assert(sizeof(PNSLR_Platform) == sizeof(Panshilar::Platform), "size mismatch");
static_assert(alignof(PNSLR_Platform) == alignof(Panshilar::Platform), "align mismatch");
PNSLR_Platform* PNSLR_Bindings_Convert(Panshilar::Platform* x) { return reinterpret_cast<PNSLR_Platform*>(x); }
Panshilar::Platform* PNSLR_Bindings_Convert(PNSLR_Platform* x) { return reinterpret_cast<Panshilar::Platform*>(x); }
PNSLR_Platform& PNSLR_Bindings_Convert(Panshilar::Platform& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Platform& PNSLR_Bindings_Convert(PNSLR_Platform& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_Architecture : u8 { };
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
   i32 line;
   i32 column;
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
   u8 buffer[64];
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

extern "C" b8 PNSLR_TryLockMutex(PNSLR_Mutex* mutex);
b8 Panshilar::TryLockMutex(Panshilar::Mutex* mutex)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockMutex(PNSLR_Bindings_Convert(mutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct alignas(8) PNSLR_RWMutex
{
   u8 buffer[200];
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

extern "C" b8 PNSLR_TryLockRWMutexShared(PNSLR_RWMutex* rwmutex);
b8 Panshilar::TryLockRWMutexShared(Panshilar::RWMutex* rwmutex)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockRWMutexShared(PNSLR_Bindings_Convert(rwmutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_TryLockRWMutexExclusive(PNSLR_RWMutex* rwmutex);
b8 Panshilar::TryLockRWMutexExclusive(Panshilar::RWMutex* rwmutex)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TryLockRWMutexExclusive(PNSLR_Bindings_Convert(rwmutex)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct alignas(8) PNSLR_Semaphore
{
   u8 buffer[32];
};
static_assert(sizeof(PNSLR_Semaphore) == sizeof(Panshilar::Semaphore), "size mismatch");
static_assert(alignof(PNSLR_Semaphore) == alignof(Panshilar::Semaphore), "align mismatch");
PNSLR_Semaphore* PNSLR_Bindings_Convert(Panshilar::Semaphore* x) { return reinterpret_cast<PNSLR_Semaphore*>(x); }
Panshilar::Semaphore* PNSLR_Bindings_Convert(PNSLR_Semaphore* x) { return reinterpret_cast<Panshilar::Semaphore*>(x); }
PNSLR_Semaphore& PNSLR_Bindings_Convert(Panshilar::Semaphore& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Semaphore& PNSLR_Bindings_Convert(PNSLR_Semaphore& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Semaphore, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::Semaphore, buffer), "buffer offset mismatch");

extern "C" PNSLR_Semaphore PNSLR_CreateSemaphore(i32 initialCount);
Panshilar::Semaphore Panshilar::CreateSemaphore(i32 initialCount)
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

extern "C" b8 PNSLR_WaitSemaphoreTimeout(PNSLR_Semaphore* semaphore, i32 timeoutNs);
b8 Panshilar::WaitSemaphoreTimeout(Panshilar::Semaphore* semaphore, i32 timeoutNs)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WaitSemaphoreTimeout(PNSLR_Bindings_Convert(semaphore), PNSLR_Bindings_Convert(timeoutNs)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_SignalSemaphore(PNSLR_Semaphore* semaphore, i32 count);
void Panshilar::SignalSemaphore(Panshilar::Semaphore* semaphore, i32 count)
{
    PNSLR_SignalSemaphore(PNSLR_Bindings_Convert(semaphore), PNSLR_Bindings_Convert(count));
}

struct alignas(8) PNSLR_ConditionVariable
{
   u8 buffer[48];
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

extern "C" b8 PNSLR_WaitConditionVariableTimeout(PNSLR_ConditionVariable* condvar, PNSLR_Mutex* mutex, i32 timeoutNs);
b8 Panshilar::WaitConditionVariableTimeout(Panshilar::ConditionVariable* condvar, Panshilar::Mutex* mutex, i32 timeoutNs)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WaitConditionVariableTimeout(PNSLR_Bindings_Convert(condvar), PNSLR_Bindings_Convert(mutex), PNSLR_Bindings_Convert(timeoutNs)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
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

extern "C" void PNSLR_MemSet(rawptr memory, i32 value, i32 size);
void Panshilar::MemSet(rawptr memory, i32 value, i32 size)
{
    PNSLR_MemSet(PNSLR_Bindings_Convert(memory), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(size));
}

extern "C" void PNSLR_MemCopy(rawptr destination, rawptr source, i32 size);
void Panshilar::MemCopy(rawptr destination, rawptr source, i32 size)
{
    PNSLR_MemCopy(PNSLR_Bindings_Convert(destination), PNSLR_Bindings_Convert(source), PNSLR_Bindings_Convert(size));
}

extern "C" void PNSLR_MemMove(rawptr destination, rawptr source, i32 size);
void Panshilar::MemMove(rawptr destination, rawptr source, i32 size)
{
    PNSLR_MemMove(PNSLR_Bindings_Convert(destination), PNSLR_Bindings_Convert(source), PNSLR_Bindings_Convert(size));
}

enum class PNSLR_AllocatorMode : u8 { };
static_assert(sizeof(PNSLR_AllocatorMode) == sizeof(Panshilar::AllocatorMode), "size mismatch");
static_assert(alignof(PNSLR_AllocatorMode) == alignof(Panshilar::AllocatorMode), "align mismatch");
PNSLR_AllocatorMode* PNSLR_Bindings_Convert(Panshilar::AllocatorMode* x) { return reinterpret_cast<PNSLR_AllocatorMode*>(x); }
Panshilar::AllocatorMode* PNSLR_Bindings_Convert(PNSLR_AllocatorMode* x) { return reinterpret_cast<Panshilar::AllocatorMode*>(x); }
PNSLR_AllocatorMode& PNSLR_Bindings_Convert(Panshilar::AllocatorMode& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorMode& PNSLR_Bindings_Convert(PNSLR_AllocatorMode& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_AllocatorCapability : u64 { };
static_assert(sizeof(PNSLR_AllocatorCapability) == sizeof(Panshilar::AllocatorCapability), "size mismatch");
static_assert(alignof(PNSLR_AllocatorCapability) == alignof(Panshilar::AllocatorCapability), "align mismatch");
PNSLR_AllocatorCapability* PNSLR_Bindings_Convert(Panshilar::AllocatorCapability* x) { return reinterpret_cast<PNSLR_AllocatorCapability*>(x); }
Panshilar::AllocatorCapability* PNSLR_Bindings_Convert(PNSLR_AllocatorCapability* x) { return reinterpret_cast<Panshilar::AllocatorCapability*>(x); }
PNSLR_AllocatorCapability& PNSLR_Bindings_Convert(Panshilar::AllocatorCapability& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorCapability& PNSLR_Bindings_Convert(PNSLR_AllocatorCapability& x) { return *PNSLR_Bindings_Convert(&x); }

enum class PNSLR_AllocatorError : u8 { };
static_assert(sizeof(PNSLR_AllocatorError) == sizeof(Panshilar::AllocatorError), "size mismatch");
static_assert(alignof(PNSLR_AllocatorError) == alignof(Panshilar::AllocatorError), "align mismatch");
PNSLR_AllocatorError* PNSLR_Bindings_Convert(Panshilar::AllocatorError* x) { return reinterpret_cast<PNSLR_AllocatorError*>(x); }
Panshilar::AllocatorError* PNSLR_Bindings_Convert(PNSLR_AllocatorError* x) { return reinterpret_cast<Panshilar::AllocatorError*>(x); }
PNSLR_AllocatorError& PNSLR_Bindings_Convert(Panshilar::AllocatorError& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorError& PNSLR_Bindings_Convert(PNSLR_AllocatorError& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" typedef rawptr (*PNSLR_AllocatorProcedure)(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
static_assert(sizeof(PNSLR_AllocatorProcedure) == sizeof(Panshilar::AllocatorProcedure), "size mismatch");
static_assert(alignof(PNSLR_AllocatorProcedure) == alignof(Panshilar::AllocatorProcedure), "align mismatch");
PNSLR_AllocatorProcedure* PNSLR_Bindings_Convert(Panshilar::AllocatorProcedure* x) { return reinterpret_cast<PNSLR_AllocatorProcedure*>(x); }
Panshilar::AllocatorProcedure* PNSLR_Bindings_Convert(PNSLR_AllocatorProcedure* x) { return reinterpret_cast<Panshilar::AllocatorProcedure*>(x); }
PNSLR_AllocatorProcedure& PNSLR_Bindings_Convert(Panshilar::AllocatorProcedure& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::AllocatorProcedure& PNSLR_Bindings_Convert(PNSLR_AllocatorProcedure& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_Allocator
{
   PNSLR_AllocatorProcedure procedure;
   rawptr data;
};
static_assert(sizeof(PNSLR_Allocator) == sizeof(Panshilar::Allocator), "size mismatch");
static_assert(alignof(PNSLR_Allocator) == alignof(Panshilar::Allocator), "align mismatch");
PNSLR_Allocator* PNSLR_Bindings_Convert(Panshilar::Allocator* x) { return reinterpret_cast<PNSLR_Allocator*>(x); }
Panshilar::Allocator* PNSLR_Bindings_Convert(PNSLR_Allocator* x) { return reinterpret_cast<Panshilar::Allocator*>(x); }
PNSLR_Allocator& PNSLR_Bindings_Convert(Panshilar::Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Allocator& PNSLR_Bindings_Convert(PNSLR_Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Allocator, procedure) == PNSLR_STRUCT_OFFSET(Panshilar::Allocator, procedure), "procedure offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Allocator, data) == PNSLR_STRUCT_OFFSET(Panshilar::Allocator, data), "data offset mismatch");

typedef struct { PNSLR_Allocator* data; i64 count; } PNSLR_ArraySlice_PNSLR_Allocator;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_Allocator) == sizeof(ArraySlice<Panshilar::Allocator>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_Allocator) == alignof(ArraySlice<Panshilar::Allocator>), "align mismatch");
PNSLR_ArraySlice_PNSLR_Allocator* PNSLR_Bindings_Convert(ArraySlice<Panshilar::Allocator>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_Allocator*>(x); }
ArraySlice<Panshilar::Allocator>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Allocator* x) { return reinterpret_cast<ArraySlice<Panshilar::Allocator>*>(x); }
PNSLR_ArraySlice_PNSLR_Allocator& PNSLR_Bindings_Convert(ArraySlice<Panshilar::Allocator>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<Panshilar::Allocator>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Allocator& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, count) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::Allocator>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, data) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::Allocator>, data), "data offset mismatch");

extern "C" rawptr PNSLR_Allocate(PNSLR_Allocator allocator, b8 zeroed, i32 size, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::Allocate(Panshilar::Allocator allocator, b8 zeroed, i32 size, i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_Allocate(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" rawptr PNSLR_Resize(PNSLR_Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::Resize(Panshilar::Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_Resize(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(newSize), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" rawptr PNSLR_DefaultResize(PNSLR_Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::DefaultResize(Panshilar::Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DefaultResize(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(newSize), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_Free(PNSLR_Allocator allocator, rawptr memory, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::Free(Panshilar::Allocator allocator, rawptr memory, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_Free(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(memory), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void PNSLR_FreeAll(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeAll(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeAll(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" u64 PNSLR_QueryAllocatorCapabilities(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
u64 Panshilar::QueryAllocatorCapabilities(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    u64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_QueryAllocatorCapabilities(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
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

extern "C" rawptr PNSLR_AllocatorFn_DefaultHeap(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::AllocatorFn_DefaultHeap(rawptr allocatorData, Panshilar::AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_DefaultHeap(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_ArenaAllocatorBlock
{
   PNSLR_ArenaAllocatorBlock* previous;
   PNSLR_Allocator allocator;
   rawptr memory;
   u32 capacity;
   u32 used;
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
   u32 totalUsed;
   u32 totalCapacity;
   u32 minimumBlockSize;
   u32 numSnapshots;
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

extern "C" PNSLR_Allocator PNSLR_NewAllocator_Arena(PNSLR_Allocator backingAllocator, u32 pageSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Panshilar::Allocator Panshilar::NewAllocator_Arena(Panshilar::Allocator backingAllocator, u32 pageSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_Allocator zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NewAllocator_Arena(PNSLR_Bindings_Convert(backingAllocator), PNSLR_Bindings_Convert(pageSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_DestroyAllocator_Arena(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::DestroyAllocator_Arena(Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_DestroyAllocator_Arena(PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" rawptr PNSLR_AllocatorFn_Arena(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::AllocatorFn_Arena(rawptr allocatorData, Panshilar::AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_Arena(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_ArenaSnapshotError : u8 { };
static_assert(sizeof(PNSLR_ArenaSnapshotError) == sizeof(Panshilar::ArenaSnapshotError), "size mismatch");
static_assert(alignof(PNSLR_ArenaSnapshotError) == alignof(Panshilar::ArenaSnapshotError), "align mismatch");
PNSLR_ArenaSnapshotError* PNSLR_Bindings_Convert(Panshilar::ArenaSnapshotError* x) { return reinterpret_cast<PNSLR_ArenaSnapshotError*>(x); }
Panshilar::ArenaSnapshotError* PNSLR_Bindings_Convert(PNSLR_ArenaSnapshotError* x) { return reinterpret_cast<Panshilar::ArenaSnapshotError*>(x); }
PNSLR_ArenaSnapshotError& PNSLR_Bindings_Convert(Panshilar::ArenaSnapshotError& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::ArenaSnapshotError& PNSLR_Bindings_Convert(PNSLR_ArenaSnapshotError& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_ArenaAllocatorSnapshot
{
   b8 valid;
   PNSLR_ArenaAllocatorPayload* payload;
   PNSLR_ArenaAllocatorBlock* block;
   u32 used;
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

extern "C" b8 PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Allocator allocator);
b8 Panshilar::ValidateArenaAllocatorSnapshotState(Panshilar::Allocator allocator)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
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
   u64 usedBytes;
   u8 buffer[8192];
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
   i32 size;
   i32 alignment;
   rawptr lastAllocation;
   rawptr lastAllocationHeader;
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
   rawptr lastAllocation;
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

extern "C" rawptr PNSLR_AllocatorFn_Stack(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
rawptr Panshilar::AllocatorFn_Stack(rawptr allocatorData, Panshilar::AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    rawptr zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AllocatorFn_Stack(PNSLR_Bindings_Convert(allocatorData), PNSLR_Bindings_Convert(mode), PNSLR_Bindings_Convert(size), PNSLR_Bindings_Convert(alignment), PNSLR_Bindings_Convert(oldMemory), PNSLR_Bindings_Convert(oldSize), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_RawArraySlice PNSLR_MakeRawSlice(i32 tySize, i32 tyAlign, i64 count, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
Intrinsics::RawArraySlice Panshilar::MakeRawSlice(i32 tySize, i32 tyAlign, i64 count, b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_RawArraySlice zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeRawSlice(PNSLR_Bindings_Convert(tySize), PNSLR_Bindings_Convert(tyAlign), PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeRawSlice(PNSLR_RawArraySlice* slice, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeRawSlice(Intrinsics::RawArraySlice* slice, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeRawSlice(PNSLR_Bindings_Convert(slice), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" void PNSLR_ResizeRawSlice(PNSLR_RawArraySlice* slice, i32 tySize, i32 tyAlign, i64 newCount, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::ResizeRawSlice(Intrinsics::RawArraySlice* slice, i32 tySize, i32 tyAlign, i64 newCount, b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_ResizeRawSlice(PNSLR_Bindings_Convert(slice), PNSLR_Bindings_Convert(tySize), PNSLR_Bindings_Convert(tyAlign), PNSLR_Bindings_Convert(newCount), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" PNSLR_UTF8STR PNSLR_MakeString(i64 count, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
utf8str Panshilar::MakeString(i64 count, b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeString(PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeString(PNSLR_UTF8STR str, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeString(utf8str str, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" cstring PNSLR_MakeCString(i64 count, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
cstring Panshilar::MakeCString(i64 count, b8 zeroed, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    cstring zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MakeCString(PNSLR_Bindings_Convert(count), PNSLR_Bindings_Convert(zeroed), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_FreeCString(cstring str, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);
void Panshilar::FreeCString(cstring str, Panshilar::Allocator allocator, Panshilar::SourceCodeLocation location, Panshilar::AllocatorError* error)
{
    PNSLR_FreeCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(location), PNSLR_Bindings_Convert(error));
}

extern "C" i64 PNSLR_NanosecondsSinceUnixEpoch();
i64 Panshilar::NanosecondsSinceUnixEpoch()
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NanosecondsSinceUnixEpoch(); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i32 PNSLR_GetCStringLength(cstring str);
i32 Panshilar::GetCStringLength(cstring str)
{
    i32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetCStringLength(PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromCString(cstring str);
utf8str Panshilar::StringFromCString(cstring str)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromCString(PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" cstring PNSLR_CStringFromString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
cstring Panshilar::CStringFromString(utf8str str, Panshilar::Allocator allocator)
{
    cstring zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringFromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_CloneString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
utf8str Panshilar::CloneString(utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CloneString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_ConcatenateStrings(PNSLR_UTF8STR str1, PNSLR_UTF8STR str2, PNSLR_Allocator allocator);
utf8str Panshilar::ConcatenateStrings(utf8str str1, utf8str str2, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ConcatenateStrings(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_UpperString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
utf8str Panshilar::UpperString(utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_UpperString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_LowerString(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
utf8str Panshilar::LowerString(utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_LowerString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_StringComparisonType : u8 { };
static_assert(sizeof(PNSLR_StringComparisonType) == sizeof(Panshilar::StringComparisonType), "size mismatch");
static_assert(alignof(PNSLR_StringComparisonType) == alignof(Panshilar::StringComparisonType), "align mismatch");
PNSLR_StringComparisonType* PNSLR_Bindings_Convert(Panshilar::StringComparisonType* x) { return reinterpret_cast<PNSLR_StringComparisonType*>(x); }
Panshilar::StringComparisonType* PNSLR_Bindings_Convert(PNSLR_StringComparisonType* x) { return reinterpret_cast<Panshilar::StringComparisonType*>(x); }
PNSLR_StringComparisonType& PNSLR_Bindings_Convert(Panshilar::StringComparisonType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StringComparisonType& PNSLR_Bindings_Convert(PNSLR_StringComparisonType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" b8 PNSLR_AreStringsEqual(PNSLR_UTF8STR str1, PNSLR_UTF8STR str2, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::AreStringsEqual(utf8str str1, utf8str str2, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreStringsEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AreStringAndCStringEqual(PNSLR_UTF8STR str1, cstring str2, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::AreStringAndCStringEqual(utf8str str1, cstring str2, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreStringAndCStringEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AreCStringsEqual(cstring str1, cstring str2, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::AreCStringsEqual(cstring str1, cstring str2, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AreCStringsEqual(PNSLR_Bindings_Convert(str1), PNSLR_Bindings_Convert(str2), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_StringStartsWith(PNSLR_UTF8STR str, PNSLR_UTF8STR prefix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::StringStartsWith(utf8str str, utf8str prefix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringStartsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_StringEndsWith(PNSLR_UTF8STR str, PNSLR_UTF8STR suffix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::StringEndsWith(utf8str str, utf8str suffix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringEndsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_StringStartsWithCString(PNSLR_UTF8STR str, cstring prefix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::StringStartsWithCString(utf8str str, cstring prefix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringStartsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_StringEndsWithCString(PNSLR_UTF8STR str, cstring suffix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::StringEndsWithCString(utf8str str, cstring suffix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringEndsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CStringStartsWith(cstring str, PNSLR_UTF8STR prefix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::CStringStartsWith(cstring str, utf8str prefix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringStartsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CStringEndsWith(cstring str, PNSLR_UTF8STR suffix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::CStringEndsWith(cstring str, utf8str suffix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringEndsWith(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CStringStartsWithCString(PNSLR_UTF8STR str, cstring prefix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::CStringStartsWithCString(utf8str str, cstring prefix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringStartsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(prefix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CStringEndsWithCString(PNSLR_UTF8STR str, cstring suffix, PNSLR_StringComparisonType comparisonType);
b8 Panshilar::CStringEndsWithCString(utf8str str, cstring suffix, Panshilar::StringComparisonType comparisonType)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CStringEndsWithCString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(suffix), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i32 PNSLR_SearchFirstIndexInString(PNSLR_UTF8STR str, PNSLR_UTF8STR substring, PNSLR_StringComparisonType comparisonType);
i32 Panshilar::SearchFirstIndexInString(utf8str str, utf8str substring, Panshilar::StringComparisonType comparisonType)
{
    i32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SearchFirstIndexInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(substring), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i32 PNSLR_SearchLastIndexInString(PNSLR_UTF8STR str, PNSLR_UTF8STR substring, PNSLR_StringComparisonType comparisonType);
i32 Panshilar::SearchLastIndexInString(utf8str str, utf8str substring, Panshilar::StringComparisonType comparisonType)
{
    i32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SearchLastIndexInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(substring), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_ReplaceInString(PNSLR_UTF8STR str, PNSLR_UTF8STR oldValue, PNSLR_UTF8STR newValue, PNSLR_Allocator allocator, PNSLR_StringComparisonType comparisonType);
utf8str Panshilar::ReplaceInString(utf8str str, utf8str oldValue, utf8str newValue, Panshilar::Allocator allocator, Panshilar::StringComparisonType comparisonType)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReplaceInString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(oldValue), PNSLR_Bindings_Convert(newValue), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(comparisonType)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_EncodedRune
{
   u8 data[4];
   i32 length;
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
   u32 rune;
   i32 length;
};
static_assert(sizeof(PNSLR_DecodedRune) == sizeof(Panshilar::DecodedRune), "size mismatch");
static_assert(alignof(PNSLR_DecodedRune) == alignof(Panshilar::DecodedRune), "align mismatch");
PNSLR_DecodedRune* PNSLR_Bindings_Convert(Panshilar::DecodedRune* x) { return reinterpret_cast<PNSLR_DecodedRune*>(x); }
Panshilar::DecodedRune* PNSLR_Bindings_Convert(PNSLR_DecodedRune* x) { return reinterpret_cast<Panshilar::DecodedRune*>(x); }
PNSLR_DecodedRune& PNSLR_Bindings_Convert(Panshilar::DecodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::DecodedRune& PNSLR_Bindings_Convert(PNSLR_DecodedRune& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_DecodedRune, rune) == PNSLR_STRUCT_OFFSET(Panshilar::DecodedRune, rune), "rune offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_DecodedRune, length) == PNSLR_STRUCT_OFFSET(Panshilar::DecodedRune, length), "length offset mismatch");

extern "C" i32 PNSLR_GetRuneLength(u32 r);
i32 Panshilar::GetRuneLength(u32 r)
{
    i32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetRuneLength(PNSLR_Bindings_Convert(r)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_EncodedRune PNSLR_EncodeRune(u32 c);
Panshilar::EncodedRune Panshilar::EncodeRune(u32 c)
{
    PNSLR_EncodedRune zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_EncodeRune(PNSLR_Bindings_Convert(c)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_DecodedRune PNSLR_DecodeRune(PNSLR_ArraySlice_u8 s);
Panshilar::DecodedRune Panshilar::DecodeRune(ArraySlice<u8> s)
{
    PNSLR_DecodedRune zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DecodeRune(PNSLR_Bindings_Convert(s)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_ArraySlice_u16 PNSLR_UTF16FromUTF8WindowsOnly(PNSLR_UTF8STR str, PNSLR_Allocator allocator);
ArraySlice<u16> Panshilar::UTF16FromUTF8WindowsOnly(utf8str str, Panshilar::Allocator allocator)
{
    PNSLR_ArraySlice_u16 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_UTF16FromUTF8WindowsOnly(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_UTF8FromUTF16WindowsOnly(PNSLR_ArraySlice_u16 utf16str, PNSLR_Allocator allocator);
utf8str Panshilar::UTF8FromUTF16WindowsOnly(ArraySlice<u16> utf16str, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_UTF8FromUTF16WindowsOnly(PNSLR_Bindings_Convert(utf16str), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_StringBuilder
{
   PNSLR_Allocator allocator;
   PNSLR_ArraySlice_u8 buffer;
   i64 writtenSize;
   i64 cursorPos;
};
static_assert(sizeof(PNSLR_StringBuilder) == sizeof(Panshilar::StringBuilder), "size mismatch");
static_assert(alignof(PNSLR_StringBuilder) == alignof(Panshilar::StringBuilder), "align mismatch");
PNSLR_StringBuilder* PNSLR_Bindings_Convert(Panshilar::StringBuilder* x) { return reinterpret_cast<PNSLR_StringBuilder*>(x); }
Panshilar::StringBuilder* PNSLR_Bindings_Convert(PNSLR_StringBuilder* x) { return reinterpret_cast<Panshilar::StringBuilder*>(x); }
PNSLR_StringBuilder& PNSLR_Bindings_Convert(Panshilar::StringBuilder& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StringBuilder& PNSLR_Bindings_Convert(PNSLR_StringBuilder& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StringBuilder, allocator) == PNSLR_STRUCT_OFFSET(Panshilar::StringBuilder, allocator), "allocator offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StringBuilder, buffer) == PNSLR_STRUCT_OFFSET(Panshilar::StringBuilder, buffer), "buffer offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StringBuilder, writtenSize) == PNSLR_STRUCT_OFFSET(Panshilar::StringBuilder, writtenSize), "writtenSize offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_StringBuilder, cursorPos) == PNSLR_STRUCT_OFFSET(Panshilar::StringBuilder, cursorPos), "cursorPos offset mismatch");

extern "C" b8 PNSLR_AppendByteToStringBuilder(PNSLR_StringBuilder* builder, u8 byte);
b8 Panshilar::AppendByteToStringBuilder(Panshilar::StringBuilder* builder, u8 byte)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendByteToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(byte)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendStringToStringBuilder(PNSLR_StringBuilder* builder, PNSLR_UTF8STR str);
b8 Panshilar::AppendStringToStringBuilder(Panshilar::StringBuilder* builder, utf8str str)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendStringToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendCStringToStringBuilder(PNSLR_StringBuilder* builder, cstring str);
b8 Panshilar::AppendCStringToStringBuilder(Panshilar::StringBuilder* builder, cstring str)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendCStringToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(str)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendRuneToStringBuilder(PNSLR_StringBuilder* builder, u32 rune);
b8 Panshilar::AppendRuneToStringBuilder(Panshilar::StringBuilder* builder, u32 rune)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendRuneToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(rune)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendB8ToStringBuilder(PNSLR_StringBuilder* builder, b8 value);
b8 Panshilar::AppendB8ToStringBuilder(Panshilar::StringBuilder* builder, b8 value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendB8ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendF32ToStringBuilder(PNSLR_StringBuilder* builder, f32 value, i32 decimalPlaces);
b8 Panshilar::AppendF32ToStringBuilder(Panshilar::StringBuilder* builder, f32 value, i32 decimalPlaces)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendF32ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendF64ToStringBuilder(PNSLR_StringBuilder* builder, f64 value, i32 decimalPlaces);
b8 Panshilar::AppendF64ToStringBuilder(Panshilar::StringBuilder* builder, f64 value, i32 decimalPlaces)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendF64ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_IntegerBase : u8 { };
static_assert(sizeof(PNSLR_IntegerBase) == sizeof(Panshilar::IntegerBase), "size mismatch");
static_assert(alignof(PNSLR_IntegerBase) == alignof(Panshilar::IntegerBase), "align mismatch");
PNSLR_IntegerBase* PNSLR_Bindings_Convert(Panshilar::IntegerBase* x) { return reinterpret_cast<PNSLR_IntegerBase*>(x); }
Panshilar::IntegerBase* PNSLR_Bindings_Convert(PNSLR_IntegerBase* x) { return reinterpret_cast<Panshilar::IntegerBase*>(x); }
PNSLR_IntegerBase& PNSLR_Bindings_Convert(Panshilar::IntegerBase& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::IntegerBase& PNSLR_Bindings_Convert(PNSLR_IntegerBase& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" b8 PNSLR_AppendU8ToStringBuilder(PNSLR_StringBuilder* builder, u8 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendU8ToStringBuilder(Panshilar::StringBuilder* builder, u8 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendU8ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendU16ToStringBuilder(PNSLR_StringBuilder* builder, u16 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendU16ToStringBuilder(Panshilar::StringBuilder* builder, u16 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendU16ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendU32ToStringBuilder(PNSLR_StringBuilder* builder, u32 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendU32ToStringBuilder(Panshilar::StringBuilder* builder, u32 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendU32ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendU64ToStringBuilder(PNSLR_StringBuilder* builder, u64 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendU64ToStringBuilder(Panshilar::StringBuilder* builder, u64 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendU64ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendI8ToStringBuilder(PNSLR_StringBuilder* builder, i8 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendI8ToStringBuilder(Panshilar::StringBuilder* builder, i8 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendI8ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendI16ToStringBuilder(PNSLR_StringBuilder* builder, i16 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendI16ToStringBuilder(Panshilar::StringBuilder* builder, i16 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendI16ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendI32ToStringBuilder(PNSLR_StringBuilder* builder, i32 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendI32ToStringBuilder(Panshilar::StringBuilder* builder, i32 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendI32ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_AppendI64ToStringBuilder(PNSLR_StringBuilder* builder, i64 value, PNSLR_IntegerBase base);
b8 Panshilar::AppendI64ToStringBuilder(Panshilar::StringBuilder* builder, i64 value, Panshilar::IntegerBase base)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_AppendI64ToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromStringBuilder(PNSLR_StringBuilder* builder);
utf8str Panshilar::StringFromStringBuilder(Panshilar::StringBuilder* builder)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromStringBuilder(PNSLR_Bindings_Convert(builder)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_ResetStringBuilder(PNSLR_StringBuilder* builder);
void Panshilar::ResetStringBuilder(Panshilar::StringBuilder* builder)
{
    PNSLR_ResetStringBuilder(PNSLR_Bindings_Convert(builder));
}

extern "C" void PNSLR_FreeStringBuilder(PNSLR_StringBuilder* builder);
void Panshilar::FreeStringBuilder(Panshilar::StringBuilder* builder)
{
    PNSLR_FreeStringBuilder(PNSLR_Bindings_Convert(builder));
}

enum class PNSLR_PrimitiveFmtType : u8 { };
static_assert(sizeof(PNSLR_PrimitiveFmtType) == sizeof(Panshilar::PrimitiveFmtType), "size mismatch");
static_assert(alignof(PNSLR_PrimitiveFmtType) == alignof(Panshilar::PrimitiveFmtType), "align mismatch");
PNSLR_PrimitiveFmtType* PNSLR_Bindings_Convert(Panshilar::PrimitiveFmtType* x) { return reinterpret_cast<PNSLR_PrimitiveFmtType*>(x); }
Panshilar::PrimitiveFmtType* PNSLR_Bindings_Convert(PNSLR_PrimitiveFmtType* x) { return reinterpret_cast<Panshilar::PrimitiveFmtType*>(x); }
PNSLR_PrimitiveFmtType& PNSLR_Bindings_Convert(Panshilar::PrimitiveFmtType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PrimitiveFmtType& PNSLR_Bindings_Convert(PNSLR_PrimitiveFmtType& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_PrimitiveFmtOptions
{
   PNSLR_PrimitiveFmtType type;
   u64 valueBufferA;
   u64 valueBufferB;
};
static_assert(sizeof(PNSLR_PrimitiveFmtOptions) == sizeof(Panshilar::PrimitiveFmtOptions), "size mismatch");
static_assert(alignof(PNSLR_PrimitiveFmtOptions) == alignof(Panshilar::PrimitiveFmtOptions), "align mismatch");
PNSLR_PrimitiveFmtOptions* PNSLR_Bindings_Convert(Panshilar::PrimitiveFmtOptions* x) { return reinterpret_cast<PNSLR_PrimitiveFmtOptions*>(x); }
Panshilar::PrimitiveFmtOptions* PNSLR_Bindings_Convert(PNSLR_PrimitiveFmtOptions* x) { return reinterpret_cast<Panshilar::PrimitiveFmtOptions*>(x); }
PNSLR_PrimitiveFmtOptions& PNSLR_Bindings_Convert(Panshilar::PrimitiveFmtOptions& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PrimitiveFmtOptions& PNSLR_Bindings_Convert(PNSLR_PrimitiveFmtOptions& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_PrimitiveFmtOptions, type) == PNSLR_STRUCT_OFFSET(Panshilar::PrimitiveFmtOptions, type), "type offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_PrimitiveFmtOptions, valueBufferA) == PNSLR_STRUCT_OFFSET(Panshilar::PrimitiveFmtOptions, valueBufferA), "valueBufferA offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_PrimitiveFmtOptions, valueBufferB) == PNSLR_STRUCT_OFFSET(Panshilar::PrimitiveFmtOptions, valueBufferB), "valueBufferB offset mismatch");

typedef struct { PNSLR_PrimitiveFmtOptions* data; i64 count; } PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions) == sizeof(ArraySlice<Panshilar::PrimitiveFmtOptions>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions) == alignof(ArraySlice<Panshilar::PrimitiveFmtOptions>), "align mismatch");
PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions* PNSLR_Bindings_Convert(ArraySlice<Panshilar::PrimitiveFmtOptions>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions*>(x); }
ArraySlice<Panshilar::PrimitiveFmtOptions>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions* x) { return reinterpret_cast<ArraySlice<Panshilar::PrimitiveFmtOptions>*>(x); }
PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions& PNSLR_Bindings_Convert(ArraySlice<Panshilar::PrimitiveFmtOptions>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<Panshilar::PrimitiveFmtOptions>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions, count) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::PrimitiveFmtOptions>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions, data) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::PrimitiveFmtOptions>, data), "data offset mismatch");

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtB8(b8 value);
Panshilar::PrimitiveFmtOptions Panshilar::FmtB8(b8 value)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtB8(PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtF32(f32 value, i32 decimalPlaces);
Panshilar::PrimitiveFmtOptions Panshilar::FmtF32(f32 value, i32 decimalPlaces)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtF32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtF64(f64 value, i32 decimalPlaces);
Panshilar::PrimitiveFmtOptions Panshilar::FmtF64(f64 value, i32 decimalPlaces)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtF64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtU8(u8 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtU8(u8 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtU8(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtU16(u16 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtU16(u16 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtU16(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtU32(u32 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtU32(u32 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtU32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtU64(u64 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtU64(u64 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtU64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtI8(i8 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtI8(i8 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtI8(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtI16(i16 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtI16(i16 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtI16(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtI32(i32 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtI32(i32 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtI32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtI64(i64 value, PNSLR_IntegerBase base);
Panshilar::PrimitiveFmtOptions Panshilar::FmtI64(i64 value, Panshilar::IntegerBase base)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtI64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtRune(u32 value);
Panshilar::PrimitiveFmtOptions Panshilar::FmtRune(u32 value)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtRune(PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtCString(cstring value);
Panshilar::PrimitiveFmtOptions Panshilar::FmtCString(cstring value)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtCString(PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_PrimitiveFmtOptions PNSLR_FmtString(PNSLR_UTF8STR value);
Panshilar::PrimitiveFmtOptions Panshilar::FmtString(utf8str value)
{
    PNSLR_PrimitiveFmtOptions zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FmtString(PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_FormatAndAppendToStringBuilder(PNSLR_StringBuilder* builder, PNSLR_UTF8STR fmtStr, PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions args);
b8 Panshilar::FormatAndAppendToStringBuilder(Panshilar::StringBuilder* builder, utf8str fmtStr, ArraySlice<Panshilar::PrimitiveFmtOptions> args)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FormatAndAppendToStringBuilder(PNSLR_Bindings_Convert(builder), PNSLR_Bindings_Convert(fmtStr), PNSLR_Bindings_Convert(args)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromBoolean(b8 value, PNSLR_Allocator allocator);
utf8str Panshilar::StringFromBoolean(b8 value, Panshilar::Allocator allocator)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromBoolean(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromF32(f32 value, PNSLR_Allocator allocator, i32 decimalPlaces);
utf8str Panshilar::StringFromF32(f32 value, Panshilar::Allocator allocator, i32 decimalPlaces)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromF32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromF64(f64 value, PNSLR_Allocator allocator, i32 decimalPlaces);
utf8str Panshilar::StringFromF64(f64 value, Panshilar::Allocator allocator, i32 decimalPlaces)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromF64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(decimalPlaces)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromU8(u8 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromU8(u8 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromU8(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromU16(u16 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromU16(u16 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromU16(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromU32(u32 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromU32(u32 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromU32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromU64(u64 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromU64(u64 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromU64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromI8(i8 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromI8(i8 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromI8(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromI16(i16 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromI16(i16 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromI16(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromI32(i32 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromI32(i32 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromI32(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_UTF8STR PNSLR_StringFromI64(i64 value, PNSLR_Allocator allocator, PNSLR_IntegerBase base);
utf8str Panshilar::StringFromI64(i64 value, Panshilar::Allocator allocator, Panshilar::IntegerBase base)
{
    PNSLR_UTF8STR zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StringFromI64(PNSLR_Bindings_Convert(value), PNSLR_Bindings_Convert(allocator), PNSLR_Bindings_Convert(base)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_BooleanFromString(PNSLR_UTF8STR str, b8* value);
b8 Panshilar::BooleanFromString(utf8str str, b8* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_BooleanFromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_F32FromString(PNSLR_UTF8STR str, f32* value);
b8 Panshilar::F32FromString(utf8str str, f32* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_F32FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_F64FromString(PNSLR_UTF8STR str, f64* value);
b8 Panshilar::F64FromString(utf8str str, f64* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_F64FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_U8FromString(PNSLR_UTF8STR str, u8* value);
b8 Panshilar::U8FromString(utf8str str, u8* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_U8FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_U16FromString(PNSLR_UTF8STR str, u16* value);
b8 Panshilar::U16FromString(utf8str str, u16* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_U16FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_U32FromString(PNSLR_UTF8STR str, u32* value);
b8 Panshilar::U32FromString(utf8str str, u32* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_U32FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_U64FromString(PNSLR_UTF8STR str, u64* value);
b8 Panshilar::U64FromString(utf8str str, u64* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_U64FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_I8FromString(PNSLR_UTF8STR str, i8* value);
b8 Panshilar::I8FromString(utf8str str, i8* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_I8FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_I16FromString(PNSLR_UTF8STR str, i16* value);
b8 Panshilar::I16FromString(utf8str str, i16* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_I16FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_I32FromString(PNSLR_UTF8STR str, i32* value);
b8 Panshilar::I32FromString(utf8str str, i32* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_I32FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_I64FromString(PNSLR_UTF8STR str, i64* value);
b8 Panshilar::I64FromString(utf8str str, i64* value)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_I64FromString(PNSLR_Bindings_Convert(str), PNSLR_Bindings_Convert(value)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
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

enum class PNSLR_PathNormalisationType : u8 { };
static_assert(sizeof(PNSLR_PathNormalisationType) == sizeof(Panshilar::PathNormalisationType), "size mismatch");
static_assert(alignof(PNSLR_PathNormalisationType) == alignof(Panshilar::PathNormalisationType), "align mismatch");
PNSLR_PathNormalisationType* PNSLR_Bindings_Convert(Panshilar::PathNormalisationType* x) { return reinterpret_cast<PNSLR_PathNormalisationType*>(x); }
Panshilar::PathNormalisationType* PNSLR_Bindings_Convert(PNSLR_PathNormalisationType* x) { return reinterpret_cast<Panshilar::PathNormalisationType*>(x); }
PNSLR_PathNormalisationType& PNSLR_Bindings_Convert(Panshilar::PathNormalisationType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PathNormalisationType& PNSLR_Bindings_Convert(PNSLR_PathNormalisationType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" PNSLR_Path PNSLR_NormalisePath(PNSLR_UTF8STR path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::NormalisePath(utf8str path, Panshilar::PathNormalisationType type, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_NormalisePath(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(type), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_SplitPath(PNSLR_Path path, PNSLR_Path* parent, PNSLR_UTF8STR* selfNameWithExtension, PNSLR_UTF8STR* selfName, PNSLR_UTF8STR* extension);
b8 Panshilar::SplitPath(Panshilar::Path path, Panshilar::Path* parent, utf8str* selfNameWithExtension, utf8str* selfName, utf8str* extension)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SplitPath(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(parent), PNSLR_Bindings_Convert(selfNameWithExtension), PNSLR_Bindings_Convert(selfName), PNSLR_Bindings_Convert(extension)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Path PNSLR_GetPathForChildFile(PNSLR_Path dir, PNSLR_UTF8STR fileNameWithExtension, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::GetPathForChildFile(Panshilar::Path dir, utf8str fileNameWithExtension, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetPathForChildFile(PNSLR_Bindings_Convert(dir), PNSLR_Bindings_Convert(fileNameWithExtension), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Path PNSLR_GetPathForSubdirectory(PNSLR_Path dir, PNSLR_UTF8STR dirName, PNSLR_Allocator allocator);
Panshilar::Path Panshilar::GetPathForSubdirectory(Panshilar::Path dir, utf8str dirName, Panshilar::Allocator allocator)
{
    PNSLR_Path zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetPathForSubdirectory(PNSLR_Bindings_Convert(dir), PNSLR_Bindings_Convert(dirName), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" typedef b8 (*PNSLR_DirectoryIterationVisitorDelegate)(rawptr payload, PNSLR_Path path, b8 isDirectory, b8* exploreCurrentDirectory);
static_assert(sizeof(PNSLR_DirectoryIterationVisitorDelegate) == sizeof(Panshilar::DirectoryIterationVisitorDelegate), "size mismatch");
static_assert(alignof(PNSLR_DirectoryIterationVisitorDelegate) == alignof(Panshilar::DirectoryIterationVisitorDelegate), "align mismatch");
PNSLR_DirectoryIterationVisitorDelegate* PNSLR_Bindings_Convert(Panshilar::DirectoryIterationVisitorDelegate* x) { return reinterpret_cast<PNSLR_DirectoryIterationVisitorDelegate*>(x); }
Panshilar::DirectoryIterationVisitorDelegate* PNSLR_Bindings_Convert(PNSLR_DirectoryIterationVisitorDelegate* x) { return reinterpret_cast<Panshilar::DirectoryIterationVisitorDelegate*>(x); }
PNSLR_DirectoryIterationVisitorDelegate& PNSLR_Bindings_Convert(Panshilar::DirectoryIterationVisitorDelegate& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::DirectoryIterationVisitorDelegate& PNSLR_Bindings_Convert(PNSLR_DirectoryIterationVisitorDelegate& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" void PNSLR_IterateDirectory(PNSLR_Path path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc);
void Panshilar::IterateDirectory(Panshilar::Path path, b8 recursive, rawptr visitorPayload, Panshilar::DirectoryIterationVisitorDelegate visitorFunc)
{
    PNSLR_IterateDirectory(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(recursive), PNSLR_Bindings_Convert(visitorPayload), PNSLR_Bindings_Convert(visitorFunc));
}

enum class PNSLR_PathExistsCheckType : u8 { };
static_assert(sizeof(PNSLR_PathExistsCheckType) == sizeof(Panshilar::PathExistsCheckType), "size mismatch");
static_assert(alignof(PNSLR_PathExistsCheckType) == alignof(Panshilar::PathExistsCheckType), "align mismatch");
PNSLR_PathExistsCheckType* PNSLR_Bindings_Convert(Panshilar::PathExistsCheckType* x) { return reinterpret_cast<PNSLR_PathExistsCheckType*>(x); }
Panshilar::PathExistsCheckType* PNSLR_Bindings_Convert(PNSLR_PathExistsCheckType* x) { return reinterpret_cast<Panshilar::PathExistsCheckType*>(x); }
PNSLR_PathExistsCheckType& PNSLR_Bindings_Convert(Panshilar::PathExistsCheckType& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::PathExistsCheckType& PNSLR_Bindings_Convert(PNSLR_PathExistsCheckType& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" b8 PNSLR_PathExists(PNSLR_Path path, PNSLR_PathExistsCheckType type);
b8 Panshilar::PathExists(Panshilar::Path path, Panshilar::PathExistsCheckType type)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_PathExists(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(type)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_DeletePath(PNSLR_Path path);
b8 Panshilar::DeletePath(Panshilar::Path path)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_DeletePath(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i64 PNSLR_GetFileTimestamp(PNSLR_Path path);
i64 Panshilar::GetFileTimestamp(Panshilar::Path path)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetFileTimestamp(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i64 PNSLR_GetFileSize(PNSLR_Path path);
i64 Panshilar::GetFileSize(Panshilar::Path path)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetFileSize(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CreateDirectoryTree(PNSLR_Path path);
b8 Panshilar::CreateDirectoryTree(Panshilar::Path path)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CreateDirectoryTree(PNSLR_Bindings_Convert(path)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

struct PNSLR_File
{
   rawptr handle;
};
static_assert(sizeof(PNSLR_File) == sizeof(Panshilar::File), "size mismatch");
static_assert(alignof(PNSLR_File) == alignof(Panshilar::File), "align mismatch");
PNSLR_File* PNSLR_Bindings_Convert(Panshilar::File* x) { return reinterpret_cast<PNSLR_File*>(x); }
Panshilar::File* PNSLR_Bindings_Convert(PNSLR_File* x) { return reinterpret_cast<Panshilar::File*>(x); }
PNSLR_File& PNSLR_Bindings_Convert(Panshilar::File& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::File& PNSLR_Bindings_Convert(PNSLR_File& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_File, handle) == PNSLR_STRUCT_OFFSET(Panshilar::File, handle), "handle offset mismatch");

extern "C" PNSLR_File PNSLR_OpenFileToRead(PNSLR_Path path, b8 allowWrite);
Panshilar::File Panshilar::OpenFileToRead(Panshilar::Path path, b8 allowWrite)
{
    PNSLR_File zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_OpenFileToRead(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(allowWrite)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_File PNSLR_OpenFileToWrite(PNSLR_Path path, b8 append, b8 allowRead);
Panshilar::File Panshilar::OpenFileToWrite(Panshilar::Path path, b8 append, b8 allowRead)
{
    PNSLR_File zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_OpenFileToWrite(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(append), PNSLR_Bindings_Convert(allowRead)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i64 PNSLR_GetSizeOfFile(PNSLR_File handle);
i64 Panshilar::GetSizeOfFile(Panshilar::File handle)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetSizeOfFile(PNSLR_Bindings_Convert(handle)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i64 PNSLR_GetCurrentPositionInFile(PNSLR_File handle);
i64 Panshilar::GetCurrentPositionInFile(Panshilar::File handle)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetCurrentPositionInFile(PNSLR_Bindings_Convert(handle)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_SeekPositionInFile(PNSLR_File handle, i64 newPos, b8 relative);
b8 Panshilar::SeekPositionInFile(Panshilar::File handle, i64 newPos, b8 relative)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SeekPositionInFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(newPos), PNSLR_Bindings_Convert(relative)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_ReadFromFile(PNSLR_File handle, PNSLR_ArraySlice_u8 dst, i64* readSize);
b8 Panshilar::ReadFromFile(Panshilar::File handle, ArraySlice<u8> dst, i64* readSize)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReadFromFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(dst), PNSLR_Bindings_Convert(readSize)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_WriteToFile(PNSLR_File handle, PNSLR_ArraySlice_u8 src);
b8 Panshilar::WriteToFile(Panshilar::File handle, ArraySlice<u8> src)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WriteToFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(src)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_FormatAndWriteToFile(PNSLR_File handle, PNSLR_UTF8STR fmtStr, PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions args);
b8 Panshilar::FormatAndWriteToFile(Panshilar::File handle, utf8str fmtStr, ArraySlice<Panshilar::PrimitiveFmtOptions> args)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FormatAndWriteToFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(fmtStr), PNSLR_Bindings_Convert(args)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_TruncateFile(PNSLR_File handle, i64 newSize);
b8 Panshilar::TruncateFile(Panshilar::File handle, i64 newSize)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TruncateFile(PNSLR_Bindings_Convert(handle), PNSLR_Bindings_Convert(newSize)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_FlushFile(PNSLR_File handle);
b8 Panshilar::FlushFile(Panshilar::File handle)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FlushFile(PNSLR_Bindings_Convert(handle)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_CloseFileHandle(PNSLR_File handle);
void Panshilar::CloseFileHandle(Panshilar::File handle)
{
    PNSLR_CloseFileHandle(PNSLR_Bindings_Convert(handle));
}

extern "C" b8 PNSLR_ReadAllContentsFromFile(PNSLR_Path path, PNSLR_ArraySlice_u8* dst, PNSLR_Allocator allocator);
b8 Panshilar::ReadAllContentsFromFile(Panshilar::Path path, ArraySlice<u8>* dst, Panshilar::Allocator allocator)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReadAllContentsFromFile(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(dst), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_WriteAllContentsToFile(PNSLR_Path path, PNSLR_ArraySlice_u8 src, b8 append);
b8 Panshilar::WriteAllContentsToFile(Panshilar::Path path, ArraySlice<u8> src, b8 append)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WriteAllContentsToFile(PNSLR_Bindings_Convert(path), PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(append)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_CopyFile(PNSLR_Path src, PNSLR_Path dst);
b8 Panshilar::CopyFile(Panshilar::Path src, Panshilar::Path dst)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_CopyFile(PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(dst)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_MoveFile(PNSLR_Path src, PNSLR_Path dst);
b8 Panshilar::MoveFile(Panshilar::Path src, Panshilar::Path dst)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_MoveFile(PNSLR_Bindings_Convert(src), PNSLR_Bindings_Convert(dst)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i32 PNSLR_PrintToStdOut(PNSLR_UTF8STR message);
i32 Panshilar::PrintToStdOut(utf8str message)
{
    i32 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_PrintToStdOut(PNSLR_Bindings_Convert(message)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_ExitProcess(i32 exitCode);
void Panshilar::ExitProcess(i32 exitCode)
{
    PNSLR_ExitProcess(PNSLR_Bindings_Convert(exitCode));
}

typedef PNSLR_ArraySlice_u8 PNSLR_IPAddress;

typedef PNSLR_ArraySlice_u8 PNSLR_IPMask;

struct PNSLR_IPNetwork
{
   PNSLR_IPAddress address;
   PNSLR_IPMask mask;
};
static_assert(sizeof(PNSLR_IPNetwork) == sizeof(Panshilar::IPNetwork), "size mismatch");
static_assert(alignof(PNSLR_IPNetwork) == alignof(Panshilar::IPNetwork), "align mismatch");
PNSLR_IPNetwork* PNSLR_Bindings_Convert(Panshilar::IPNetwork* x) { return reinterpret_cast<PNSLR_IPNetwork*>(x); }
Panshilar::IPNetwork* PNSLR_Bindings_Convert(PNSLR_IPNetwork* x) { return reinterpret_cast<Panshilar::IPNetwork*>(x); }
PNSLR_IPNetwork& PNSLR_Bindings_Convert(Panshilar::IPNetwork& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::IPNetwork& PNSLR_Bindings_Convert(PNSLR_IPNetwork& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_IPNetwork, address) == PNSLR_STRUCT_OFFSET(Panshilar::IPNetwork, address), "address offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_IPNetwork, mask) == PNSLR_STRUCT_OFFSET(Panshilar::IPNetwork, mask), "mask offset mismatch");

typedef struct { PNSLR_IPNetwork* data; i64 count; } PNSLR_ArraySlice_PNSLR_IPNetwork;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_IPNetwork) == sizeof(ArraySlice<Panshilar::IPNetwork>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_IPNetwork) == alignof(ArraySlice<Panshilar::IPNetwork>), "align mismatch");
PNSLR_ArraySlice_PNSLR_IPNetwork* PNSLR_Bindings_Convert(ArraySlice<Panshilar::IPNetwork>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_IPNetwork*>(x); }
ArraySlice<Panshilar::IPNetwork>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_IPNetwork* x) { return reinterpret_cast<ArraySlice<Panshilar::IPNetwork>*>(x); }
PNSLR_ArraySlice_PNSLR_IPNetwork& PNSLR_Bindings_Convert(ArraySlice<Panshilar::IPNetwork>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<Panshilar::IPNetwork>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_IPNetwork& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_IPNetwork, count) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::IPNetwork>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_IPNetwork, data) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::IPNetwork>, data), "data offset mismatch");

extern "C" b8 PNSLR_GetInterfaceIPAddresses(PNSLR_ArraySlice_PNSLR_IPNetwork* networks, PNSLR_Allocator allocator);
b8 Panshilar::GetInterfaceIPAddresses(ArraySlice<Panshilar::IPNetwork>* networks, Panshilar::Allocator allocator)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetInterfaceIPAddresses(PNSLR_Bindings_Convert(networks), PNSLR_Bindings_Convert(allocator)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

enum class PNSLR_StreamMode : u8 { };
static_assert(sizeof(PNSLR_StreamMode) == sizeof(Panshilar::StreamMode), "size mismatch");
static_assert(alignof(PNSLR_StreamMode) == alignof(Panshilar::StreamMode), "align mismatch");
PNSLR_StreamMode* PNSLR_Bindings_Convert(Panshilar::StreamMode* x) { return reinterpret_cast<PNSLR_StreamMode*>(x); }
Panshilar::StreamMode* PNSLR_Bindings_Convert(PNSLR_StreamMode* x) { return reinterpret_cast<Panshilar::StreamMode*>(x); }
PNSLR_StreamMode& PNSLR_Bindings_Convert(Panshilar::StreamMode& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StreamMode& PNSLR_Bindings_Convert(PNSLR_StreamMode& x) { return *PNSLR_Bindings_Convert(&x); }

extern "C" typedef b8 (*PNSLR_StreamProcedure)(rawptr streamData, PNSLR_StreamMode mode, PNSLR_ArraySlice_u8 data, i64 offset, i64* extraRet);
static_assert(sizeof(PNSLR_StreamProcedure) == sizeof(Panshilar::StreamProcedure), "size mismatch");
static_assert(alignof(PNSLR_StreamProcedure) == alignof(Panshilar::StreamProcedure), "align mismatch");
PNSLR_StreamProcedure* PNSLR_Bindings_Convert(Panshilar::StreamProcedure* x) { return reinterpret_cast<PNSLR_StreamProcedure*>(x); }
Panshilar::StreamProcedure* PNSLR_Bindings_Convert(PNSLR_StreamProcedure* x) { return reinterpret_cast<Panshilar::StreamProcedure*>(x); }
PNSLR_StreamProcedure& PNSLR_Bindings_Convert(Panshilar::StreamProcedure& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::StreamProcedure& PNSLR_Bindings_Convert(PNSLR_StreamProcedure& x) { return *PNSLR_Bindings_Convert(&x); }

struct PNSLR_Stream
{
   PNSLR_StreamProcedure procedure;
   rawptr data;
};
static_assert(sizeof(PNSLR_Stream) == sizeof(Panshilar::Stream), "size mismatch");
static_assert(alignof(PNSLR_Stream) == alignof(Panshilar::Stream), "align mismatch");
PNSLR_Stream* PNSLR_Bindings_Convert(Panshilar::Stream* x) { return reinterpret_cast<PNSLR_Stream*>(x); }
Panshilar::Stream* PNSLR_Bindings_Convert(PNSLR_Stream* x) { return reinterpret_cast<Panshilar::Stream*>(x); }
PNSLR_Stream& PNSLR_Bindings_Convert(Panshilar::Stream& x) { return *PNSLR_Bindings_Convert(&x); }
Panshilar::Stream& PNSLR_Bindings_Convert(PNSLR_Stream& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Stream, procedure) == PNSLR_STRUCT_OFFSET(Panshilar::Stream, procedure), "procedure offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_Stream, data) == PNSLR_STRUCT_OFFSET(Panshilar::Stream, data), "data offset mismatch");

typedef struct { PNSLR_Stream* data; i64 count; } PNSLR_ArraySlice_PNSLR_Stream;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_Stream) == sizeof(ArraySlice<Panshilar::Stream>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_Stream) == alignof(ArraySlice<Panshilar::Stream>), "align mismatch");
PNSLR_ArraySlice_PNSLR_Stream* PNSLR_Bindings_Convert(ArraySlice<Panshilar::Stream>* x) { return reinterpret_cast<PNSLR_ArraySlice_PNSLR_Stream*>(x); }
ArraySlice<Panshilar::Stream>* PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Stream* x) { return reinterpret_cast<ArraySlice<Panshilar::Stream>*>(x); }
PNSLR_ArraySlice_PNSLR_Stream& PNSLR_Bindings_Convert(ArraySlice<Panshilar::Stream>& x) { return *PNSLR_Bindings_Convert(&x); }
ArraySlice<Panshilar::Stream>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Stream& x) { return *PNSLR_Bindings_Convert(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Stream, count) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::Stream>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Stream, data) == PNSLR_STRUCT_OFFSET(ArraySlice<Panshilar::Stream>, data), "data offset mismatch");

extern "C" i64 PNSLR_GetSizeOfStream(PNSLR_Stream stream);
i64 Panshilar::GetSizeOfStream(Panshilar::Stream stream)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetSizeOfStream(PNSLR_Bindings_Convert(stream)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" i64 PNSLR_GetCurrentPositionInStream(PNSLR_Stream stream);
i64 Panshilar::GetCurrentPositionInStream(Panshilar::Stream stream)
{
    i64 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_GetCurrentPositionInStream(PNSLR_Bindings_Convert(stream)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_SeekPositionInStream(PNSLR_Stream stream, i64 newPos, b8 relative);
b8 Panshilar::SeekPositionInStream(Panshilar::Stream stream, i64 newPos, b8 relative)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_SeekPositionInStream(PNSLR_Bindings_Convert(stream), PNSLR_Bindings_Convert(newPos), PNSLR_Bindings_Convert(relative)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_ReadFromStream(PNSLR_Stream stream, PNSLR_ArraySlice_u8 dst, i64* readSize);
b8 Panshilar::ReadFromStream(Panshilar::Stream stream, ArraySlice<u8> dst, i64* readSize)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_ReadFromStream(PNSLR_Bindings_Convert(stream), PNSLR_Bindings_Convert(dst), PNSLR_Bindings_Convert(readSize)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_WriteToStream(PNSLR_Stream stream, PNSLR_ArraySlice_u8 src);
b8 Panshilar::WriteToStream(Panshilar::Stream stream, ArraySlice<u8> src)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_WriteToStream(PNSLR_Bindings_Convert(stream), PNSLR_Bindings_Convert(src)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_FormatAndWriteToStream(PNSLR_Stream stream, PNSLR_UTF8STR fmtStr, PNSLR_ArraySlice_PNSLR_PrimitiveFmtOptions args);
b8 Panshilar::FormatAndWriteToStream(Panshilar::Stream stream, utf8str fmtStr, ArraySlice<Panshilar::PrimitiveFmtOptions> args)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FormatAndWriteToStream(PNSLR_Bindings_Convert(stream), PNSLR_Bindings_Convert(fmtStr), PNSLR_Bindings_Convert(args)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_TruncateStream(PNSLR_Stream stream, i64 newSize);
b8 Panshilar::TruncateStream(Panshilar::Stream stream, i64 newSize)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_TruncateStream(PNSLR_Bindings_Convert(stream), PNSLR_Bindings_Convert(newSize)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" b8 PNSLR_FlushStream(PNSLR_Stream stream);
b8 Panshilar::FlushStream(Panshilar::Stream stream)
{
    b8 zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_FlushStream(PNSLR_Bindings_Convert(stream)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" void PNSLR_CloseStream(PNSLR_Stream stream);
void Panshilar::CloseStream(Panshilar::Stream stream)
{
    PNSLR_CloseStream(PNSLR_Bindings_Convert(stream));
}

extern "C" PNSLR_Stream PNSLR_StreamFromFile(PNSLR_File file);
Panshilar::Stream Panshilar::StreamFromFile(Panshilar::File file)
{
    PNSLR_Stream zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StreamFromFile(PNSLR_Bindings_Convert(file)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

extern "C" PNSLR_Stream PNSLR_StreamFromStringBuilder(PNSLR_StringBuilder* builder);
Panshilar::Stream Panshilar::StreamFromStringBuilder(Panshilar::StringBuilder* builder)
{
    PNSLR_Stream zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = PNSLR_StreamFromStringBuilder(PNSLR_Bindings_Convert(builder)); return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW);
}

#undef PNSLR_STRUCT_OFFSET

#endif//PNSLR_CXX_IMPL

