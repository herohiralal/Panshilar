#include "Allocators.h"

rawptr PNSLR_Allocate(PNSLR_Allocator allocator, b8 zeroed, i32 size, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure)
    {
        if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
        return nil;
    }

    return allocator.procedure(
        allocator.data,
        zeroed ? PNSLR_AllocatorMode_Allocate : PNSLR_AllocatorMode_AllocateNoZero,
        size,
        alignment,
        nil,
        0,
        location,
        error
    );
}

rawptr PNSLR_Resize(PNSLR_Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure)
    {
        if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
        return nil;
    }

    return allocator.procedure(
        allocator.data,
        zeroed ? PNSLR_AllocatorMode_Resize : PNSLR_AllocatorMode_ResizeNoZero,
        newSize,
        alignment,
        oldMemory,
        oldSize,
        location,
        error
    );
}

rawptr PNSLR_DefaultResize(PNSLR_Allocator allocator, b8 zeroed, rawptr oldMemory, i32 oldSize, i32 newSize, i32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure) { return nil; }

    rawptr newMemory = PNSLR_Allocate(allocator, zeroed, newSize, alignment, location, error);
    if (!newMemory && error)
    {
        *error = PNSLR_AllocatorError_OutOfMemory;
        return nil;
    }

    PNSLR_Intrinsic_MemCopy(newMemory, oldMemory, oldSize < newSize ? oldSize : newSize);
    PNSLR_Free(allocator, oldMemory, location, error);
    return newMemory;
}

void PNSLR_Free(PNSLR_Allocator allocator, rawptr memory, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure || !memory)
    {
        if (error) { *error = PNSLR_AllocatorError_None; } // no memory to free or no allocator to free from
        return;
    }

    allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_Free,
        0,
        0,
        memory,
        0,
        location,
        error
    );
}

void PNSLR_FreeAll(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure)
    {
        if (error) { *error = PNSLR_AllocatorError_None; } // no allocator to free from
        return;
    }

    allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_FreeAll,
        0,
        0,
        nil,
        0,
        location,
        error
    );
}

u64 PNSLR_QueryAllocatorCapabilities(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure)
    {
        if (error) { *error = PNSLR_AllocatorError_None; }
        return PNSLR_AllocatorCapability_HintNil;
    }

    rawptr result = allocator.procedure(
        allocator.data,
        PNSLR_AllocatorMode_QueryCapabilities,
        0,
        0,
        nil,
        0,
        location,
        error
    );

    // reinterpret rawptr to u64
    return (u64)(result);
}

PNSLR_Allocator PNSLR_GetAllocator_DefaultHeap(void)
{
    return (PNSLR_Allocator) {.procedure = PNSLR_AllocatorFn_DefaultHeap, .data = nil};
}

rawptr PNSLR_AllocatorFn_DefaultHeap(rawptr allocatorData, u8 mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    switch (mode)
    {
        case PNSLR_AllocatorMode_Allocate:
        case PNSLR_AllocatorMode_AllocateNoZero:
        {
            rawptr memory = PNSLR_Intrinsic_Malloc(alignment, size);
            if (!memory)
            {
                if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
                return nil;
            }

            if (memory && mode == PNSLR_AllocatorMode_Allocate) { PNSLR_Intrinsic_MemSet(memory, 0, size); }
            return memory;
        }
        case PNSLR_AllocatorMode_Resize:
        case PNSLR_AllocatorMode_ResizeNoZero:
        {
            return PNSLR_DefaultResize(
                (PNSLR_Allocator) {.procedure = PNSLR_AllocatorFn_DefaultHeap, .data = allocatorData},
                mode == PNSLR_AllocatorMode_Resize,
                oldMemory,
                oldSize,
                size,
                alignment,
                location,
                error
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
            return (rawptr)(
                PNSLR_AllocatorCapability_ThreadSafe |
                PNSLR_AllocatorCapability_Free |
                PNSLR_AllocatorCapability_ResizeFR |
                PNSLR_AllocatorCapability_HintHeap
            );
        }
        default:
            return nil; // Unsupported mode.
    }

    return nil; // Should not reach here.
}

PNSLR_Allocator PNSLR_CreateAllocator_Stack(PNSLR_Allocator backingAllocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    PNSLR_StackAllocatorPayload* payload = PNSLR_Allocate(
        backingAllocator,
        true,
        sizeof(PNSLR_StackAllocatorPayload),
        alignof(PNSLR_StackAllocatorPayload),
        location,
        error
    );

    if (!payload) { return PNSLR_NIL_ALLOCATOR; }

    payload->currentPage = PNSLR_Allocate(
        backingAllocator,
        true,
        sizeof(PNSLR_StackAllocatorPage),
        alignof(PNSLR_StackAllocatorPage),
        location,
        error
    );

    return (PNSLR_Allocator) {
        .procedure = PNSLR_AllocatorFn_Stack,
        .data = payload
    };
}

rawptr PNSLR_AllocatorFn_Stack(rawptr allocatorData, u8 mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    PNSLR_StackAllocatorPayload* payload = (PNSLR_StackAllocatorPayload*)allocatorData;
    if (!payload || !payload->currentPage) { return nil; }

    switch (mode)
    {
        case PNSLR_AllocatorMode_Allocate:
        case PNSLR_AllocatorMode_AllocateNoZero:
        {
            if (payload->currentPage->usedBytes + size > sizeof(payload->currentPage->buffer))
            {
                // Allocate a new page if the current one is full
                PNSLR_StackAllocatorPage* newPage = PNSLR_Allocate(
                    (PNSLR_Allocator) {.procedure = PNSLR_AllocatorFn_Stack, .data = allocatorData},
                    true,
                    sizeof(PNSLR_StackAllocatorPage),
                    alignof(PNSLR_StackAllocatorPage),
                    location,
                    error
                );

                if (!newPage) { return nil; }

                newPage->previousPage = payload->currentPage;
                newPage->usedBytes = 0;
                payload->currentPage = newPage;
            }

            rawptr memory = payload->currentPage->buffer + payload->currentPage->usedBytes;
            payload->currentPage->usedBytes += size;

            if (mode == PNSLR_AllocatorMode_Allocate) { PNSLR_Intrinsic_MemSet(memory, 0, size); }
            return memory;
        }
        case PNSLR_AllocatorMode_Resize:
        case PNSLR_AllocatorMode_ResizeNoZero:
        {
            break;
        }
        case PNSLR_AllocatorMode_Free:
        {
            break;
        }
        case PNSLR_AllocatorMode_FreeAll:
        {
            break;
        }
        case PNSLR_AllocatorMode_QueryCapabilities:
        {
            break;
        }
        default:
            return nil; // Unsupported mode.
    }

    return nil; // Should not reach here.
}
