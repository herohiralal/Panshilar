#include "Generator.h"
#include "Lexer.h"

cstring G_GenCxxHeaderPrefix = ""
"// #pragma once is intentionally ignored.\n"
"#ifndef __cplusplus\n"
"    #error \"Please use the C bibndings.\";\n"
"#else\n"
"#ifndef PANSHILAR_MAIN\n"
"#define PANSHILAR_MAIN\n"
"\n"
"namespace Panshilar\n"
"{\n"
"\n"
"    typedef bool                b8;\n"
"    typedef unsigned char       u8;\n"
"    typedef unsigned short int  u16;\n"
"    typedef unsigned int        u32;\n"
"    typedef unsigned long long  u64;\n"
"    typedef signed char         i8;\n"
"    typedef signed short int    i16;\n"
"    typedef signed int          i32;\n"
"    typedef signed long long    i64;\n"
"    template <typename T> struct ArraySlice { i64 count; T* data; };\n"
"    typedef ArraySlice<u8> utf8str;\n"
"\n"
"";

cstring G_GenCxxHeaderSuffix = ""
"\n"
"}//namespace end\n"
"\n"
"#endif//PANSHILAR_MAIN\n"
"\n"
"#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"    static_assert(sizeof(Panshilar::b8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::u8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::u16) == 2, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::u32) == 4, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::u64) == 8, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::i8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::i16) == 2, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::i32) == 4, \"Size mismatch.\");\n"
"    static_assert(sizeof(Panshilar::i64) == 8, \"Size mismatch.\");\n"
"#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"";

cstring G_GenCxxSourcePrefix = ""
"\n"
"#ifndef PNSLR_IMPLEMENTATION\n"
"#define PNSLR_SKIP_IMPLEMENTATION\n"
"#endif\n"
"\n"
"#ifndef PNSLR_SKIP_IMPLEMENTATION\n"
"#define PNSLR_SKIP_IMPLEMENTATION\n"
"\n"
"";

cstring G_GenCxxSourceSuffix = ""
"\n"
"#endif//PNSLR_SKIP_IMPLEMENTATION\n"
"#endif//__cplusplus\n"
"";

#define ARR_FROM_STR(str__) (ArraySlice(u8)){.count = str__.count, .data = str__.data}
#define ARR_FROM_STR_SKIP_PREFIX(str__, prefixSize__) (ArraySlice(u8)){.count = str__.count - (prefixSize__), .data = str__.data + (prefixSize__)}
#define ARR_STR_LIT(str__) (ArraySlice(u8)){.count = sizeof(str__) - 1, .data = (u8*) str__}

