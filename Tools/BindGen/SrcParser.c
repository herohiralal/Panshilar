#include "SrcParser.h"
#include "Lexer.h"

void InitialiseTypeTable(ParsedContent* content, PNSLR_Allocator allocator)
{
    ArraySlice(DeclTypeInfo) tt = PNSLR_MakeSlice(DeclTypeInfo, 512, false, allocator, nil);
    if (!tt.data || !tt.count) FORCE_DBG_TRAP;

    i64 cnt = 0;

    #define DECLARE_TYPE_TABLE_ENTRY(xxxx) \
        i64 xxxx##Idx = cnt; \
        /*regular*/ tt.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_None,  .u.name = PNSLR_STRING_LITERAL(#xxxx)}; \
        /*ptr2reg*/ tt.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Ptr,   .u.polyTgtIdx = xxxx##Idx}; \
        /*slice*/   tt.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Slice, .u.polyTgtIdx = xxxx##Idx}; \
        /*ptr2arr*/ tt.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Ptr,   .u.polyTgtIdx = (xxxx##Idx + 2)};

    DECLARE_TYPE_TABLE_ENTRY(void   ); // void must be the 0th type
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

    if (content)
    {
        content->types      = tt;
        content->typesCount = cnt;
    }
}

void AddNewType(ParsedContent* content, utf8str name)
{
    if (!content) return;

    i64 cnt = content->typesCount;
    if (cnt >= content->types.count) { printf("buffer size not large enough for type table"); FORCE_DBG_TRAP; return; }

    i64 idx = cnt;
    content->types.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_None, .u.name       = name};
    content->types.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Ptr,  .u.polyTgtIdx = idx };

    content->typesCount = cnt;
}

void AddNewArrayType(ParsedContent* content, u32 baseTyIdx)
{
    if (!content) return;

    i64 cnt = content->typesCount;
    if (cnt >= content->types.count) { printf("buffer size not large enough for type table"); FORCE_DBG_TRAP; return; }

    i64 idx = cnt;
    content->types.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Slice, .u.polyTgtIdx = (i64) baseTyIdx};
    content->types.data[cnt++] = (DeclTypeInfo) {.polyTy = PolymorphicDeclType_Ptr,   .u.polyTgtIdx = idx            };

    content->typesCount = cnt;
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

// has an awkward signature, because if the user already 'got' the token and needs to now check if it is a type, then they can pass the string
// as `tokenStr` and the function will evaluate if that string has any meaning; if passed with zero-values, then an identifier token will be
// dequeued from the source, and used as if that was passed into the function as `tokenStr`
b8 ProcessIdentifierAsTypeName(ParsedContent* parsedContent, utf8str pathRel, FileIterInfo* iter, utf8str tokenStr, u32* typeIdx,PNSLR_Allocator allocator)
{
    if (typeIdx) *typeIdx = U32_MAX;

    // if the user didn't pass an actual string input, get the next identifier token from the source
    if (!tokenStr.data || !tokenStr.count)
    {
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, TokenType_Identifier, &tokenStr, allocator))
        {
            return false;
        }
    }

    u32 typeIdxTemp = U32_MAX;
    if (PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("ArraySlice"), 0)) // is an array slice
    {
        b8 success = ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesOpen, nil, allocator)
                  && ProcessIdentifierAsTypeName(parsedContent, pathRel, iter, (utf8str) {0}, &typeIdxTemp, allocator)
                  && ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesClose, nil, allocator);

        if (!success) return false;
    }
    else // is not an array slice
    {
        if (PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("struct"), 0))
        {
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &tokenStr, allocator)) return false;
        }

        b8 success = false;
        for (i64 i = 0; i < parsedContent->typesCount; i++)
        {
            DeclTypeInfo* typeEntry = &(parsedContent->types.data[i]);
            if (typeEntry->polyTy != PolymorphicDeclType_None) continue; // only care about non-polymorphic types at this stage; pointers get resolved later

            if (PNSLR_AreStringsEqual(typeEntry->u.name, tokenStr, 0)) // if name matches
            {
                typeIdxTemp = (u32) i;
                success = true;
                break;
            }
        }

        if (!success) return false;
    }

    // check if type is a ptr
    b8 isPtr = false;
    {
        // next token is an asterisk
        utf8str nextToken = {0};
        if (PeekNextToken(iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, &nextToken)
            && nextToken.count == 1 && nextToken.data[0] == '*')
        {
            isPtr = true;
            DequeueNextTokenSpan(iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, nil);
        }
    }

    if (isPtr) typeIdxTemp++; // all ptr-types are always +1 of base type
    if (typeIdx) *typeIdx = typeIdxTemp;
    return true;
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

