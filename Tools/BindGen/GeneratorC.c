#include "Generator.h"

cstring G_GenCPrefix = ""
"#ifndef PANSHILAR_MAIN\n"
"#define PANSHILAR_MAIN\n"
"\n"
"#ifdef __cplusplus\n"
"extern \"C\" {\n"
"#endif\n"
"\n"
"#if defined(_MSC_VER)\n"
"    #define PNSLR_ALIGNAS(x) __declspec(align(x))\n"
"#elif defined(__clang__) || defined(__GNUC__)\n"
"    #define PNSLR_ALIGNAS(x) __attribute__((aligned(x)))\n"
"#else\n"
"    #error \"UNSUPPORTED COMPILER!\";\n"
"#endif\n"
"\n"
"/** An array slice of type 'ty'. */\n"
"#define PNSLR_ArraySlice(ty) PNSLR_ArraySlice_##ty\n"
"\n"
"/** Declare an array slice of type 'ty'. */\n"
"#define PNSLR_DECLARE_ARRAY_SLICE(ty) \\\n"
"    typedef union PNSLR_ArraySlice(ty) { struct { ty* data; i64 count; }; PNSLR_RawArraySlice raw; } PNSLR_ArraySlice(ty);\n"
"\n"
"typedef unsigned char       b8;\n"
"typedef unsigned char       u8;\n"
"typedef unsigned short int  u16;\n"
"typedef unsigned int        u32;\n"
"typedef unsigned long long  u64;\n"
"typedef signed char         i8;\n"
"typedef signed short int    i16;\n"
"typedef signed int          i32;\n"
"typedef signed long long    i64;\n"
"typedef float               f32;\n"
"typedef double              f64;\n"
"typedef char*               cstring;\n"
"typedef void*               rawptr;\n"
"\n"
"";

cstring G_GenCSuffix = ""
"#undef PNSLR_ALIGNAS\n"
"\n"
"/** Create a utf8str from a string literal. */\n"
"#define PNSLR_StringLiteral(str) (utf8str) {.count = sizeof(str) - 1, .data = (u8*) str}\n"
"\n"
"/** Get the current source code location. */\n"
"#define PNSLR_GET_LOC() (PNSLR_SourceCodeLocation) \\\n"
"    { \\\n"
"        .file = PNSLR_StringLiteral(__FILE__), \\\n"
"        .line = __LINE__, \\\n"
"        .function = PNSLR_StringLiteral(__FUNCTION__) \\\n"
"    }\n"
"\n"
"/** Allocate an object of type 'ty' using the provided allocator. */\n"
"#define PNSLR_New(ty, allocator, loc, error__) \\\n"
"    ((ty*) PNSLR_Allocate(allocator, sizeof(ty), alignof(ty), loc, error__))\n"
"\n"
"/** Delete an object allocated with `PNSLR_New`, using the provided allocator. */\n"
"#define PNSLR_Delete(obj, allocator, loc, error__) \\\n"
"    do { if (obj) PNSLR_Free(allocator, obj, loc, error__); } while(0)\n"
"\n"
"/** Allocate an array of 'count' elements of type 'ty' using the provided allocator. Optionally zeroed. */\n"
"#define PNSLR_MakeSlice(ty, count, zeroed, allocator, loc, error__) \\\n"
"    (PNSLR_ArraySlice_##ty) {.raw = PNSLR_MakeRawSlice((i32) sizeof(ty), (i32) alignof(ty), (i64) count, zeroed, allocator, loc, error__)}\n"
"\n"
"/** Free a 'slice' (passed by ptr) allocated with `PNSLR_MakeSlice`, using the provided allocator. */\n"
"#define PNSLR_FreeSlice(slice, allocator, loc, error__) \\\n"
"    do { if (slice) PNSLR_FreeRawSlice(&((slice)->raw), allocator, loc, error__); } while(0)\n"
"\n"
"/** Resize a 'slice' (passed by ptr) to one with 'newCount' elements of type 'ty' using the provided allocator. Optionally zeroed. */\n"
"#define PNSLR_ResizeSlice(ty, slice, newCount, zeroed, allocator, loc, error__) \\\n"
"    do { if (slice) PNSLR_ResizeRawSlice(&((slice)->raw), (i32) sizeof(ty), (i32) alignof(ty), (i64) newCount, zeroed, allocator, loc, error__); } while(0)\n"
"\n"
"#ifdef __cplusplus\n"
"} // extern c\n"
"#endif\n"
"\n"
"#endif//PANSHILAR_MAIN\n"
"\n"
"#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"    #if !defined(__cplusplus) && !defined(static_assert)\n"
"        #define static_assert _Static_assert\n"
"        #define PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT\n"
"    #endif\n"
"\n"
"    static_assert(sizeof(b8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(u8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(u16) == 2, \"Size mismatch.\");\n"
"    static_assert(sizeof(u32) == 4, \"Size mismatch.\");\n"
"    static_assert(sizeof(u64) == 8, \"Size mismatch.\");\n"
"    static_assert(sizeof(i8 ) == 1, \"Size mismatch.\");\n"
"    static_assert(sizeof(i16) == 2, \"Size mismatch.\");\n"
"    static_assert(sizeof(i32) == 4, \"Size mismatch.\");\n"
"    static_assert(sizeof(i64) == 8, \"Size mismatch.\");\n"
"    static_assert(sizeof(f32) == 4, \"Size mismatch.\");\n"
"    static_assert(sizeof(f64) == 8, \"Size mismatch.\");\n"
"\n"
"    #ifdef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT\n"
"        #undef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT\n"
"        #undef static_assert\n"
"    #endif\n"
"#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"";

