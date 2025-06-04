#include "Compiler.h"

PRAGMA_SUPPRESS_WARNINGS

#if PNSLR_CLANG || PNSLR_GCC
    #include <stdlib.h>
#elif PNSLR_MSVC
    #include <malloc.h>
    #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#else
    #error "Unsupported compiler. Please define the appropriate headers for memory allocation."
#endif

#include <string.h>

PRAGMA_REENABLE_WARNINGS

#include "Intrinsics.h"

#ifndef NULL
    #define NULL ((rawptr)0)
#endif

rawptr PNSLR_Intrinsic_Malloc(i32 alignment, i32 size)
{
    if (size <= 0 || alignment <= 0) { return NULL; }
    return aligned_alloc((u64) alignment, (u64) size);
}

void PNSLR_Intrinsic_Free(rawptr memory)
{
    if (memory == NULL) { return; }
    free(memory);
}

void PNSLR_Intrinsic_MemSet(rawptr memory, i32 value, i32 size)
{
    if (memory == NULL || size <= 0) { return; }
    memset(memory, value, (u64) size);
}

void PNSLR_Intrinsic_MemCopy(rawptr destination, const rawptr source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memcpy(destination, source, (u64) size);
}

void PNSLR_Intrinsic_MemMove(rawptr destination, const rawptr source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memmove(destination, source, (u64) size);
}
