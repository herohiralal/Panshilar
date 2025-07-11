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
    if (size < 0 )
    {
        if (error) { *error = PNSLR_AllocatorError_InvalidSize; }
        return nil; // Invalid size or alignment
    }

    // check alignment
    if ((alignment < 1) || (alignment & (alignment - 1))) // if alignment is not a power of two
    {
        if (error) { *error = PNSLR_AllocatorError_InvalidAlignment; }
        return nil;
    }

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
            if (oldMemory == nil)
            {
                if (error) { *error = PNSLR_AllocatorError_None; } // No memory to free
                return nil;
            }

            PNSLR_Intrinsic_Free(oldMemory);
            return nil;
        }
        case PNSLR_AllocatorMode_FreeAll:
        {
            if (error) { *error = PNSLR_AllocatorError_CantFreeAll; } // No error by default
            return nil;
        }
        case PNSLR_AllocatorMode_QueryCapabilities:
        {
            return (rawptr)(
                PNSLR_AllocatorCapability_ThreadSafe |
                PNSLR_AllocatorCapability_Free |
                PNSLR_AllocatorCapability_Resize |
                PNSLR_AllocatorCapability_HintHeap
            );
        }
        default:
            if (error) { *error = PNSLR_AllocatorError_InvalidMode; }
            return nil; // Unsupported mode.
    }

    return nil; // Should not reach here.
}

PNSLR_Allocator PNSLR_NewAllocator_Stack(PNSLR_Allocator backingAllocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
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

    payload->lastAllocation       = nil;
    payload->lastAllocationHeader = nil;
    payload->backingAllocator     = backingAllocator;
    payload->currentPage          = PNSLR_Allocate(
        backingAllocator,
        true,
        sizeof(PNSLR_StackAllocatorPage),
        alignof(PNSLR_StackAllocatorPage),
        location,
        error
    );

    payload->currentPage->previousPage = nil;
    payload->currentPage->usedBytes    = 0;

    return (PNSLR_Allocator) {
        .procedure = PNSLR_AllocatorFn_Stack,
        .data = payload
    };
}

void PNSLR_DestroyAllocator_Stack(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure || !allocator.data) { return; }
    if (error) { *error = PNSLR_AllocatorError_None; } // No error by default

    PNSLR_StackAllocatorPayload* payload = (PNSLR_StackAllocatorPayload*) allocator.data;

    // Free all pages in the stack allocator
    for (PNSLR_StackAllocatorPage* page = payload->currentPage; page; page = page->previousPage)
    {
        // Free the current page
        PNSLR_Free(payload->backingAllocator, page, location, error);
        if (error && *error != PNSLR_AllocatorError_None) { return; } // Stop on error
    }

    // Free the payload itself
    PNSLR_Free(payload->backingAllocator, payload, location, error);
    if (error && *error != PNSLR_AllocatorError_None) { return; } // Stop on error
}

