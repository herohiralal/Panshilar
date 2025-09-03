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
    template <typename T> struct ArraySlice { i64 count; T* data; };

    // #######################################################################################
    // Intrinsics
    // #######################################################################################

    // Primitive ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
// #define PNSLR_SKIP_IMPLEMENTATION
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

typedef struct { PNSLR_I64 count; PNSLR_B8* data; } PNSLR_ArraySlice_PNSLR_B8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_B8) == sizeof(Panshilar::ArraySlice<Panshilar::b8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_B8) == alignof(Panshilar::ArraySlice<Panshilar::b8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_B8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::b8>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_B8*>(&x); }
Panshilar::ArraySlice<Panshilar::b8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_B8& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::b8>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_B8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::b8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_B8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::b8>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_U8* data; } PNSLR_ArraySlice_PNSLR_U8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U8) == sizeof(Panshilar::ArraySlice<Panshilar::u8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U8) == alignof(Panshilar::ArraySlice<Panshilar::u8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u8>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_U8*>(&x); }
Panshilar::ArraySlice<Panshilar::u8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U8& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::u8>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u8>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_U16* data; } PNSLR_ArraySlice_PNSLR_U16;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U16) == sizeof(Panshilar::ArraySlice<Panshilar::u16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U16) == alignof(Panshilar::ArraySlice<Panshilar::u16>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U16& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u16>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_U16*>(&x); }
Panshilar::ArraySlice<Panshilar::u16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U16& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::u16>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U16, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U16, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u16>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_U32* data; } PNSLR_ArraySlice_PNSLR_U32;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U32) == sizeof(Panshilar::ArraySlice<Panshilar::u32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U32) == alignof(Panshilar::ArraySlice<Panshilar::u32>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U32& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u32>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_U32*>(&x); }
Panshilar::ArraySlice<Panshilar::u32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U32& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::u32>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U32, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U32, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u32>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_U64* data; } PNSLR_ArraySlice_PNSLR_U64;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_U64) == sizeof(Panshilar::ArraySlice<Panshilar::u64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_U64) == alignof(Panshilar::ArraySlice<Panshilar::u64>), "align mismatch");
PNSLR_ArraySlice_PNSLR_U64& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::u64>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_U64*>(&x); }
Panshilar::ArraySlice<Panshilar::u64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_U64& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::u64>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U64, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_U64, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::u64>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_I8* data; } PNSLR_ArraySlice_PNSLR_I8;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I8) == sizeof(Panshilar::ArraySlice<Panshilar::i8>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I8) == alignof(Panshilar::ArraySlice<Panshilar::i8>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I8& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i8>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_I8*>(&x); }
Panshilar::ArraySlice<Panshilar::i8>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I8& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::i8>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I8, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i8>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I8, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i8>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_I16* data; } PNSLR_ArraySlice_PNSLR_I16;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I16) == sizeof(Panshilar::ArraySlice<Panshilar::i16>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I16) == alignof(Panshilar::ArraySlice<Panshilar::i16>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I16& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i16>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_I16*>(&x); }
Panshilar::ArraySlice<Panshilar::i16>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I16& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::i16>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I16, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i16>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I16, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i16>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_I32* data; } PNSLR_ArraySlice_PNSLR_I32;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I32) == sizeof(Panshilar::ArraySlice<Panshilar::i32>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I32) == alignof(Panshilar::ArraySlice<Panshilar::i32>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I32& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i32>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_I32*>(&x); }
Panshilar::ArraySlice<Panshilar::i32>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I32& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::i32>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I32, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i32>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I32, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i32>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; PNSLR_I64* data; } PNSLR_ArraySlice_PNSLR_I64;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_I64) == sizeof(Panshilar::ArraySlice<Panshilar::i64>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_I64) == alignof(Panshilar::ArraySlice<Panshilar::i64>), "align mismatch");
PNSLR_ArraySlice_PNSLR_I64& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::i64>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_I64*>(&x); }
Panshilar::ArraySlice<Panshilar::i64>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_I64& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::i64>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I64, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i64>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_I64, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::i64>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; float* data; } PNSLR_ArraySlice_float;
static_assert(sizeof(PNSLR_ArraySlice_float) == sizeof(Panshilar::ArraySlice<float>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_float) == alignof(Panshilar::ArraySlice<float>), "align mismatch");
PNSLR_ArraySlice_float& PNSLR_Bindings_Convert(Panshilar::ArraySlice<float>& x) { return *reinterpret_cast<PNSLR_ArraySlice_float*>(&x); }
Panshilar::ArraySlice<float>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_float& x) { return *reinterpret_cast<Panshilar::ArraySlice<float>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_float, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<float>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_float, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<float>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; double* data; } PNSLR_ArraySlice_double;
static_assert(sizeof(PNSLR_ArraySlice_double) == sizeof(Panshilar::ArraySlice<double>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_double) == alignof(Panshilar::ArraySlice<double>), "align mismatch");
PNSLR_ArraySlice_double& PNSLR_Bindings_Convert(Panshilar::ArraySlice<double>& x) { return *reinterpret_cast<PNSLR_ArraySlice_double*>(&x); }
Panshilar::ArraySlice<double>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_double& x) { return *reinterpret_cast<Panshilar::ArraySlice<double>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_double, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<double>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_double, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<double>, data), "data offset mismatch");

