#include "Lexer.h"

#define LEXER_DECODE_RUNE(runeVarName, widthVarName, contents, contentsStart) \
    do \
    { \
        PNSLR_DecodedRune tempXXX##__LINE__ = PNSLR_DecodeRune((ArraySlice(u8)){.count = contents.count - (i64)(contentsStart), .data = contents.data + contentsStart}); \
        runeVarName = tempXXX##__LINE__.rune; \
        widthVarName = tempXXX##__LINE__.length; \
    } while (false); \

PNSLR_TokenSpanInfo PNSLR_GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace)
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
        // TODO
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
        // TODO
    }

    if (r >= '0' && r <= '9' && i == startOfToken)
    {
        // TODO
    }

    if (!isLastRune && (PNSLR_IsSymbol(nextRune) || PNSLR_IsSpace(nextRune)))
    {
        // TODO
    }

    retOut.span.type       = PNSLR_TokenType_Invalid;
    retOut.iterateFwd      = width;
    retOut.newStartOfToken = startOfToken;
    return retOut;
}

#undef LEXER_DECODE_RUNE
