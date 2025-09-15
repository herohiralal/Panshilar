#ifndef PNSLR_LEXER_H
#define PNSLR_LEXER_H
#include "../../Source/Panshilar.h"
#include "TokenMatch.h"

typedef struct
{
    ArraySlice(u8) contents;
    i32            i;
    i32            startOfToken;
} FileIterInfo;

ENUM_START(TknTy, u64)
    #define TknTy_Invalid                       ((TknTy) (         0))
    #define TknTy_Unused0______________________ ((TknTy) (1ULL <<  0))
    #define TknTy_Identifier                    ((TknTy) (1ULL <<  1))
    #define TknTy_IdentifierButCouldBeHexNumber ((TknTy) (1ULL <<  2))
    #define TknTy_Integer                       ((TknTy) (1ULL <<  3))
    #define TknTy_Float                         ((TknTy) (1ULL <<  4))
    #define TknTy_HexNumber                     ((TknTy) (1ULL <<  5))
    #define TknTy_String                        ((TknTy) (1ULL <<  6))
    #define TknTy_BooleanTrue                   ((TknTy) (1ULL <<  7))
    #define TknTy_BooleanFalse                  ((TknTy) (1ULL <<  8))
    #define TknTy_IncompleteString              ((TknTy) (1ULL <<  9))
    #define TknTy_SymbolPlus                    ((TknTy) (1ULL << 10))
    #define TknTy_SymbolMinus                   ((TknTy) (1ULL << 11))
    #define TknTy_SymbolAsterisk                ((TknTy) (1ULL << 12))
    #define TknTy_SymbolDollar                  ((TknTy) (1ULL << 13))
    #define TknTy_SymbolHash                    ((TknTy) (1ULL << 14))
    #define TknTy_SymbolTilde                   ((TknTy) (1ULL << 15))
    #define TknTy_SymbolColon                   ((TknTy) (1ULL << 16))
    #define TknTy_SymbolSemicolon               ((TknTy) (1ULL << 17))
    #define TknTy_SymbolComma                   ((TknTy) (1ULL << 18))
    #define TknTy_SymbolDot                     ((TknTy) (1ULL << 19))
    #define TknTy_SymbolUnderscore              ((TknTy) (1ULL << 20))
    #define TknTy_SymbolQuestion                ((TknTy) (1ULL << 21))
    #define TknTy_SymbolExclamation             ((TknTy) (1ULL << 22))
    #define TknTy_SymbolParenthesesOpen         ((TknTy) (1ULL << 23))
    #define TknTy_SymbolParenthesesClose        ((TknTy) (1ULL << 24))
    #define TknTy_SymbolBracesOpen              ((TknTy) (1ULL << 25))
    #define TknTy_SymbolBracesClose             ((TknTy) (1ULL << 26))
    #define TknTy_SymbolBracketOpen             ((TknTy) (1ULL << 27))
    #define TknTy_SymbolBracketClose            ((TknTy) (1ULL << 28))
    #define TknTy_SymbolLesserThan              ((TknTy) (1ULL << 29))
    #define TknTy_SymbolGreaterThan             ((TknTy) (1ULL << 30))
    #define TknTy_SymbolLeftShift               ((TknTy) (1ULL << 31))
    #define TknTy_SymbolRightShift              ((TknTy) (1ULL << 32))
    #define TknTy_SymbolUnknown                 ((TknTy) (1ULL << 33))
    #define TknTy_Spaces                        ((TknTy) (1ULL << 34))
    #define TknTy_NewLine                       ((TknTy) (1ULL << 35))
    #define TknTy_Tab                           ((TknTy) (1ULL << 36))
    #define TknTy_LineEndComment                ((TknTy) (1ULL << 37))
    #define TknTy_BlockComment                  ((TknTy) (1ULL << 38))
    #define TknTy_IncompleteBlockComment        ((TknTy) (1ULL << 39))
    #define TknTy_PreprocessorDefine            ((TknTy) (1ULL << 40))
    #define TknTy_PreprocessorIfdef             ((TknTy) (1ULL << 41))
    #define TknTy_PreprocessorIfndef            ((TknTy) (1ULL << 42))
    #define TknTy_PreprocessorIf                ((TknTy) (1ULL << 43))
    #define TknTy_PreprocessorEndif             ((TknTy) (1ULL << 44))
    #define TknTy_PreprocessorInclude           ((TknTy) (1ULL << 45))
    #define TknTy_MetaExternCBegin              ((TknTy) (1ULL << 46))
    #define TknTy_MetaExternCEnd                ((TknTy) (1ULL << 47))
    #define TknTy_MetaSkipReflectBegin          ((TknTy) (1ULL << 48))
    #define TknTy_MetaSkipReflectEnd            ((TknTy) (1ULL << 49))
    #define TknTy_EOF                           ((TknTy) (1ULL << 63))
ENUM_END

typedef struct
{
    TknTy type;
    i32   start;
    i32   end;
} TokenSpan;

typedef struct
{
    TokenSpan span;
    i32       iterateFwd;
    i32       newStartOfToken;
} TokenSpanInfo;

ENUM_FLAGS_START(TokenIgnoreMask, u8)
    #define TokenIgnoreMask_None         ((TokenIgnoreMask) (     0))
    #define TokenIgnoreMask_Spaces       ((TokenIgnoreMask) (1ULL << 0))
    #define TokenIgnoreMask_NewLine      ((TokenIgnoreMask) (1ULL << 1))
    #define TokenIgnoreMask_Comments     ((TokenIgnoreMask) (1ULL << 2))
ENUM_END

utf8str GetTokenTypeString(TknTy type);
utf8str GetTokenTypeMaskString(TknTy type, utf8str joiner, PNSLR_Allocator allocator);
b8 DequeueNextLineSpan(FileIterInfo* file, i32* outLineStart, i32* outLineEnd);
b8 DequeueNextTokenSpan(FileIterInfo* file, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);
b8 PeekNextToken(FileIterInfo* file, TokenIgnoreMask ignoreMask, utf8str* outToken);
b8 PeekNextTokenSpan(FileIterInfo* file, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);
b8 IterateNextTokenSpan(FileIterInfo* file, b8 moveFwd, TokenIgnoreMask ignoreMask, TokenSpan* outTokenSpan);

#endif