b8 ConsumeEnumDeclBlock(ParsedContent* content, CachedLasts* cachedLasts, utf8str pathRel, FileIterInfo* iter, utf8str* doc, b8 isFlags, PNSLR_Allocator allocator)
{
    i32 enumStart = iter->startOfToken - 1, enumEnd = iter->i;

    ParsedEnum* enm = PNSLR_New(ParsedEnum, allocator, nil);
    if (!enm) FORCE_DBG_TRAP;

    enm->header.type = DeclType_Enum;
    enm->header.doc  = *doc;
    *doc = (utf8str) {0};

    if (cachedLasts->lastDecl) cachedLasts->lastDecl->next         = &(enm->header);
    else                       cachedLasts->lastFile->declarations = &(enm->header);
    cachedLasts->lastDecl                                          = &(enm->header);

    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &(enm->header.name), allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolComma, nil, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
    utf8str enumBacking = {0};
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &enumBacking, allocator)) return false;

    if (false) { }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("u8"),  0)) { enm->size =  8; enm->negative = false; }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("u16"), 0)) { enm->size = 16; enm->negative = false; }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("u32"), 0)) { enm->size = 32; enm->negative = false; }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("u64"), 0)) { enm->size = 64; enm->negative = false; }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("i8"),  0)) { enm->size =  8; enm->negative = true;  }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("i16"), 0)) { enm->size = 16; enm->negative = true;  }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("i32"), 0)) { enm->size = 32; enm->negative = true;  }
    else if (PNSLR_AreStringsEqual(enumBacking, PNSLR_STRING_LITERAL("i64"), 0)) { enm->size = 64; enm->negative = true;  }
    else
    {
        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Invalid enum backing type."));
        return false;
    }

    if (isFlags && enm->negative)
    {
        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Negative flags enums are not allowed."));
        return false;
    }

    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_NewLine, nil, allocator)) return false;

    AddNewType(content, enm->header.name);

    while (iter->i < iter->contents.count)
    {
        utf8str tokenStr;
        TokenType rec = ForceGetNextToken(pathRel, iter,
            TokenIgnoreMask_None,
            TokenType_Spaces |
            TokenType_Identifier |
            TokenType_Invalid,
            &tokenStr,
            allocator
        );
        if (!rec) return false;

        if (rec == TokenType_Identifier && PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("ENUM_END"), 0)) // end
        {
            return true;
        }

        if (rec == TokenType_Spaces) // assumedly a variant
        {
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_PreprocessorDefine, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;

            ParsedEnumVariant* var = PNSLR_New(ParsedEnumVariant, allocator, nil);
            if (!var) FORCE_DBG_TRAP;

            if (cachedLasts->lastVariant) cachedLasts->lastVariant->next = var;
            else                          enm->variants                  = var;
            cachedLasts->lastVariant                                     = var;

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &(var->name), allocator)) return false;
            if (!PNSLR_StringStartsWith(var->name, enm->header.name, 0))
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Enum variant name does not start with enum name as prefix."));
                return false;
            }

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
            utf8str nameMatch = {0};
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier, &nameMatch, allocator)) return false;
            if (!PNSLR_AreStringsEqual(nameMatch, enm->header.name, 0))
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Enum variant cast does not match enum name."));
                return false;
            }
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesClose, nil, allocator)) return false;

            if (isFlags) // flags
            {
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
                utf8str mustBe1 = {0};
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Integer, &mustBe1, allocator)) return false;
                if (mustBe1.count != 1 || !mustBe1.data)
                {
                    PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Expected '1' or '0' for flags enum variant."));
                    return false;
                }

                if (mustBe1.data[0] == '0')
                {
                    var->idx = 0;
                    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
                    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
                    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments, TokenType_NewLine, nil, allocator)) return false;

                    continue;
                }
                else if (mustBe1.data[0] == '1')
                {
                    // continue as usual
                }
                else
                {
                    PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Expected '1' or '0' for flags enum variant."));
                    return false;
                }


                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolLeftShift, nil, allocator)) return false;
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Spaces, nil, allocator)) return false;
            }

            utf8str idxToken = {0};
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Integer, &idxToken, allocator)) return false;
            var->negative = (idxToken.count >= 1 && idxToken.data && idxToken.data[0] == '-');
            if (var->negative) idxToken = (utf8str) {.data = idxToken.data + 1, .count = idxToken.count - 1}; // trim '-' sign
            var->idx = (u64) strtoull(PNSLR_CStringFromString(idxToken, allocator), nil, 10); // TODO: replace with pnslr fn once implemented
            if (isFlags) var->idx = (1ULL << var->idx);

            if (isFlags) // flags
            {
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
            }

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments, TokenType_NewLine, nil, allocator)) return false;

            continue;
        }

        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("unexpected token"));
        return false;
    }

    PrintParseError(pathRel, iter->contents, enumStart, enumEnd, PNSLR_STRING_LITERAL("Incomplete enum declaration."));
    return false;
}

