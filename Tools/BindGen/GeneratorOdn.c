#include "Generator.h"
#include "Lexer.h"

cstring G_GenOdnPrefix = ""
"package Panshilar\n"
"";

cstring G_GenOdnSuffix = ""
"#assert(size_of(int)  == 8, \" int must be 8 bytes\")\n"
"#assert(size_of(uint) == 8, \"uint must be 8 bytes\")\n"
"";

#define ARR_FROM_STR(str__) (ArraySlice(u8)){.count = str__.count, .data = str__.data}
#define ARR_FROM_STR_SKIP_PREFIX(str__, prefixSize__) (ArraySlice(u8)){.count = str__.count - (prefixSize__), .data = str__.data + (prefixSize__)}
#define ARR_STR_LIT(str__) (ArraySlice(u8)){.count = sizeof(str__) - 1, .data = (u8*) str__}

void WriteOdnTypeName(PNSLR_File file, ArraySlice(DeclTypeInfo) types, u32 ty)
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
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("void"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("()"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("b8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u16"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("u64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i8"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i16"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("i64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("f32"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("f64"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("cstring"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("rawptr"), nameStr, 0)) { }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("utf8str"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("string"); }
            else if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("char"), nameStr, 0)) { nameStr = PNSLR_STRING_LITERAL("#error"); }
            else { skipPrefix = true; }

            PNSLR_WriteToFile(file, ARR_FROM_STR_SKIP_PREFIX(nameStr, (skipPrefix ? 6 : 0)));
            break;
        }
        case PolymorphicDeclType_Slice:
        {
            PNSLR_WriteToFile(file, ARR_STR_LIT("[]"));
            WriteOdnTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            break;
        }
        case PolymorphicDeclType_Ptr:
        {
            PNSLR_WriteToFile(file, ARR_STR_LIT("^"));
            WriteOdnTypeName(file, types, (u32) declTy.u.polyTgtIdx);
            break;
        }
        default: FORCE_DBG_TRAP; break;
    }
}

