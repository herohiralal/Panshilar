#include "SrcParser.h"
#include "Lexer.h"

ArraySlice(DeclTypeInfo) BuildTypeTable(PNSLR_Allocator allocator, i64* count)
{
    ArraySlice(DeclTypeInfo) output = PNSLR_MakeSlice(DeclTypeInfo, 256, false, allocator, nil);
    if (!output.data || !output.count) FORCE_DBG_TRAP;

    i64 countTemp = 0;

    #define DECLARE_TYPE_TABLE_ENTRY(xxxx) \
        i64 xxxx##Idx = countTemp; \
        output.data[countTemp++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_None,  .u.name = PNSLR_STRING_LITERAL(#xxxx)}; \
        output.data[countTemp++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Ptr,   .u.polyTgtIdx = xxxx##Idx}; \
        output.data[countTemp++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Slice, .u.polyTgtIdx = xxxx##Idx};

    DECLARE_TYPE_TABLE_ENTRY(void   );
    DECLARE_TYPE_TABLE_ENTRY(b8     );
    DECLARE_TYPE_TABLE_ENTRY(b32    );
    DECLARE_TYPE_TABLE_ENTRY(u8     );
    DECLARE_TYPE_TABLE_ENTRY(u16    );
    DECLARE_TYPE_TABLE_ENTRY(u32    );
    DECLARE_TYPE_TABLE_ENTRY(u64    );
    DECLARE_TYPE_TABLE_ENTRY(i8     );
    DECLARE_TYPE_TABLE_ENTRY(i16    );
    DECLARE_TYPE_TABLE_ENTRY(i32    );
    DECLARE_TYPE_TABLE_ENTRY(i64    );
    DECLARE_TYPE_TABLE_ENTRY(f32    );
    DECLARE_TYPE_TABLE_ENTRY(f64    );
    DECLARE_TYPE_TABLE_ENTRY(utf8ch );
    DECLARE_TYPE_TABLE_ENTRY(utf16ch);
    DECLARE_TYPE_TABLE_ENTRY(rune   );
    DECLARE_TYPE_TABLE_ENTRY(cstring);
    DECLARE_TYPE_TABLE_ENTRY(rawptr );
    DECLARE_TYPE_TABLE_ENTRY(utf8str);

    #undef DECLARE_TYPE_TABLE_ENTRY

    *count = countTemp;
    return output;
}

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
    if ((start - errLineStart) > 0) printf("\033[1;36m%.*s", start - errLineStart, contents.data + errLineStart);
    if ((end - start         ) > 0) printf("\033[31m%.*s",   end - start,          contents.data + start       );
    if ((errLineEnd - end    ) > 0) printf("\033[36m%.*s",   errLineEnd - end,     contents.data + end         );
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

TokenType ForceGetNextToken(utf8str pathRel, FileIterInfo* iter, TokenIgnoreMask ignoreMask, TokenType typeMask, utf8str* tokenStr, PNSLR_Allocator allocator)
{
    // empty by default
    if (tokenStr) *tokenStr = (utf8str) {0};

    TokenSpan currSpan = {0};
    if (!DequeueNextTokenSpan(iter, ignoreMask, &currSpan))
    {
        if (typeMask & TokenType_EOF) { return TokenType_EOF; } // user expecting EOF

        // user not expecting EOF
        PrintParseError(pathRel, iter->contents, currSpan.start, currSpan.end, PNSLR_STRING_LITERAL("Not expecting EOF."));
        return TokenType_Invalid;
    }

    utf8str tokenStr2 = (utf8str) {.data = iter->contents.data + currSpan.start, .count = currSpan.end - currSpan.start};
    if (tokenStr) *tokenStr = tokenStr2;
    if (!(currSpan.type & typeMask)) // an unexpected token type encountered
    {
        utf8str errorPrefix = PNSLR_STRING_LITERAL("Unexpected token encountered, expecting one of: ");
        utf8str maskStr = GetTokenTypeMaskString(typeMask, PNSLR_STRING_LITERAL(", "), allocator);
        utf8str errorFull = PNSLR_ConcatenateStrings(errorPrefix, maskStr, allocator);
        PrintParseError(pathRel, iter->contents, currSpan.start, currSpan.end, errorFull);
        return TokenType_Invalid;
    }

    return currSpan.type;
}

b8 ConsumeFileIntro(utf8str pathRel, FileIterInfo* iter, utf8str* includeGuardIdentifier, PNSLR_Allocator allocator)
{
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
    utf8str includeGuardIdentifierCheck = {0};
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &includeGuardIdentifierCheck, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;

    utf8str beginComment = {0};
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_LineEndComment, &beginComment, allocator)) return false;
    i64 expectedCommentLength = (84 - (((i64) sizeof("#define") - 1) + 1 /*space*/ + includeGuardIdentifierCheck.count + 1 /*space*/));
    if (beginComment.count != expectedCommentLength)
    {
        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Begin comment does not match expected length."));
        return false;
    }
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_NewLine, nil, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_PreprocessorDefine, nil, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
    utf8str includeGuardIdentifierDecl = {0};
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &includeGuardIdentifierDecl, allocator)) return false;
    if (!PNSLR_AreStringsEqual(includeGuardIdentifierCheck, includeGuardIdentifierDecl, 0))
    {
        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Include guard identifiers do not match."));
        return false;
    }
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_NewLine, nil, allocator)) return false;

    return true;
}

