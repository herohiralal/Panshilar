#ifndef PNSLR_SRC_PARSER_H
#define PNSLR_SRC_PARSER_H
#include "Panshilar.h"

// base ============================================================================

ENUM_START(DeclType, u8)
    #define DeclType_Invalid  ((DeclType) 0)
    #define DeclType_Section  ((DeclType) 1)
    #define DeclType_Enum     ((DeclType) 2)
    #define DeclType_Struct   ((DeclType) 3)
    #define DeclType_Function ((DeclType) 4)
ENUM_END

typedef struct DeclHeader
{
    DeclType           type;
    struct DeclHeader* next;
    utf8str            name;
    utf8str            doc;
} DeclHeader;

// section =========================================================================

typedef struct
{
    DeclHeader header;
} ParsedSection;

// enums ===========================================================================

typedef struct ParsedEnumVariant
{
    utf8str name;
    u64     idx;
    b8      negative;
} ParsedEnumVariant;

typedef struct
{
    DeclHeader         header;
    u8                 size;
    b8                 negative;
    ParsedEnumVariant* variants; // linked list
} ParsedEnum;

// structs =========================================================================

ENUM_START(PolymorphicTypeDeclType, u8)
    #define PolymorphicDeclType_None   ((PolymorphicTypeDeclType) 0)
    #define PolymorphicDeclType_Ptr    ((PolymorphicTypeDeclType) 1)
    #define PolymorphicDeclType_Slice  ((PolymorphicTypeDeclType) 2)
ENUM_END

typedef struct
{
    PolymorphicTypeDeclType polyTy;
    union
    {
        utf8str             name;
        i64                 polyTgtIdx; // if the type is ptr/slice,
                                        // this will hold the target index
                                        // of the type; -1 otherwise
    } u;
} DeclTypeInfo;

DECLARE_ARRAY_SLICE(DeclTypeInfo);

typedef struct ParsedStructMember
{
    struct ParsedStructMember* next;
    DeclTypeInfo               ty;
    utf8str                    name;
} ParsedStructMember;

typedef struct
{
    DeclHeader          header;
    ParsedStructMember* members; // linked list
} ParsedStruct;

// function ========================================================================

typedef struct ParsedFnArg
{
    struct ParsedFnArg* next;
    DeclTypeInfo        ty;
    utf8str             name;
} ParsedFnArg;

typedef struct
{
    DeclHeader   header;
    b8           isDelegate;
    DeclTypeInfo retTy;
    ParsedFnArg* args; // linked list
} ParsedFunction;

// overall =========================================================================

typedef struct ParsedFileContents
{
    struct ParsedFileContents* next;
    utf8str                    name;
    utf8str                    doc;
    DeclHeader*                declarations; // polymorphic linked list
} ParsedFileContents;

typedef struct
{
    ArraySlice(DeclTypeInfo) types;
    i64                      typesCount;
    ParsedFileContents*      files; // linked list
} ParsedContent;

// some cached 'current' addresses for the linked lists that'll be updated on successful parsing
typedef struct
{
    ParsedFileContents* lastFile;
    DeclHeader*         lastDecl;
    ParsedEnumVariant*  lastVariant;
    ParsedStructMember* lastMember;
    ParsedFnArg*        lastFnArg;
} CachedLasts;

ArraySlice(DeclTypeInfo) BuildTypeTable(PNSLR_Allocator allocator, i64* count);

b8 ProcessFile(ParsedContent* parsedContent, CachedLasts* cachedLasts, utf8str pathRel, ArraySlice(u8) contents, PNSLR_Allocator allocator);

#endif
