#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    i64 bytesCount = 64;
    i64 intsCount  = bytesCount / 4;

    PNSLR_AllocatorError err  = PNSLR_AllocatorError_None;
    ArraySlice(u8)       data = PNSLR_MakeSlice(u8, bytesCount, true, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);
    if (!Assert(err == PNSLR_AllocatorError_None)) return;
    if (!Assert(!!data.data )) return;
    if (!Assert(!!data.count)) return;

    // ensure zeroed and set to max
    // if this code throws a null ref, then the allocator is broken
    for (i32 i = 0; i < (i32) bytesCount; ++i)
    {
        Assert((data.data[i] == 0));

        data.data[i] = U8_MAX;
    }

    // ensure punning works correctly
    for (i32 i = 0; i < (i32) intsCount; ++i)
    {
        u32 value = ((u32*) data.data)[i];

        Assert(value == U32_MAX);
    }

    err = PNSLR_AllocatorError_None;
    PNSLR_FreeSlice(data, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);
    Assert(err == PNSLR_AllocatorError_None);
}
