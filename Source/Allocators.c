#include "Allocators.h"

void* PNSLR_Allocate(PNSLR_Allocator allocator, b8 zeroed, i32 size, i32 alignment, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure) { return nil; }

    return allocator.procedure(
        allocator.data,
        zeroed ? PNSLR_AllocatorMode_Allocate : PNSLR_AllocatorMode_AllocateNoZero,
        size,
        alignment,
        nil,
        0,
        location
    );
}

void* PNSLR_Resize(PNSLR_Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure) { return nil; }

    return allocator.procedure(
        allocator.data,
        zeroed ? PNSLR_AllocatorMode_Resize : PNSLR_AllocatorMode_ResizeNoZero,
        newSize,
        alignment,
        oldMemory,
        oldSize,
        location
    );
}

void* PNSLR_DefaultResize(PNSLR_Allocator allocator, b8 zeroed, void* oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure) { return nil; }

    void* newMemory = PNSLR_Allocate(allocator, zeroed, newSize, alignment, location);
    PNSLR_Intrinsic_MemCopy(newMemory, oldMemory, oldSize < newSize ? oldSize : newSize);
    PNSLR_Free(allocator, oldMemory, location);
    return newMemory;
}

void PNSLR_Free(PNSLR_Allocator allocator, void* memory, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure || !memory) { return; }

    allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_Free,
        0,
        0,
        memory,
        0,
        location
    );
}

void PNSLR_FreeAll(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure) { return; }

    allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_FreeAll,
        0,
        0,
        nil,
        0,
        location
    );
}

u64 PNSLR_QueryAllocatorCapabilities(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location)
{
    if (!allocator.procedure) { return PNSLR_AllocatorCapability_None; }

    void* result = allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_QueryCapabilities,
        0,
        0,
        nil,
        0,
        location
    );

    // reinterpret void* to u64
    return (u64)(result);
}

PNSLR_Allocator PNSLR_GetDefaultHeapAllocator(void)
{
    return (PNSLR_Allocator) {.procedure = PNSLR_DefaultHeapAllocatorFn, .data = nil};
}

// Allocates to the system heap.
void* PNSLR_DefaultHeapAllocatorFn(void* allocatorData, u8 mode, i32 size, i32 alignment, void* oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location)
{
    switch (mode)
    {
        case PNSLR_AllocatorMode_Allocate:
        case PNSLR_AllocatorMode_AllocateNoZero:
        {
            void* memory = PNSLR_Intrinsic_Malloc(alignment, size);
            if (memory && mode == PNSLR_AllocatorMode_Allocate) { PNSLR_Intrinsic_MemSet(memory, 0, size); }
            return memory;
        }
        case PNSLR_AllocatorMode_Resize:
        case PNSLR_AllocatorMode_ResizeNoZero:
        {
            PNSLR_DefaultResize(
                (PNSLR_Allocator) {.procedure = PNSLR_DefaultHeapAllocatorFn, .data = allocatorData},
                mode == PNSLR_AllocatorMode_Resize,
                oldMemory,
                oldSize,
                size,
                alignment,
                location
            );
        }
        case PNSLR_AllocatorMode_Free:
        {
            PNSLR_Intrinsic_Free(oldMemory);
            return nil;
        }
        case PNSLR_AllocatorMode_FreeAll:
        {
            return nil;
        }
        case PNSLR_AllocatorMode_QueryCapabilities:
        {
            return (void*)(
                PNSLR_AllocatorCapability_ThreadSafe |
                PNSLR_AllocatorCapability_Free |
                PNSLR_AllocatorCapability_ResizeFR |
                PNSLR_AllocatorCapability_HintHeap
            );
        }
        default:
            return nil; // Unsupported mode.
    }
}
