#include "BindMetaParser.h"
#include "Lexer.h"

b8 ResolveMetaKey(const BindMeta* meta, utf8str key, utf8str* outValue)
{
    if (!meta || !meta->keys.data || !meta->values.data) return false;

    for (i64 i = 0; i < meta->keys.count; i++)
    {
        if (PNSLR_AreStringsEqual(meta->keys.data[i], key, 0))
        {
            if (outValue) *outValue = meta->values.data[i];
            return true;
        }
    }

    return false;
}

b8 LoadBindMeta(PNSLR_Path srcDir, BindMeta* output, PNSLR_Allocator allocator)
{
    PNSLR_Path filePath = PNSLR_GetPathForChildFile(srcDir, PNSLR_StringLiteral(".bindmeta.txt"), allocator);
    if (!PNSLR_PathExists(filePath, PNSLR_PathExistsCheckType_File)) { return false; }

    BindMeta data = {.domainDir = PNSLR_CloneString(srcDir.path, allocator)};
    PNSLR_ArraySlice(u8) contents = {0};
    if (!PNSLR_ReadAllContentsFromFile(filePath, &contents, allocator)) { return false; }

    i64 varCount = 0;
    FileIterInfo iter = {.contents = contents};
    i32 lineStart = 0, lineEnd = 0;
    while (DequeueNextLineSpan(&iter, &lineStart, &lineEnd))
    {
        if (lineEnd - lineStart <= 0) continue; // empty line

        PNSLR_ArraySlice(u8) line = {.count = lineEnd - lineStart, .data = contents.data + lineStart};
        if (line.count < 4) continue; // too short to be valid

        if (line.data[0] == '$' && line.data[1] == '$' && line.data[2] == '$') // var start decl
        {
            varCount++;
        }
    }

    data.keys = PNSLR_MakeSlice(utf8str, varCount, false, allocator, PNSLR_GET_LOC(), nil);
    data.values = PNSLR_MakeSlice(utf8str, varCount, false, allocator, PNSLR_GET_LOC(), nil);
    i64 varIterator = 0;

    iter.i = 0; iter.startOfToken = 0; // reset iter
    lineStart = 0; lineEnd = 0; // reset line info
    while (DequeueNextLineSpan(&iter, &lineStart, &lineEnd))
    {
        if (lineEnd - lineStart <= 0) continue; // empty line

        utf8str line = {.count = lineEnd - lineStart, .data = contents.data + lineStart};
        if (line.count < 4) continue; // too short to be valid

        if (line.data[0] == '$' && line.data[1] == '$' && line.data[2] == '$') // var start decl
        {
            // everything from index 3 till the end of the line is the var name
            utf8str varName = {.count = line.count - 3, .data = line.data + 3};
            data.keys.data[varIterator] = varName;
            i64 valStart = -1, valEnd = -1;
            i32 prevLineStart = lineStart, prevLineEnd = lineEnd;

            FileIterInfo tempIter = iter;
            while (DequeueNextLineSpan(&tempIter, &lineStart, &lineEnd)) // TODO: optimise to not double-read lines
            {
                utf8str line2 = {.count = lineEnd - lineStart, .data = contents.data + lineStart};
                if (valStart == -1) { valStart = lineStart; } // init value start if not yet started

                // go until the start of the new variable
                if (line2.count >= 3 && line2.data[0] == '$' && line2.data[1] == '$' && line2.data[2] == '$')
                {
                    valEnd = prevLineEnd;
                    break;
                }

                prevLineStart = lineStart; prevLineEnd = lineEnd;
            }

            if (valEnd == -1) { valEnd = prevLineEnd; } // last variable of file

            if (valStart != -1 && valEnd != -1 && valEnd > valStart) { data.values.data[varIterator] = (utf8str) {.count = valEnd - valStart, .data = contents.data + valStart}; }
            else data.values.data[varIterator] = (utf8str) {0};

            varIterator++;
        }
    }

    if (output) *output = data;
    return true;
}

typedef struct
{
    BindMetaCollection* output;
    PNSLR_Allocator     allocator;
} BindMetaGathererPayload;

b8 BindMetaGathererFn(rawptr payload, PNSLR_Path path, b8 isDirectory, b8* exploreCurrentDirectory)
{
    if (!isDirectory) return true; // only interested in directories

    BindMetaGathererPayload* data = (BindMetaGathererPayload*) payload;
    LoadAllBindMetas(path, data->output, data->allocator);

    return true;
}

b8 LoadAllBindMetas(PNSLR_Path rootDir, BindMetaCollection* outColl, PNSLR_Allocator allocator)
{
    b8 metaInSrcFound = false, metaInRootFound = false;
    BindMeta metaInSrc = {0}, metaInRoot = {0};

    PNSLR_Path srcDir = PNSLR_GetPathForSubdirectory(rootDir, PNSLR_StringLiteral("Source"), allocator);
    if (PNSLR_PathExists(srcDir, PNSLR_PathExistsCheckType_Directory))
    {
        PNSLR_Path depDir = PNSLR_GetPathForSubdirectory(srcDir, PNSLR_StringLiteral("Dependencies"), allocator);
        if (PNSLR_PathExists(depDir, PNSLR_PathExistsCheckType_Directory))
        {
            BindMetaGathererPayload payload = {
                .output    = outColl,
                .allocator = allocator
            };

            PNSLR_IterateDirectory(depDir, false, &payload, BindMetaGathererFn);
        }

        metaInSrcFound = LoadBindMeta(srcDir, &metaInSrc, allocator);
    }

    metaInRootFound = LoadBindMeta(rootDir, &metaInRoot, allocator);

    if (!metaInSrcFound && !metaInRootFound) return false; // no metas found

    utf8str srcPkgName = {0}, rootPkgName = {0};
    b8 srcHasPkgName  = ResolveMetaKey(&metaInSrc,  PNSLR_StringLiteral("PACKAGE_NAME"), &srcPkgName );
    b8 rootHasPkgName = ResolveMetaKey(&metaInRoot, PNSLR_StringLiteral("PACKAGE_NAME"), &rootPkgName);

    if (!srcHasPkgName && !rootHasPkgName) return false; // no package names found

    if (outColl)
    {
        if (outColl->numMetas >= outColl->metas.count)
        {
            i64 newCount = outColl->metas.count == 0 ? 4 : outColl->metas.count * 2;
            PNSLR_ResizeSlice(BindMeta, &(outColl->metas), newCount, false, allocator, PNSLR_GET_LOC(), nil);
            PNSLR_ResizeSlice(utf8str, &(outColl->pkgNames), newCount, false, allocator, PNSLR_GET_LOC(), nil);
        }

        if (srcHasPkgName)
        {
            outColl->pkgNames.data[outColl->numMetas] = srcPkgName;
            outColl->metas.data[outColl->numMetas] = metaInSrc;
            outColl->numMetas++;
        }

        if (rootHasPkgName)
        {
            outColl->pkgNames.data[outColl->numMetas] = rootPkgName;
            outColl->metas.data[outColl->numMetas] = metaInRoot;
            outColl->numMetas++;
        }
    }

    return true;
}
