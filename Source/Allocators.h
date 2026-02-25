#ifndef PNSLR_ALLOCATORS_H // ======================================================
#define PNSLR_ALLOCATORS_H
#include "__Prelude.h"
#include "Runtime.h"
EXTERN_C_BEGIN

// Allocator Declaration ===========================================================

/**
 * Defines the mode to be used when calling the allocator function.
 */
ENUM_START(PNSLR_AllocatorMode, u8)
    #define PNSLR_AllocatorMode_Allocate           ((PNSLR_AllocatorMode)   0)
    #define PNSLR_AllocatorMode_Resize             ((PNSLR_AllocatorMode)   1)
    #define PNSLR_AllocatorMode_Free               ((PNSLR_AllocatorMode)   2)
    #define PNSLR_AllocatorMode_FreeAll            ((PNSLR_AllocatorMode)   3)
    #define PNSLR_AllocatorMode_AllocateNoZero     ((PNSLR_AllocatorMode)   4)
    #define PNSLR_AllocatorMode_ResizeNoZero       ((PNSLR_AllocatorMode)   5)
    #define PNSLR_AllocatorMode_QueryCapabilities  ((PNSLR_AllocatorMode) 255)
ENUM_END

/**
 * Defines the capabilities of an allocator.
 */
ENUM_FLAGS_START(PNSLR_AllocatorCapability, u64)
    #define PNSLR_AllocatorCapability_None       ((PNSLR_AllocatorCapability) (         0))
    #define PNSLR_AllocatorCapability_ThreadSafe ((PNSLR_AllocatorCapability) (1ULL <<  0))
    #define PNSLR_AllocatorCapability_Resize     ((PNSLR_AllocatorCapability) (1ULL <<  1))
    #define PNSLR_AllocatorCapability_Free       ((PNSLR_AllocatorCapability) (1ULL <<  2))
    #define PNSLR_AllocatorCapability_FreeAll    ((PNSLR_AllocatorCapability) (1ULL <<  3))
    #define PNSLR_AllocatorCapability_HintNil    ((PNSLR_AllocatorCapability) (1ULL << 26))
    #define PNSLR_AllocatorCapability_HintBump   ((PNSLR_AllocatorCapability) (1ULL << 27))
    #define PNSLR_AllocatorCapability_HintHeap   ((PNSLR_AllocatorCapability) (1ULL << 28))
    #define PNSLR_AllocatorCapability_HintTemp   ((PNSLR_AllocatorCapability) (1ULL << 29))
    #define PNSLR_AllocatorCapability_HintDebug  ((PNSLR_AllocatorCapability) (1ULL << 30))
ENUM_END

/**
 * Defines the error codes that can be returned by the allocator.
 */
ENUM_START(PNSLR_AllocatorError, u8)
    #define PNSLR_AllocatorError_None             ((PNSLR_AllocatorError) 0)
    #define PNSLR_AllocatorError_OutOfMemory      ((PNSLR_AllocatorError) 1)
    #define PNSLR_AllocatorError_InvalidAlignment ((PNSLR_AllocatorError) 2)
    #define PNSLR_AllocatorError_InvalidSize      ((PNSLR_AllocatorError) 3)
    #define PNSLR_AllocatorError_InvalidMode      ((PNSLR_AllocatorError) 4)
    #define PNSLR_AllocatorError_Internal         ((PNSLR_AllocatorError) 5)
    #define PNSLR_AllocatorError_OutOfOrderFree   ((PNSLR_AllocatorError) 6)
    #define PNSLR_AllocatorError_DoubleFree       ((PNSLR_AllocatorError) 7)
    #define PNSLR_AllocatorError_CantFreeAll      ((PNSLR_AllocatorError) 8)
ENUM_END

/**
 * Defines the delegate type for the allocator function.
 */
typedef rawptr (*PNSLR_AllocatorProcedure)(
    rawptr                   allocatorData,
    PNSLR_AllocatorMode      mode,
    i32                      size,
    i32                      alignment,
    rawptr                   oldMemory,
    i32                      oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

/**
 * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
 */
typedef struct PNSLR_Allocator
{
    PNSLR_AllocatorProcedure procedure;
    rawptr                   data; // Optional data for the allocator function
} PNSLR_Allocator;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_Allocator);

