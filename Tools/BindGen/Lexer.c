#include "Lexer.h"

utf8str GetTokenTypeString(TokenType type)
{
    #define TOKEN_VALUE(x) case TokenType_##x: return PNSLR_STRING_LITERAL(#x);
    switch (type)
    {
        TOKEN_VALUE(Invalid                       )
        TOKEN_VALUE(Identifier                    )
        TOKEN_VALUE(IdentifierButCouldBeHexNumber )
        TOKEN_VALUE(Integer                       )
        TOKEN_VALUE(Float                         )
        TOKEN_VALUE(HexNumber                     )
        TOKEN_VALUE(String                        )
        TOKEN_VALUE(BooleanTrue                   )
        TOKEN_VALUE(BooleanFalse                  )
        TOKEN_VALUE(IncompleteString              )
        TOKEN_VALUE(PlusSymbol                    )
        TOKEN_VALUE(MinusSymbol                   )
        TOKEN_VALUE(AsteriskSymbol                )
        TOKEN_VALUE(DollarSymbol                  )
        TOKEN_VALUE(HashSymbol                    )
        TOKEN_VALUE(TildeSymbol                   )
        TOKEN_VALUE(ColonSymbol                   )
        TOKEN_VALUE(CommaSymbol                   )
        TOKEN_VALUE(DotSymbol                     )
        TOKEN_VALUE(UnderscoreSymbol              )
        TOKEN_VALUE(QuestionSymbol                )
        TOKEN_VALUE(ExclamationSymbol             )
        TOKEN_VALUE(ParenthesesOpenSymbol         )
        TOKEN_VALUE(ParenthesesCloseSymbol        )
        TOKEN_VALUE(BracesOpenSymbol              )
        TOKEN_VALUE(BracesCloseSymbol             )
        TOKEN_VALUE(BracketOpenSymbol             )
        TOKEN_VALUE(BracketCloseSymbol            )
        TOKEN_VALUE(UnknownSymbol                 )
        TOKEN_VALUE(Spaces                        )
        TOKEN_VALUE(NewLine                       )
        TOKEN_VALUE(Tab                           )
        TOKEN_VALUE(Comment                       )
        TOKEN_VALUE(IncompleteComment             )
        TOKEN_VALUE(SkipReflectStart              )
        TOKEN_VALUE(SkipReflectEnd                )
        TOKEN_VALUE(KeywordDefine                 )
        TOKEN_VALUE(KeywordIfdef                  )
        TOKEN_VALUE(KeywordIfndef                 )
        TOKEN_VALUE(KeywordIf                     )
        TOKEN_VALUE(KeywordEndif                  )
        TOKEN_VALUE(KeywordExternCBegin           )
        TOKEN_VALUE(KeywordExternCEnd             )
        TOKEN_VALUE(KeywordInclude                )
        TOKEN_VALUE(MetaSkipReflectBegin          )
        TOKEN_VALUE(MetaSkipReflectEnd            )
    }
    #undef TOKEN_VALUE

    return PNSLR_STRING_LITERAL("__UNKNOWN_TOKEN_TYPE__");
}

static TokenSpanInfo GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace);

b8 DequeueNextLineSpan(FileIterInfo* file, i32* outLineStart, i32* outLineEnd)
{
    i32 outLineStartAlt = 0, outLineEndAlt = 0;
    outLineStart = (outLineStart) ? outLineStart : &outLineStartAlt;
    outLineEnd   = (outLineEnd)   ? outLineEnd   : &outLineEndAlt  ;

    i32 fileSize = (i32) file->contents.count;
    for (i32 i = file->startOfToken, w = 0; i < fileSize; i += w)
    {
        PNSLR_DecodedRune decodedRune = PNSLR_DecodeRune((ArraySlice(u8)){.count = file->contents.count - i, .data = file->contents.data + i});
        w = decodedRune.length;

        b8 isLastRune = ((i + w) == fileSize);

        u32 r2 = 0;
        if (!isLastRune)
        {
            PNSLR_DecodedRune nextDecodedRune = PNSLR_DecodeRune((ArraySlice(u8)){.count = file->contents.count - (i + w), .data = file->contents.data + (i + w)});
            r2 = nextDecodedRune.rune;
        }

        if (decodedRune.rune == '\r' && !isLastRune && r2 == '\n')
        {
            *outLineStart      = file->startOfToken;
            *outLineEnd        = i;
            file->startOfToken = (i + w + PNSLR_GetRuneLength('\n'));
            file->i            = (i + w + PNSLR_GetRuneLength('\n'));
            return true;
        }

        if (decodedRune.rune == '\n')
        {
            *outLineStart      = file->startOfToken;
            *outLineEnd        = i;
            file->startOfToken = (i + w);
            file->i            = (i + w);
            return true;
        }

        if (isLastRune)
        {
            *outLineStart      = file->startOfToken;
            *outLineEnd        = fileSize;
            file->startOfToken = fileSize;
            file->i            = fileSize;
            return true;
        }
    }

    *outLineStart = file->startOfToken;
    *outLineEnd   = fileSize;
    return false;
}