void WriteCxxTypeName(PNSLR_File file, ArraySlice(DeclTypeInfo) types, u32 ty)
{
    if (ty >= (u32) types.count) FORCE_DBG_TRAP;

    DeclTypeInfo declTy = types.data[ty];

    switch (declTy.polyTy)
    {
        case PolymorphicDeclType_None:
        {
            b8 skipPrefix = false;
            utf8str nameStr = declTy.u.name;
            if (false) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("void"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("b8"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("b8"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("b32"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("u32"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u8"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("u8"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u16"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("u16"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u32"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("u32"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u64"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("u64"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i8"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("i8"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i16"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("i16"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i32"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("i32"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i64"), nameStr, 0)) {nameStr = PNSLR_STRING_LITERAL("i64"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("f32"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("float"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("f64"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("double"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("utf8ch"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("u8"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("utf16ch"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("u16"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("rune"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("u32"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("cstring"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("char*"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("rawptr"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("void*"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("utf8str"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("utf8str"); }
            else { skipPrefix = true; }

            PNSLR_WriteToFile(file, ARR_FROM_STR_SKIP_PREFIX(nameStr, (skipPrefix ? 6 : 0)));
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            // WILL NOT WORK FOR `cstring` AND `rawptr` WHICH ARE TYPEDEF'D PTR TYPES
            // but should be fine for now because we're not using arrays of them anyway
            PNSLR_WriteToFile(file, ARR_STR_LIT("ArraySlice<"));
            WriteCxxTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            PNSLR_WriteToFile(file, ARR_STR_LIT(">"));
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            WriteCxxTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            PNSLR_WriteToFile(file, ARR_STR_LIT("*"));
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void GenerateCxxBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_Path headerPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_STRING_LITERAL("Panshilar.hpp"), allocator);
    PNSLR_File f = PNSLR_OpenFileToWrite(headerPath, false, false);

    utf8str hPrefixStr = PNSLR_StringFromCString(G_GenCxxHeaderPrefix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(hPrefixStr));

    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (file->declarations != nil)
        {
            PNSLR_WriteToFile(f, ARR_STR_LIT("    // #######################################################################################\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("    // "));
            PNSLR_WriteToFile(f, ARR_FROM_STR(file->name));
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("    // #######################################################################################\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }

        for (DeclHeader* decl = file->declarations; decl != nil; decl = decl->next)
        {
            if (decl->type == DeclType_Array) continue;

            if (decl->doc.count > 0)
            {
                i32 lineStart = -1, lineEnd = -1;
                FileIterInfo docIter = {.contents = ARR_FROM_STR(decl->doc)};
                while (DequeueNextLineSpan(&docIter, &lineStart, &lineEnd))
                {
                    ArraySlice(u8) lineSlice = { .count = lineEnd - lineStart, .data = docIter.contents.data + lineStart };
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                    PNSLR_WriteToFile(f, lineSlice);
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                }
            }

            switch (decl->type)
            {
                case DeclType_Section:
                {
                    ParsedSection* sec = (ParsedSection*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    // "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(sec->header.name));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    for (i64 i = (4 + sec->header.name.count); i < 90; i++) { PNSLR_WriteToFile(f, ARR_STR_LIT("~")); }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    break;
                }
                case DeclType_Enum:
                {
                    ParsedEnum* enm = (ParsedEnum*) decl;
                    utf8str backing = {0};
                    {
                        switch (enm->size)
                        {
                            case  8: backing = enm->negative ? PNSLR_STRING_LITERAL("i8" ) : PNSLR_STRING_LITERAL("u8" ); break;
                            case 16: backing = enm->negative ? PNSLR_STRING_LITERAL("i16") : PNSLR_STRING_LITERAL("u16"); break;
                            case 32: backing = enm->negative ? PNSLR_STRING_LITERAL("i32") : PNSLR_STRING_LITERAL("u32"); break;
                            case 64: backing = enm->negative ? PNSLR_STRING_LITERAL("i64") : PNSLR_STRING_LITERAL("u64"); break;
                            default: FORCE_DBG_TRAP; break;
                        }
                    }

                    PNSLR_WriteToFile(f, ARR_STR_LIT("    enum class "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(enm->header.name, 6));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" : "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                    if (enm->flags) PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as flags */"));
                    else            PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as value */"));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n    {\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("        "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(var->name, (enm->header.name.count + 1)));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" = "));
                        if (var->negative) { PNSLR_WriteToFile(f, ARR_STR_LIT("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", var->idx);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(",\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    };\n"));
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    PNSLR_WriteToFile(f, ARR_STR_LIT("    struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("alignas("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(strct->header.name, 6));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n    {\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("       "));
                        WriteCxxTypeName(f, content->types, member->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(member->name));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(f, ARR_STR_LIT("]"));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    };\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                    if (fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                        WriteCxxTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (*"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCxxTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, ARR_STR_LIT("\n    "));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                        WriteCxxTypeName(f, content->types, arg->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                        if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(","));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\n    "));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(");\n"));
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }
    }

    utf8str hSuffixStr = PNSLR_StringFromCString(G_GenCxxHeaderSuffix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(hSuffixStr));

    utf8str sPrefixStr = PNSLR_StringFromCString(G_GenCxxSourcePrefix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(sPrefixStr));

    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        for (DeclHeader* decl = file->declarations; decl != nil; decl = decl->next)
        {
            switch (decl->type)
            {
                case DeclType_Section:
                {
                    continue;
                }
                case DeclType_Array:
                {
                    // ParsedArrayDecl* arr = (ParsedArrayDecl*) decl;
                    break;
                }
                case DeclType_Enum:
                {
                    ParsedEnum* enm = (ParsedEnum*) decl;
                    utf8str backing = {0};
                    {
                        switch (enm->size)
                        {
                            case  8: backing = enm->negative ? PNSLR_STRING_LITERAL("i8" ) : PNSLR_STRING_LITERAL("u8" ); break;
                            case 16: backing = enm->negative ? PNSLR_STRING_LITERAL("i16") : PNSLR_STRING_LITERAL("u16"); break;
                            case 32: backing = enm->negative ? PNSLR_STRING_LITERAL("i32") : PNSLR_STRING_LITERAL("u32"); break;
                            case 64: backing = enm->negative ? PNSLR_STRING_LITERAL("i64") : PNSLR_STRING_LITERAL("u64"); break;
                            default: FORCE_DBG_TRAP; break;
                        }
                    }

                    PNSLR_WriteToFile(f, ARR_STR_LIT("    enum class "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(enm->header.name, 6));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" : "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                    if (enm->flags) PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as flags */"));
                    else            PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as value */"));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n    {\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("        "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(var->name, (enm->header.name.count + 1)));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" = "));
                        if (var->negative) { PNSLR_WriteToFile(f, ARR_STR_LIT("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", var->idx);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(",\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    };\n"));
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    PNSLR_WriteToFile(f, ARR_STR_LIT("    struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("alignas("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(strct->header.name, 6));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n    {\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("       "));
                        WriteCxxTypeName(f, content->types, member->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(member->name));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(f, ARR_STR_LIT("]"));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    };\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                    if (fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                        WriteCxxTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (*"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCxxTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, ARR_STR_LIT("\n    "));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                        WriteCxxTypeName(f, content->types, arg->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                        if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(","));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\n    "));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(");\n"));
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }
    }

    utf8str sSuffixStr = PNSLR_StringFromCString(G_GenCxxSourceSuffix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(sSuffixStr));

    PNSLR_CloseFileHandle(f);
}

#undef ARR_STR_LIT
#undef ARR_FROM_STR
