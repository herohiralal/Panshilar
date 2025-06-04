#ifndef PNSLR_ALLOCATORS // ========================================================
#define PNSLR_ALLOCATORS

#include "__Prelude.h"

/**
 * Allocate memory using the provided allocator.
 */
void* PNSLR_Allocate(Allocator allocator, b8 zeroed, i32 size, i32 alignment, SourceCodeLocation location);

/**
 * Resize memory using the provided allocator.
 */
void* PNSLR_Resize(Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, SourceCodeLocation location);

/**
 * Fallback resize function that can be used when the allocator does not support resizing.
 */
void* PNSLR_DefaultResize(Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, SourceCodeLocation location);

/**
 * Free memory using the provided allocator.
 */
void PNSLR_Free(Allocator allocator, void* memory, SourceCodeLocation location);

/**
 * Free all memory allocated by the provided allocator.
 */
void PNSLR_FreeAll(Allocator allocator, SourceCodeLocation location);

/**
 * Query the capabilities of the provided allocator.
 */
u64 PNSLR_QueryAllocatorCapabilities(Allocator allocator, SourceCodeLocation location);

// Get the default heap allocator.
Allocator PNSLR_GetDefaultHeapAllocator(void);

/**
 * Main allocator function for the default heap allocator.
 */
void* PNSLR_DefaultHeapAllocatorFn(void* allocatorData, u8 mode, i32 size, i32 alignment, void* oldMemory, i32 oldSize, SourceCodeLocation location);

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

/**
 * Short-hand for the default temporary allocator.
 */
#define PNSLR_DEFAULT_TEMP_ALLOCATOR \
    PNSLR_GetDefaultTempAllocator()

//-skipreflect

#endif // PNSLR_ALLOCATORS =========================================================
