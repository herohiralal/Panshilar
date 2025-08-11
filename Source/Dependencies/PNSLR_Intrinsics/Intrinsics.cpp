#include "Compiler.h"

PRAGMA_SUPPRESS_WARNINGS

#if PNSLR_CLANG || PNSLR_GCC
    #include <cstdlib>
    #define aligned_free(block)            free(block)
#elif PNSLR_MSVC
    #include <malloc.h>
    #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
    #define aligned_free(block)            _aligned_free(block)
#else
    #error "Unsupported compiler. Please define the appropriate headers for memory allocation."
#endif

#include <string.h>

PRAGMA_REENABLE_WARNINGS

#include "Intrinsics.h"

struct PNSLR_Intrinsic_ThreadCleanupHelper
{
    PNSLR_Intrinsic_ThreadCleanupDelegate delegates[8];

    ~PNSLR_Intrinsic_ThreadCleanupHelper()
    {
        for (i32 i = 0; i < 8; ++i)
        {
            PNSLR_Intrinsic_ThreadCleanupDelegate& delegate = delegates[i];

            if (delegate != nil)
            {
                delegate();
                delegate = (PNSLR_Intrinsic_ThreadCleanupDelegate) (nil);
            }
        }
    }
};

static thread_local PNSLR_Intrinsic_ThreadCleanupHelper G_ThreadCleanupHelper;

void PNSLR_Intrinsic_RegisterThreadCleanup(PNSLR_Intrinsic_ThreadCleanupDelegate delegate)
{
    for (i32 i = 0; i < 8; ++i)
    {
        if (G_ThreadCleanupHelper.delegates[i] == nil)
        {
            G_ThreadCleanupHelper.delegates[i] = delegate;
            return;
        }
    }
}

rawptr PNSLR_Intrinsic_Malloc(i32 alignment, i32 size)
{
    if (size <= 0 || alignment <= 0) { return nil; }
    return aligned_alloc((u64) alignment, (u64) size);
}

void PNSLR_Intrinsic_Free(rawptr memory)
{
    if (memory == nil) { return; }
    aligned_free(memory);
}

void PNSLR_Intrinsic_MemSet(rawptr memory, i32 value, i32 size)
{
    if (memory == nil || size <= 0) { return; }
    memset(memory, value, (u64) size);
}

void PNSLR_Intrinsic_MemCopy(rawptr destination, const rawptr source, i32 size)
{
    if (destination == nil || source == nil || size <= 0) { return; }
    memcpy(destination, source, (u64) size);
}

void PNSLR_Intrinsic_MemMove(rawptr destination, const rawptr source, i32 size)
{
    if (destination == nil || source == nil || size <= 0) { return; }
    memmove(destination, source, (u64) size);
}