b8 ConsumeStructDeclBlock(ParsedContent* content, CachedLasts* cachedLasts, utf8str pathRel, FileIterInfo* iter, utf8str* doc, PNSLR_Allocator allocator)
{
    i32 structStart = iter->startOfToken - 1, structEnd = iter->i;

    ParsedStruct* strct = PNSLR_New(ParsedStruct, allocator, nil);
    if (!strct) FORCE_DBG_TRAP;

    strct->header.type = DeclType_Struct;
    strct->header.doc  = *doc;
    *doc = (utf8str) {0};

    if (cachedLasts->lastDecl) cachedLasts->lastDecl->next         = &(strct->header);
    else                       cachedLasts->lastFile->declarations = &(strct->header);
    cachedLasts->lastDecl                                          = &(strct->header);

    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_Identifier, &(strct->header.name), allocator)) return false;
    if (PNSLR_AreStringsEqual(strct->header.name, PNSLR_STRING_LITERAL("alignas"), 0))
    {
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
        utf8str alignasVal = {0};
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_Identifier | TokenType_Integer, &alignasVal, allocator)) return false;
        if (PNSLR_AreStringsEqual(alignasVal, PNSLR_STRING_LITERAL("PNSLR_PTR_SIZE"), 0))
        {
            strct->alignasVal = sizeof(rawptr);
        }
        else
        {
            strct->alignasVal = (i32) strtoull(PNSLR_CStringFromString(alignasVal, allocator), nil, 10); // TODO: replace with pnslr fn once implemented
            if (!strct->alignasVal)
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Invalid alignas value."));
                return false;
            }
        }

        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Identifier, &(strct->header.name), allocator)) return false;
    }

    AddNewType(content, strct->header.name);
    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolBracesOpen, nil, allocator)) return false;

    while (iter->i < iter->contents.count)
    {
        utf8str tokenStr;
        TokenType rec = ForceGetNextToken(pathRel, iter,
            TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces | TokenIgnoreMask_Comments,
            TokenType_Identifier |
            TokenType_SymbolBracesClose |
            TokenType_Invalid,
            &tokenStr,
            allocator
        );
        if (!rec) return false;

        if (rec == TokenType_SymbolBracesClose)
        {
            utf8str nameCheck = {0};
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces | TokenIgnoreMask_Comments, TokenType_Identifier, &nameCheck, allocator)) return false;
            if (!PNSLR_AreStringsEqual(nameCheck, strct->header.name, 0))
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Struct name does not match."));
                return false;
            }

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces | TokenIgnoreMask_Comments, TokenType_SymbolSemicolon, nil, allocator)) return false;

            return true;
        }

        if (rec == TokenType_Identifier)
        {
            ParsedStructMember* mem = PNSLR_New(ParsedStructMember, allocator, nil);
            if (!mem) FORCE_DBG_TRAP;

            mem->arrSize = -1;

            if (cachedLasts->lastMember) cachedLasts->lastMember->next = mem;
            else                         strct->members                = mem;
            cachedLasts->lastMember                                    = mem;

            if (!ProcessIdentifierAsTypeName(content, pathRel, iter, tokenStr, &(mem->ty), allocator))
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("Invalid type name."));
                return false;
            }

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Identifier, &(mem->name), allocator)) return false;
            TokenType tt = ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolBracketOpen | TokenType_SymbolSemicolon, nil, allocator);
            if (!tt) return false;

            if (tt == TokenType_SymbolBracketOpen)
            {
                utf8str arrCount = {0};
                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Integer, &arrCount, allocator)) return false;
                mem->arrSize = (u32) strtoull(PNSLR_CStringFromString(arrCount, allocator), nil, 10); // TODO: replace with pnslr fn once implemented

                TokenType tt2 = ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolBracketClose | TokenType_SymbolAsterisk, nil, allocator);
                if (!tt2) return false;
                if (tt2 == TokenType_SymbolAsterisk)
                {
                    utf8str secondPart = {0};
                    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_Identifier, &secondPart, allocator)) return false;
                    if (!PNSLR_AreStringsEqual(secondPart, PNSLR_STRING_LITERAL("PNSLR_PTR_SIZE"), 0))
                    {
                        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("invalid multiplication syntax type thing"));
                        return false;
                    }

                    mem->arrSize *= sizeof(rawptr);

                    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces, TokenType_SymbolBracketClose, nil, allocator)) return false;
                }

                if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_None, TokenType_SymbolSemicolon, nil, allocator)) return false;
            }
        }
    }

    PrintParseError(pathRel, iter->contents, structStart, structEnd, PNSLR_STRING_LITERAL("Incomplete struct declaration."));
    return false;
}

