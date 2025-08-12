#include "Allocators.h"

static inline u64 AlignU64Forward(u64 value, u64 alignment)
{
    u64 modulo = value & (alignment - 1);
    if (modulo != 0) { value += (alignment - modulo); }
    return value;
}

static inline rawptr AlignRawptrForward(rawptr ptr, u64 alignment)
{
    return (rawptr) AlignU64Forward((u64) ptr, alignment);
}

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
        1,
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
        1,
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
        1,
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

rawptr PNSLR_AllocatorFn_DefaultHeap(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    // clear error by default
    if (error) { *error = PNSLR_AllocatorError_None; }

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
}

static PNSLR_ArenaAllocatorBlock* NewArenaAllocatorBlock(PNSLR_Allocator backingAllocator, u32 capacity, u32 alignment, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    u32 baseOffset = (alignment > sizeof(PNSLR_ArenaAllocatorBlock) ? alignment : sizeof(PNSLR_ArenaAllocatorBlock));
    u32 totalSize  = capacity + baseOffset;

    i32 minAlignment = (alignof(PNSLR_ArenaAllocatorBlock) > alignment ? alignof(PNSLR_ArenaAllocatorBlock) : (i32) alignment);
    minAlignment     = (16 > minAlignment ? 16 : minAlignment);

    rawptr output = PNSLR_Allocate(backingAllocator, true, (i32) totalSize, minAlignment, location, error);
    if (output == nil) { return nil; }

    PNSLR_ArenaAllocatorBlock* block    = (PNSLR_ArenaAllocatorBlock*) output;
    u64                        end      = (u64) block + totalSize;
    rawptr                     memory   = (rawptr)((u8*) block + baseOffset);
    u32                        realCap  = (u32) (end - (u64) memory);

    *block = (PNSLR_ArenaAllocatorBlock)
    {
        .allocator = backingAllocator,
        .memory    = memory,
        .capacity  = realCap,
        .used      = 0,
        .previous  = nil
    };

    return block;
}

static void DestroyArenaAllocatorBlock(PNSLR_ArenaAllocatorBlock* block, PNSLR_SourceCodeLocation location)
{
    if (block)
    {
        PNSLR_Free(block->allocator, block, location, nil);
    }
}

static ArraySlice(u8) AllocateFromArenaAllocatorBlock(PNSLR_ArenaAllocatorBlock* block, u32 minSize, u32 alignment, PNSLR_AllocatorError* error)
{
    if (!block)
    {
        if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
        return (ArraySlice(u8)) {0};
    }

    u64 alignmentOffset = 0;
    {
        u64 ptr = (((u64) block->memory) + block->used);
        u64 mask = (u64)(alignment - 1);
        u64 ptrAndMask = (u64) (ptr & mask);
        if (ptrAndMask != 0) { alignmentOffset = ((u64) alignment) - ptrAndMask; }
    }

    u64 size = minSize + alignmentOffset;
    if (size < minSize || size < alignmentOffset)
    {
        if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
        return (ArraySlice(u8)) {0};
    }

    u64 toBeUsed = block->used + size;
    if (toBeUsed > block->capacity || toBeUsed < block->used || toBeUsed < size)
    {
        if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
        return (ArraySlice(u8)) {0};
    }

    ArraySlice(u8) output = (ArraySlice(u8)) { .data = ((u8*) block->memory) + block->used + alignmentOffset, .count = (i64) minSize };
    block->used += (u32) size;
    return output;
}

