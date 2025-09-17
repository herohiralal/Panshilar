#include "Generator.h"
#include "Lexer.h"

#define ARR_FROM_STR(str__) (PNSLR_ArraySlice(u8)){.count = str__.count, .data = str__.data}
#define ARR_FROM_STR_SKIP_PREFIX(str__, prefixSize__) (PNSLR_ArraySlice(u8)){.count = str__.count - (prefixSize__), .data = str__.data + (prefixSize__)}
#define ARR_STR_LIT(str__) (PNSLR_ArraySlice(u8)){.count = sizeof(str__) - 1, .data = (u8*) str__}

void BeginCxxHeaderBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMetaCollection* mc, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str pkgName = meta->pkgName;

    PNSLR_Path hPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_ConcatenateStrings(pkgName, PNSLR_StringLiteral(".hpp"), allocator), allocator);
    *f = PNSLR_OpenFileToWrite(hPath, false, false);

    cstring noC = ""
        "#ifndef __cplusplus\n"
        "    #error \"Please use the C bindings.\";\n"
        "#endif\n\n";

    utf8str noCStr = PNSLR_StringFromCString(noC);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(noCStr));

    utf8str hPrefix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("CXX_HEADER_PREFIX"), &hPrefix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(hPrefix));
    }

    for (i32 i = 0; i < (i32) meta->deps.count; i++)
    {
        PNSLR_WriteToFile(*f, ARR_STR_LIT("#include \""));
        PNSLR_WriteToFile(*f, ARR_FROM_STR(meta->deps.data[i]));
        PNSLR_WriteToFile(*f, ARR_STR_LIT(".hpp\"\n"));
    }

    PNSLR_WriteToFile(*f, ARR_STR_LIT("\nnamespace "));
    PNSLR_WriteToFile(*f, ARR_FROM_STR(pkgName));
    PNSLR_WriteToFile(*f, ARR_STR_LIT("\n{\n"));

    for (i32 i = 0; i < (i32) meta->deps.count; i++)
    {
        PNSLR_WriteToFile(*f, ARR_STR_LIT("    using namespace "));
        PNSLR_WriteToFile(*f, ARR_FROM_STR(meta->deps.data[i]));
        PNSLR_WriteToFile(*f, ARR_STR_LIT(";\n"));
    }

    PNSLR_WriteToFile(*f, ARR_STR_LIT("\n"));
}

void EndCxxHeaderBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMeta* meta, PNSLR_Allocator allocator)
{
    PNSLR_WriteToFile(*f, ARR_STR_LIT("} // namespace end\n\n"));

    utf8str hSuffix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("CXX_HEADER_SUFFIX"), &hSuffix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(hSuffix));
    }

    PNSLR_FlushFile(*f);
    PNSLR_CloseFileHandle(*f);
    *f = (PNSLR_File) {0};
}

void BeginCxxSourceBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str pkgName = meta->pkgName;

    PNSLR_Path hPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_ConcatenateStrings(pkgName, PNSLR_StringLiteral(".hpp"), allocator), allocator);
    *f = PNSLR_OpenFileToWrite(hPath, true, false);

    PNSLR_WriteToFile(*f, ARR_STR_LIT("#ifdef PNSLR_CXX_IMPL\n\n"));

    cstring strctOffsetDecl = ""
        "#if (_MSC_VER)\n"
        "    #define PNSLR_STRUCT_OFFSET(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))\n"
        "#elif (__clang__) || (__GNUC__)\n"
        "    #define PNSLR_STRUCT_OFFSET(type, member) ((u64) offsetof(type, member))\n"
        "#else\n"
        "    #error \"UNSUPPORTED COMPILER!\";\n"
        "#endif\n\n";

    utf8str strctOffsetDeclStr = PNSLR_StringFromCString(strctOffsetDecl);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(strctOffsetDeclStr));

    utf8str sPrefix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("CXX_SOURCE_PREFIX"), &sPrefix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(sPrefix));
    }
}

void EndCxxSourceBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str sSuffix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("CXX_SOURCE_SUFFIX"), &sSuffix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(sSuffix));
    }

    cstring strctOffsetUndef = "#undef PNSLR_STRUCT_OFFSET\n\n";
    utf8str strctOffsetUndefStr = PNSLR_StringFromCString(strctOffsetUndef);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(strctOffsetUndefStr));

    PNSLR_WriteToFile(*f, ARR_STR_LIT("#endif//PNSLR_CXX_IMPL\n\n"));

    PNSLR_CloseFileHandle(*f);
    *f = (PNSLR_File) {0};
}

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

            if (addNamespace)
            {
                PNSLR_WriteToFile(file, ARR_FROM_STR(declTy.pkgName));
                PNSLR_WriteToFile(file, ARR_STR_LIT("::"));
            }
            PNSLR_WriteToFile(file, ARR_FROM_STR_SKIP_PREFIX(nameStr, (skipPrefix ? declTy.namespace.count + 1 : 0)));
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
    PNSLR_File f = {0};

    BindMeta* bm = nil;
    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (bm != file->associatedMeta)
        {
            if (bm != nil) EndCxxHeaderBindMeta(tgtDir, &f, bm, allocator);
            bm = file->associatedMeta;
            BeginCxxHeaderBindMeta(tgtDir, &f, &(content->metas), bm, allocator);
        }

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
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, (fn->header.namespace.count + 1)));
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

    if (bm != nil) { EndCxxHeaderBindMeta(tgtDir, &f, bm, allocator); }

    bm = nil;
    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (bm != file->associatedMeta)
        {
            if (bm != nil) EndCxxSourceBindMeta(tgtDir, &f, bm, allocator);
            bm = file->associatedMeta;
            BeginCxxSourceBindMeta(tgtDir, &f, bm, allocator);
        }

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
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(fn->header.pkgName));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("::"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, (fn->header.namespace.count + 1)));
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

    if (bm != nil) { EndCxxSourceBindMeta(tgtDir, &f, bm, allocator); }
}

#undef ARR_STR_LIT
#undef ARR_FROM_STR_SKIP_PREFIX
#undef ARR_FROM_STR
