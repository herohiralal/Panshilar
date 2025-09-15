#ifndef PNSLR_FILES_GATHER_H
#define PNSLR_FILES_GATHER_H
#include "../../Source/Panshilar.h"

typedef struct
{
    utf8str              pathRel;
    PNSLR_ArraySlice(u8) contents;
} CollectedFile;

PNSLR_DECLARE_ARRAY_SLICE(CollectedFile);

PNSLR_ArraySlice(CollectedFile) GatherSourceFiles(PNSLR_Path srcDir, utf8str startingPath, PNSLR_Allocator globalAllocator);

#endif
