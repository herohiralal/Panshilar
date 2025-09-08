#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    PNSLR_I64 bytesCount = 64;
    PNSLR_I64 longsCount = bytesCount / (PNSLR_I64) sizeof(PNSLR_U64);
    PNSLR_I64 intsCount  = bytesCount / (PNSLR_I64) sizeof(PNSLR_U32);

    PNSLR_AllocatorError err  = PNSLR_AllocatorError_None;
    PNSLR_ArraySlice(PNSLR_U64)      data = PNSLR_MakeSlice(PNSLR_U64, longsCount, true, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), &err);
    if (!Assert(err == PNSLR_AllocatorError_None)) return;
    if (!Assert(!!data.data )) return;
    if (!Assert(!!data.count)) return;

    // alignment test
    {
        PNSLR_U64 ptr = (PNSLR_U64) (void*) data.data;
        Assert((ptr % alignof(PNSLR_U64)) == 0);
    }

    // ensure zeroed and set to max
    // if this code throws a null ref, then the allocator is broken
    for (PNSLR_I32 i = 0; i < (PNSLR_I32) longsCount; ++i)
    {
        Assert((data.data[i] == 0));

        data.data[i] = 18446744073709551615ULL;
    }

    // ensure punning works correctly
    for (PNSLR_I32 i = 0; i < (PNSLR_I32) intsCount; ++i)
    {
        PNSLR_U32 value = ((PNSLR_U32*) data.data)[i];

        Assert(value == 4294967295U);
    }

    err = PNSLR_AllocatorError_None;
    PNSLR_ResizeSlice(PNSLR_U64, &data, longsCount * 2, true, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), &err);
    if (!Assert(err == PNSLR_AllocatorError_None)) return;

    // ensure allocated memory is read-write accessible
    for (PNSLR_I32 i = (PNSLR_I32) longsCount; i < (PNSLR_I32) (longsCount * 2); ++i)
    {
        Assert((data.data[i] == 0));

        data.data[i] = 1;
    }

    err = PNSLR_AllocatorError_None;
    PNSLR_FreeSlice(&data, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), &err);
    Assert(err == PNSLR_AllocatorError_None);
}
