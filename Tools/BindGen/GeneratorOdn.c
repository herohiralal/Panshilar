#include "Generator.h"
#include "Lexer.h"

#define ARR_FROM_STR_SKIP_PREFIX(str__, prefixSize__) (PNSLR_ArraySlice(u8)){.count = str__.count - (prefixSize__), .data = str__.data + (prefixSize__)}

void BeginOdnBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMetaCollection* mc, utf8str* currPkgName, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str pkgName = meta->pkgName;

    PNSLR_Path pkgPath = PNSLR_GetPathForSubdirectory(tgtDir, pkgName, allocator);
    PNSLR_CreateDirectoryTree(pkgPath);
    PNSLR_Path fPath = PNSLR_GetPathForChildFile(pkgPath, PNSLR_ConcatenateStrings(pkgName, PNSLR_StringLiteral(".odin"), allocator), allocator);
    *f = PNSLR_OpenFileToWrite(fPath, false, false);

    PNSLR_WriteToFile(*f, PNSLR_StringLiteral("package "));
    PNSLR_WriteToFile(*f, (pkgName));
    PNSLR_WriteToFile(*f, PNSLR_StringLiteral("\n\n"));

    for (i32 i = 0; i < (i32) meta->deps.count; i++)
    {
        PNSLR_WriteToFile(*f, PNSLR_StringLiteral("import \"../"));
        PNSLR_WriteToFile(*f, (meta->deps.data[i]));
        PNSLR_WriteToFile(*f, PNSLR_StringLiteral("\"\n"));
    }

    PNSLR_WriteToFile(*f, PNSLR_StringLiteral("\n"));

    if (currPkgName) *currPkgName = pkgName;

    utf8str pref = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("ODN_SRC_PREFIX"), &pref))
    {
        PNSLR_WriteToFile(*f, (pref));
    }
}

void EndOdnBindMeta(PNSLR_Path tgtDir, PNSLR_File* f, const BindMeta* meta, PNSLR_Allocator allocator)
{
    utf8str suff = {0};
    if (ResolveMetaKey(meta, PNSLR_StringLiteral("ODN_SRC_SUFFIX"), &suff))
    {
        PNSLR_WriteToFile(*f, (suff));
    }

    PNSLR_CloseFileHandle(*f);
    *f = (PNSLR_File) {0};
}