b8 DequeueNextTokenSpan(FileIterInfo* file, b8 ignoreSpace, TokenSpan* outTokenSpan)
{
    return IterateNextTokenSpan(file, true, ignoreSpace, outTokenSpan);
}

b8 PeekNextToken(FileIterInfo* file, b8 ignoreSpace, utf8str* outToken)
{
    TokenSpan span = {0};
    if (!PeekNextTokenSpan(file, ignoreSpace, &span))
    {
        if (outToken) *outToken = (utf8str) {0};
        return false;
    }

    if (outToken) *outToken = (utf8str) {.data = file->contents.data + span.start, .count = span.end - span.start};
    return true;
}

b8 PeekNextTokenSpan(FileIterInfo* file, b8 ignoreSpace, TokenSpan* outTokenSpan)
{
    return IterateNextTokenSpan(file, false, ignoreSpace, outTokenSpan);
}

b8 IterateNextTokenSpan(FileIterInfo* file, b8 moveFwd, b8 ignoreSpace, TokenSpan* outTokenSpan)
{
    i32 i = file->i;
    i32 startOfToken = file->startOfToken;

    while (true)
    {
        TokenSpanInfo tokenSpanInfo = GetCurrentTokenSpanInfo(file->contents, i, startOfToken, ignoreSpace);
        i += tokenSpanInfo.iterateFwd;
        startOfToken = tokenSpanInfo.newStartOfToken;

        b8 success = false;
        if (tokenSpanInfo.span.type != TokenType_Invalid)
        {
            if (outTokenSpan) *outTokenSpan = tokenSpanInfo.span;
            success = true;
        }
        else if (i >= file->contents.count)
        {
            if (outTokenSpan) *outTokenSpan = (TokenSpan) {.start = (i32) file->contents.count - 1, .end = (i32) file->contents.count, .type = TokenType_Invalid};
            success = false;
        }
        else { continue; }

        if (moveFwd)
        {
            file->i = i;
            file->startOfToken = startOfToken;
        }

        return success;
    }
}

#define LEXER_DECODE_RUNE(runeVarName, widthVarName, contents, contentsStart) \
    do \
    { \
        PNSLR_DecodedRune tempXXX##__LINE__ = PNSLR_DecodeRune((ArraySlice(u8)){.count = contents.count - (i64)(contentsStart), .data = contents.data + contentsStart}); \
        runeVarName = tempXXX##__LINE__.rune; \
        widthVarName = tempXXX##__LINE__.length; \
    } while (false); \