typedef struct { PNSLR_I64 count; char* data; } PNSLR_ArraySlice_char;
static_assert(sizeof(PNSLR_ArraySlice_char) == sizeof(Panshilar::ArraySlice<char>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_char) == alignof(Panshilar::ArraySlice<char>), "align mismatch");
PNSLR_ArraySlice_char& PNSLR_Bindings_Convert(Panshilar::ArraySlice<char>& x) { return *reinterpret_cast<PNSLR_ArraySlice_char*>(&x); }
Panshilar::ArraySlice<char>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_char& x) { return *reinterpret_cast<Panshilar::ArraySlice<char>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<char>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_char, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<char>, data), "data offset mismatch");

    typedef PNSLR_ArraySlice_PNSLR_U8 PNSLR_UTF8STR;

typedef struct { PNSLR_I64 count; PNSLR_UTF8STR* data; } PNSLR_ArraySlice_PNSLR_UTF8STR;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_UTF8STR) == sizeof(Panshilar::ArraySlice<Panshilar::utf8str>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_UTF8STR) == alignof(Panshilar::ArraySlice<Panshilar::utf8str>), "align mismatch");
PNSLR_ArraySlice_PNSLR_UTF8STR& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::utf8str>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_UTF8STR*>(&x); }
Panshilar::ArraySlice<Panshilar::utf8str>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_UTF8STR& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::utf8str>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::utf8str>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_UTF8STR, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::utf8str>, data), "data offset mismatch");












































typedef struct { PNSLR_I64 count; PNSLR_Allocator* data; } PNSLR_ArraySlice_PNSLR_Allocator;
static_assert(sizeof(PNSLR_ArraySlice_PNSLR_Allocator) == sizeof(Panshilar::ArraySlice<Panshilar::Allocator>), "size mismatch");
static_assert(alignof(PNSLR_ArraySlice_PNSLR_Allocator) == alignof(Panshilar::ArraySlice<Panshilar::Allocator>), "align mismatch");
PNSLR_ArraySlice_PNSLR_Allocator& PNSLR_Bindings_Convert(Panshilar::ArraySlice<Panshilar::Allocator>& x) { return *reinterpret_cast<PNSLR_ArraySlice_PNSLR_Allocator*>(&x); }
Panshilar::ArraySlice<Panshilar::Allocator>& PNSLR_Bindings_Convert(PNSLR_ArraySlice_PNSLR_Allocator& x) { return *reinterpret_cast<Panshilar::ArraySlice<Panshilar::Allocator>*>(&x); }
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, count) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::Allocator>, count), "count offset mismatch");
static_assert(PNSLR_STRUCT_OFFSET(PNSLR_ArraySlice_PNSLR_Allocator, data) == PNSLR_STRUCT_OFFSET(Panshilar::ArraySlice<Panshilar::Allocator>, data), "data offset mismatch");





















































































#undef PNSLR_STRUCT_OFFSET

#endif//PNSLR_SKIP_IMPLEMENTATION
#endif//__cplusplus