void WriteOdnTypeName(PNSLR_File file, PNSLR_ArraySlice(DeclTypeInfo) types, u32 ty, utf8str currPkgName)
{
    if (ty >= (u32) types.count) FORCE_DBG_TRAP;

    DeclTypeInfo declTy = types.data[ty];

    switch (declTy.polyTy)
    {
        case PolymorphicDeclType_None:
        {
            b8 addNs = false;
            b8 skipPrefix = false;
            utf8str nameStr = declTy.u.name;
            if (false) { }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("void"), nameStr, 0)) { nameStr = PNSLR_StringLiteral("()"); }
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
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("utf8str"), nameStr, 0)) { nameStr = PNSLR_StringLiteral("string"); }
            else if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("char"), nameStr, 0)) { nameStr = PNSLR_StringLiteral("#error"); }
            else { skipPrefix = true; addNs = true; }

            if (addNs && !PNSLR_AreStringsEqual(declTy.pkgName, currPkgName, 0) && declTy.pkgName.count > 0)
            {
                PNSLR_WriteToFile(file, (declTy.pkgName));
                PNSLR_WriteToFile(file, PNSLR_StringLiteral("."));
            }

            PNSLR_WriteToFile(file, ARR_FROM_STR_SKIP_PREFIX(nameStr, (skipPrefix ? (declTy.namespace.count + 1) : 0)));
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            PNSLR_WriteToFile(file, PNSLR_StringLiteral("[]"));
            WriteOdnTypeName(file, types, (u32) declTy.u.polyTgtIdx, currPkgName);
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            PNSLR_WriteToFile(file, PNSLR_StringLiteral("^"));
            WriteOdnTypeName(file, types, (u32) declTy.u.polyTgtIdx, currPkgName);
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void WriteOdnDoc(PNSLR_File f, utf8str doc, u8 indentCount)
{
    if (!doc.data || !doc.count) return;
    i32 lineStart = -1, lineEnd = -1;
    FileIterInfo docIter = {.contents = (doc)};
    while (DequeueNextLineSpan(&docIter, &lineStart, &lineEnd))
    {
        PNSLR_ArraySlice(u8) lineSlice = { .count = lineEnd - lineStart, .data = docIter.contents.data + lineStart };
        for (u8 i = 0; i < indentCount; i++) { PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t")); }
        if (PNSLR_AreStringsEqual(lineSlice, PNSLR_StringLiteral("/**"), 0)) { lineSlice = PNSLR_StringLiteral("/*"); }
        else if (PNSLR_StringStartsWith(lineSlice, PNSLR_StringLiteral(" * "), 0)) { lineSlice.data += 3; lineSlice.count -= 3; }
        else if (PNSLR_AreStringsEqual(lineSlice, PNSLR_StringLiteral(" */"), 0)) { lineSlice = PNSLR_StringLiteral("*/"); }
        PNSLR_WriteToFile(f, lineSlice);
        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
    }
}

void GenerateOdnBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_File f = {0};
    utf8str currPkgName = {0};

    BindMeta* bm = nil;
    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
        if (bm != file->associatedMeta)
        {
            if (bm != nil) EndOdnBindMeta(tgtDir, &f, bm, allocator);
            bm = file->associatedMeta;
            BeginOdnBindMeta(tgtDir, &f, &(content->metas), &currPkgName, bm, allocator);
        }

        if (file->declarations != nil)
        {
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("// #######################################################################################\n"));
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("// "));
            PNSLR_WriteToFile(f, (file->name));
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("// #######################################################################################\n"));
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
        }

        for (DeclHeader* decl = file->declarations; decl != nil; decl = decl->next)
        {
            b8 isFunction = decl->type == DeclType_Function && !((ParsedFunction*) decl)->isDelegate;
            if (!isFunction) { WriteOdnDoc(f, decl->doc, 0); }

            switch (decl->type)
            {
                case DeclType_Section:
                {
                    ParsedSection* sec = (ParsedSection*) decl;
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("// "));
                    PNSLR_WriteToFile(f, (sec->header.name));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" "));
                    for (i64 i = (4 + sec->header.name.count); i < 90; i++) { PNSLR_WriteToFile(f, PNSLR_StringLiteral("~")); }
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
                    break;
                }
                case DeclType_Array:
                {
                    ParsedArrayDecl* arr = (ParsedArrayDecl*) decl;
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("// declare "));
                    WriteOdnTypeName(f, content->types, arr->header.ty, currPkgName);
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
                    break;
                }
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    if (PNSLR_AreStringsEqual(PNSLR_StringLiteral("utf8str"), tyAl->header.name, 0)) { PNSLR_WriteToFile(f, PNSLR_StringLiteral("// ")); }
                    WriteOdnTypeName(f, content->types, tyAl->header.ty, currPkgName);
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" :: "));
                    WriteOdnTypeName(f, content->types, tyAl->tgt, currPkgName);
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
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

                    if (enm->flags) // kept at the top because the doc is right before this.
                    {
                        WriteOdnTypeName(f, content->types, enm->header.ty, currPkgName);
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(" :: distinct bit_set["));
                        WriteOdnTypeName(f, content->types, enm->header.ty, currPkgName);
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("Values; "));
                        PNSLR_WriteToFile(f, (backing));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("]\n\n"));
                    }

                    WriteOdnTypeName(f, content->types, enm->header.ty, currPkgName);
                    if (enm->flags) PNSLR_WriteToFile(f, PNSLR_StringLiteral("Values"));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" :: enum "));
                    if (enm->flags) PNSLR_WriteToFile(f, PNSLR_StringLiteral("u8")); // don't need a larger space for flags container
                    else            PNSLR_WriteToFile(f, (backing));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" {\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        if (enm->flags && !var->idx) continue;

                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(var->name, (enm->header.name.count + 1)));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(" = "));
                        if (var->negative) { PNSLR_WriteToFile(f, PNSLR_StringLiteral("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", enm->flags ? (u64) var->flagsOffset : var->idx);
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(",\n"));
                    }
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("}\n"));
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    WriteOdnTypeName(f, content->types, strct->header.ty, currPkgName);
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" :: struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("#align("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(") "));
                    }
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" {\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                        PNSLR_WriteToFile(f, (member->name));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(": "));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, PNSLR_StringLiteral("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (PNSLR_ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(f, PNSLR_StringLiteral("]"));
                        }
                        WriteOdnTypeName(f, content->types, member->ty, currPkgName);
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(",\n"));
                    }
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("}\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    if (!fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("@(link_prefix=\"PNSLR_\")\n"));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("foreign {\n"));
                        // append indented doc
                        WriteOdnDoc(f, decl->doc, 1);
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                    }
                    if (fn->isDelegate) WriteOdnTypeName(f, content->types, fn->header.ty, currPkgName);
                    else                PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(" :: "));
                    if (fn->isDelegate) PNSLR_WriteToFile(f, PNSLR_StringLiteral("#type "));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("proc \"c\" ("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
                    if (fn->args != nil && !fn->isDelegate) PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                        PNSLR_WriteToFile(f, (arg->name));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(": "));
                        WriteOdnTypeName(f, content->types, arg->ty, currPkgName);
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(","));
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
                        if (!fn->isDelegate) PNSLR_WriteToFile(f, PNSLR_StringLiteral("\t"));
                    }
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral(")"));
                    if (fn->retTy)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral(" -> "));
                        WriteOdnTypeName(f, content->types, fn->retTy, currPkgName);
                    }
                    if (!fn->isDelegate) PNSLR_WriteToFile(f, PNSLR_StringLiteral(" ---"));
                    PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
                    if (!fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, PNSLR_StringLiteral("}\n"));
                    }
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
        }
    }

    if (bm != nil) { EndOdnBindMeta(tgtDir, &f, bm, allocator); }
}

#undef ARR_FROM_STR_SKIP_PREFIX
