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

    BindMeta data = {0};
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

            if (valStart != -1 && valEnd != -1) { data.values.data[varIterator] = (utf8str) {.count = valEnd - valStart, .data = contents.data + valStart}; }
            else data.values.data[varIterator] = (utf8str) {0};

            varIterator++;
        }
    }

    if (output) *output = data;
    return true;
}