// Allocation ease-of-use functions ================================================

/**
 * Allocate memory using the provided allocator.
 */
rawptr PNSLR_Allocate(
    PNSLR_Allocator allocator,
    b8  zeroed,
    i32 size,
    i32 alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

/**
 * Resize memory using the provided allocator.
 */
rawptr PNSLR_Resize(
    PNSLR_Allocator allocator,
    b8     zeroed,
    rawptr oldMemory,
    i32    oldSize,
    i32    newSize,
    i32    alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

/**
 * Fallback resize function that can be used when the allocator does not support resizing.
 */
rawptr PNSLR_DefaultResize(
    PNSLR_Allocator allocator,
    b8     zeroed,
    rawptr oldMemory,
    i32    oldSize,
    i32    newSize,
    i32    alignment,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(
    PNSLR_Allocator allocator,
    rawptr memory,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

/**
 * Free all memory allocated by the provided allocator.
 */
void PNSLR_FreeAll(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

/**
 * Query the capabilities of the provided allocator.
 */
u64 PNSLR_QueryAllocatorCapabilities(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error OPT_ARG
);

// Nil allocator ===================================================================

/**
 * Get the 'nil' allocator. Reports 'out of memory' when requesting memory.
 * Otherwise no-ops all around.
 */
PNSLR_Allocator PNSLR_GetAllocator_Nil(void);

// Default Heap Allocator ==========================================================

/**
 * Get the default heap allocator.
 */
PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap(void);

/**
 * Main allocator function for the default heap allocator.
 */
rawptr PNSLR_AllocatorFn_DefaultHeap(
    rawptr                   allocatorData,
    PNSLR_AllocatorMode      mode,
    i32                      size,
    i32                      alignment,
    rawptr                   oldMemory,
    i32                      oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

// Arena Alloator ==================================================================

/**
 * A block of memory used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorBlock
{
    struct PNSLR_ArenaAllocatorBlock* previous;
    PNSLR_Allocator                   allocator;
    rawptr                            memory;
    u32                               capacity;
    u32                               used;
} PNSLR_ArenaAllocatorBlock;

/**
 * The payload used by the arena allocator.
 */
typedef struct PNSLR_ArenaAllocatorPayload
{
    PNSLR_Allocator            backingAllocator;
    PNSLR_ArenaAllocatorBlock* currentBlock;
    u32                        totalUsed;
    u32                        totalCapacity;
    u32                        minimumBlockSize;
    u32                        numSnapshots;
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
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Destroy an arena allocator and free all its resources.
 * This does not free the backing allocator, only the arena allocator's own resources.
 */
void PNSLR_DestroyAllocator_Arena(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Main allocator function for the arena allocator.
 */
rawptr PNSLR_AllocatorFn_Arena(
    rawptr                   allocatorData,
    PNSLR_AllocatorMode      mode,
    i32                      size,
    i32                      alignment,
    rawptr                   oldMemory,
    i32                      oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

ENUM_START(PNSLR_ArenaSnapshotError, u8)
    #define PNSLR_ArenaSnapshotError_None                        ((PNSLR_ArenaSnapshotError) 0)
    #define PNSLR_ArenaSnapshotError_InvalidData                 ((PNSLR_ArenaSnapshotError) 1)
    #define PNSLR_ArenaSnapshotError_MemoryBlockNotOwned         ((PNSLR_ArenaSnapshotError) 2)
    #define PNSLR_ArenaSnapshotError_OutOfOrderRestoreUsage      ((PNSLR_ArenaSnapshotError) 3)
    #define PNSLR_ArenaSnapshotError_DoubleRestoreOrDiscardUsage ((PNSLR_ArenaSnapshotError) 4)
ENUM_END

/**
 * A snapshot of the arena allocator, recording its state at a specific point in time.
 * Can be saved/loaded/discarded as needed.
 */
typedef struct PNSLR_ArenaAllocatorSnapshot
{
    b8                           valid;
    PNSLR_ArenaAllocatorPayload* payload;
    PNSLR_ArenaAllocatorBlock*   block;
    u32                          used;
} PNSLR_ArenaAllocatorSnapshot;

/**
 * Ensures that the arena allocator has either restored/discarded all the
 * snapshots that were taken.
 */
b8 PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Allocator allocator);

/**
 * Captures a snapshot of the arena allocator.
 * The returned value can be used to load back the existing state at this point.
 */
PNSLR_ArenaAllocatorSnapshot PNSLR_CaptureArenaAllocatorSnapshot(PNSLR_Allocator allocator);

/**
 * Restores the state of the arena allocator from a snapshot.
 * Upon success, the snapshot is marked as invalid.
 */
PNSLR_ArenaSnapshotError PNSLR_RestoreArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot, PNSLR_SourceCodeLocation loc);

/**
 * Discards a snapshot of the arena allocator.
 */
PNSLR_ArenaSnapshotError PNSLR_DiscardArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot);

// Stack Allocator =================================================================

/**
 * A page of a stack allocator.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_StackAllocatorPage
{
    struct PNSLR_StackAllocatorPage* previousPage;
    u64                              usedBytes;
    u8                               buffer[8192]; // 8KB page size
} PNSLR_StackAllocatorPage;

/**
 * The header used for every separate stack allocation.
 * This is used to store metadata about the allocation and deallocate appropriately.
 */
typedef struct PNSLR_StackAllocationHeader
{
    PNSLR_StackAllocatorPage* page;
    i32                       size;
    i32                       alignment;
    rawptr                    lastAllocation;
    rawptr                    lastAllocationHeader;
} PNSLR_StackAllocationHeader;

/**
 * The payload used by the stack allocator.
 */
typedef struct PNSLR_StackAllocatorPayload
{
    PNSLR_Allocator              backingAllocator;
    PNSLR_StackAllocatorPage*    currentPage;
    rawptr                       lastAllocation; // to debug double frees or skipped frees
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
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Destroy a stack allocator and free all its resources.
 * This does not free the backing allocator, only the stack allocator's own resources.
 */
void PNSLR_DestroyAllocator_Stack(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Main allocator function for the stack allocator.
 */
rawptr PNSLR_AllocatorFn_Stack(
    rawptr                   allocatorData,
    PNSLR_AllocatorMode      mode,
    i32                      size,
    i32                      alignment,
    rawptr                   oldMemory,
    i32                      oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

// Collections make/free functions =================================================

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
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Free a raw array slice allocated with `PNSLR_MakeRawSlice`, using the provided allocator.
 */
void PNSLR_FreeRawSlice(
    PNSLR_RawArraySlice* slice,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
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
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Allocate a UTF-8 string of 'count__' characters using the provided allocator. Optionally zeroed.
 */
utf8str PNSLR_MakeString(
    i64 count,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Free a UTF-8 string allocated with `PNSLR_MakeString`, using the provided allocator.
 */
void PNSLR_FreeString(
    utf8str str,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Allocate a C-style null-terminated string of 'count__' characters (excluding the null terminator) using the provided allocator. Optionally zeroed.
 */
cstring PNSLR_MakeCString(
    i64 count,
    b8 zeroed,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

/**
 * Free a C-style null-terminated string allocated with `PNSLR_MakeCString`, using the provided allocator.
 */
void PNSLR_FreeCString(
    cstring str,
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError* error OPT_ARG
);

//+skipreflect

// C++ Convenience Templates =======================================================

EXTERN_C_END

#ifdef __cplusplus

    namespace Panshilar
    {
        template <typename T> T* NewT(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* err = nil)
        {
            return (T*) PNSLR_Allocate(allocator, true, (i32) sizeof(T), (i32) alignof(T), loc, err);
        }

        template <typename T> void DeleteT(T* obj, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* err = nil)
        {
            if (obj) { PNSLR_Free(allocator, obj, loc, err); }
        }

        template <typename T> ArraySlice<T> MakeSliceT(i64 count, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* err = nil)
        {
            static_assert( sizeof(ArraySlice<T>) ==  sizeof(PNSLR_RawArraySlice), "ArraySlice<T> must be the same size as PNSLR_RawArraySlice");
            static_assert(alignof(ArraySlice<T>) == alignof(PNSLR_RawArraySlice), "ArraySlice<T> must have the same alignment as PNSLR_RawArraySlice");

            PNSLR_RawArraySlice raw = PNSLR_MakeRawSlice((i32) sizeof(T), (i32) alignof(T), count, zeroed, allocator, loc, err);
            return *reinterpret_cast<ArraySlice<T>*>(&raw);
        }

        template <typename T> void FreeSliceT(ArraySlice<T>* slice, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* err = nil)
        {
            static_assert( sizeof(ArraySlice<T>) ==  sizeof(PNSLR_RawArraySlice), "ArraySlice<T> must be the same size as PNSLR_RawArraySlice");
            static_assert(alignof(ArraySlice<T>) == alignof(PNSLR_RawArraySlice), "ArraySlice<T> must have the same alignment as PNSLR_RawArraySlice");

            if (slice) PNSLR_FreeRawSlice(reinterpret_cast<PNSLR_RawArraySlice*>(slice), allocator, loc, err);
        }

        template <typename T> void ResizeSliceT(ArraySlice<T>* slice, i64 newCount, b8 zeroed, PNSLR_Allocator allocator, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* err = nil)
        {
            static_assert( sizeof(ArraySlice<T>) ==  sizeof(PNSLR_RawArraySlice), "ArraySlice<T> must be the same size as PNSLR_RawArraySlice");
            static_assert(alignof(ArraySlice<T>) == alignof(PNSLR_RawArraySlice), "ArraySlice<T> must have the same alignment as PNSLR_RawArraySlice");

            if (slice) PNSLR_ResizeRawSlice(reinterpret_cast<PNSLR_RawArraySlice*>(slice), (i32) sizeof(T), (i32) alignof(T), newCount, zeroed, allocator, loc, err);
        }
    }

#endif

EXTERN_C_BEGIN

// Allocation Macros ===============================================================

#ifdef __cplusplus

    /**
     * Allocate an object of type 'ty' using the provided allocator.
     */
    #define PNSLR_New(ty, allocator, loc, error__) \
        Panshilar::NewT<ty>(allocator, loc, error__)

    /**
     * Delete an object allocated with `PNSLR_New`, using the provided allocator.
     */
    #define PNSLR_Delete(obj, allocator, loc, error__) \
        Panshilar::DeleteT(obj, allocator, loc, error__)

    /**
     * Allocate an array of 'count' elements of type 'ty' using the provided allocator. Optionally zeroed.
     */
    #define PNSLR_MakeSlice(ty, count, zeroed, allocator, loc, error__) \
        Panshilar::MakeSliceT<ty>(count, zeroed, allocator, loc, error__)

    /**
     * Free a 'slice' (passed by ptr) allocated with `PNSLR_MakeSlice`, using the provided allocator.
     */
    #define PNSLR_FreeSlice(slice, allocator, loc, error__) \
        Panshilar::FreeSliceT(slice, allocator, loc, error__)

    /**
     * Resize a 'slice' (passed by ptr) to one with 'newCount' elements of type 'ty' using the provided allocator. Optionally zeroed.
     */
    #define PNSLR_ResizeSlice(ty, slice, newCount, zeroed, allocator, loc, error__) \
        Panshilar::ResizeSliceT<ty>(slice, newCount, zeroed, allocator, loc, error__)

#else

    /**
     * Allocate an object of type 'ty' using the provided allocator.
     */
    #define PNSLR_New(ty, allocator, loc, error__) \
        ((ty*) PNSLR_Allocate(allocator, true, sizeof(ty), alignof(ty), loc, error__))

    /**
     * Delete an object allocated with `PNSLR_New`, using the provided allocator.
     */
    #define PNSLR_Delete(obj, allocator, loc, error__) \
        do { if (obj) { PNSLR_Free(allocator, obj, loc, error__); } } while (false)

    /**
     * Allocate an array of 'count' elements of type 'ty' using the provided allocator. Optionally zeroed.
     */
    #define PNSLR_MakeSlice(ty, count, zeroed, allocator, loc, error__) \
        (PNSLR_ArraySlice(ty)) {.raw = PNSLR_MakeRawSlice((i32) sizeof(ty), (i32) alignof(ty), (i64) count, zeroed, allocator, loc, error__)}

    /**
     * Free a 'slice' (passed by ptr) allocated with `PNSLR_MakeSlice`, using the provided allocator.
     */
    #define PNSLR_FreeSlice(slice, allocator, loc, error__) \
        do { if (slice) PNSLR_FreeRawSlice(&((slice)->raw), allocator, loc, error__); } while(false)

    /**
     * Resize a 'slice' (passed by ptr) to one with 'newCount' elements of type 'ty' using the provided allocator. Optionally zeroed.
     */
    #define PNSLR_ResizeSlice(ty, slice, newCount, zeroed, allocator, loc, error__) \
        do { if (slice) PNSLR_ResizeRawSlice(&((slice)->raw), (i32) sizeof(ty), (i32) alignof(ty), (i64) newCount, zeroed, allocator, loc, error__); } while(false)

#endif

#ifdef PNSLR_IMPLEMENTATION

    #define PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(name, pageSizeInKilobytes) \
        typedef struct alignas(16) PNSLR_Internal_##name##InternalAllocatorBuffer \
        { \
            u8 data[pageSizeInKilobytes * 1024]; \
        } PNSLR_Internal_##name##InternalAllocatorBuffer; \
        \
        typedef struct PNSLR_Internal_##name##InternalAllocator \
        { \
            b8                                    initialised; \
            PNSLR_ArenaAllocatorPayload           arenaPayload; \
            PNSLR_ArenaAllocatorBlock             arenaBlock; \
            PNSLR_Internal_##name##InternalAllocatorBuffer buffer; \
        } PNSLR_Internal_##name##InternalAllocatorInfo; \
        \
        static thread_local PNSLR_Internal_##name##InternalAllocatorInfo G_PNSLR_Internal_##name##InternalAllocatorInfo = {0}; \
        \
        static PNSLR_Allocator PNSLR_Internal_Acquire##name##InternalAllocator(void) \
        { \
            if (!G_PNSLR_Internal_##name##InternalAllocatorInfo.initialised) \
            { \
                G_PNSLR_Internal_##name##InternalAllocatorInfo.initialised = true; \
                \
                G_PNSLR_Internal_##name##InternalAllocatorInfo.buffer = (PNSLR_Internal_##name##InternalAllocatorBuffer) {0}; \
                \
                G_PNSLR_Internal_##name##InternalAllocatorInfo.arenaBlock = (PNSLR_ArenaAllocatorBlock) \
                { \
                    .previous  = nil, \
                    .allocator = PNSLR_GetAllocator_Nil(), \
                    .memory    = (rawptr) &G_PNSLR_Internal_##name##InternalAllocatorInfo.buffer.data, \
                    .capacity  = sizeof(G_PNSLR_Internal_##name##InternalAllocatorInfo.buffer.data), \
                    .used      = 0, \
                }; \
                \
                G_PNSLR_Internal_##name##InternalAllocatorInfo.arenaPayload = (PNSLR_ArenaAllocatorPayload) \
                { \
                    .backingAllocator    = PNSLR_GetAllocator_Nil(), \
                    .currentBlock        = &G_PNSLR_Internal_##name##InternalAllocatorInfo.arenaBlock, \
                    .totalUsed           = 0, \
                    .totalCapacity       = sizeof(G_PNSLR_Internal_##name##InternalAllocatorInfo.buffer.data), \
                    .minimumBlockSize    = 0, \
                    .numSnapshots        = 0, \
                }; \
            } \
            \
            return (PNSLR_Allocator) {.procedure = PNSLR_AllocatorFn_Arena, .data = &G_PNSLR_Internal_##name##InternalAllocatorInfo.arenaPayload}; \
        }

    #define PNSLR_INTERNAL_ALLOCATOR_INIT(name, varName) \
        PNSLR_Allocator              varName           = PNSLR_Internal_Acquire##name##InternalAllocator(); \
        PNSLR_ArenaAllocatorSnapshot varName##Snapshot = PNSLR_CaptureArenaAllocatorSnapshot(varName);

    #define PNSLR_INTERNAL_ALLOCATOR_RESET(name, varName) \
        PNSLR_ArenaSnapshotError varName##SnapshotErr = PNSLR_RestoreArenaAllocatorSnapshot(&varName##Snapshot, PNSLR_GET_LOC()); \
        if (PNSLR_ArenaSnapshotError_None != varName##SnapshotErr) { FORCE_DBG_TRAP; } else { }

#endif

//-skipreflect

EXTERN_C_END
#endif // PNSLR_ALLOCATORS_H =======================================================
