#ifndef PNSLR_LEXER_H
#define PNSLR_LEXER_H
#include "Panshilar.h"
#include "TokenMatch.h"

typedef struct
{
    utf8str        pathRel;
    ArraySlice(u8) contents;
    i32            i;
    i32            startOfToken;
} PNSLR_FileIterInfo;

ENUM_START(PNSLR_TokenType, u8)
    #define PNSLR_TokenType_Invalid                       ((PNSLR_TokenType)  0)
    #define PNSLR_TokenType_Identifier                    ((PNSLR_TokenType)  1)
    #define PNSLR_TokenType_IdentifierButCouldBeHexNumber ((PNSLR_TokenType)  2)
    #define PNSLR_TokenType_Integer                       ((PNSLR_TokenType)  3)
    #define PNSLR_TokenType_Float                         ((PNSLR_TokenType)  4)
    #define PNSLR_TokenType_HexNumber                     ((PNSLR_TokenType)  5)
    #define PNSLR_TokenType_String                        ((PNSLR_TokenType)  6)
    #define PNSLR_TokenType_BooleanTrue                   ((PNSLR_TokenType)  7)
    #define PNSLR_TokenType_BooleanFalse                  ((PNSLR_TokenType)  8)
    #define PNSLR_TokenType_IncompleteString              ((PNSLR_TokenType)  9)
    #define PNSLR_TokenType_PlusSymbol                    ((PNSLR_TokenType) 10)
    #define PNSLR_TokenType_MinusSymbol                   ((PNSLR_TokenType) 11)
    #define PNSLR_TokenType_AsteriskSymbol                ((PNSLR_TokenType) 12)
    #define PNSLR_TokenType_DollarSymbol                  ((PNSLR_TokenType) 13)
    #define PNSLR_TokenType_TildeSymbol                   ((PNSLR_TokenType) 14)
    #define PNSLR_TokenType_ColonSymbol                   ((PNSLR_TokenType) 15)
    #define PNSLR_TokenType_CommaSymbol                   ((PNSLR_TokenType) 16)
    #define PNSLR_TokenType_DotSymbol                     ((PNSLR_TokenType) 17)
    #define PNSLR_TokenType_UnderscoreSymbol              ((PNSLR_TokenType) 18)
    #define PNSLR_TokenType_UnknownSymbol                 ((PNSLR_TokenType) 19)
    #define PNSLR_TokenType_Spaces                        ((PNSLR_TokenType) 20)
    #define PNSLR_TokenType_NewLine                       ((PNSLR_TokenType) 21)
    #define PNSLR_TokenType_Tab                           ((PNSLR_TokenType) 22)
    #define PNSLR_TokenType_Comment                       ((PNSLR_TokenType) 23)
    #define PNSLR_TokenType_IncompleteComment             ((PNSLR_TokenType) 24)
    #define PNSLR_TokenType_SkipReflectStart              ((PNSLR_TokenType) 25)
    #define PNSLR_TokenType_SkipReflectEnd                ((PNSLR_TokenType) 26)
ENUM_END

typedef struct
{
    PNSLR_TokenType type;
    i32             start;
    i32             end;
} PNSLR_TokenSpan;

typedef struct
{
    PNSLR_TokenSpan span;
    i32             iterateFwd;
    i32             newStartOfToken;
} PNSLR_TokenSpanInfo;

PNSLR_TokenSpanInfo PNSLR_GetCurrentTokenSpanInfo(ArraySlice(u8) fileContents, i32 i, i32 startOfToken, b8 ignoreSpace);

#endif