b8 ConsumeSkipReflectBlock(utf8str pathRel, FileIterInfo* iter)
{
    i32 skipReflectStart = iter->startOfToken - 1, skipReflectEnd = iter->i;

    TokenSpan currSpan = {0};
    while (DequeueNextTokenSpan(iter, TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments | TokenIgnoreMask_Spaces, &currSpan))
    {
        if (currSpan.type == TokenType_MetaSkipReflectEnd)
        {
            return true;
        }
    }

    // file finished but block didn't end
    PrintParseError(pathRel, iter->contents, skipReflectStart, skipReflectEnd, PNSLR_STRING_LITERAL("skip reflect block not closed."));
    return false;
}

b8 ProcessExternCBlock(ParsedContent* parsedContent, CachedLasts* cachedLasts, utf8str pathRel, FileIterInfo* iter, PNSLR_Allocator allocator)
{
    i32 externCStart = iter->startOfToken - 1, externCEnd = iter->i;

    utf8str lastDoc = {0};
    while (iter->i < iter->contents.count)
    {
        utf8str tokenStr;
        TokenType rec = ForceGetNextToken(pathRel, iter,
            TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces,
            TokenType_MetaExternCEnd |
            TokenType_MetaSkipReflectBegin |
            TokenType_BlockComment |
            TokenType_LineEndComment |
            TokenType_Invalid,
            &tokenStr,
            allocator
        );
        if (!rec) return false;

        if (rec == TokenType_MetaExternCEnd)
        {
            return true;
        }

        if (rec == TokenType_MetaSkipReflectBegin) // if skipreflect, consume till end
        {
            if (!ConsumeSkipReflectBlock(pathRel, iter)) return false;

            continue;
        }

        if (rec == TokenType_LineEndComment)
        {
            if (tokenStr.count == 84 && PNSLR_StringEndsWith(tokenStr, PNSLR_STRING_LITERAL("======="), 0))
            {
                ParsedSection* sec = PNSLR_New(ParsedSection, allocator, nil);
                if (!sec) FORCE_DBG_TRAP;

                sec->header.type = DeclType_Section;
                sec->header.name = tokenStr;

                if (cachedLasts->lastDecl) cachedLasts->lastDecl->next         = &(sec->header);
                else                       cachedLasts->lastFile->declarations = &(sec->header);
                cachedLasts->lastDecl                                          = &(sec->header);
            }

            continue;
        }

        if (rec == TokenType_BlockComment)
        {
            lastDoc = tokenStr;
            continue;
        }
    }

    // file finished but block didn't end
    PrintParseError(pathRel, iter->contents, externCStart, externCEnd, PNSLR_STRING_LITERAL("extern c block not closed."));
    return false;
}

