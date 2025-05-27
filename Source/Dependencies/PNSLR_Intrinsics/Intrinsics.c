#include "Intrinsics.h"

#if defined(__GNUC__) || defined(__clang__)
    #include <stdlib.h>
#elif defined(_MSC_VER)
    #include <malloc.h>
    #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#else
    #error "Unsupported compiler. Please define the appropriate headers for memory allocation."
#endif

#include <string.h>

#ifndef NULL
    #define NULL ((void*)0)
#endif

void* PNSLR_Intrinsic_Malloc(i32 alignment, i32 size)
{
    if (size <= 0 || alignment <= 0) { return NULL; }
    return aligned_alloc(alignment, size);
}

void PNSLR_Intrinsic_Free(void* memory)
{
    if (memory == NULL) { return; }
    free(memory);
}

void PNSLR_Intrinsic_MemSet(void* memory, i32 value, i32 size)
{
    if (memory == NULL || size <= 0) { return; }
    memset(memory, value, size);
}

void PNSLR_Intrinsic_MemCopy(void* destination, const void* source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memcpy(destination, source, size);
}

void PNSLR_Intrinsic_MemMove(void* destination, const void* source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memmove(destination, source, size);
}