#define ARR_FROM_STR(str__) (PNSLR_ArraySlice(u8)){.count = str__.count, .data = str__.data}
#define ARR_STR_LIT(str__) (PNSLR_ArraySlice(u8)){.count = sizeof(str__) - 1, .data = (u8*) str__}

void WriteCTypeName(PNSLR_File file, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty)
{
    if (ty >= (u32) types.count) FORCE_DBG_TRAP;

    DeclTypeInfo declTy = types.data[ty];

    switch (declTy.polyTy)
    {
        case PolymorphicDeclType_None:
        {
            PNSLR_WriteToFile(file, ARR_FROM_STR(declTy.u.name));
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            PNSLR_WriteToFile(file, ARR_STR_LIT("PNSLR_ArraySlice("));
            WriteCTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            PNSLR_WriteToFile(file, ARR_STR_LIT(")"));
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            WriteCTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            PNSLR_WriteToFile(file, ARR_STR_LIT("*"));
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void GenerateCBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_Path headerPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_StringLiteral("Panshilar.h"), allocator);
    PNSLR_File headerFile = PNSLR_OpenFileToWrite(headerPath, false, false);

    utf8str prefixStr = PNSLR_StringFromCString(G_GenCPrefix);
    PNSLR_WriteToFile(headerFile, ARR_FROM_STR(prefixStr));

    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (file->declarations != nil)
        {
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("// #######################################################################################\n"));
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("// "));
            PNSLR_WriteToFile(headerFile, ARR_FROM_STR(file->name));
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("// #######################################################################################\n"));
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
        }

        for (DeclHeader* decl = file->declarations; decl != nil; decl = decl->next)
        {
            if (decl->doc.count > 0)
            {
                PNSLR_WriteToFile(headerFile, ARR_FROM_STR(decl->doc));
                PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
            }

            switch (decl->type)
            {
                case DeclType_Section:
                {
                    ParsedSection* sec = (ParsedSection*) decl;
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("// "));
                    PNSLR_WriteToFile(headerFile, ARR_FROM_STR(sec->header.name));
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                    for (i64 i = (4 + sec->header.name.count); i < 90; i++) { PNSLR_WriteToFile(headerFile, ARR_STR_LIT("~")); }
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
                    break;
                }
                case DeclType_Array:
                {
                    ParsedArrayDecl* arr = (ParsedArrayDecl*) decl;
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("PNSLR_DECLARE_ARRAY_SLICE("));
                    WriteCTypeName(headerFile, content->types, arr->tgtTy);
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(");\n"));
                    break;
                }
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("typedef "));
                    WriteCTypeName(headerFile, content->types, tyAl->tgt);
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                    WriteCTypeName(headerFile, content->types, tyAl->header.ty);

                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(";\n"));
                    break;
                }
                case DeclType_Enum:
                {
                    ParsedEnum* enm = (ParsedEnum*) decl;
                    utf8str backing = {0};
                    {
                        switch (enm->size)
                        {
                            case  8: backing = enm->negative ? PNSLR_StringLiteral("i8" ) : PNSLR_StringLiteral("u8" ); break;
                            case 16: backing = enm->negative ? PNSLR_StringLiteral("i16") : PNSLR_StringLiteral("u16"); break;
                            case 32: backing = enm->negative ? PNSLR_StringLiteral("i32") : PNSLR_StringLiteral("u32"); break;
                            case 64: backing = enm->negative ? PNSLR_StringLiteral("i64") : PNSLR_StringLiteral("u64"); break;
                            default: FORCE_DBG_TRAP; break;
                        }
                    }

                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("typedef "));
                    PNSLR_WriteToFile(headerFile, ARR_FROM_STR(backing));
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                    WriteCTypeName(headerFile, content->types, enm->header.ty);
                    if (enm->flags) PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" /* use as flags */"));
                    else            PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" /* use as value */"));
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(";\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("#define "));
                        PNSLR_WriteToFile(headerFile, ARR_FROM_STR(var->name));
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" (("));
                    WriteCTypeName(headerFile, content->types, enm->header.ty);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(") "));
                        if (var->negative) { PNSLR_WriteToFile(headerFile, ARR_STR_LIT("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", var->idx);
                        PNSLR_WriteToFile(headerFile, (PNSLR_ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(")\n"));
                    }
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("typedef struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("PNSLR_ALIGNAS("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(headerFile, (PNSLR_ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(") "));
                    }
                    WriteCTypeName(headerFile, content->types, strct->header.ty);
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n{\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("    "));
                        b8 addStructPrefix = (member->ty == (strct->header.ty + 1)); // type is a ptr to self
                        if (addStructPrefix) PNSLR_WriteToFile(headerFile, ARR_STR_LIT("struct "));
                        WriteCTypeName(headerFile, content->types, member->ty);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(headerFile, ARR_FROM_STR(member->name));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(headerFile, (PNSLR_ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("]"));
                        }
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(";\n"));
                    }
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("} "));
                    WriteCTypeName(headerFile, content->types, strct->header.ty);
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(";\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    if (fn->isDelegate)
                    {
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("typedef "));
                        WriteCTypeName(headerFile, content->types, fn->retTy);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" (*"));
                        WriteCTypeName(headerFile, content->types, fn->header.ty);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCTypeName(headerFile, content->types, fn->retTy);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(headerFile, ARR_FROM_STR(fn->header.name));
                    }
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT("("));
                    if (fn->args != nil) PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
                    else                 PNSLR_WriteToFile(headerFile, ARR_STR_LIT("void"));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("    "));
                        WriteCTypeName(headerFile, content->types, arg->ty);
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(headerFile, ARR_FROM_STR(arg->name));
                        if (arg->next != nil) PNSLR_WriteToFile(headerFile, ARR_STR_LIT(","));
                        PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
                    }
                    PNSLR_WriteToFile(headerFile, ARR_STR_LIT(");\n"));
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(headerFile, ARR_STR_LIT("\n"));
        }
    }

    utf8str suffixStr = PNSLR_StringFromCString(G_GenCSuffix);
    PNSLR_WriteToFile(headerFile, ARR_FROM_STR(suffixStr));

    PNSLR_CloseFileHandle(headerFile);
}

#undef ARR_STR_LIT
#undef ARR_FROM_STR