static ArraySlice(u8) AllocateFromArenaAllocator(PNSLR_ArenaAllocatorPayload* data, u32 size, u32 alignment, PNSLR_SourceCodeLocation loc, PNSLR_AllocatorError* error)
{
    u64 needed = AlignU64Forward((u64) size, (u64) alignment);
    b8 createNewBlock = false;
    if (!data->currentBlock)
    {
        createNewBlock = true;
    }
    else
    {
        u64 maxNeeded = data->currentBlock->used + needed;
        maxNeeded = ((maxNeeded < data->currentBlock->used) || maxNeeded < needed) ? 0 : maxNeeded;
        if (maxNeeded > data->currentBlock->capacity)
        {
            createNewBlock = true;
        }
    }

    if (createNewBlock)
    {
        if (data->minimumBlockSize == 0) { data->minimumBlockSize = 8 * 1024 * 1024; } // 8 MiB
        u32 blockSize = (data->minimumBlockSize < (u32) needed) ? (u32) needed : data->minimumBlockSize; // pick larger

        PNSLR_AllocatorError err2 = PNSLR_AllocatorError_None;
        PNSLR_ArenaAllocatorBlock* newBlock = NewArenaAllocatorBlock(data->backingAllocator, blockSize, alignment, loc, &err2);
        if (err2 != PNSLR_AllocatorError_None)
        {
            if (error) { *error = err2; }
            return (ArraySlice(u8)) {0};
        }

        newBlock->previous = data->currentBlock;
        data->currentBlock = newBlock;
        data->totalCapacity += newBlock->capacity;
    }

    u32 previousUsed = data->currentBlock->used;
    ArraySlice(u8) output = AllocateFromArenaAllocatorBlock(data->currentBlock, (u32) size, (u32) alignment, error);
    data->totalUsed += data->currentBlock->used - previousUsed;
    return output;
}

static void FreeAllFromArenaAllocator(PNSLR_ArenaAllocatorPayload* data, PNSLR_SourceCodeLocation loc)
{
    while (data->currentBlock && data->currentBlock->previous)
    {
        PNSLR_ArenaAllocatorBlock* freeBlock = data->currentBlock;
        data->currentBlock = freeBlock->previous;

        data->totalCapacity -= freeBlock->capacity;
        DestroyArenaAllocatorBlock(freeBlock, loc);
    }

    if (data->currentBlock != nil)
    {
        PNSLR_Intrinsic_MemSet(data->currentBlock->memory, 0, (i32) data->currentBlock->used);
        data->currentBlock->used = 0;
    }

    data->totalUsed = 0;
}

PNSLR_Allocator PNSLR_NewAllocator_Arena(PNSLR_Allocator backingAllocator, u32 pageSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    PNSLR_ArenaAllocatorPayload* payload = PNSLR_Allocate(
        backingAllocator,
        true,
        sizeof(PNSLR_ArenaAllocatorPayload),
        alignof(PNSLR_ArenaAllocatorPayload),
        location,
        error
    );

    if (!payload) { return PNSLR_NIL_ALLOCATOR; }

    PNSLR_ArenaAllocatorBlock* block = NewArenaAllocatorBlock(backingAllocator, pageSize, 0, location, error);
    if (!block) { return PNSLR_NIL_ALLOCATOR; }

    *payload = (PNSLR_ArenaAllocatorPayload)
    {
        .backingAllocator = backingAllocator,
        .currentBlock     = block,
        .totalUsed        = 0,
        .totalCapacity    = 0,
        .minimumBlockSize = pageSize,
        .numSnapshots     = 0,
    };

    payload->totalCapacity += block->capacity;
    return (PNSLR_Allocator) {
        .procedure = PNSLR_AllocatorFn_Arena,
        .data = payload
    };
}

void PNSLR_DestroyAllocator_Arena(PNSLR_Allocator allocator, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    if (!allocator.procedure || !allocator.data) { return; }
    if (error) { *error = PNSLR_AllocatorError_None; } // No error by default

    PNSLR_ArenaAllocatorPayload* payload = (PNSLR_ArenaAllocatorPayload*) allocator.data;

    while (payload->currentBlock)
    {
        PNSLR_ArenaAllocatorBlock* freeBlock = payload->currentBlock;
        payload->currentBlock = freeBlock->previous;

        payload->totalCapacity -= freeBlock->capacity;
        DestroyArenaAllocatorBlock(freeBlock, location);
    }

    payload->totalUsed = 0;
    payload->totalCapacity = 0;

    PNSLR_Free(payload->backingAllocator, payload, location, error);
}

