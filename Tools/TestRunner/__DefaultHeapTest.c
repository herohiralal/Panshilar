#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    i64 bytesCount = 64;
    i64 longsCount = bytesCount / (i64) sizeof(u64);
    i64 intsCount  = bytesCount / (i64) sizeof(u32);

    PNSLR_AllocatorError err  = PNSLR_AllocatorError_None;
    ArraySlice(u64)      data = PNSLR_MakeSlice(u64, longsCount, true, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);
    if (!Assert(err == PNSLR_AllocatorError_None)) return;
    if (!Assert(!!data.data )) return;
    if (!Assert(!!data.count)) return;

    // alignment test
    {
        u64 ptr = (u64) (rawptr) data.data;
        Assert((ptr % alignof(u64)) == 0);
    }

    // ensure zeroed and set to max
    // if this code throws a null ref, then the allocator is broken
    for (i32 i = 0; i < (i32) longsCount; ++i)
    {
        Assert((data.data[i] == 0));

        data.data[i] = U64_MAX;
    }

    // ensure punning works correctly
    for (i32 i = 0; i < (i32) intsCount; ++i)
    {
        u32 value = ((u32*) data.data)[i];

        Assert(value == U32_MAX);
    }

    err = PNSLR_AllocatorError_None;
    PNSLR_ResizeSlice(u64, data, longsCount * 2, true, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);
    if (!Assert(err == PNSLR_AllocatorError_None)) return;

    // ensure allocated memory is read-write accessible
    for (i32 i = (i32) longsCount; i < (i32) (longsCount * 2); ++i)
    {
        Assert((data.data[i] == 0));

        data.data[i] = 1;
    }

    err = PNSLR_AllocatorError_None;
    PNSLR_FreeSlice(data, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);
    Assert(err == PNSLR_AllocatorError_None);
}
