#ifndef PNSLR_LEXER_H
#define PNSLR_LEXER_H
#include "Panshilar.h"
#include "TokenMatch.h"

typedef struct
{
    ArraySlice(u8) contents;
    i32            i;
    i32            startOfToken;
} FileIterInfo;

ENUM_START(TokenType, u64)
    #define TokenType_Invalid                       ((TokenType) (         0))
    #define TokenType_Unused0______________________ ((TokenType) (1ULL <<  0))
    #define TokenType_Identifier                    ((TokenType) (1ULL <<  1))
    #define TokenType_IdentifierButCouldBeHexNumber ((TokenType) (1ULL <<  2))
    #define TokenType_Integer                       ((TokenType) (1ULL <<  3))
    #define TokenType_Float                         ((TokenType) (1ULL <<  4))
    #define TokenType_HexNumber                     ((TokenType) (1ULL <<  5))
    #define TokenType_String                        ((TokenType) (1ULL <<  6))
    #define TokenType_BooleanTrue                   ((TokenType) (1ULL <<  7))
    #define TokenType_BooleanFalse                  ((TokenType) (1ULL <<  8))
    #define TokenType_IncompleteString              ((TokenType) (1ULL <<  9))
    #define TokenType_SymbolPlus                    ((TokenType) (1ULL << 10))
    #define TokenType_SymbolMinus                   ((TokenType) (1ULL << 11))
    #define TokenType_SymbolAsterisk                ((TokenType) (1ULL << 12))
    #define TokenType_SymbolDollar                  ((TokenType) (1ULL << 13))
    #define TokenType_SymbolHash                    ((TokenType) (1ULL << 14))
    #define TokenType_SymbolTilde                   ((TokenType) (1ULL << 15))
    #define TokenType_SymbolColon                   ((TokenType) (1ULL << 16))
    #define TokenType_SymbolSemicolon               ((TokenType) (1ULL << 17))
    #define TokenType_SymbolComma                   ((TokenType) (1ULL << 18))
    #define TokenType_SymbolDot                     ((TokenType) (1ULL << 19))
    #define TokenType_SymbolUnderscore              ((TokenType) (1ULL << 20))
    #define TokenType_SymbolQuestion                ((TokenType) (1ULL << 21))
    #define TokenType_SymbolExclamation             ((TokenType) (1ULL << 22))
    #define TokenType_SymbolParenthesesOpen         ((TokenType) (1ULL << 23))
    #define TokenType_SymbolParenthesesClose        ((TokenType) (1ULL << 24))
    #define TokenType_SymbolBracesOpen              ((TokenType) (1ULL << 25))
    #define TokenType_SymbolBracesClose             ((TokenType) (1ULL << 26))
    #define TokenType_SymbolBracketOpen             ((TokenType) (1ULL << 27))
    #define TokenType_SymbolBracketClose            ((TokenType) (1ULL << 28))
    #define TokenType_SymbolLesserThan              ((TokenType) (1ULL << 29))
    #define TokenType_SymbolGreaterThan             ((TokenType) (1ULL << 30))
    #define TokenType_SymbolLeftShift               ((TokenType) (1ULL << 31))
    #define TokenType_SymbolRightShift              ((TokenType) (1ULL << 32))
    #define TokenType_SymbolUnknown                 ((TokenType) (1ULL << 33))
    #define TokenType_Spaces                        ((TokenType) (1ULL << 34))
    #define TokenType_NewLine                       ((TokenType) (1ULL << 35))
    #define TokenType_Tab                           ((TokenType) (1ULL << 36))
    #define TokenType_LineEndComment                ((TokenType) (1ULL << 37))
    #define TokenType_BlockComment                  ((TokenType) (1ULL << 38))
    #define TokenType_IncompleteBlockComment        ((TokenType) (1ULL << 39))
    #define TokenType_PreprocessorDefine            ((TokenType) (1ULL << 40))
    #define TokenType_PreprocessorIfdef             ((TokenType) (1ULL << 41))
    #define TokenType_PreprocessorIfndef            ((TokenType) (1ULL << 42))
    #define TokenType_PreprocessorIf                ((TokenType) (1ULL << 43))
    #define TokenType_PreprocessorEndif             ((TokenType) (1ULL << 44))
    #define TokenType_PreprocessorInclude           ((TokenType) (1ULL << 45))
    #define TokenType_MetaExternCBegin              ((TokenType) (1ULL << 46))
    #define TokenType_MetaExternCEnd                ((TokenType) (1ULL << 47))
    #define TokenType_MetaSkipReflectBegin          ((TokenType) (1ULL << 48))
    #define TokenType_MetaSkipReflectEnd            ((TokenType) (1ULL << 49))
    #define TokenType_EOF                           ((TokenType) (1ULL << 63))
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

ENUM_FLAGS_START(TokenIgnoreMask, u8)
    #define TokenIgnoreMask_None         ((TokenIgnoreMask) (     0))
    #define TokenIgnoreMask_Spaces       ((TokenIgnoreMask) (1 << 0))
    #define TokenIgnoreMask_NewLine      ((TokenIgnoreMask) (1 << 1))
    #define TokenIgnoreMask_Comments     ((TokenIgnoreMask) (1 << 2))
ENUM_END

utf8str GetTokenTypeString(TokenType type);
utf8str GetTokenTypeMaskString(TokenType type, utf8str joiner, PNSLR_Allocator allocator);
b8 DequeueNextLineSpan(FileIterInfo* file, i32* outLineStart, i32* outLineEnd);
b8 DequeueNextTokenSpan(FileIterInfo* file, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);
b8 PeekNextToken(FileIterInfo* file, TokenIgnoreMask ignoreMask, utf8str* outToken);
b8 PeekNextTokenSpan(FileIterInfo* file, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);
b8 IterateNextTokenSpan(FileIterInfo* file, b8 moveFwd, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);

#endif