rawptr PNSLR_AllocatorFn_Arena(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    // clear error by default
    if (error) { *error = PNSLR_AllocatorError_None; }

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

    PNSLR_ArenaAllocatorPayload* payload = (PNSLR_ArenaAllocatorPayload*) allocatorData;

    switch (mode)
    {
        case PNSLR_AllocatorMode_Allocate:
        case PNSLR_AllocatorMode_AllocateNoZero:
        {
            return AllocateFromArenaAllocator(payload, (u32) size, (u32) alignment, location, error).data;
        }
        case PNSLR_AllocatorMode_Free:
        {
            if (error) { *error = PNSLR_AllocatorError_InvalidMode; }
            return nil;
        }
        case PNSLR_AllocatorMode_FreeAll:
        {
            FreeAllFromArenaAllocator(payload, location);
            return nil;
        }
        case PNSLR_AllocatorMode_Resize:
        case PNSLR_AllocatorMode_ResizeNoZero:
        {
            u8* oldData = (u8*) oldMemory;

            if (!oldData)
            {
                return AllocateFromArenaAllocator(payload, (u32) size, (u32) alignment, location, error).data;
            }
            else if (size == oldSize)
            {
                // return old memory
                if (oldMemory && mode == PNSLR_AllocatorMode_Resize) { PNSLR_Intrinsic_MemSet(oldMemory, 0, oldSize); }
                return oldMemory;
            }
            else if ((((u64) oldData) & ((u64) (alignment - 1))) == 0)
            {
                // shrink in place
                if (size < oldSize) { return oldMemory; }

                PNSLR_ArenaAllocatorBlock* block = payload->currentBlock;
                if (block)
                {
                    u64 start = (u64) oldData - (u64) block->memory;
                    u64 oldEnd = start + (u64) oldSize;
                    u64 newEnd = start + (u64) size;
                    if (start < oldEnd && oldEnd == block->used && newEnd <= block->capacity)
                    {
                        // grow output in-place, adjust for next allocation
                        block->used = (u32) newEnd;
                        return (rawptr) (((u8*) block->memory) + start);
                    }
                }
            }

            rawptr newMemory = AllocateFromArenaAllocator(payload, (u32) size, (u32) alignment, location, error).data;
            if (!newMemory) { return nil; }
            PNSLR_Intrinsic_MemCopy(newMemory, oldMemory, oldSize < size ? oldSize : size);
            return newMemory;
        }
        case PNSLR_AllocatorMode_QueryCapabilities:
        {
            return (rawptr)(
                PNSLR_AllocatorCapability_Resize |
                PNSLR_AllocatorCapability_FreeAll |
                PNSLR_AllocatorCapability_HintBump |
                PNSLR_AllocatorCapability_HintTemp
            );
        }
        default:
            if (error) { *error = PNSLR_AllocatorError_InvalidMode; }
            return nil; // Unsupported mode.
    }
}

b8 PNSLR_ValidateArenaAllocatorSnapshotState(PNSLR_Allocator allocator)
{
    if (allocator.procedure != PNSLR_AllocatorFn_Arena || !allocator.data) { return true; }

    return (((PNSLR_ArenaAllocatorPayload*) allocator.data)->numSnapshots == 0);
}

PNSLR_ArenaAllocatorSnapshot PNSLR_CaptureArenaAllocatorSnapshot(PNSLR_Allocator allocator)
{
    if (allocator.procedure != PNSLR_AllocatorFn_Arena || !allocator.data) { return (PNSLR_ArenaAllocatorSnapshot) {0}; }

    PNSLR_ArenaAllocatorSnapshot snapshot = {0};
    snapshot.valid   = true;
    snapshot.payload = ((PNSLR_ArenaAllocatorPayload*) allocator.data);
    snapshot.block   = snapshot.payload->currentBlock;

    if (snapshot.block) snapshot.used = snapshot.block->used;

    snapshot.payload->numSnapshots++;

    return snapshot;
}

