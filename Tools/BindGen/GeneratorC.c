#include "Generator.h"

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

void BeginCBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMetaCollection* mc, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str pkgName = meta->pkgName;

    PNSLR_Path hPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_ConcatenateStrings(pkgName, PNSLR_StringLiteral(".h"), allocator), allocator);
    *f = PNSLR_OpenFileToWrite(hPath, false, false);

    utf8str hPrefix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("C_HEADER_PREFIX"), &hPrefix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(hPrefix));
    }

    for (i32 i = 0; i < (i32) meta->deps.count; i++)
    {
        PNSLR_WriteToFile(*f, ARR_STR_LIT("#include \""));
        PNSLR_WriteToFile(*f, ARR_FROM_STR(meta->deps.data[i]));
        PNSLR_WriteToFile(*f, ARR_STR_LIT(".h\"\n"));
    }

    cstring externCStart = "\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n";

    utf8str externCStartStr = PNSLR_StringFromCString(externCStart);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(externCStartStr));

    cstring alignasDecl = ""
        "#if defined(__cplusplus)\n"
        "    #define PNSLR_ALIGNAS(x) alignas(x)\n"
        "#elif defined(_MSC_VER)\n"
        "    #define PNSLR_ALIGNAS(x) __declspec(align(x))\n"
        "#elif defined(__clang__) || defined(__GNUC__)\n"
        "    #define PNSLR_ALIGNAS(x) __attribute__((aligned(x)))\n"
        "#else\n"
        "    #error \"UNSUPPORTED COMPILER!\";\n"
        "#endif\n\n";

    utf8str alignasDeclStr = PNSLR_StringFromCString(alignasDecl);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(alignasDeclStr));
}

void EndCBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMeta* meta, PNSLR_Allocator allocator)
{
    cstring alignasUndef = "#undef PNSLR_ALIGNAS\n\n";

    utf8str alignasUndefStr = PNSLR_StringFromCString(alignasUndef);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(alignasUndefStr));

    cstring externCEnd = ""
        "#ifdef __cplusplus\n"
        "} // extern c\n"
        "#endif\n\n";

    utf8str externCEndStr = PNSLR_StringFromCString(externCEnd);
    PNSLR_WriteToFile(*f, ARR_FROM_STR(externCEndStr));

    utf8str hSuffix = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("C_HEADER_SUFFIX"), &hSuffix))
    {
        PNSLR_WriteToFile(*f, ARR_FROM_STR(hSuffix));
    }

    PNSLR_CloseFileHandle(*f);
    *f = (PNSLR_File) {0};
}

void GenerateCBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_File f = {0};

    BindMeta* bm = nil;
    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (bm != file->associatedMeta)
        {
            if (bm != nil) EndCBindMeta(tgtDir, &f, bm, allocator);
            bm = file->associatedMeta;
            BeginCBindMeta(tgtDir, &f, &(content->metas), bm, allocator);
        }

        if (file->declarations != nil)
        {
            PNSLR_WriteToFile(f, ARR_STR_LIT("// #######################################################################################\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("// "));
            PNSLR_WriteToFile(f, ARR_FROM_STR(file->name));
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("// #######################################################################################\n"));
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }

        for (DeclHeader* decl = file->declarations; decl != nil; decl = decl->next)
        {
            if (decl->doc.count > 0)
            {
                PNSLR_WriteToFile(f, ARR_FROM_STR(decl->doc));
                PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
            }

            switch (decl->type)
            {
                case DeclType_Section:
                {
                    ParsedSection* sec = (ParsedSection*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("// "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(sec->header.name));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    for (i64 i = (4 + sec->header.name.count); i < 90; i++) { PNSLR_WriteToFile(f, ARR_STR_LIT("~")); }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    break;
                }
                case DeclType_Array:
                {
                    ParsedArrayDecl* arr = (ParsedArrayDecl*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("PNSLR_DECLARE_ARRAY_SLICE("));
                    WriteCTypeName(f, content->types, arr->tgtTy);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(");\n"));
                    break;
                }
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                    WriteCTypeName(f, content->types, tyAl->tgt);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    WriteCTypeName(f, content->types, tyAl->header.ty);

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

                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                    PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                    WriteCTypeName(f, content->types, enm->header.ty);
                    if (enm->flags) PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as flags */"));
                    else            PNSLR_WriteToFile(f, ARR_STR_LIT(" /* use as value */"));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("#define "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(var->name));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (("));
                    WriteCTypeName(f, content->types, enm->header.ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                        if (var->negative) { PNSLR_WriteToFile(f, ARR_STR_LIT("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", var->idx);
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")\n"));
                    }
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    PNSLR_WriteToFile(f, ARR_STR_LIT("typedef struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("PNSLR_ALIGNAS("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    WriteCTypeName(f, content->types, strct->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n{\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                        b8 addStructPrefix = (member->ty == (strct->header.ty + 1)); // type is a ptr to self
                        if (addStructPrefix) PNSLR_WriteToFile(f, ARR_STR_LIT("struct "));
                        WriteCTypeName(f, content->types, member->ty);
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
                    PNSLR_WriteToFile(f, ARR_STR_LIT("} "));
                    WriteCTypeName(f, content->types, strct->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(";\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    if (fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("typedef "));
                        WriteCTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" (*"));
                        WriteCTypeName(f, content->types, fn->header.ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    }
                    else
                    {
                        WriteCTypeName(f, content->types, fn->retTy);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(fn->header.name));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    else                 PNSLR_WriteToFile(f, ARR_STR_LIT("void"));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("    "));
                        WriteCTypeName(f, content->types, arg->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                        if (arg->next != nil) PNSLR_WriteToFile(f, ARR_STR_LIT(","));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(");\n"));
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }
    }

    if (bm != nil) { EndCBindMeta(tgtDir, &f, bm, allocator); }
}

#undef ARR_STR_LIT
#undef ARR_FROM_STR