b8 ConsumeFnDeclBlock(ParsedContent* content, CachedLasts* cachedLasts, utf8str pathRel, FileIterInfo* iter, utf8str* doc, u32 retTy, b8 isDelegate, PNSLR_Allocator allocator)
{
    i32 fnStart = iter->startOfToken - 1, fnEnd = iter->i;

    ParsedFunction* fn = PNSLR_New(ParsedFunction, allocator, nil);
    if (!fn) FORCE_DBG_TRAP;

    fn->header.type = DeclType_Function;
    fn->header.doc  = *doc;
    fn->retTy       = retTy;
    fn->isDelegate  = isDelegate;
    *doc = (utf8str) {0};

    if (cachedLasts->lastDecl) cachedLasts->lastDecl->next         = &(fn->header);
    else                       cachedLasts->lastFile->declarations = &(fn->header);
    cachedLasts->lastDecl                                          = &(fn->header);

    if (isDelegate)
    {
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolAsterisk, nil, allocator)) return false;
    }

    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_Identifier, &(fn->header.name), allocator)) return false;

    if (isDelegate)
    {
        AddNewType(content, fn->header.name);
        if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolParenthesesClose, nil, allocator)) return false;
    }

    if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;

    while (iter->i < iter->contents.count)
    {
        u32 tyIdx = U32_MAX;
        if (!ProcessIdentifierAsTypeName(content, pathRel, iter, (utf8str) {0}, &tyIdx, allocator)) return false;
        if (tyIdx == 0) // is `void`, which means it must not have any real args and the fn must end
        {
            if (fn->args != nil) // there has been an arg before
            {
                PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("there are already actual args, void not expected here."));
                return false;
            }

            // keep going, expect only 'fn finished' after
        }
        else // is an actual arg and not just `void`
        {
            ParsedFnArg* arg = PNSLR_New(ParsedFnArg, allocator, nil);
            if (!arg) FORCE_DBG_TRAP;

            arg->ty = tyIdx;

            if (cachedLasts->lastFnArg) cachedLasts->lastFnArg->next = arg;
            else                        fn->args                     = arg;
            cachedLasts->lastFnArg                                   = arg;

            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_Identifier, &(arg->name), allocator)) break;
        }

        TokenType rec = ForceGetNextToken(pathRel, iter,
            TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments,
            (tyIdx == 0 ? 0 : TokenType_SymbolComma) | TokenType_SymbolParenthesesClose, // if void input, don't accept any more args
            nil,
            allocator);

        if (!rec) return false;

        if (rec == TokenType_SymbolComma) continue; // onto next arg

        if (rec == TokenType_SymbolParenthesesClose)
        {
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Spaces | TokenIgnoreMask_NewLine | TokenIgnoreMask_Comments, TokenType_SymbolSemicolon, nil, allocator)) return false;

            return true;
        }

        FORCE_DBG_TRAP; // shoulddn't reach
    }

    PrintParseError(pathRel, iter->contents, fnStart, fnEnd, PNSLR_STRING_LITERAL("Incomplete fn declaration."));
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
            TokenType_Identifier |
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

        if (rec == TokenType_Identifier && PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("DECLARE_ARRAY_SLICE"), 0)) // slice
        {
            u32 dstType = U32_MAX;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesOpen, nil, allocator)) return false;
            if (!ProcessIdentifierAsTypeName(parsedContent, pathRel, iter, (utf8str){0}, &dstType, allocator)) return false;
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, TokenType_SymbolParenthesesClose, nil, allocator)) return false;

            AddNewArrayType(parsedContent, dstType);
            lastDoc = (utf8str) {0};
            continue;
        }

        if (rec == TokenType_Identifier && PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("ENUM_START"), 0)) // enum
        {
            if (!ConsumeEnumDeclBlock(parsedContent, cachedLasts, pathRel, iter, &lastDoc, false, allocator)) return false;

            continue;
        }

        if (rec == TokenType_Identifier && PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("ENUM_FLAGS_START"), 0)) // enum flags
        {
            if (!ConsumeEnumDeclBlock(parsedContent, cachedLasts, pathRel, iter, &lastDoc, true, allocator)) return false;

            continue;
        }

        if (rec == TokenType_Identifier && PNSLR_AreStringsEqual(tokenStr, PNSLR_STRING_LITERAL("typedef"), 0)) // delegate or struct
        {
            utf8str nextToken = {0};
            if (!ForceGetNextToken(pathRel, iter, TokenIgnoreMask_Comments | TokenIgnoreMask_NewLine | TokenIgnoreMask_Spaces, TokenType_Identifier, &nextToken, allocator))
                return false;

            u32 delRetTyIdx = U32_MAX;
            if (PNSLR_AreStringsEqual(nextToken, PNSLR_STRING_LITERAL("struct"), 0)) // struct
            {
                if (!ConsumeStructDeclBlock(parsedContent, cachedLasts, pathRel, iter, &lastDoc, allocator)) return false;

                continue;
            }
            else if (ProcessIdentifierAsTypeName(parsedContent, pathRel, iter, nextToken, &delRetTyIdx, allocator)) // delegate
            {
                if (!ConsumeFnDeclBlock(parsedContent, cachedLasts, pathRel, iter, &lastDoc, delRetTyIdx, true, allocator)) return false;

                continue;
            }

            PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("unexpected token"));
            return false;
        }

        u32 retTyIdx = U32_MAX;
        if (ProcessIdentifierAsTypeName(parsedContent, pathRel, iter, tokenStr, &retTyIdx, allocator)) // function
        {
            if (!ConsumeFnDeclBlock(parsedContent, cachedLasts, pathRel, iter, &lastDoc, retTyIdx, false, allocator)) return false;

            continue;
        }

        PrintParseError(pathRel, iter->contents, iter->startOfToken - 1, iter->i, PNSLR_STRING_LITERAL("unexpected token"));
        return false;
    }

    // file finished but block didn't end
    PrintParseError(pathRel, iter->contents, externCStart, externCEnd, PNSLR_STRING_LITERAL("extern c block not closed."));
    return false;
}

b8 ProcessFile(ParsedContent *parsedContent, CachedLasts *cachedLasts, utf8str pathRel, ArraySlice(u8) contents, PNSLR_Allocator allocator)
{
    ParsedFileContents* file = PNSLR_New(ParsedFileContents, allocator, nil);
    if (!file) FORCE_DBG_TRAP;

    // gather file name
    {
        i32 lastIdxOfSlash = PNSLR_SearchLastIndexInString(pathRel, PNSLR_STRING_LITERAL("/"), 0);
        i32 lastIdxOfDot   = PNSLR_SearchLastIndexInString(pathRel, PNSLR_STRING_LITERAL("."), 0);
        if (lastIdxOfDot < 0) lastIdxOfDot = (i32) pathRel.count;
        file->name = (utf8str) {.data = pathRel.data + lastIdxOfSlash + 1, .count = (i64) (lastIdxOfDot - lastIdxOfSlash - 1)};
    }

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
        if (ProcessExternCBlock(parsedContent, cachedLasts, pathRel, &iter, allocator)) continue;

        // failed somewhere
        return false;
    }

    return true;
}
