#include "Generator.h"
#include "Lexer.h"

cstring G_GenCxxHeaderPrefix = ""
"// #pragma once is intentionally ignored.\n"
"#ifndef __cplusplus\n"
"    #error \"Please use the C bibndings.\";\n"
"#else\n"
"#ifndef PANSHILAR_CXX_MAIN\n"
"#define PANSHILAR_CXX_MAIN\n"
"\n"
"typedef bool                b8;\n"
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
"template <typename T> struct ArraySlice { T* data; i64 count; };\n"
"struct utf8str : public ArraySlice<u8>\n"
"{\n"
"    utf8str() = default;\n"
"    utf8str(const ArraySlice<u8>& other) : ArraySlice<u8>(other) { }\n"
"    b8 operator==(const utf8str& other) const;\n"
"    b8 operator!=(const utf8str& other) const;\n"
"};\n"
"namespace Panshilar\n"
"{\n"
"";

cstring G_GenCxxHeaderSuffix = ""
"    /** Create a utf8str from a string literal. */\n"
"    template <u64 N> constexpr utf8str STRING_LITERAL(const char (&str)[N]) { utf8str output; output.count = (i64) (N - 1); output.data = (u8*) str; return output; }\n"
"\n"
"    /** Get the current source code location. */\n"
"    #define PNSLR_GET_LOC() Panshilar::SourceCodeLocation{Panshilar::STRING_LITERAL(__FILE__), __LINE__, 0, Panshilar::STRING_LITERAL(__FUNCTION__)}\n"
"\n"
"    /** Allocate an object of type 'ty' using the provided allocator. */\n"
"    template <typename T> T* New(Allocator allocator, SourceCodeLocation loc, AllocatorError* err)\n"
"    {\n"
"        return (T*) Allocate(allocator, true, (i32) sizeof(T), (i32) alignof(T), loc, err);\n"
"    }\n"
"\n"
"    /** Delete an object allocated with `PNSLR_New`, using the provided allocator. */\n"
"    template <typename T> void Delete(T* obj, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)\n"
"    {\n"
"        if (obj) { Free(allocator, obj, loc, err); }\n"
"    }\n"
"\n"
"    /** Allocate an array of 'count__' elements of type 'ty' using the provided allocator. Optionally zeroed. */\n"
"    template <typename T> ArraySlice<T> MakeSlice(i64 count, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)\n"
"    {\n"
"        RawArraySlice raw = MakeRawSlice((i32) sizeof(T), (i32) alignof(T), count, zeroed, allocator, loc, err);\n"
"        return *reinterpret_cast<ArraySlice<T>*>(&raw);\n"
"}\n"
"\n"
"    /** Free a 'slice' allocated with `PNSLR_MakeSlice`, using the provided allocator. Expects a reassignable variable. */\n"
"    template <typename T> void FreeSlice(ArraySlice<T>* slice, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)\n"
"    {\n"
"        if (slice) FreeRawSlice(reinterpret_cast<RawArraySlice*>(slice), allocator, loc, err);\n"
"    }\n"
"\n"
"    /** Resize a slice to one with 'newCount__' elements of type 'ty' using the provided allocator. Optionally zeroed. Expects a reassignable variable. */\n"
"    template <typename T> void ResizeSlice(ArraySlice<T>* slice, i64 newCount, b8 zeroed, Allocator allocator, SourceCodeLocation loc, AllocatorError* err)\n"
"    {\n"
"        if (slice) ResizeRawSlice(reinterpret_cast<RawArraySlice*>(slice), (i32) sizeof(T), (i32) alignof(T), newCount, zeroed, allocator, loc, err);\n"
"    }\n"
"}//namespace end\n"
"\n"
"b8 utf8str::operator==(const utf8str& other) const { return Panshilar::AreStringsEqual(*this, other, Panshilar::StringComparisonType::CaseSensitive); }\n"
"\n"
"b8 utf8str::operator!=(const utf8str& other) const { return !(*this == other); }\n"
"\n"
"#endif//PANSHILAR_CXX_MAIN\n"
"\n"
"#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
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
"#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS\n"
"";

