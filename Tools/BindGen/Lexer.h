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
} FileIterInfo;

ENUM_START(TokenType, u8)
    #define TokenType_Invalid                       ((TokenType)  0)
    #define TokenType_Identifier                    ((TokenType)  1)
    #define TokenType_IdentifierButCouldBeHexNumber ((TokenType)  2)
    #define TokenType_Integer                       ((TokenType)  3)
    #define TokenType_Float                         ((TokenType)  4)
    #define TokenType_HexNumber                     ((TokenType)  5)
    #define TokenType_String                        ((TokenType)  6)
    #define TokenType_BooleanTrue                   ((TokenType)  7)
    #define TokenType_BooleanFalse                  ((TokenType)  8)
    #define TokenType_IncompleteString              ((TokenType)  9)
    #define TokenType_PlusSymbol                    ((TokenType) 10)
    #define TokenType_MinusSymbol                   ((TokenType) 11)
    #define TokenType_AsteriskSymbol                ((TokenType) 12)
    #define TokenType_DollarSymbol                  ((TokenType) 13)
    #define TokenType_TildeSymbol                   ((TokenType) 14)
    #define TokenType_ColonSymbol                   ((TokenType) 15)
    #define TokenType_CommaSymbol                   ((TokenType) 16)
    #define TokenType_DotSymbol                     ((TokenType) 17)
    #define TokenType_UnderscoreSymbol              ((TokenType) 18)
    #define TokenType_UnknownSymbol                 ((TokenType) 19)
    #define TokenType_Spaces                        ((TokenType) 20)
    #define TokenType_NewLine                       ((TokenType) 21)
    #define TokenType_Tab                           ((TokenType) 22)
    #define TokenType_Comment                       ((TokenType) 23)
    #define TokenType_IncompleteComment             ((TokenType) 24)
    #define TokenType_SkipReflectStart              ((TokenType) 25)
    #define TokenType_SkipReflectEnd                ((TokenType) 26)
ENUM_END

typedef struct
{
    TokenType type;
    i32       start;
    i32       end;
} TokenSpan;

typedef struct
{
    TokenSpan span;
    i32       iterateFwd;
    i32       newStartOfToken;
} TokenSpanInfo;

b8 DequeueNextLineSpan(FileIterInfo* file, b8 ignoreSpace, i32* outLineStart, i32* outLineEnd);
b8 DequeueNextTokenSpan(FileIterInfo* file, b8 ignoreSpace, TokenSpan* outTokenSpan);
b8 PeekNextToken(FileIterInfo* file, b8 ignoreSpace, utf8str* outToken);
b8 PeekNextTokenSpan(FileIterInfo* file, b8 ignoreSpace, TokenSpan* outTokenSpan);
b8 IterateNextTokenSpan(FileIterInfo* file, b8 moveFwd, b8 ignoreSpace, TokenSpan* outTokenSpan);

#endif
