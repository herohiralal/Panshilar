#ifndef PNSLR_BIND_META_PARSER_H
#define PNSLR_BIND_META_PARSER_H
#include "../../Source/Panshilar.h"

typedef struct
{
    PNSLR_Path                domainDir;
    PNSLR_ArraySlice(utf8str) keys;
    PNSLR_ArraySlice(utf8str) values;
} BindMeta;

PNSLR_DECLARE_ARRAY_SLICE(BindMeta);

typedef struct
{
    PNSLR_ArraySlice(BindMeta) metas;
    i64                        numMetas;
} BindMetaCollection;

b8 ResolveMetaKey(const BindMeta* meta, utf8str key, utf8str* outValue);
b8 LoadBindMeta(PNSLR_Path srcDir, BindMeta* output, PNSLR_Allocator allocator);
b8 LoadAllBindMetas(PNSLR_Path rootDir, BindMetaCollection* outColl, PNSLR_Allocator allocator);
b8 ResolveMeta(const BindMetaCollection* coll, PNSLR_Path file, BindMeta** outMetaPtr);

#endif