rawptr PNSLR_AllocatorFn_Stack(rawptr allocatorData, u8 mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError *error)
{
    if (size < 0 )
    {
        if (error) { *error = PNSLR_AllocatorError_InvalidSize; }
        return nil; // Invalid size or alignment
    }

    // check alignment
    if ((alignment < 1) || (alignment & (alignment - 1))) // if alignment is not a power of two
    {
        if (error) { *error = PNSLR_AllocatorError_InvalidAlignment; }
        return nil;
    }

    PNSLR_StackAllocatorPayload* payload = (PNSLR_StackAllocatorPayload*)allocatorData;
    if (!payload || !payload->currentPage) { return nil; }

    switch (mode)
    {
        case PNSLR_AllocatorMode_Allocate:
        case PNSLR_AllocatorMode_AllocateNoZero:
        {
            const i32 headerSize       = sizeof(PNSLR_StackAllocationHeader);
            const i32 headerAlignment  = alignof(PNSLR_StackAllocationHeader);

            // this is a very rough calculation and doesn't actually account for offsets or the alignment of the allocation itself
            // but it should be enough for most cases, and the cases where it will throw an out-of-memory error are rare
            // and are actually off by a few bytes
            if ((headerAlignment + headerSize + alignment + size) > sizeof(payload->currentPage->buffer))
            {
                if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
                return nil; // Not enough space in the current page
            }

            u64 usedBytesEndPtr              = (u64)(rawptr)(((u8*)(payload->currentPage->buffer)) + payload->currentPage->usedBytes);
            u64 usedBytesEndPtrHeaderAligned = (usedBytesEndPtr + headerAlignment - 1) & ~(headerAlignment - 1);
            u64 headerEndPtr                 = usedBytesEndPtrHeaderAligned + headerSize;
            u64 headerEndPtrAllocAligned     = (headerEndPtr + alignment - 1) & ~(alignment - 1);
            u64 allocEndPtr                  = headerEndPtrAllocAligned + size;
            u64 effectiveUsedBytes           = allocEndPtr - (u64)(rawptr)(payload->currentPage->buffer);

            if (effectiveUsedBytes > sizeof(payload->currentPage->buffer))
            {
                // allocate a new page
                PNSLR_StackAllocatorPage* newPage = PNSLR_Allocate(
                    payload->backingAllocator,
                    true,
                    sizeof(PNSLR_StackAllocatorPage),
                    alignof(PNSLR_StackAllocatorPage),
                    location,
                    error
                );

                if (!newPage)
                {
                    if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
                    return nil; // Not enough space in the backing allocator
                }

                newPage->previousPage = payload->currentPage;
                newPage->usedBytes    = 0;
                payload->currentPage  = newPage;

                usedBytesEndPtr              = (u64)(rawptr)(newPage->buffer);
                usedBytesEndPtrHeaderAligned = (usedBytesEndPtr + headerAlignment - 1) & ~(headerAlignment - 1);
                headerEndPtr                 = usedBytesEndPtrHeaderAligned + headerSize;
                headerEndPtrAllocAligned     = (headerEndPtr + alignment - 1) & ~(alignment - 1);
                allocEndPtr                  = headerEndPtrAllocAligned + size;
                effectiveUsedBytes           = allocEndPtr - (u64)(rawptr)(payload->currentPage->buffer);
            }

            PNSLR_StackAllocatorPage* pageToUse        = payload->currentPage;
            pageToUse->usedBytes                       = effectiveUsedBytes;
            PNSLR_StackAllocationHeader* header        = (PNSLR_StackAllocationHeader*) usedBytesEndPtrHeaderAligned;
            rawptr                       tgtAllocation = (rawptr) headerEndPtrAllocAligned;

            header->page                  = pageToUse;
            header->size                  = size;
            header->alignment             = alignment;
            header->lastAllocation        = payload->lastAllocation;
            header->lastAllocationHeader  = payload->lastAllocationHeader;
            payload->lastAllocation       = tgtAllocation;
            payload->lastAllocationHeader = (rawptr)header;

            if (mode == PNSLR_AllocatorMode_Allocate)
            {
                PNSLR_Intrinsic_MemSet(tgtAllocation, 0, size); // Zero the memory if requested
            }

            return tgtAllocation;
        }
        case PNSLR_AllocatorMode_Resize:
        case PNSLR_AllocatorMode_ResizeNoZero:
        {
            if (error) { *error = PNSLR_AllocatorError_InvalidMode; }
            return nil; // Resizing breaks stack semantics
        }
        case PNSLR_AllocatorMode_Free:
        {
            if (oldMemory == nil)
            {
                if (error) { *error = PNSLR_AllocatorError_None; } // No memory to free
                return nil;
            }

            if (oldMemory != payload->lastAllocation)
            {
                if (error) { *error = PNSLR_AllocatorError_OutOfOrderFree; }
                return nil; // Invalid free, not the last allocated memory
            }

            PNSLR_StackAllocationHeader* header = payload->lastAllocationHeader;
            if (!header)
            {
                if (error) { *error = PNSLR_AllocatorError_DoubleFree; }
                return nil; // Double free detected
            }

            if (!header->page)
            {
                if (error) { *error = PNSLR_AllocatorError_Internal; }
                return nil; // Internal error, page should not be nil
            }

            // find whether the header's page is even in the current page chain
            {
                b8 found = false;
                for (PNSLR_StackAllocatorPage* page = payload->currentPage; page; page = page->previousPage)
                {
                    if (page == header->page)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    if (error) { *error = PNSLR_AllocatorError_Internal; }
                    return nil;
                    // we've already confirmed that the allocation belongs to the current allocator
                    // but if the allocation's page is not in the current page chain, it means that the allocator is in an invalid state
                }
            }

            // free all the pages till the current allocation's page is reached
            {
                PNSLR_StackAllocatorPage* page = nil;
                for (page = payload->currentPage; page != header->page; page = page->previousPage)
                {
                    if (!page)
                    {
                        if (error) { *error = PNSLR_AllocatorError_Internal; }
                        return nil; // Internal error, page should not be nil
                    }

                    PNSLR_Free(payload->backingAllocator, page, location, error);
                    if (error && *error != PNSLR_AllocatorError_None) { return nil; } // Stop on error
                }

                payload->currentPage = page; // set the current page to the page of the allocation being freed
            }

            payload->lastAllocation       = header->lastAllocation;
            payload->lastAllocationHeader = header->lastAllocationHeader;

            u64 usedBytesTemp               = ((u64)(header)) - ((u64)(rawptr)((u8*)(payload->currentPage->buffer)));
            if (usedBytesTemp > I32_MAX)
            {
                if (error) { *error = PNSLR_AllocatorError_Internal; }
                return nil; // Internal error, used bytes cannot be negative
            }
            payload->currentPage->usedBytes = (i32) usedBytesTemp;

            // clear the header to avoid double frees
            header->page                  = nil;
            header->size                  = 0;
            header->alignment             = 0;
            header->lastAllocation        = nil;
            header->lastAllocationHeader  = nil;

            break;
        }
        case PNSLR_AllocatorMode_FreeAll:
        {
            PNSLR_StackAllocatorPage* page = payload->currentPage;
            while (page && page->previousPage)
            {
                PNSLR_StackAllocatorPage* prev = page->previousPage;
                PNSLR_Free(payload->backingAllocator, page, location, error);
                if (error && *error != PNSLR_AllocatorError_None) { return nil; }
                page = prev;
            }

            payload->lastAllocation       = nil;
            payload->lastAllocationHeader = nil;
            payload->currentPage          = page;
            if (page)
            {
                page->previousPage = nil;
                page->usedBytes    = 0;
            }

            if (error) { *error = PNSLR_AllocatorError_None; }
            return nil;
        }
        case PNSLR_AllocatorMode_QueryCapabilities:
        {
            return (rawptr)(
                PNSLR_AllocatorCapability_Free |
                PNSLR_AllocatorCapability_FreeAll |
                PNSLR_AllocatorCapability_HintBump
            );
        }
        default:
            if (error) { *error = PNSLR_AllocatorError_InvalidMode; }
            return nil; // Unsupported mode.
    }

    return nil; // Should not reach here.
}