PNSLR_ArenaSnapshotError PNSLR_RestoreArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot, PNSLR_SourceCodeLocation loc)
{
    if (!snapshot) { return PNSLR_ArenaSnapshotError_InvalidData; }

    PNSLR_ArenaSnapshotError output = PNSLR_ArenaSnapshotError_None;
    if (!snapshot->valid) { return output; }

    if (snapshot->block)
    {
        b8 memoryBlockFound = false;

        for (PNSLR_ArenaAllocatorBlock* block = snapshot->payload->currentBlock; block; block = block->previous)
        {
            if (block == snapshot->block)
            {
                memoryBlockFound = true;
                break;
            }
        }

        if (!memoryBlockFound)
        {
            output = PNSLR_ArenaSnapshotError_MemoryBlockNotOwned;
            goto exitFunction;
        }

        while (snapshot->payload->currentBlock != snapshot->block)
        {
            PNSLR_ArenaAllocatorBlock* tempBlock = snapshot->payload->currentBlock;
            snapshot->payload->currentBlock      = tempBlock->previous;

            snapshot->payload->totalCapacity -= tempBlock->capacity;
            DestroyArenaAllocatorBlock(tempBlock, loc);
        }

        PNSLR_ArenaAllocatorBlock* block = snapshot->payload->currentBlock;
        if (block)
        {
            if (block->used < snapshot->used)
            {
                output = PNSLR_ArenaSnapshotError_OutOfOrderRestoreUsage;
                goto exitFunction;
            }

            u32 amountToZero = block->used - snapshot->used;
            PNSLR_Intrinsic_MemSet(((u8*) block->memory) + snapshot->used, 0, amountToZero);
            block->used = snapshot->used;
            snapshot->payload->totalUsed -= amountToZero;
        }
    }

    if (!snapshot->payload->numSnapshots) { output = PNSLR_ArenaSnapshotError_DoubleRestoreOrDiscardUsage; }
    snapshot->payload->numSnapshots--;

    exitFunction:
    ;
    *snapshot = (PNSLR_ArenaAllocatorSnapshot) {0};
    return output;
}

PNSLR_ArenaSnapshotError PNSLR_DiscardArenaAllocatorSnapshot(PNSLR_ArenaAllocatorSnapshot* snapshot)
{
    if (!snapshot) { return PNSLR_ArenaSnapshotError_InvalidData; }

    PNSLR_ArenaSnapshotError output = PNSLR_ArenaSnapshotError_None;
    if (!snapshot->valid) { return output; }

    if (!snapshot->payload->numSnapshots) { output = PNSLR_ArenaSnapshotError_DoubleRestoreOrDiscardUsage; }
    snapshot->payload->numSnapshots--;

    *snapshot = (PNSLR_ArenaAllocatorSnapshot) {0};
    return output;
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
    for (PNSLR_StackAllocatorPage* page = payload->currentPage; page;)
    {
        // Free the current page
        PNSLR_StackAllocatorPage* nextPageToFree = page->previousPage;
        PNSLR_Free(payload->backingAllocator, page, location, error);
        if (error && *error != PNSLR_AllocatorError_None) { return; } // Stop on error
        page = nextPageToFree;
    }

    // Free the payload itself
    PNSLR_Free(payload->backingAllocator, payload, location, error);
    if (error && *error != PNSLR_AllocatorError_None) { return; } // Stop on error
}

rawptr PNSLR_AllocatorFn_Stack(rawptr allocatorData, PNSLR_AllocatorMode mode, i32 size, i32 alignment, rawptr oldMemory, i32 oldSize, PNSLR_SourceCodeLocation location, PNSLR_AllocatorError* error)
{
    // clear error by default
    if (error) { *error = PNSLR_AllocatorError_None; }

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
            if ((headerAlignment + headerSize + alignment + size) > (i32) sizeof(payload->currentPage->buffer))
            {
                if (error) { *error = PNSLR_AllocatorError_OutOfMemory; }
                return nil; // Not enough space in the current page
            }

            u64 usedBytesEndPtr              = (u64)(rawptr)(((u8*)(payload->currentPage->buffer)) + payload->currentPage->usedBytes);
            u64 usedBytesEndPtrHeaderAligned = (usedBytesEndPtr + headerAlignment - 1) & ~(u64)(headerAlignment - 1);
            u64 headerEndPtr                 = usedBytesEndPtrHeaderAligned + headerSize;
            u64 headerEndPtrAllocAligned     = (headerEndPtr + (u64) alignment - 1) & ~(u64)(alignment - 1);
            u64 allocEndPtr                  = headerEndPtrAllocAligned + (u64) size;
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
                usedBytesEndPtrHeaderAligned = (usedBytesEndPtr + headerAlignment - 1) & ~(u64)(headerAlignment - 1);
                headerEndPtr                 = usedBytesEndPtrHeaderAligned + headerSize;
                headerEndPtrAllocAligned     = (headerEndPtr + (u64) alignment - 1) & ~(u64)(alignment - 1);
                allocEndPtr                  = headerEndPtrAllocAligned + (u64) size;
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
            payload->currentPage->usedBytes = usedBytesTemp;

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
