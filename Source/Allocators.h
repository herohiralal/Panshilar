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
ENUM_START(PNSLR_AllocatorCapability, u64)
    #define PNSLR_AllocatorCapability_None       ((PNSLR_AllocatorCapability) (1 <<  0))
    #define PNSLR_AllocatorCapability_ThreadSafe ((PNSLR_AllocatorCapability) (1 <<  1))
    #define PNSLR_AllocatorCapability_Resize     ((PNSLR_AllocatorCapability) (1 <<  2))
    #define PNSLR_AllocatorCapability_Free       ((PNSLR_AllocatorCapability) (1 <<  3))
    #define PNSLR_AllocatorCapability_FreeAll    ((PNSLR_AllocatorCapability) (1 <<  4))
    #define PNSLR_AllocatorCapability_HintNil    ((PNSLR_AllocatorCapability) (1 << 26))
    #define PNSLR_AllocatorCapability_HintBump   ((PNSLR_AllocatorCapability) (1 << 27))
    #define PNSLR_AllocatorCapability_HintHeap   ((PNSLR_AllocatorCapability) (1 << 28))
    #define PNSLR_AllocatorCapability_HintTemp   ((PNSLR_AllocatorCapability) (1 << 29))
    #define PNSLR_AllocatorCapability_HintDebug  ((PNSLR_AllocatorCapability) (1 << 30))
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
    u8                       mode,
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

DECLARE_ARRAY_SLICE(PNSLR_Allocator);

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
    PNSLR_AllocatorError*    error
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
    PNSLR_AllocatorError*    error
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
    PNSLR_AllocatorError*    error
);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(
    PNSLR_Allocator allocator,
    rawptr memory,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

/**
 * Free all memory allocated by the provided allocator.
 */
void PNSLR_FreeAll(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

/**
 * Query the capabilities of the provided allocator.
 */
u64 PNSLR_QueryAllocatorCapabilities(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

// Default Heap Allocator ==========================================================

/**
 * Get the default heap allocator.
 */
PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap(void);

/**
 * Main allocator function for the default heap allocator.
 */
rawptr PNSLR_AllocatorFn_DefaultHeap(
    rawptr allocatorData,
    u8     mode,
    i32    size,
    i32    alignment,
    rawptr oldMemory,
    i32    oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

// Stack Allocator =================================================================

/**
 * A page of a stack allocator.
 */
typedef struct alignas(PNSLR_PTR_SIZE) PNSLR_StackAllocatorPage {
    struct PNSLR_StackAllocatorPage* previousPage;
    i32                              usedBytes;
    u8                               buffer[4096];
} PNSLR_StackAllocatorPage;

/**
 * The header used for every separate stack allocation.
 * This is used to store metadata about the allocation and deallocate appropriately.
 */
typedef struct PNSLR_StackAllocationHeader {
    PNSLR_StackAllocatorPage* page;
    i32                       size;
    i32                       alignment;
    rawptr                    lastAllocation;
    rawptr                    lastAllocationHeader;
} PNSLR_StackAllocationHeader;

/**
 * The payload used by the stack allocator.
 */
typedef struct PNSLR_StackAllocatorPayload {
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
PNSLR_Allocator PNSLR_NewAllocator_Stack(PNSLR_Allocator backingAllocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);

/**
 * Destroy a stack allocator and free all its resources.
 * This does not free the backing allocator, only the stack allocator's own resources.
 */
void PNSLR_DestroyAllocator_Stack(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error);

/**
 * Main allocator function for the stack allocator.
 */
rawptr PNSLR_AllocatorFn_Stack(
    rawptr allocatorData,
    u8     mode,
    i32    size,
    i32    alignment,
    rawptr oldMemory,
    i32    oldSize,
    PNSLR_SourceCodeLocation location,
    PNSLR_AllocatorError*    error
);

//+skipreflect

// Allocation Macros ===============================================================

/**
 * Allocate an object of type 'ty' using the provided allocator.
 */
#define PNSLR_New(ty, allocator, error__) \
    ((ty*) PNSLR_Allocate(allocator, true, sizeof(ty), alignof(ty), CURRENT_LOC(), error__))

/**
 * Allocate an array of 'count__' elements of type 'ty' using the provided allocator. Optionally zeroed.
 */
#define PNSLR_MakeSlice(ty, count__, zeroed, allocator, error__) \
    (ArraySlice(ty)) \
    { \
        .count = (i64)(count__), \
        .data = (ty*) PNSLR_Allocate(allocator, zeroed, (count__) * (i32) (sizeof(ty)), alignof(ty), CURRENT_LOC(), error__) \
    }

/**
 * Free a 'slice' allocated with `PNSLR_MakeSlice`, using the provided allocator.
 */
#define PNSLR_FreeSlice(slice, allocator, error__) \
    do \
    { \
        if ((slice).data) \
        { \
            PNSLR_Free(allocator, (slice).data, CURRENT_LOC(), error__); \
            (slice).data = nil; \
            (slice).count = 0; \
        } \
    } while (false)

/**
 * Delete an object allocated with `PNSLR_New`, using the provided allocator.
 */
#define PNSLR_Delete(obj, allocator, error__) \
    do \
    { \
        if (obj) \
        { \
            PNSLR_Free(allocator, obj, CURRENT_LOC(), error__); \
            obj = nil; \
        } \
    } while (false)

/**
 * Short-hand for a nil allocator, which does nothing.
 */
#define PNSLR_NIL_ALLOCATOR \
    (PNSLR_Allocator) { .procedure = nil, .data = nil }

/**
 * Short-hand for the default heap allocator.
 */
#define PNSLR_DEFAULT_HEAP_ALLOCATOR \
    PNSLR_GetAllocator_DefaultHeap()

//-skipreflect

EXTERN_C_END

#endif // PNSLR_ALLOCATORS_H =======================================================