cstring G_GenCxxSourcePrefix = ""
"\n"
"#ifndef PNSLR_IMPLEMENTATION\n"
"    #define PNSLR_SKIP_IMPLEMENTATION\n"
"#else //PNSLR_IMPLEMENTATION\n"
"    #undef  PNSLR_SKIP_IMPLEMENTATION\n"
"    #undef  PNSLR_IMPLEMENTATION\n"
"#endif//PNSLR_IMPLEMENTATION\n"
"\n"
"#ifndef PNSLR_SKIP_IMPLEMENTATION\n"
"#define PNSLR_SKIP_IMPLEMENTATION\n"
"\n"
"b8      PNSLR_Bindings_Convert(b8      x) { return x; }\n"
"u8      PNSLR_Bindings_Convert(u8      x) { return x; }\n"
"u16     PNSLR_Bindings_Convert(u16     x) { return x; }\n"
"u32     PNSLR_Bindings_Convert(u32     x) { return x; }\n"
"u64     PNSLR_Bindings_Convert(u64     x) { return x; }\n"
"i8      PNSLR_Bindings_Convert(i8      x) { return x; }\n"
"i16     PNSLR_Bindings_Convert(i16     x) { return x; }\n"
"i32     PNSLR_Bindings_Convert(i32     x) { return x; }\n"
"i64     PNSLR_Bindings_Convert(i64     x) { return x; }\n"
"f32     PNSLR_Bindings_Convert(f32     x) { return x; }\n"
"f64     PNSLR_Bindings_Convert(f64     x) { return x; }\n"
"rawptr  PNSLR_Bindings_Convert(rawptr  x) { return x; }\n"
"cstring PNSLR_Bindings_Convert(cstring x) { return x; }\n"
"\n"
"#if (_MSC_VER)\n"
"    #define PNSLR_STRUCT_OFFSET(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))\n"
"#elif (__clang__) || (__GNUC__)\n"
"    #define PNSLR_STRUCT_OFFSET(type, member) ((u64) offsetof(type, member))\n"
"#else\n"
"    #error \"UNSUPPORTED COMPILER!\";\n"
"#endif\n"
"\n"
"";

cstring G_GenCxxSourceSuffix = ""
"\n"
"#undef PNSLR_STRUCT_OFFSET\n"
"\n"
"#endif//PNSLR_SKIP_IMPLEMENTATION\n"
"#endif//__cplusplus\n"
"";

#define ARR_FROM_STR(str__) (PNSLR_ArraySlice(u8)){.count = str__.count, .data = str__.data}
#define ARR_FROM_STR_SKIP_PREFIX(str__, prefixSize__) (PNSLR_ArraySlice(u8)){.count = str__.count - (prefixSize__), .data = str__.data + (prefixSize__)}
#define ARR_STR_LIT(str__) (PNSLR_ArraySlice(u8)){.count = sizeof(str__) - 1, .data = (u8*) str__}

