#include "Intrinsics.h"

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weverything"
#elif defined(_MSC_VER)
    #pragma warning(push, 0)
#endif

#if defined(__GNUC__) || defined(__clang__)
    #include <stdlib.h>
#elif defined(_MSC_VER)
    #include <malloc.h>
    #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#else
    #error "Unsupported compiler. Please define the appropriate headers for memory allocation."
#endif

#include <string.h>

#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif

#ifndef NULL
    #define NULL ((void*)0)
#endif

void* PNSLR_Intrinsic_Malloc(i32 alignment, i32 size)
{
    if (size <= 0 || alignment <= 0) { return NULL; }
    return aligned_alloc((u64) alignment, (u64) size);
}

void PNSLR_Intrinsic_Free(void* memory)
{
    if (memory == NULL) { return; }
    free(memory);
}

void PNSLR_Intrinsic_MemSet(void* memory, i32 value, i32 size)
{
    if (memory == NULL || size <= 0) { return; }
    memset(memory, value, (u64) size);
}

void PNSLR_Intrinsic_MemCopy(void* destination, const void* source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memcpy(destination, source, (u64) size);
}

void PNSLR_Intrinsic_MemMove(void* destination, const void* source, i32 size)
{
    if (destination == NULL || source == NULL || size <= 0) { return; }
    memmove(destination, source, (u64) size);
}
