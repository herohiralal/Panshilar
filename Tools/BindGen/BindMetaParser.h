#ifndef PNSLR_BIND_META_PARSER_H
#define PNSLR_BIND_META_PARSER_H
#include "../../Source/Panshilar.h"

typedef struct
{
    PNSLR_ArraySlice(utf8str) keys;
    PNSLR_ArraySlice(utf8str) values;
} BindMeta;

b8 ResolveMetaKey(const BindMeta* meta, utf8str key, utf8str* outValue);
b8 LoadBindMeta(PNSLR_Path srcDir, BindMeta* output, PNSLR_Allocator allocator);

#endif