b8 ProcessFile(ParsedContent *parsedContent, CachedLasts *cachedLasts, utf8str pathRel, ArraySlice(u8) contents, PNSLR_Allocator allocator)
{
    ParsedFileContents* file = PNSLR_New(ParsedFileContents, allocator, nil);
    if (!file) FORCE_DBG_TRAP;

    if (cachedLasts->lastFile) cachedLasts->lastFile->next = file;
    else                       parsedContent->files        = file;
    cachedLasts->lastFile                                  = file;

    FileIterInfo iter = {0};
    iter.contents     = contents;

    // extract file doc
    {
        utf8str fileDocTemp = {0};
        TokenType rec = ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine,
            TokenType_BlockComment | TokenType_PreprocessorIfndef, &fileDocTemp, allocator);
        if (rec == TokenType_BlockComment)
        {
            file->doc = fileDocTemp;
            rec = ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments,
                TokenType_PreprocessorIfndef, nil, allocator);
        }
        if (!rec) return false;
    }

    utf8str includeGuardIdentifier = {0};
    if (!ConsumeFileIntro(pathRel, &iter, &includeGuardIdentifier, allocator)) return false;

    while (iter.i < contents.count)
    {
        utf8str tokenStr;
        TokenType rec = ForceGetNextToken(pathRel, &iter,
            TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments | TokenIgnoreMask_Spaces,
            TokenType_MetaSkipReflectBegin |
            TokenType_PreprocessorEndif |
            TokenType_MetaExternCBegin |
            TokenType_PreprocessorInclude |
            TokenType_Invalid,
            &tokenStr,
            allocator
        );
        if (!rec) return false;

        if (rec == TokenType_MetaSkipReflectBegin) // if skipreflect, consume till end
        {
            if (!ConsumeSkipReflectBlock(pathRel, &iter)) return false;

            continue;
        }

        if (rec == TokenType_PreprocessorInclude) // if #include, consume the whole line
        {
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_String, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Comments | TokenIgnoreMask_Spaces, TokenType_NewLine, nil, allocator)) return false;

            continue;
        }

        if (rec == TokenType_PreprocessorEndif) // include guard ended
        {
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;

            utf8str endComment = {0};
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_LineEndComment, &endComment, allocator)) return false;
            {
                i64 expectedLength = (84 - (((i64) sizeof("#endif") - 1) + 1 /*space*/));
                if (endComment.count != expectedLength)
                {
                    PrintParseError(pathRel, iter.contents, iter.startOfToken - 1, iter.i, PNSLR_STRING_LITERAL("End comment does not match expected length."));
                    return false;
                }

                utf8str endCommentIncludeGuardIdentifierPart = (utf8str){.data = endComment.data + 3, .count = endComment.count - 3};
                if (!PNSLR_StringStartsWith(endCommentIncludeGuardIdentifierPart, includeGuardIdentifier, 0))
                {
                    PrintParseError(pathRel, iter.contents, iter.startOfToken - 1, iter.i, PNSLR_STRING_LITERAL("Include guard identifiers do not match."));
                    return false;
                }
            }

            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces | TokenIgnoreMask_Comments, TokenType_EOF, nil, allocator)) return false;
            break;
        }

        // ONLY REACHES HERE IF THE CURRENT TOKEN IS EXTERN C BEGIN
        if (rec != TokenType_MetaExternCBegin) { FORCE_DBG_TRAP; }
        if (!ProcessExternCBlock(parsedContent, cachedLasts, pathRel, &iter, allocator)) return false;
        break;
    }

    return true;
}
