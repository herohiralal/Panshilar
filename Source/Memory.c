#define PNSLR_IMPLEMENTATION
#include "Memory.h"

void PNSLR_MemSet(rawptr memory, i32 value, i32 size)
{
    if (memory == nil || size <= 0) { return; }
    memset(memory, value, (u64) size);
}

void PNSLR_MemCopy(rawptr destination, rawptr source, i32 size)
{
    if (destination == nil || source == nil || size <= 0) { return; }
    memcpy(destination, source, (u64) size);
}

void PNSLR_MemMove(rawptr destination, rawptr source, i32 size)
{
    if (destination == nil || source == nil || size <= 0) { return; }
    memmove(destination, source, (u64) size);
}
