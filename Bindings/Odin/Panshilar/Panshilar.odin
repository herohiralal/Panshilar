package Panshilar

import "../Intrinsics"

// #######################################################################################
// Environment
// #######################################################################################

/**
 * Defines the platforms supported by the library.
 */
Platform :: enum u8 {
	Unknown = 0,
	Windows = 1,
	Linux = 2,
	OSX = 3,
	Android = 4,
	iOS = 5,
	PS5 = 6,
	XBoxSeries = 7,
	Switch = 8,
}

/**
 * Defines the architectures supported by the library.
 */
Architecture :: enum u8 {
	Unknown = 0,
	X64 = 1,
	ARM64 = 2,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Get the current platform.
     */
	GetPlatform :: proc "c" () -> Platform ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Get the current architecture.
     */
	GetArchitecture :: proc "c" () -> Architecture ---
}

// #######################################################################################
// Runtime
// #######################################################################################

/**
 * Defines the source code location for debugging purposes.
 */
SourceCodeLocation :: struct  {
	file: string,
	line: i32,
	column: i32,
	function: string,
}

// #######################################################################################
// Sync
// #######################################################################################

// Mutex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * The most basic synchronization primitive.
 */
Mutex :: struct #align(8)  {
	buffer: [64]u8,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Creates a mutex.
     */
	CreateMutex :: proc "c" () -> Mutex ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroys a mutex.
     */
	DestroyMutex :: proc "c" (
		mutex: ^Mutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Locks a mutex.
     */
	LockMutex :: proc "c" (
		mutex: ^Mutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Unlocks a mutex.
     */
	UnlockMutex :: proc "c" (
		mutex: ^Mutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Tries to lock a mutex.
     * Returns true if the mutex was successfully locked, false otherwise.
     */
	TryLockMutex :: proc "c" (
		mutex: ^Mutex,
	) -> b8 ---
}

// Read-Write ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A read-write mutex.
 * This is a synchronization primitive that allows multiple readers or a single writer.
 * It is useful for scenarios where reads are more frequent than writes.
 */
RWMutex :: struct #align(8)  {
	buffer: [200]u8,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Creates a read-write mutex.
     */
	CreateRWMutex :: proc "c" () -> RWMutex ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroys a read-write mutex.
     */
	DestroyRWMutex :: proc "c" (
		rwmutex: ^RWMutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Locks a read-write mutex for reading.
     * Multiple threads can read simultaneously.
     */
	LockRWMutexShared :: proc "c" (
		rwmutex: ^RWMutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Locks a read-write mutex for writing.
     * Only one thread can write at a time, and no other threads can read while writing.
     */
	LockRWMutexExclusive :: proc "c" (
		rwmutex: ^RWMutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Unlocks a read-write mutex after reading.
     * Allows other threads to read or write.
     */
	UnlockRWMutexShared :: proc "c" (
		rwmutex: ^RWMutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Unlocks a read-write mutex after writing.
     * Allows other threads to read or write.
     */
	UnlockRWMutexExclusive :: proc "c" (
		rwmutex: ^RWMutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Tries to lock a read-write mutex for reading.
     * Returns true if the mutex was successfully locked for reading, false otherwise.
     */
	TryLockRWMutexShared :: proc "c" (
		rwmutex: ^RWMutex,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Tries to lock a read-write mutex for writing.
     * Returns true if the mutex was successfully locked for writing, false otherwise.
     */
	TryLockRWMutexExclusive :: proc "c" (
		rwmutex: ^RWMutex,
	) -> b8 ---
}

// Semaphore ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A semaphore synchronization primitive.
 * It allows a certain number of threads to access a resource concurrently.
 */
Semaphore :: struct #align(8)  {
	buffer: [32]u8,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Creates a semaphore.
     * The initial count specifies how many threads can access the resource concurrently.
     */
	CreateSemaphore :: proc "c" (
		initialCount: i32,
	) -> Semaphore ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroys a semaphore.
     */
	DestroySemaphore :: proc "c" (
		semaphore: ^Semaphore,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Waits on a semaphore.
     * The calling thread will block until the semaphore count is greater than zero.
     */
	WaitSemaphore :: proc "c" (
		semaphore: ^Semaphore,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Waits on a semaphore with a timeout.
     * The calling thread will block until the semaphore count is greater than zero or the timeout expires.
     * Returns true if the semaphore was acquired, false if the timeout expired.
     */
	WaitSemaphoreTimeout :: proc "c" (
		semaphore: ^Semaphore,
		timeoutNs: i32,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Signals a semaphore, incrementing its count by a specified amount.
     * If the count was zero, this will wake up one or more waiting threads.
     */
	SignalSemaphore :: proc "c" (
		semaphore: ^Semaphore,
		count: i32,
	) ---
}

// Condition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A condition variable for signaling between threads.
 * It allows threads to wait for a condition to be signaled.
 */
ConditionVariable :: struct #align(8)  {
	buffer: [48]u8,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Creates a condition variable.
     */
	CreateConditionVariable :: proc "c" () -> ConditionVariable ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroys a condition variable.
     */
	DestroyConditionVariable :: proc "c" (
		condvar: ^ConditionVariable,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Waits on a condition variable.
     * The calling thread will block until the condition variable is signaled.
     * The mutex must be locked before calling this function.
     */
	WaitConditionVariable :: proc "c" (
		condvar: ^ConditionVariable,
		mutex: ^Mutex,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Waits on a condition variable with a timeout.
     * The calling thread will block until the condition variable is signaled or the timeout expires.
     * The mutex must be locked before calling this function.
     * Returns true if the condition variable was signaled, false if the timeout expired.
     */
	WaitConditionVariableTimeout :: proc "c" (
		condvar: ^ConditionVariable,
		mutex: ^Mutex,
		timeoutNs: i32,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Signals a condition variable, waking up one waiting thread.
     * If no threads are waiting, this has no effect.
     */
	SignalConditionVariable :: proc "c" (
		condvar: ^ConditionVariable,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Signals a condition variable, waking up all waiting threads.
     * If no threads are waiting, this has no effect.
     */
	BroadcastConditionVariable :: proc "c" (
		condvar: ^ConditionVariable,
	) ---
}

// #######################################################################################
// Memory
// #######################################################################################

@(link_prefix="PNSLR_")
foreign {
    /**
     * Set a block of memory to a specific value.
     */
	MemSet :: proc "c" (
		memory: rawptr,
		value: i32,
		size: i32,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Copy a block of memory from source to destination.
     */
	MemCopy :: proc "c" (
		destination: rawptr,
		source: rawptr,
		size: i32,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Copy a block of memory from source to destination, handling overlapping regions.
     */
	MemMove :: proc "c" (
		destination: rawptr,
		source: rawptr,
		size: i32,
	) ---
}

// #######################################################################################
// Allocators
// #######################################################################################

// Allocator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Defines the mode to be used when calling the allocator function.
 */
AllocatorMode :: enum u8 {
	Allocate = 0,
	Resize = 1,
	Free = 2,
	FreeAll = 3,
	AllocateNoZero = 4,
	ResizeNoZero = 5,
	QueryCapabilities = 255,
}

/**
 * Defines the capabilities of an allocator.
 */
AllocatorCapability :: distinct bit_set[AllocatorCapabilityValues; u64]

AllocatorCapabilityValues :: enum u8 {
	ThreadSafe = 0,
	Resize = 1,
	Free = 2,
	FreeAll = 3,
	HintNil = 26,
	HintBump = 27,
	HintHeap = 28,
	HintTemp = 29,
	HintDebug = 30,
}

/**
 * Defines the error codes that can be returned by the allocator.
 */
AllocatorError :: enum u8 {
	None = 0,
	OutOfMemory = 1,
	InvalidAlignment = 2,
	InvalidSize = 3,
	InvalidMode = 4,
	Internal = 5,
	OutOfOrderFree = 6,
	DoubleFree = 7,
	CantFreeAll = 8,
}

/**
 * Defines the delegate type for the allocator function.
 */
AllocatorProcedure :: #type proc "c" (
	allocatorData: rawptr,
	mode: AllocatorMode,
	size: i32,
	alignment: i32,
	oldMemory: rawptr,
	oldSize: i32,
	location: SourceCodeLocation,
	error: ^AllocatorError,
) -> rawptr

/**
 * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
 */
Allocator :: struct  {
	procedure: AllocatorProcedure,
	data: rawptr,
}

// declare []Allocator

// Allocation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Allocate memory using the provided allocator.
     */
	Allocate :: proc "c" (
		allocator: Allocator,
		zeroed: b8,
		size: i32,
		alignment: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Resize memory using the provided allocator.
     */
	Resize :: proc "c" (
		allocator: Allocator,
		zeroed: b8,
		oldMemory: rawptr,
		oldSize: i32,
		newSize: i32,
		alignment: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Fallback resize function that can be used when the allocator does not support resizing.
     */
	DefaultResize :: proc "c" (
		allocator: Allocator,
		zeroed: b8,
		oldMemory: rawptr,
		oldSize: i32,
		newSize: i32,
		alignment: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Free memory using the provided allocator.
     */
	Free :: proc "c" (
		allocator: Allocator,
		memory: rawptr,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Free all memory allocated by the provided allocator.
     */
	FreeAll :: proc "c" (
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Query the capabilities of the provided allocator.
     */
	QueryAllocatorCapabilities :: proc "c" (
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> u64 ---
}

// Nil ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
     * Otherwise no-ops all around.
     */
	GetAllocator_Nil :: proc "c" () -> Allocator ---
}

// Default ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Get the default heap allocator.
     */
	GetAllocator_DefaultHeap :: proc "c" () -> Allocator ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Main allocator function for the default heap allocator.
     */
	AllocatorFn_DefaultHeap :: proc "c" (
		allocatorData: rawptr,
		mode: AllocatorMode,
		size: i32,
		alignment: i32,
		oldMemory: rawptr,
		oldSize: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

// Arena ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A block of memory used by the arena allocator.
 */
ArenaAllocatorBlock :: struct  {
	previous: ^ArenaAllocatorBlock,
	allocator: Allocator,
	memory: rawptr,
	capacity: u32,
	used: u32,
}

/**
 * The payload used by the arena allocator.
 */
ArenaAllocatorPayload :: struct  {
	backingAllocator: Allocator,
	currentBlock: ^ArenaAllocatorBlock,
	totalUsed: u32,
	totalCapacity: u32,
	minimumBlockSize: u32,
	numSnapshots: u32,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Create a new arena allocator with the specified backing allocator.
     * The arena allocator will use the backing allocator to allocate its blocks.
     * The arena allocator will not free the backing allocator, so it is the caller's responsibility to
     * free the backing allocator when it is no longer needed.
     */
	NewAllocator_Arena :: proc "c" (
		backingAllocator: Allocator,
		pageSize: u32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> Allocator ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroy an arena allocator and free all its resources.
     * This does not free the backing allocator, only the arena allocator's own resources.
     */
	DestroyAllocator_Arena :: proc "c" (
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Main allocator function for the arena allocator.
     */
	AllocatorFn_Arena :: proc "c" (
		allocatorData: rawptr,
		mode: AllocatorMode,
		size: i32,
		alignment: i32,
		oldMemory: rawptr,
		oldSize: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

ArenaSnapshotError :: enum u8 {
	None = 0,
	InvalidData = 1,
	MemoryBlockNotOwned = 2,
	OutOfOrderRestoreUsage = 3,
	DoubleRestoreOrDiscardUsage = 4,
}

/**
 * A snapshot of the arena allocator, recording its state at a specific point in time.
 * Can be saved/loaded/discarded as needed.
 */
ArenaAllocatorSnapshot :: struct  {
	valid: b8,
	payload: ^ArenaAllocatorPayload,
	block: ^ArenaAllocatorBlock,
	used: u32,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Ensures that the arena allocator has either restored/discarded all the
     * snapshots that were taken.
     */
	ValidateArenaAllocatorSnapshotState :: proc "c" (
		allocator: Allocator,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Captures a snapshot of the arena allocator.
     * The returned value can be used to load back the existing state at this point.
     */
	CaptureArenaAllocatorSnapshot :: proc "c" (
		allocator: Allocator,
	) -> ArenaAllocatorSnapshot ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Restores the state of the arena allocator from a snapshot.
     * Upon success, the snapshot is marked as invalid.
     */
	RestoreArenaAllocatorSnapshot :: proc "c" (
		snapshot: ^ArenaAllocatorSnapshot,
		loc: SourceCodeLocation,
	) -> ArenaSnapshotError ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Discards a snapshot of the arena allocator.
     */
	DiscardArenaAllocatorSnapshot :: proc "c" (
		snapshot: ^ArenaAllocatorSnapshot,
	) -> ArenaSnapshotError ---
}

// Stack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * A page of a stack allocator.
 */
StackAllocatorPage :: struct #align(8)  {
	previousPage: ^StackAllocatorPage,
	usedBytes: u64,
	buffer: [8192]u8,
}

/**
 * The header used for every separate stack allocation.
 * This is used to store metadata about the allocation and deallocate appropriately.
 */
StackAllocationHeader :: struct  {
	page: ^StackAllocatorPage,
	size: i32,
	alignment: i32,
	lastAllocation: rawptr,
	lastAllocationHeader: rawptr,
}

/**
 * The payload used by the stack allocator.
 */
StackAllocatorPayload :: struct  {
	backingAllocator: Allocator,
	currentPage: ^StackAllocatorPage,
	lastAllocation: rawptr,
	lastAllocationHeader: ^StackAllocationHeader,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Create a stack allocator with the specified backing allocator.
     * The stack allocator will use the backing allocator to allocate its pages.
     * The stack allocator will not free the backing allocator, so it is the caller's responsibility to
     * free the backing allocator when it is no longer needed.
     */
	NewAllocator_Stack :: proc "c" (
		backingAllocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> Allocator ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Destroy a stack allocator and free all its resources.
     * This does not free the backing allocator, only the stack allocator's own resources.
     */
	DestroyAllocator_Stack :: proc "c" (
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Main allocator function for the stack allocator.
     */
	AllocatorFn_Stack :: proc "c" (
		allocatorData: rawptr,
		mode: AllocatorMode,
		size: i32,
		alignment: i32,
		oldMemory: rawptr,
		oldSize: i32,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> rawptr ---
}

// Collections ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Allocate a raw array slice of 'count' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
     */
	MakeRawSlice :: proc "c" (
		tySize: i32,
		tyAlign: i32,
		count: i64,
		zeroed: b8,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> Intrinsics.RawArraySlice ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Free a raw array slice allocated with `PNSLR_MakeRawSlice`, using the provided allocator.
     */
	FreeRawSlice :: proc "c" (
		slice: ^Intrinsics.RawArraySlice,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Resize a raw array slice to one with 'newCount' elements, each of size 'tySize' and alignment 'tyAlign', using the provided allocator. Optionally zeroed.
     */
	ResizeRawSlice :: proc "c" (
		slice: ^Intrinsics.RawArraySlice,
		tySize: i32,
		tyAlign: i32,
		newCount: i64,
		zeroed: b8,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
     */
	MakeString :: proc "c" (
		count: i64,
		zeroed: b8,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> string ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
     */
	FreeString :: proc "c" (
		str: string,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
     */
	MakeCString :: proc "c" (
		count: i64,
		zeroed: b8,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) -> cstring ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
     */
	FreeCString :: proc "c" (
		str: cstring,
		allocator: Allocator,
		location: SourceCodeLocation,
		error: ^AllocatorError,
	) ---
}

// #######################################################################################
// Chrono
// #######################################################################################

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns the current time in nanoseconds since the Unix epoch (January 1, 1970).
     */
	NanosecondsSinceUnixEpoch :: proc "c" () -> i64 ---
}

// #######################################################################################
// Strings
// #######################################################################################

// Basics/Conversions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns the length of the given C-style null-terminated string, excluding the null terminator.
     */
	GetCStringLength :: proc "c" (
		str: cstring,
	) -> i32 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Clone a C-style string into a new allocated string.
     */
	StringFromCString :: proc "c" (
		str: cstring,
	) -> string ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Clones a UTF-8 string to a C-style null-terminated string.
     * The returned string is allocated using the specified allocator.
     */
	CStringFromString :: proc "c" (
		str: string,
		allocator: Allocator,
	) -> cstring ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Clones a UTF-8 string to a new allocated UTF-8 string.
     * The returned string is allocated using the specified allocator.
     */
	CloneString :: proc "c" (
		str: string,
		allocator: Allocator,
	) -> string ---
}

// Basic/Manipulation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Concatenates two UTF-8 strings into a new allocated string.
     * The returned string is allocated using the specified allocator.
     */
	ConcatenateStrings :: proc "c" (
		str1: string,
		str2: string,
		allocator: Allocator,
	) -> string ---
}

// Casing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Converts a UTF-8 string to uppercase.
     * The returned string is allocated using the specified allocator.
     */
	UpperString :: proc "c" (
		str: string,
		allocator: Allocator,
	) -> string ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Converts a UTF-8 string to lowercase.
     * The returned string is allocated using the specified allocator.
     */
	LowerString :: proc "c" (
		str: string,
		allocator: Allocator,
	) -> string ---
}

// Comparisons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Represents the type of string comparison to perform.
 */
StringComparisonType :: enum u8 {
	CaseSensitive = 0,
	CaseInsensitive = 1,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if two UTF-8 strings contain the same data.
     * Returns true if they are equal, false otherwise.
     */
	AreStringsEqual :: proc "c" (
		str1: string,
		str2: string,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
     * Returns true if they are equal, false otherwise.
     */
	AreStringAndCStringEqual :: proc "c" (
		str1: string,
		str2: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if two C-style null-terminated strings are equal.
     * Returns true if they are equal, false otherwise.
     */
	AreCStringsEqual :: proc "c" (
		str1: cstring,
		str2: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a UTF-8 string starts with the specified prefix.
     * Returns true if it does, false otherwise.
     */
	StringStartsWith :: proc "c" (
		str: string,
		prefix: string,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a UTF-8 string ends with the specified suffix.
     * Returns true if it does, false otherwise.
     */
	StringEndsWith :: proc "c" (
		str: string,
		suffix: string,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a C-style null-terminated string starts with the specified prefix.
     */
	StringStartsWithCString :: proc "c" (
		str: string,
		prefix: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a C-style null-terminated string ends with the specified suffix.
     */
	StringEndsWithCString :: proc "c" (
		str: string,
		suffix: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns the length of the given C-style null-terminated string, excluding the null terminator.
     */
	CStringStartsWith :: proc "c" (
		str: cstring,
		prefix: string,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
	CStringEndsWith :: proc "c" (
		str: cstring,
		suffix: string,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
     */
	CStringStartsWithCString :: proc "c" (
		str: string,
		prefix: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
     */
	CStringEndsWithCString :: proc "c" (
		str: string,
		suffix: cstring,
		comparisonType: StringComparisonType,
	) -> b8 ---
}

// Advanced ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Searches for the first occurrence of a substring within a string.
     * Returns the index of the first occurrence, or -1 if not found.
     */
	SearchFirstIndexInString :: proc "c" (
		str: string,
		substring: string,
		comparisonType: StringComparisonType,
	) -> i32 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Searches for the last occurrence of a substring within a string.
     * Returns the index of the last occurrence, or -1 if not found.
     */
	SearchLastIndexInString :: proc "c" (
		str: string,
		substring: string,
		comparisonType: StringComparisonType,
	) -> i32 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Replaces all occurrences of a substring within a string with a new value.
     * The returned string is allocated using the specified allocator.
     */
	ReplaceInString :: proc "c" (
		str: string,
		oldValue: string,
		newValue: string,
		allocator: Allocator,
		comparisonType: StringComparisonType,
	) -> string ---
}

// UTF-8 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Result structure for UTF-8 rune encoding.
 * Contains the encoded bytes and the number of bytes used.
 */
EncodedRune :: struct  {
	data: [4]u8,
	length: i32,
}

/**
 * Result structure for UTF-8 rune decoding.
 * Contains the decoded rune and the number of bytes consumed.
 */
DecodedRune :: struct  {
	rune: u32,
	length: i32,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns the number of bytes required to encode the given rune in UTF-8.
     */
	GetRuneLength :: proc "c" (
		r: u32,
	) -> i32 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Encodes a rune into UTF-8 byte sequence and returns the structure containing encoded bytes/length.
     * Invalid runes or surrogates are replaced with the error rune (U+FFFD).
     */
	EncodeRune :: proc "c" (
		c: u32,
	) -> EncodedRune ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Decodes a UTF-8 byte sequence into a rune and returns the structure containing the rune/length.
     * Returns error rune (U+FFFD) for invalid sequences.
     */
	DecodeRune :: proc "c" (
		s: []u8,
	) -> DecodedRune ---
}

// Windows-specific ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@(link_prefix="PNSLR_")
foreign {
    /**
     * Converts a UTF-8 string to a UTF-16 string.
     * The returned string is allocated using the specified allocator.
     * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
     */
	UTF16FromUTF8WindowsOnly :: proc "c" (
		str: string,
		allocator: Allocator,
	) -> []u16 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Converts a UTF-16 string to a UTF-8 string.
     * The returned string is allocated using the specified allocator.
     * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
     */
	UTF8FromUTF16WindowsOnly :: proc "c" (
		utf16str: []u16,
		allocator: Allocator,
	) -> string ---
}

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
Path :: struct  {
	path: string,
}

/**
 * Represents the type of path normalisation to perform.
 */
PathNormalisationType :: enum u8 {
	File = 0,
	Directory = 1,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Normalises a path to a consistent format, as specified in `PNSLR_PathNormalisationType`.
     */
	NormalisePath :: proc "c" (
		path: string,
		type: PathNormalisationType,
		allocator: Allocator,
	) -> Path ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Split a path into its components. Assumes that the path is a valid normalised path in Panshilar conventions.
     */
	SplitPath :: proc "c" (
		path: Path,
		parent: ^Path,
		selfNameWithExtension: ^string,
		selfName: ^string,
		extension: ^string,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns a normalised path for a file inside a given directory.
     */
	GetPathForChildFile :: proc "c" (
		dir: Path,
		fileNameWithExtension: string,
		allocator: Allocator,
	) -> Path ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Returns a normalised path for a subdirectory inside a given directory.
     */
	GetPathForSubdirectory :: proc "c" (
		dir: Path,
		dirName: string,
		allocator: Allocator,
	) -> Path ---
}

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
DirectoryIterationVisitorDelegate :: #type proc "c" (
	payload: rawptr,
	path: Path,
	isDirectory: b8,
	exploreCurrentDirectory: ^b8,
) -> b8

@(link_prefix="PNSLR_")
foreign {
    /**
     * Iterates over a directory and calls the visitor function for each file/directory found.
     * If `recursive` is true, it will also iterate over subdirectories.
     */
	IterateDirectory :: proc "c" (
		path: Path,
		recursive: b8,
		visitorPayload: rawptr,
		visitorFunc: DirectoryIterationVisitorDelegate,
	) ---
}

/**
 * Represents the type of path check to perform when checking if a path exists.
 */
PathExistsCheckType :: enum u8 {
	Either = 0,
	File = 1,
	Directory = 2,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Checks if a file/directory exists at the specified path.
     */
	PathExists :: proc "c" (
		path: Path,
		type: PathExistsCheckType,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Deletes a file/directory at a path, if it exists.
     */
	DeletePath :: proc "c" (
		path: Path,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
     */
	GetFileTimestamp :: proc "c" (
		path: Path,
	) -> i64 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Gets the size of a file at the specified path in bytes.
     */
	GetFileSize :: proc "c" (
		path: Path,
	) -> i64 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Creates a directory tree, if it doesn't exist.
     * Note that if the path doesn't have a trailing slash, it'll assume it's a file.
     * So, the last component of the path (if is a directory) will not be created.
     */
	CreateDirectoryTree :: proc "c" (
		path: Path,
	) -> b8 ---
}

/**
 * Represents an opened file.
 */
File :: struct  {
	handle: rawptr,
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Opens a file for reading, optionally allow writing to the stream.
     * If the file does not exist, this function will fail.
     */
	OpenFileToRead :: proc "c" (
		path: Path,
		allowWrite: b8,
	) -> File ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Opens a file for writing (or appending), optionally allow reading from the stream.
     * If the file does not exist, it will be created.
     */
	OpenFileToWrite :: proc "c" (
		path: Path,
		append: b8,
		allowRead: b8,
	) -> File ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Gets the size of an opened file.
     */
	GetSizeOfFile :: proc "c" (
		handle: File,
	) -> i64 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Seeks to a specific position in an opened file.
     * If not relative, it's absolute from the start.
     */
	SeekPositionInFile :: proc "c" (
		handle: File,
		newPos: i64,
		relative: b8,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Reads data from an opened file at the current position.
     */
	ReadFromFile :: proc "c" (
		handle: File,
		dst: []u8,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Writes data to an opened file at the current position.
     */
	WriteToFile :: proc "c" (
		handle: File,
		src: []u8,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Truncates an opened file to a specific size.
     */
	TruncateFile :: proc "c" (
		handle: File,
		newSize: i64,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Flushes any buffered data to the file.
     */
	FlushFile :: proc "c" (
		handle: File,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Closes an opened file.
     */
	CloseFileHandle :: proc "c" (
		handle: File,
	) ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Reads a file fully end-to-end and stores in a buffer. Won't work if dst is nil.
     * Provided allocator is used for creating the buffer.
     */
	ReadAllContentsFromFile :: proc "c" (
		path: Path,
		dst: ^[]u8,
		allocator: Allocator,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Dump a bunch of data into a file. Optionally append it instead of overwriting.
     */
	WriteAllContentsToFile :: proc "c" (
		path: Path,
		src: []u8,
		append: b8,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Copies a file from src to dst. If dst exists, it will be overwritten.
     */
	CopyFile :: proc "c" (
		src: Path,
		dst: Path,
	) -> b8 ---
}

@(link_prefix="PNSLR_")
foreign {
    /**
     * Moves a file from src to dst. If dst exists, it will be overwritten.
     */
	MoveFile :: proc "c" (
		src: Path,
		dst: Path,
	) -> b8 ---
}

// #######################################################################################
// Console
// #######################################################################################

@(link_prefix="PNSLR_")
foreign {
    /**
     * Print a message to the standard output stream.
     */
	PrintToStdOut :: proc "c" (
		message: string,
	) -> i32 ---
}

// #######################################################################################
// Process
// #######################################################################################

@(link_prefix="PNSLR_")
foreign {
    /**
     * Exits the current process immediately with the specified exit code.
     */
	ExitProcess :: proc "c" (
		exitCode: i32,
	) ---
}

// #######################################################################################
// Network
// #######################################################################################

/**
 * Represents an IP address in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
IPAddress :: []u8

/**
 * Represents a subnet mask in binary form.
 * For IPv4, it's 4 bytes. For IPv6, it's 16 bytes.
 */
IPMask :: []u8

/**
 * Represents an IP network, consisting of an IP address and a subnet mask.
 */
IPNetwork :: struct  {
	address: IPAddress,
	mask: IPMask,
}

// declare []IPNetwork

@(link_prefix="PNSLR_")
foreign {
	GetInterfaceIPAddresses :: proc "c" (
		networks: ^[]IPNetwork,
		allocator: Allocator,
	) -> b8 ---
}

#assert(size_of(int)  == 8, " int must be 8 bytes")
#assert(size_of(uint) == 8, "uint must be 8 bytes")

GET_LOC :: proc(loc := #caller_location) -> SourceCodeLocation {return {file = loc.file_path, line = loc.line, column = loc.column, function = loc.procedure}}

New :: proc($T: typeid, allocator: Allocator, loc: SourceCodeLocation, err: ^AllocatorError) -> ^T {return (^T)(Allocate(allocator, true, size_of(T), align_of(T), loc, err))}

Delete :: proc(obj: ^$T, allocator: Allocator, loc: SourceCodeLocation, err: ^AllocatorError) {if obj != nil {Free(allocator, obj, loc, err)}}

MakeSlice :: proc($T: typeid/[]$E, count: i64, zeroed: b8, allocator: Allocator, loc: SourceCodeLocation, err: ^AllocatorError) -> T {return transmute(T)MakeRawSlice(size_of(E), align_of(E), count, zeroed, allocator, loc, err)}

FreeSlice :: proc(slice: ^[]$T, allocator: Allocator, loc: SourceCodeLocation, err: ^AllocatorError) {if slice != nil {FreeRawSlice(transmute(^RawArraySlice) slice, allocator, loc, err)}}

ResizeSlice :: proc(slice: ^[]$T, newCount: i64, zeroed: b8, allocator: Allocator, loc: SourceCodeLocation, err: ^AllocatorError) {if slice != nil {ResizeRawSlice(transmute(^RawArraySlice) slice, size_of(T), align_of(T), newCount, zeroed, allocator, loc, err)}}