void GenerateOdnBindings(PNSLR_Path tgtDir, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_Path headerPath = PNSLR_GetPathForChildFile(tgtDir, PNSLR_STRING_LITERAL("Panshilar.odin"), allocator);
    PNSLR_File f = PNSLR_OpenFileToWrite(headerPath, false, false);

    utf8str prefixStr = PNSLR_StringFromCString(G_GenOdnPrefix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(prefixStr));

    for (ParsedFileContents* file = content->files; file != nil; file = file->next)
    {
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
            b8 isFunction = decl->type == DeclType_Function && !((ParsedFunction*) decl)->isDelegate;
            if (!isFunction && decl->doc.count > 0)
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
                    PNSLR_WriteToFile(f, ARR_STR_LIT("// declare "));
                    WriteOdnTypeName(f, content->types, arr->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    break;
                }
                case DeclType_TyAlias:
                {
                    ParsedTypeAlias* tyAl = (ParsedTypeAlias*) decl;
                    if (PNSLR_AreStringsEqual(PNSLR_STRING_LITERAL("utf8str"), tyAl->header.name, 0)) { PNSLR_WriteToFile(f, ARR_STR_LIT("// ")); }
                    WriteOdnTypeName(f, content->types, tyAl->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" :: "));
                    WriteOdnTypeName(f, content->types, tyAl->tgt);
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

                    if (enm->flags) // kept at the top because the doc is right before this.
                    {
                        WriteOdnTypeName(f, content->types, enm->header.ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" :: distinct bit_set["));
                        WriteOdnTypeName(f, content->types, enm->header.ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT("Values; "));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("]\n\n"));
                    }

                    WriteOdnTypeName(f, content->types, enm->header.ty);
                    if (enm->flags) PNSLR_WriteToFile(f, ARR_STR_LIT("Values"));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" :: enum "));
                    if (enm->flags) PNSLR_WriteToFile(f, ARR_STR_LIT("u8")); // don't need a larger space for flags container
                    else            PNSLR_WriteToFile(f, ARR_FROM_STR(backing));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" {\n"));
                    for (ParsedEnumVariant* var = enm->variants; var != nil; var = var->next)
                    {
                        if (enm->flags && !var->idx) continue;

                        PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(var->name, (enm->header.name.count + 1)));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" = "));
                        if (var->negative) { PNSLR_WriteToFile(f, ARR_STR_LIT("-")); }
                        char idxPrintBuff[16];
                        i32 idxPrintFilled = snprintf(idxPrintBuff, sizeof(idxPrintBuff), "%llu", enm->flags ? (u64) var->flagsOffset : var->idx);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) idxPrintFilled, .data = (u8*) idxPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(",\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("}\n"));
                    break;
                }
                case DeclType_Struct:
                {
                    ParsedStruct* strct = (ParsedStruct*) decl;

                    WriteOdnTypeName(f, content->types, strct->header.ty);
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" :: struct "));
                    if (strct->alignasVal != 0)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("#align("));
                        char alignPrintBuff[16];
                        i32 alignPrintFilled = snprintf(alignPrintBuff, sizeof(alignPrintBuff), "%d", strct->alignasVal);
                        PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) alignPrintFilled, .data = (u8*) alignPrintBuff});
                        PNSLR_WriteToFile(f, ARR_STR_LIT(") "));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" {\n"));
                    for (ParsedStructMember* member = strct->members; member != nil; member = member->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(member->name));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(": "));
                        if (member->arrSize > 0)
                        {
                            PNSLR_WriteToFile(f, ARR_STR_LIT("["));
                            char arrSizePrintBuff[32];
                            i32 arrSizePrintFilled = snprintf(arrSizePrintBuff, sizeof(arrSizePrintBuff), "%lld", member->arrSize);
                            PNSLR_WriteToFile(f, (ArraySlice(u8)){.count = (i64) arrSizePrintFilled, .data = (u8*) arrSizePrintBuff});
                            PNSLR_WriteToFile(f, ARR_STR_LIT("]"));
                        }
                        WriteOdnTypeName(f, content->types, member->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(",\n"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT("}\n"));

                    break;
                }
                case DeclType_Function:
                {
                    ParsedFunction* fn = (ParsedFunction*) decl;
                    if (!fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("@(link_prefix=\"PNSLR_\")\n"));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("foreign {\n"));
                        // append indented doc
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
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                    }
                    if (fn->isDelegate) WriteOdnTypeName(f, content->types, fn->header.ty);
                    else                PNSLR_WriteToFile(f, ARR_FROM_STR_SKIP_PREFIX(fn->header.name, 6));
                    PNSLR_WriteToFile(f, ARR_STR_LIT(" :: "));
                    if (fn->isDelegate) PNSLR_WriteToFile(f, ARR_STR_LIT("#type "));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("proc \"c\" ("));
                    if (fn->args != nil) PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    if (fn->args != nil && !fn->isDelegate) PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                    for (ParsedFnArg* arg = fn->args; arg != nil; arg = arg->next)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                        PNSLR_WriteToFile(f, ARR_FROM_STR(arg->name));
                        PNSLR_WriteToFile(f, ARR_STR_LIT(": "));
                        WriteOdnTypeName(f, content->types, arg->ty);
                        PNSLR_WriteToFile(f, ARR_STR_LIT(","));
                        PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                        if (!fn->isDelegate) PNSLR_WriteToFile(f, ARR_STR_LIT("\t"));
                    }
                    PNSLR_WriteToFile(f, ARR_STR_LIT(")"));
                    if (fn->retTy)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT(" -> "));
                        WriteOdnTypeName(f, content->types, fn->retTy);
                    }
                    if (!fn->isDelegate) PNSLR_WriteToFile(f, ARR_STR_LIT(" ---"));
                    PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
                    if (!fn->isDelegate)
                    {
                        PNSLR_WriteToFile(f, ARR_STR_LIT("}\n"));
                    }
                    break;
                }
                default: FORCE_DBG_TRAP; break;
            }
            PNSLR_WriteToFile(f, ARR_STR_LIT("\n"));
        }
    }

    utf8str suffixStr = PNSLR_StringFromCString(G_GenOdnSuffix);
    PNSLR_WriteToFile(f, ARR_FROM_STR(suffixStr));

    PNSLR_CloseFileHandle(f);
}

#undef ARR_STR_LIT
#undef ARR_FROM_STR_SKIP_PREFIX
#undef ARR_FROM_STR