static TokenSpanInfo GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace)
{
    TokenSpanInfo retOut = {0};

    i32 fileSize = (i32) fileContents.count;
    u32 r; i32 width;
    LEXER_DECODE_RUNE(r, width, fileContents, i);
    b8 isLastRune = ((i + width) >= fileSize);

    u32 nextRune = 0;
    if (!isLastRune)
    {
        i32 _unused;
        LEXER_DECODE_RUNE(nextRune, _unused, fileContents, (i + width));
        _unused -= _unused;
    }

    if (IsSpace(r))
    {
        if (!ignoreSpace)
        {
            // ignore carriage return
            if (r == '\r')
            {
                retOut.span.type       = TokenType_Invalid;
                retOut.iterateFwd      = width;
                retOut.newStartOfToken = startOfToken + width;
                return retOut;
            }

            // send new line or tab as individuala tokens
            if (r == '\n' || r == '\t')
            {
                if (r == '\n')      { retOut.span.type = TokenType_NewLine; }
                else if (r == '\t') { retOut.span.type = TokenType_Tab;     }
                else                { printf("unimplemented"); FORCE_DBG_TRAP;    }

                retOut.span.start      = startOfToken;
                retOut.span.end        = i + width;
                retOut.iterateFwd      = width;
                retOut.newStartOfToken = i + width;
                return retOut;
            }

            // batch all ' ' characters
            if (r == ' ')
            {
                for (i32 j = (i + width), w2 = 0; j < fileSize; j += w2)
                {
                    u32 r2;
                    LEXER_DECODE_RUNE(r2, w2, fileContents, j);
                    if (r2 != ' ')
                    {
                        retOut.span.type       = TokenType_Spaces;
                        retOut.span.start      = startOfToken;
                        retOut.span.end        = j;
                        retOut.iterateFwd      = j - i;
                        retOut.newStartOfToken = j;
                        return retOut;
                    }
                }

                // reached end of file but space didn't finish
                retOut.span.type       = TokenType_Spaces;
                retOut.span.start      = startOfToken;
                retOut.span.end        = fileSize;
                retOut.iterateFwd      = fileSize - i;
                retOut.newStartOfToken = fileSize;
                return retOut;
            }
        }

        retOut.span.type       = TokenType_Invalid;
        retOut.iterateFwd      = width;
        retOut.newStartOfToken = startOfToken + width;
        return retOut;
    }

    if (r == '/' && !isLastRune && nextRune == '/')
    {
        // skip to the end of the line
        for (i32 j = (i + width + PNSLR_GetRuneLength('/')), w2 = 0; j < fileSize; j += w2)
        {
            u32 r2 = 0;
            LEXER_DECODE_RUNE(r2, w2, fileContents, j);
            b8 r2IsLastRune = ((j + w2) >= fileSize);

            u32 r3 = 0;
            if (!r2IsLastRune)
            {
                i32 _unused;
                LEXER_DECODE_RUNE(r3, _unused, fileContents, (j + w2));
                _unused -= _unused;
            }

            if (!r2IsLastRune && r3 == '\n')
            {
                retOut.span.type       = TokenType_Comment;
                // check for spl comments
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + (r2 == '\r' ? 0 : w2);
                {
                    utf8str spanStr = (utf8str) {.count = retOut.span.end - retOut.span.start, .data = fileContents.data + retOut.span.start};
                    if (PNSLR_AreStringsEqual(spanStr, PNSLR_STRING_LITERAL("//+skipreflect"), 0))
                    {
                        retOut.span.type = TokenType_MetaSkipReflectBegin;
                    }
                    else if (PNSLR_AreStringsEqual(spanStr, PNSLR_STRING_LITERAL("//-skipreflect"), 0))
                    {
                        retOut.span.type = TokenType_MetaSkipReflectEnd;
                    }
                }
                retOut.iterateFwd      = j + (r2 == '\r' ? 0 : w2) - i;
                retOut.newStartOfToken = j + (r2 == '\r' ? 0 : w2);
                return retOut;
            }
        }

        // reached the end of file but comment didn't finish
        retOut.span.type       = TokenType_Comment;
        retOut.span.start      = startOfToken;
        retOut.span.end        = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (r == '/' && !isLastRune && nextRune == '*')
    {
        // skip to the end of the comment
        for (i32 j = (i + width + PNSLR_GetRuneLength('*')), w2 = 0; j < fileSize; j += w2)
        {
            u32 r2 = 0;
            LEXER_DECODE_RUNE(r2, w2, fileContents, j);
            b8 r2IsLastRune = ((j + w2) >= fileSize);

            u32 r3 = 0;
            if (!r2IsLastRune)
            {
                i32 _unused;
                LEXER_DECODE_RUNE(r3, _unused, fileContents, (j + w2));
                _unused -= _unused;
            }

            if (r2 == '*' && !r2IsLastRune && r3 == '/')
            {
                retOut.span.type       = TokenType_Comment;
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + w2 + PNSLR_GetRuneLength('/');
                retOut.iterateFwd      = j + w2 + PNSLR_GetRuneLength('/') - i;
                retOut.newStartOfToken = j + w2 + PNSLR_GetRuneLength('/');
                return retOut;
            }
        }

        // reached end of file but comment didn't finish
        retOut.span.type       = TokenType_IncompleteComment;
        retOut.span.start      = startOfToken;
        retOut.span.end        = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (r == '"')
    {
        // skip to the end of the string
        u32 prevRune = r;
        for (i32 j = (i + width), w2 = 0; j < fileSize; j+= w2)
        {
            u32 r2;
            LEXER_DECODE_RUNE(r2, w2, fileContents, j);
            if (r2 == '"' && prevRune != '\\')
            {
                retOut.span.type       = TokenType_String;
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + w2;
                retOut.iterateFwd      = j + w2 - i;
                retOut.newStartOfToken = j + w2;
                return retOut;
            }

            prevRune = r2;
        }

        // reaached end of file but string didn't finish
        retOut.span.type       = TokenType_IncompleteString;
        retOut.span.start      = startOfToken;
        retOut.span.end        = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (IsSymbol(r))
    {
        if (r == '-')
        {
            u32 nextRune2 = 0;
            { i32 _unused; LEXER_DECODE_RUNE(nextRune2, _unused, fileContents, (i + width)); _unused -= _unused; }
            if (nextRune2 < '0' || nextRune2 > '9')
            {
                // do nothing, '-' will be returned as a single token
            }
            else
            {
                // skip to the end of the number
                b8 hasDecimalPoint = false;
                for (i32 j = (i + width), w2 = 0; j < fileSize; j += w2)
                {
                    u32 r2;
                    LEXER_DECODE_RUNE(r2, w2, fileContents, j);

                    b8 splCharValid = false;

                    if (r2 == '.')
                    {
                        if (hasDecimalPoint) // multiple decimal pts??
                        {
                            retOut.span.type       = TokenType_Float;
                            retOut.span.start      = startOfToken;
                            retOut.span.end        = j;
                            retOut.iterateFwd      = (j - i);
                            retOut.newStartOfToken = j;
                            return retOut;
                        }

                        hasDecimalPoint = true;
                        splCharValid    = true;
                    }

                    if (!((r2 >= '0' && r2 <= '9') || splCharValid))
                    {
                        retOut.span.type       = hasDecimalPoint ? TokenType_Float : TokenType_Integer;
                        retOut.span.start      = startOfToken;
                        retOut.span.end        = j;
                        retOut.iterateFwd      = (j - i);
                        retOut.newStartOfToken = j;
                        return retOut;
                    }
                }

                // reached end of file but number didn't finish
                retOut.span.type       = hasDecimalPoint ? TokenType_Float : TokenType_Integer;
                retOut.span.start      = startOfToken;
                retOut.span.end        = fileSize;
                retOut.iterateFwd      = (fileSize - i);
                retOut.newStartOfToken = fileSize;
                return retOut;
            }
        }

        switch (r)
        {
            case '+': retOut.span.type = TokenType_PlusSymbol;     break;
            case '-': retOut.span.type = TokenType_MinusSymbol;    break;
            case '*': retOut.span.type = TokenType_AsteriskSymbol; break;
            case '$': retOut.span.type = TokenType_DollarSymbol;   break;
            case '~': retOut.span.type = TokenType_TildeSymbol;    break;
            case ':': retOut.span.type = TokenType_ColonSymbol;    break;
            case ',': retOut.span.type = TokenType_CommaSymbol;    break;
            case '.': retOut.span.type = TokenType_DotSymbol;      break;
            case '#': retOut.span.type = TokenType_HashSymbol;     break;
            case '?': retOut.span.type = TokenType_QuestionSymbol; break;
            case '!': retOut.span.type = TokenType_ExclamationSymbol; break;
            case '(': retOut.span.type = TokenType_ParenthesesOpenSymbol; break;
            case ')': retOut.span.type = TokenType_ParenthesesCloseSymbol; break;
            case '{': retOut.span.type = TokenType_BracesOpenSymbol; break;
            case '}': retOut.span.type = TokenType_BracesCloseSymbol; break;
            case '[': retOut.span.type = TokenType_BracketOpenSymbol; break;
            case ']': retOut.span.type = TokenType_BracketCloseSymbol; break;
            default:  retOut.span.type = TokenType_UnknownSymbol;  break;
        }

        retOut.span.start      = startOfToken;
        retOut.span.end        = i + width;
        retOut.iterateFwd      = width;
        retOut.newStartOfToken = i + width;
        return retOut;
    }

    if (r >= '0' && r <= '9' && i == startOfToken)
    {
        // skip to the end of the number
        b8 isHex = false, hasDecimalPt = false;
        for (i32 j = i + width, w2 = 0; j < fileSize; j += w2)
        {
            u32 r2;
            LEXER_DECODE_RUNE(r2, w2, fileContents, j);

            b8 splCharValid = false;

            if (r2 >= 'a' && r2 <= 'f')
            {
                if (hasDecimalPt)
                {
                    retOut.span.type       = TokenType_Float;
                    retOut.span.start      = startOfToken;
                    retOut.span.end        = j;
                    retOut.iterateFwd      = (j - i);
                    retOut.newStartOfToken = j;
                    return retOut;
                }

                isHex        = true;
                splCharValid = true;
            }

            if (r2 == '.')
            {
                if (isHex)
                {
                    retOut.span.type       = TokenType_HexNumber;
                    retOut.span.start      = startOfToken;
                    retOut.span.end        = j;
                    retOut.iterateFwd      = (j - i);
                    retOut.newStartOfToken = j;
                    return retOut;
                }

                if (hasDecimalPt) // multiple decimal pts ??
                {
                    retOut.span.type       = TokenType_Float;
                    retOut.span.start      = startOfToken;
                    retOut.span.end        = j;
                    retOut.iterateFwd      = (j - i);
                    retOut.newStartOfToken = j;
                    return retOut;
                }

                hasDecimalPt = true;
                splCharValid = true;
            }

            if (!((r2 >= '0' && r2 <= '9') || splCharValid))
            {
                if (isHex)
                {
                    retOut.span.type   = TokenType_HexNumber;
                }
                else if (hasDecimalPt)
                {
                    retOut.span.type   = TokenType_Float;
                }
                else
                {
                    retOut.span.type   = TokenType_Integer;
                }

                retOut.span.start      = startOfToken;
                retOut.span.end        = j;
                retOut.iterateFwd      = (j - i);
                retOut.newStartOfToken = j;
                return retOut;
            }
        }

        // reached end of file but number didn't finish
        if (isHex)
        {
            retOut.span.type   = TokenType_HexNumber;
        }
        else if (hasDecimalPt)
        {
            retOut.span.type   = TokenType_Float;
        }
        else
        {
            retOut.span.type   = TokenType_Integer;
        }

        retOut.span.start      = startOfToken;
        retOut.span.end        = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (!isLastRune && (IsSymbol(nextRune) || IsSpace(nextRune)))
    {
        utf8str currentSpanStr = (utf8str) {.data = fileContents.data + startOfToken, .count = (i64) (i + width - startOfToken)};

        if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("true"), 0))
        {
            retOut.span.type = TokenType_BooleanTrue;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("false"), 0))
        {
            retOut.span.type = TokenType_BooleanFalse;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("define"), 0))
        {
            retOut.span.type = TokenType_KeywordDefine;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("ifdef"), 0))
        {
            retOut.span.type = TokenType_KeywordIfdef;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("ifndef"), 0))
        {
            retOut.span.type = TokenType_KeywordIfndef;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("if"), 0))
        {
            retOut.span.type = TokenType_KeywordIf;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("endif"), 0))
        {
            retOut.span.type = TokenType_KeywordEndif;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("EXTERN_C_BEGIN"), 0))
        {
            retOut.span.type = TokenType_KeywordExternCBegin;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("EXTERN_C_END"), 0))
        {
            retOut.span.type = TokenType_KeywordExternCEnd;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("include"), 0))
        {
            retOut.span.type = TokenType_KeywordInclude;
        }
        else if (IsValidHexNumber(currentSpanStr))
        {
            retOut.span.type = TokenType_IdentifierButCouldBeHexNumber;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("_"), 0))
        {
            retOut.span.type = TokenType_UnderscoreSymbol;
        }
        else
        {
            retOut.span.type = TokenType_Identifier;
        }

        retOut.span.start      = startOfToken;
        retOut.span.end        = i + width;
        retOut.iterateFwd      = width;
        retOut.newStartOfToken = i + width;
        return retOut;
    }

    retOut.span.type       = TokenType_Invalid;
    retOut.iterateFwd      = width;
    retOut.newStartOfToken = startOfToken;
    return retOut;
}

#undef LEXER_DECODE_RUNE
