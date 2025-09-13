#ifndef PNSLR_MEMORY_H // ==========================================================
#define PNSLR_MEMORY_H
#include "__Prelude.h"
#include "Runtime.h"
EXTERN_C_BEGIN

/**
 * Set a block of memory to a specific value.
 */
void PNSLR_MemSet(rawptr memory, i32 value, i32 size);

/**
 * Copy a block of memory from source to destination.
 */
void PNSLR_MemCopy(rawptr destination, rawptr source, i32 size);

/**
 * Copy a block of memory from source to destination, handling overlapping regions.
 */
void PNSLR_MemMove(rawptr destination, rawptr source, i32 size);

EXTERN_C_END
#endif // PNSLR_MEMORY_H ===========================================================