void WriteCxxTypeName(PNSLR_File file, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty, b8 addNamespace)
{
    if (ty >= (u32) types.count) FORCE_DBG_TRAP;

    DeclTypeInfo declTy = types.data[ty];

    switch (declTy.polyTy)
    {
        case PolymorphicDeclType_None:
        {
            b8 addNsOriginalVal = addNamespace;
            addNamespace = false; // avoids a bunch of boilerplate in a few lines

            b8 skipPrefix = false;
            utf8str nameStr = declTy.u.name;
            if (false) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("void"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("b8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("u8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("u16"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("u32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("u64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("i8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("i16"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("i32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("i64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("f32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("f64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("cstring"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("rawptr"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("utf8str"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("char"), nameStr, 0)) { }
            else { skipPrefix = true; addNamespace = addNsOriginalVal; }

            if (addNamespace) PNSLR_WriteToFile(file, ARR_STR_LIT("Panshilar::"));
            PNSLR_WriteToFile(file, ARR_FROM_STR_SKIP_PREFIX(nameStr, (skipPrefix ? 6 : 0)));
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            PNSLR_WriteToFile(file, ARR_STR_LIT("ArraySlice<"));
            WriteCxxTypeName(file, types, (u32) declTy.u.polyTgtIdx, addNamespace);
            PNSLR_WriteToFile(file, ARR_STR_LIT(">"));
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            WriteCxxTypeName(file, types, (u32) declTy.u.polyTgtIdx, addNamespace);
            PNSLR_WriteToFile(file, ARR_STR_LIT("*"));
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void WriteCTypeNameForCxx(PNSLR_File file, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty)
{
    if (ty >= (u32) types.count) FORCE_DBG_TRAP;

    DeclTypeInfo declTy = types.data[ty];

    switch (declTy.polyTy)
    {
        case PolymorphicDeclType_None:
        {
            utf8str nameStr = declTy.u.name;
            if (false) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("utf8str"), nameStr, 0)) { nameStr = PNSLR_StringLiteral("PNSLR_UTF8STR"); }
            else { }

            PNSLR_WriteToFile(file, nameStr);
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            PNSLR_WriteToFile(file, ARR_STR_LIT("PNSLR_ArraySlice_"));
            WriteCTypeNameForCxx(file, types, (u32) declTy.u.polyTgtIdx);
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            WriteCTypeNameForCxx(file, types, (u32) declTy.u.polyTgtIdx);
            PNSLR_WriteToFile(file, ARR_STR_LIT("*"));
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void WriteCxxReinterpretBoilerplate(PNSLR_File f, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty)
{
    PNSLR_WriteToFile(f, ARR_STR_LIT("static_assert(sizeof("));
    WriteCTypeNameForCxx(f, types, ty);
    PNSLR_WriteToFile(f, ARR_STR_LIT(") == sizeof("));
    WriteCxxTypeName(f, types, ty, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT("), \"size mismatch\");\n"));

    PNSLR_WriteToFile(f, ARR_STR_LIT("static_assert(alignof("));
    WriteCTypeNameForCxx(f, types, ty);
    PNSLR_WriteToFile(f, ARR_STR_LIT(") == alignof("));
    WriteCxxTypeName(f, types, ty, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT("), \"align mismatch\");\n"));

    WriteCTypeNameForCxx(f, types, ty + 1);
    PNSLR_WriteToFile(f, ARR_STR_LIT(" PNSLR_Bindings_Convert("));
    WriteCxxTypeName(f, types, ty + 1, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT(" x) { return reinterpret_cast<"));
    WriteCTypeNameForCxx(f, types, ty + 1);
    PNSLR_WriteToFile(f, ARR_STR_LIT(">(x); }\n"));

    WriteCxxTypeName(f, types, ty + 1, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT(" PNSLR_Bindings_Convert("));
    WriteCTypeNameForCxx(f, types, ty + 1);
    PNSLR_WriteToFile(f, ARR_STR_LIT(" x) { return reinterpret_cast<"));
    WriteCxxTypeName(f, types, ty + 1, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT(">(x); }\n"));

    WriteCTypeNameForCxx(f, types, ty);
    PNSLR_WriteToFile(f, ARR_STR_LIT("& PNSLR_Bindings_Convert("));
    WriteCxxTypeName(f, types, ty, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT("& x) { return *PNSLR_Bindings_Convert(&x); }\n"));

    WriteCxxTypeName(f, types, ty, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT("& PNSLR_Bindings_Convert("));
    WriteCTypeNameForCxx(f, types, ty);
    PNSLR_WriteToFile(f, ARR_STR_LIT("& x) { return *PNSLR_Bindings_Convert(&x); }\n"));
}

void WriteCxxMemberParityBoilerplate(PNSLR_File f, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty, utf8str memberName)
{
    PNSLR_WriteToFile(f, ARR_STR_LIT("static_assert(PNSLR_STRUCT_OFFSET("));
    WriteCTypeNameForCxx(f, types, ty);
    PNSLR_WriteToFile(f, ARR_STR_LIT(", "));
    PNSLR_WriteToFile(f, ARR_FROM_STR(memberName));
    PNSLR_WriteToFile(f, ARR_STR_LIT(") == PNSLR_STRUCT_OFFSET("));
    WriteCxxTypeName(f, types, ty, true);
    PNSLR_WriteToFile(f, ARR_STR_LIT(", "));
    PNSLR_WriteToFile(f, ARR_FROM_STR(memberName));
    PNSLR_WriteToFile(f, ARR_STR_LIT("), \""));
    PNSLR_WriteToFile(f, ARR_FROM_STR(memberName));
    PNSLR_WriteToFile(f, ARR_STR_LIT(" offset mismatch\");\n"));
}

void GenerateCxxBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_Path headerPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_StringLiteral("Panshilar.hpp"), allocator);
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
                    PNSLR_ArraySlice(u8) lineSlice = { .count = lineEnd - lineStart, .data = docIter.contents.data + lineStart };
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
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                    if (PNSLR_AreStringsEqual(tyAl->header.name, PNSLR_StringLiteral("utf8str"), 0))
                        PNSLR_WriteToFile(f, ARR_STR_LIT("// "));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                    WriteCxxTypeName(f, content->types, tyAl->tgt, false);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    WriteCxxTypeName(f, content->types, tyAl->header.ty, false);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
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

                    PNSLR_WriteToFile(f, ARR_STR_LIT("    enum class "));
                    WriteCxxTypeName(f, content->types, enm->header.ty, false);
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
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
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
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    WriteCxxTypeName(f, content->types, strct->header.ty, false);
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n    {\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("       "));
                        WriteCxxTypeName(f, content->types, member->ty, false);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(member->name));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
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
                        WriteCxxTypeName(f, content->types, fn->retTy, false);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (*"));
                        WriteCxxTypeName(f, content->types, fn->header.ty, false);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCxxTypeName(f, content->types, fn->retTy, false);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, ARR_STR_LIT("\n    "));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                        WriteCxxTypeName(f, content->types, arg->ty, false);
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
                    // ParsedSection* sec = (ParsedSection*) decl;
                    continue;
                }
                case DeclType_Array:
                {
                    ParsedArrayDecl* arr = (ParsedArrayDecl*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef struct { "));
                    WriteCTypeNameForCxx(f, content->types, arr->tgtTy + 1 /* +1 for ptr ty */);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" data; i64 count; } "));
                    WriteCTypeNameForCxx(f, content->types, arr->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
                    WriteCxxReinterpretBoilerplate(f, content->types, arr->header.ty);
                    WriteCxxMemberParityBoilerplate(f, content->types, arr->header.ty, PNSLR_StringLiteral("count"));
                    WriteCxxMemberParityBoilerplate(f, content->types, arr->header.ty, PNSLR_StringLiteral("data"));
                    break;
                }
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                    WriteCTypeNameForCxx(f, content->types, tyAl->tgt);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    WriteCTypeNameForCxx(f, content->types, tyAl->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
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

                    PNSLR_WriteToFile(f, ARR_STR_LIT("enum class "));
                    WriteCTypeNameForCxx(f, content->types, enm->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" : "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" { };\n"));
                    WriteCxxReinterpretBoilerplate(f, content->types, enm->header.ty);
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    PNSLR_WriteToFile(f, ARR_STR_LIT("struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("alignas("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    WriteCTypeNameForCxx(f, content->types, strct->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n{\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("   "));
                        WriteCTypeNameForCxx(f, content->types, member->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(member->name));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(f, ARR_STR_LIT("]"));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("};\n"));
                    WriteCxxReinterpretBoilerplate(f, content->types, strct->header.ty);
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                        WriteCxxMemberParityBoilerplate(f, content->types, strct->header.ty, member->name);
                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("extern \"C\" "));
                    if (fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                        WriteCTypeNameForCxx(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (*"));
                        WriteCTypeNameForCxx(f, content->types, fn->header.ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCTypeNameForCxx(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(fn->header.name));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        WriteCTypeNameForCxx(f, content->types, arg->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                        if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(", "));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(");\n"));
                    if (fn->isDelegate) WriteCxxReinterpretBoilerplate(f, content->types, fn->header.ty);
                    else
                    {
                        WriteCxxTypeName(f, content->types, fn->retTy, true);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" Panshilar::"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                        for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                        {
                            WriteCxxTypeName(f, content->types, arg->ty, true);
                            PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                            PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                            if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(", "));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")\n{\n    "));
                        if (fn->retTy != 0)
                        {
                            WriteCTypeNameForCxx(f, content->types, fn->retTy);
                            PNSLR_WriteToFile(f, ARR_STR_LIT(" zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW = "));
                        }

                        PNSLR_WriteToFile(f, ARR_FROM_STR(fn->header.name));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                        for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("PNSLR_Bindings_Convert("));
                            PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                            PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                            if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(", "));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));

                        if (fn->retTy != 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("; return PNSLR_Bindings_Convert(zzzz_RetValXYZABCDEFGHIJKLMNOPQRSTUVW)"));
                        }
                        PNSLR_WriteToFile(f, ARR_STR_LIT(";\n}\n"));
                    }
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
