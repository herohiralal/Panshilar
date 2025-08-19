#include "Lexer.h"

static PNSLR_TokenSpanInfo PNSLR_GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace);

b8 PNSLR_DequeueNextLineSpan(PNSLR_FileIterInfo* file, b8 ignoreSpace, i32* outLineStart, i32* outLineEnd)
{
    i32 outLineStartAlt = 0, outLineEndAlt = 0;
    outLineStart = (outLineStart) ? outLineStart : &outLineStartAlt;
    outLineEnd   = (outLineEnd)   ? outLineEnd   : &outLineEndAlt  ;

    i32 fileSize = (i32) file->contents.count;
    for (i32 i = file->startOfToken, w = 0; i < fileSize; i += w)
    {
        PNSLR_DecodedRune decodedRune = PNSLR_DecodeRune((ArraySlice(u8)){.count = file->contents.count - i, .data = file->contents.data + i});

        b8 isLastRune = ((i + w) == fileSize);

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

b8 PNSLR_DequeueNextTokenSpan(PNSLR_FileIterInfo* file, b8 ignoreSpace, PNSLR_TokenSpan* outTokenSpan)
{
    return PNSLR_IterateNextTokenSpan(file, true, ignoreSpace, outTokenSpan);
}

b8 PNSLR_PeekNextToken(PNSLR_FileIterInfo* file, b8 ignoreSpace, utf8str* outToken)
{
    PNSLR_TokenSpan span = {0};
    if (!PNSLR_PeekNextTokenSpan(file, ignoreSpace, &span))
    {
        if (outToken) *outToken = (utf8str) {0};
        return false;
    }

    if (outToken) *outToken = (utf8str) {.data = file->contents.data + span.start, .count = span.end - span.start};
    return true;
}

b8 PNSLR_PeekNextTokenSpan(PNSLR_FileIterInfo* file, b8 ignoreSpace, PNSLR_TokenSpan* outTokenSpan)
{
    return PNSLR_IterateNextTokenSpan(file, false, ignoreSpace, outTokenSpan);
}

b8 PNSLR_IterateNextTokenSpan(PNSLR_FileIterInfo* file, b8 moveFwd, b8 ignoreSpace, PNSLR_TokenSpan* outTokenSpan)
{
    i32 i = file->i;
    i32 startOfToken = file->startOfToken;

    while (true)
    {
        PNSLR_TokenSpanInfo tokenSpanInfo = PNSLR_GetCurrentTokenSpanInfo(file->contents, i, startOfToken, ignoreSpace);
        i += tokenSpanInfo.iterateFwd;
        startOfToken = tokenSpanInfo.newStartOfToken;

        b8 success = false;
        if (tokenSpanInfo.span.type == PNSLR_TokenType_Invalid)
        {
            if (outTokenSpan) *outTokenSpan = tokenSpanInfo.span;
            success = true;
        }
        else if (i >= file->contents.count)
        {
            if (outTokenSpan) *outTokenSpan = (PNSLR_TokenSpan) {.start = file->contents.count - 1, .end = file->contents.count, .type = PNSLR_TokenType_Invalid};
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

static PNSLR_TokenSpanInfo PNSLR_GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace)
{
    PNSLR_TokenSpanInfo retOut = {0};

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

    if (PNSLR_IsSpace(r))
    {
        if (!ignoreSpace)
        {
            // ignore carriage return
            if (r == '\r')
            {
                retOut.span.type       = PNSLR_TokenType_Invalid;
                retOut.iterateFwd      = width;
                retOut.newStartOfToken = startOfToken + width;
                return retOut;
            }

            // send new line or tab as individuala tokens
            if (r == '\n' || r == '\t')
            {
                if (r == '\n')      { retOut.span.type = PNSLR_TokenType_NewLine; }
                else if (r == '\t') { retOut.span.type = PNSLR_TokenType_Tab;     }
                else                { printf("unimplemented"); FORCE_DBG_TRAP;    }

                retOut.span.start      = startOfToken;
                retOut.span.end        = i + width;
                retOut.iterateFwd      = width;
                retOut.newStartOfToken = i + width;
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
                        retOut.span.type       = PNSLR_TokenType_Spaces;
                        retOut.span.start      = startOfToken;
                        retOut.span.end        = j;
                        retOut.iterateFwd      = j - i;
                        retOut.newStartOfToken = j;
                        return retOut;
                    }
                }

                // reached end of file but space didn't finish
                retOut.span.type       = PNSLR_TokenType_Spaces;
                retOut.span.start      = startOfToken;
                retOut.span.end        = fileSize;
                retOut.iterateFwd      = fileSize - i;
                retOut.newStartOfToken = fileSize;
                return retOut;
            }
        }

        retOut.span.type       = PNSLR_TokenType_Invalid;
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
            if (r2 == '\n')
            {
                retOut.span.type       = PNSLR_TokenType_Comment;
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + w2;
                retOut.iterateFwd      = j + w2 - i;
                retOut.newStartOfToken = j + w2;
                return retOut;
            }
        }

        // reached the end of file but comment didn't finish
        retOut.span.type       = PNSLR_TokenType_Comment;
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
                retOut.span.type       = PNSLR_TokenType_Comment;
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + w2 + PNSLR_GetRuneLength('/');
                retOut.iterateFwd      = j + w2 + PNSLR_GetRuneLength('/') - i;
                retOut.newStartOfToken = j + w2 + PNSLR_GetRuneLength('/');
                return retOut;
            }
        }

        // reached end of file but comment didn't finish
        retOut.span.type       = PNSLR_TokenType_IncompleteComment;
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
                retOut.span.type       = PNSLR_TokenType_String;
                retOut.span.start      = startOfToken;
                retOut.span.end        = j + w2;
                retOut.iterateFwd      = j + w2 - i;
                retOut.newStartOfToken = j + w2;
                return retOut;
            }

            prevRune = r2;
        }

        // reaached end of file but string didn't finish
        retOut.span.type        = PNSLR_TokenType_IncompleteString;
        retOut.span.start       = startOfToken;
        retOut.span.end         = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (PNSLR_IsSymbol(r))
    {
        if (r == '-')
        {
            u32 nextRune = 0;
            { i32 _unused; LEXER_DECODE_RUNE(nextRune, _unused, fileContents, (i + width)); _unused -= _unused; }
            if (nextRune < '0' || nextRune > '9')
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
                            retOut.span.type       = PNSLR_TokenType_Float;
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
                        retOut.span.type       = hasDecimalPoint ? PNSLR_TokenType_Float : PNSLR_TokenType_Integer;
                        retOut.span.start      = startOfToken;
                        retOut.span.end        = j;
                        retOut.iterateFwd      = (j - i);
                        retOut.newStartOfToken = j;
                        return retOut;
                    }
                }

                // reached end of file but number didn't finish
                retOut.span.type       = hasDecimalPoint ? PNSLR_TokenType_Float : PNSLR_TokenType_Integer;
                retOut.span.start      = startOfToken;
                retOut.span.end        = fileSize;
                retOut.iterateFwd      = (fileSize - i);
                retOut.newStartOfToken = fileSize;
                return retOut;
            }
        }

        switch (r)
        {
            case '+': retOut.span.type = PNSLR_TokenType_PlusSymbol;     break;
            case '-': retOut.span.type = PNSLR_TokenType_MinusSymbol;    break;
            case '*': retOut.span.type = PNSLR_TokenType_AsteriskSymbol; break;
            case '$': retOut.span.type = PNSLR_TokenType_DollarSymbol;   break;
            case '~': retOut.span.type = PNSLR_TokenType_TildeSymbol;    break;
            case ':': retOut.span.type = PNSLR_TokenType_ColonSymbol;    break;
            case ',': retOut.span.type = PNSLR_TokenType_CommaSymbol;    break;
            case '.': retOut.span.type = PNSLR_TokenType_DotSymbol;      break;
            default:  retOut.span.type = PNSLR_TokenType_Invalid;        break;
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
                    retOut.span.type       = PNSLR_TokenType_Float;
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
                    retOut.span.type      = PNSLR_TokenType_HexNumber;
                    retOut.span.start      = startOfToken;
                    retOut.span.end        = j;
                    retOut.iterateFwd      = (j - i);
                    retOut.newStartOfToken = j;
                    return retOut;
                }

                if (hasDecimalPt) // multiple decimal pts ??
                {
                    retOut.span.type       = PNSLR_TokenType_Float;
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
                    retOut.span.type   = PNSLR_TokenType_HexNumber;
                }
                else if (hasDecimalPt)
                {
                    retOut.span.type   = PNSLR_TokenType_Float;
                }
                else
                {
                    retOut.span.type   = PNSLR_TokenType_Integer;
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
            retOut.span.type   = PNSLR_TokenType_HexNumber;
        }
        else if (hasDecimalPt)
        {
            retOut.span.type   = PNSLR_TokenType_Float;
        }
        else
        {
            retOut.span.type   = PNSLR_TokenType_Integer;
        }

        retOut.span.start      = startOfToken;
        retOut.span.end        = fileSize;
        retOut.iterateFwd      = fileSize - i;
        retOut.newStartOfToken = fileSize;
        return retOut;
    }

    if (!isLastRune && (PNSLR_IsSymbol(nextRune) || PNSLR_IsSpace(nextRune)))
    {
        utf8str currentSpanStr = (utf8str) {.data = fileContents.data + startOfToken, .count = (i64) (i + width - startOfToken)};

        if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("true"), PNSLR_StringComparisonType_CaseSensitive))
        {
            retOut.span.type = PNSLR_TokenType_BooleanTrue;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("false"), PNSLR_StringComparisonType_CaseSensitive))
        {
            retOut.span.type = PNSLR_TokenType_BooleanFalse;
        }
        else if (PNSLR_IsValidHexNumber(currentSpanStr))
        {
            retOut.span.type = PNSLR_TokenType_IdentifierButCouldBeHexNumber;
        }
        else if (PNSLR_AreStringsEqual(currentSpanStr, PNSLR_STRING_LITERAL("_"), PNSLR_StringComparisonType_CaseSensitive))
        {
            retOut.span.type = PNSLR_TokenType_UnderscoreSymbol;
        }
        else
        {
            retOut.span.type = PNSLR_TokenType_Identifier;
        }

        retOut.span.start      = startOfToken;
        retOut.span.end        = i + width;
        retOut.iterateFwd      = width;
        retOut.newStartOfToken = i + width;
        return retOut;
    }

    retOut.span.type       = PNSLR_TokenType_Invalid;
    retOut.iterateFwd      = width;
    retOut.newStartOfToken = startOfToken;
    return retOut;
}

#undef LEXER_DECODE_RUNE
