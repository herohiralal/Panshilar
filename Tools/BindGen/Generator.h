#ifndef PNSLR_BINDGEN_GEN
#define PNSLR_BINDGEN_GEN
#include "Panshilar.h"
#include "SrcParser.h"

typedef void (*GeneratorFn)(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator);

void RunGenerator(GeneratorFn generator, PNSLR_Path tgtDir, utf8str subDirName, ParsedContent* content, PNSLR_Allocator allocator);

#endif
