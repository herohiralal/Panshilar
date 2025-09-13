#ifndef PNSLR_FILES_GATHER_H
#define PNSLR_FILES_GATHER_H
#include "../../Source/Panshilar.h"

typedef struct
{
    utf8str        pathRel;
    ArraySlice(u8) contents;
} CollectedFile;

DECLARE_ARRAY_SLICE(CollectedFile);

ArraySlice(CollectedFile) GatherSourceFiles(PNSLR_Path srcDir, utf8str startingPath, PNSLR_Allocator globalAllocator);

#endif
