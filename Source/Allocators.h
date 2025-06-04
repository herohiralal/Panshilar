#ifndef PNSLR_ALLOCATORS // ========================================================
#define PNSLR_ALLOCATORS

#include "__Prelude.h"
#include "Runtime.h"

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
    #define PNSLR_AllocatorCapability_ResizeFR   ((PNSLR_AllocatorCapability) (1 <<  2))
    #define PNSLR_AllocatorCapability_Free       ((PNSLR_AllocatorCapability) (1 <<  3))
    #define PNSLR_AllocatorCapability_HintBump   ((PNSLR_AllocatorCapability) (1 << 17))
    #define PNSLR_AllocatorCapability_HintHeap   ((PNSLR_AllocatorCapability) (1 << 18))
    #define PNSLR_AllocatorCapability_HintTemp   ((PNSLR_AllocatorCapability) (1 << 19))
    #define PNSLR_AllocatorCapability_HintDebug  ((PNSLR_AllocatorCapability) (1 << 20))
ENUM_END

/**
 * Defines the delegate type for the allocator function.
 */
typedef void* (*PNSLR_AllocatorProcedure)(
    void*                    allocatorData,
    u8                       mode,
    i32                      size,
    i32                      alignment,
    void*                    oldMemory,
    i32                      oldSize,
    PNSLR_SourceCodeLocation location
);

/**
 * Defines a generic allocator structure that can be used to allocate, resize, and free memory.
 */
typedef struct PNSLR_Allocator
{
    PNSLR_AllocatorProcedure procedure;
    void*                    data; // Optional data for the allocator function
} PNSLR_Allocator;

/**
 * Allocate memory using the provided allocator.
 */
void* PNSLR_Allocate(
    PNSLR_Allocator allocator,
    b8 zeroed,
    i32 size,
    i32 alignment,
    PNSLR_SourceCodeLocation location
);

/**
 * Resize memory using the provided allocator.
 */
void* PNSLR_Resize(
    PNSLR_Allocator allocator,
    b8 zeroed,
    void* oldMemory,
    i32 oldSize,
    i32 newSize,
    i32 alignment,
    PNSLR_SourceCodeLocation location
);

/**
 * Fallback resize function that can be used when the allocator does not support resizing.
 */
void* PNSLR_DefaultResize(
    PNSLR_Allocator allocator,
    b8 zeroed,
    void* oldMemory,
    i32 oldSize,
    i32 newSize,
    i32 alignment,
    PNSLR_SourceCodeLocation location
);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(
    PNSLR_Allocator allocator,
    void* memory,
    PNSLR_SourceCodeLocation location
);

/**
 * Free all memory allocated by the provided allocator.
 */
void PNSLR_FreeAll(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location
);

/**
 * Query the capabilities of the provided allocator.
 */
u64 PNSLR_QueryAllocatorCapabilities(
    PNSLR_Allocator allocator,
    PNSLR_SourceCodeLocation location
);

// Get the default heap allocator.
PNSLR_Allocator PNSLR_GetDefaultHeapAllocator(void);

/**
 * Main allocator function for the default heap allocator.
 */
void* PNSLR_DefaultHeapAllocatorFn(
    void* allocatorData,
    u8 mode,
    i32 size,
    i32 alignment,
    void* oldMemory,
    i32 oldSize,
    PNSLR_SourceCodeLocation location
);

//+skipreflect

/**
 * Allocate an object of type 'ty' using the provided allocator.
 */
#define PNSLR_New(ty, allocator) (ty*) PNSLR_Allocate(allocator, true, sizeof(ty), alignof(ty), CURRENT_LOC())

/**
 * Allocate an array of 'count__' elements of type 'ty' using the provided allocator. Optionally zeroed.
 */
#define PNSLR_MakeSlice(ty, count__, zeroed, allocator) \
    (ArraySlice(ty)) \
    { \
        .count = (i64)(count__), \
        .data = (ty*) PNSLR_Allocate(allocator, zeroed, (count__) * (i32) (sizeof(ty)), alignof(ty), CURRENT_LOC()) \
    }

/**
 * Free a 'slice' allocated with `PNSLR_MakeSlice`, using the provided allocator.
 */
#define PNSLR_FreeSlice(slice, allocator) \
    do \
    { \
        if ((slice).data) \
        { \
            PNSLR_Free(allocator, (slice).data, CURRENT_LOC()); \
            (slice).data = nil; \
            (slice).count = 0; \
        } \
    } while (false)

/**
 * Delete an object allocated with `PNSLR_New`, using the provided allocator.
 */
#define PNSLR_Delete(obj, allocator) \
    do \
    { \
        if (obj) \
        { \
            PNSLR_Free(allocator, obj, CURRENT_LOC()); \
            obj = nil; \
        } \
    } while (false)

/**
 * Short-hand for the default heap allocator.
 */
#define PNSLR_DEFAULT_HEAP_ALLOCATOR \
    PNSLR_GetDefaultHeapAllocator()

//-skipreflect

#endif // PNSLR_ALLOCATORS =========================================================
