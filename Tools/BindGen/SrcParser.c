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

b8 ProcessFile(utf8str pathRel, ArraySlice(u8) contents, PNSLR_Allocator allocator)
{
    FileIterInfo iter = {0};
    iter.contents     = contents;

    utf8str fileDoc = {0};
    {
        utf8str fileDocTemp = {0};
        TokenType rec = ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine,
            TokenType_BlockComment | TokenType_PreprocessorIfndef, &fileDocTemp, allocator);
        if (rec == TokenType_BlockComment)
        {
            fileDoc = fileDocTemp;
            rec = ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments,
                TokenType_PreprocessorIfndef, nil, allocator);
        }
        if (!rec) return false;
    }

    utf8str includeGuardIdentifier = {0};
    {
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
        utf8str includeGuardIdentifierCheck = {0};
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Identifier, &includeGuardIdentifierCheck, allocator)) return false;
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;

        utf8str beginComment = {0};
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_LineEndComment, &beginComment, allocator)) return false;
        i64 expectedCommentLength = (84 - (((i64) sizeof("#define") - 1) + 1 /*space*/ + includeGuardIdentifierCheck.count + 1 /*space*/));
        if (beginComment.count != expectedCommentLength)
        {
            PrintParseError(pathRel, iter.contents, iter.startOfToken - 1, iter.i, PNSLR_STRING_LITERAL("Begin comment does not match expected length."));
            return false;
        }
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_NewLine, nil, allocator)) return false;
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_PreprocessorDefine, nil, allocator)) return false;
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
        utf8str includeGuardIdentifierDecl = {0};
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Identifier, &includeGuardIdentifierDecl, allocator)) return false;
        if (!PNSLR_AreStringsEqual(includeGuardIdentifierCheck, includeGuardIdentifierDecl, 0))
        {
            PrintParseError(pathRel, iter.contents, iter.startOfToken - 1, iter.i, PNSLR_STRING_LITERAL("Include guard identifiers do not match."));
            return false;
        }
        if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_NewLine, nil, allocator)) return false;

        includeGuardIdentifier = includeGuardIdentifierCheck;
    }

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

        if (rec == TokenType_MetaSkipReflectBegin)
        {
            i32 skipReflectStart = iter.startOfToken - 1, skipReflectEnd = iter.i;

            TokenSpan currSpan = {0};
            b8 endFound = false;
            while (DequeueNextTokenSpan(&iter, TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments | TokenIgnoreMask_Spaces, &currSpan))
            {
                if (currSpan.type == TokenType_MetaSkipReflectEnd)
                {
                    endFound = true;
                    break;
                }
            }

            if (endFound) continue;

            // file finished but block didn't end
            PrintParseError(pathRel, iter.contents, skipReflectStart, skipReflectEnd, PNSLR_STRING_LITERAL("skip reflect block not closed."));
            return false;
        }

        if (rec == TokenType_PreprocessorInclude) // if #include, consume the whole line
        {
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_None, TokenType_String, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, &iter, TokenIgnoreMask_Comments | TokenIgnoreMask_Spaces, TokenType_NewLine, nil, allocator)) return false;

            continue;
        }

        if (rec == TokenType_PreprocessorEndif)
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
    }

    return true;
}
