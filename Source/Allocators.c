#include "Allocators.h"

void* PNSLR_Allocate(Allocator allocator, b8 zeroed, i32 size, i32 alignment, SourceCodeLocation location)
{
    if (!allocator.function) { return nil; }

    return allocator.function(
        allocator.data,
        zeroed ? ALLOCATOR_ALLOCATE : ALLOCATOR_ALLOCATE_NO_ZERO,
        size,
        alignment,
        nil,
        0,
        location
    );
}

void* PNSLR_Resize(Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, SourceCodeLocation location)
{
    if (!allocator.function) { return nil; }

    return allocator.function(
        allocator.data,
        zeroed ? ALLOCATOR_RESIZE : ALLOCATOR_RESIZE_NO_ZERO,
        newSize,
        alignment,
        oldMemory,
        oldSize,
        location
    );
}

void* PNSLR_DefaultResize(Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, SourceCodeLocation location)
{
    if (!allocator.function) { return nil; }

    void* newMemory = PNSLR_Allocate(allocator, zeroed, newSize, alignment, location);
    PNSLR_Intrinsic_MemCopy(newMemory, oldMemory, oldSize < newSize ? oldSize : newSize);
    PNSLR_Free(allocator, oldMemory, location);
    return newMemory;
}

void PNSLR_Free(Allocator allocator, void* memory, SourceCodeLocation location)
{
    if (!allocator.function || !memory) { return; }

    allocator.function(
        allocator.data,
        ALLOCATOR_FREE,
        0,
        0,
        memory,
        0,
        location
    );
}

void PNSLR_FreeAll(Allocator allocator, SourceCodeLocation location)
{
    if (!allocator.function) { return; }

    allocator.function(
        allocator.data,
        ALLOCATOR_FREE_ALL,
        0,
        0,
        nil,
        0,
        location
    );
}

u64 PNSLR_QueryAllocatorCapabilities(Allocator allocator, SourceCodeLocation location)
{
    if (!allocator.function) { return ALLOCATOR_CAPABILITY_NONE; }

    void* result = allocator.function(
        allocator.data,
        ALLOCATOR_QUERY_CAPABILITIES,
        0,
        0,
        nil,
        0,
        location
    );

    // reinterpret void* to u64
    return (u64)(result);
}

Allocator PNSLR_GetDefaultHeapAllocator(void)
{
    return (Allocator) {.function = PNSLR_DefaultHeapAllocatorFn, .data = nil};
}

// Allocates to the system heap.
void* PNSLR_DefaultHeapAllocatorFn(void* allocatorData, u8 mode, i32 size, i32 alignment, void* oldMemory, i32 oldSize, SourceCodeLocation location)
{
    switch (mode)
    {
        case ALLOCATOR_ALLOCATE:
        case ALLOCATOR_ALLOCATE_NO_ZERO:
        {
            void* memory = PNSLR_Intrinsic_Malloc(alignment, size);
            if (memory && mode == ALLOCATOR_ALLOCATE) { PNSLR_Intrinsic_MemSet(memory, 0, size); }
            return memory;
        }
        case ALLOCATOR_RESIZE:
        case ALLOCATOR_RESIZE_NO_ZERO:
        {
            PNSLR_DefaultResize(
                (Allocator) {.function = PNSLR_DefaultHeapAllocatorFn, .data = allocatorData},
                mode != ALLOCATOR_RESIZE_NO_ZERO,
                oldMemory,
                oldSize,
                size,
                alignment,
                location
            );
        }
        case ALLOCATOR_FREE:
        {
            PNSLR_Intrinsic_Free(oldMemory);
            return nil;
        }
        case ALLOCATOR_FREE_ALL:
        {
            return nil;
        }
        case ALLOCATOR_QUERY_CAPABILITIES:
        {
            return (void*)(
                ALLOCATOR_CAPABILITY_THREAD_SAFE |
                ALLOCATOR_CAPABILITY_FREE |
                ALLOCATOR_CAPABILITY_RESIZE_FR |
                ALLOCATOR_CAPABILITY_HINT_HEAP
            );
        }
        default:
            return nil; // Unsupported mode.
    }
}

Allocator PNSLR_GetDefaultTempAllocator(void)
{
    return (Allocator) {.function = PNSLR_DefaultTempAllocatorFn, .data = nil};
}

void* PNSLR_DefaultTempAllocatorFn(void* allocatorData, u8 mode, i32 size, i32 alignment, void* oldMemory, i32 oldSize, SourceCodeLocation location)
{
    return nil;
}
