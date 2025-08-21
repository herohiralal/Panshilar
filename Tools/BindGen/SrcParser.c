#include "SrcParser.h"
#include "Lexer.h"

void PrintParseError(utf8str pathRel, ArraySlice(u8) contents, i32 start, i32 end, utf8str err)
{
    i32 errLineStart = -1, errLineEnd = -1;
    i32 lineIdx = 0;
    for (i32 j = 0, w = 0; j < start; j += w)
    {
        rune r;
        PNSLR_DecodedRune rDecoded = PNSLR_DecodeRune((ArraySlice(u8)) {.data = contents.data + j, .count = contents.count - (i64) j});
        r = rDecoded.rune; w = rDecoded.length;
        if (r == '\n')
        {
            errLineStart = j + 1;
            lineIdx++;
        }
    }

    for (i32 j = start, w = 0; j < contents.count; j += w)
    {
        rune r;
        PNSLR_DecodedRune rDecoded = PNSLR_DecodeRune((ArraySlice(u8)) {.data = contents.data + j, .count = contents.count - (i64) j});
        r = rDecoded.rune; w = rDecoded.length;
        if (r == '\n')
        {
            errLineEnd = j;
            break;
        }
    }

    if (errLineStart == -1) errLineStart = 0;                    // error is in the first line
    if (errLineEnd   == -1) errLineEnd   = (i32) contents.count; // error is in the  last line

    printf("\n\n");
    printf(
        "Error in file \033[1;3m%.*s:%d:%d\033[22;23m: %.*s\n",
        (i32) pathRel.count, pathRel.data,
        lineIdx + 1, start - errLineStart + 1,
        (i32) err.count, err.data
    );

    printf(">   ");
    printf("\033[1;36m%.*s", errLineStart - start, contents.data + errLineStart);
    printf("\033[31m%.*s",   end - start,          contents.data + start       );
    printf("\033[36m%.*s",   errLineEnd - end,     contents.data + end         );
    printf("\033[0m\n");

    // highlight bad part
    {
        printf(">   ");
        for (i32 j = errLineStart; j < start; j++) { printf(" "); }
        printf("^");
        for (i32 j = end - start - 1; j > 0; j--) { printf("^"); }
        printf("\n");
    }
}

void ProcessFile(utf8str pathRel, ArraySlice(u8) contents)
{
    FileIterInfo iter = {0};
    iter.contents     = contents;

    // utf8str fileDoc = {0};

    b8 skipping = false, isInIncludeGuard = false;
    TokenSpan span = {0};
    while (DequeueNextTokenSpan(&iter, true, &span))
    {
        if (span.type == TokenType_LineEndComment) continue;

        utf8str tokenStr = (utf8str) {.count = span.end - span.start, .data = contents.data + span.start};

        // skipping handling
        {
            if (!skipping && span.type == TokenType_MetaSkipReflectBegin) { skipping = true;            }
            if (skipping && span.type == TokenType_MetaSkipReflectEnd)    { skipping = false; continue; }
            if (skipping)                                                 {                   continue; }
        }

        if (!isInIncludeGuard)
        {
            // if (span.type == TokenType_BlockComment)
            // {
            //     if (!fileDoc.count && !fileDoc.data) fileDoc = tokenStr;
            //     else continue; // meaningless
            // }

            // if (span.type == TokenType_SymbolHash)
            // {
            // }
        }

        utf8str tokenTypeStr = GetTokenTypeString(span.type);
        printf("[%.*s]", (i32) tokenTypeStr.count, tokenTypeStr.data);
        for (i32 j = 0; j < (32 - (i32) tokenTypeStr.count); ++j) { printf(" "); }
        printf("<%.*s>\n", (i32) tokenStr.count, tokenStr.data);
    }
}
