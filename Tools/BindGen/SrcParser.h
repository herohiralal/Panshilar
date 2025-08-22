#ifndef PNSLR_SRC_PARSER_H
#define PNSLR_SRC_PARSER_H
#include "Panshilar.h"

DECLARE_ARRAY_SLICE(DeclTypeInfo);

// base ============================================================================

ENUM_START(DeclType, u8)
    #define DeclType_Invalid  ((DeclType) 0)
    #define DeclType_Enum     ((DeclType) 1)
    #define DeclType_Struct   ((DeclType) 2)
    #define DeclType_Function ((DeclType) 3)
ENUM_END

typedef struct DeclHeader
{
    DeclType           type;
    struct DeclHeader* next;
    utf8str            name;
    utf8str            doc;
} DeclHeader;

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

typedef struct
{
    utf8str name;
    i64     sliceTgtIdx; // if the type is a slice,
                         // this will hold the target index
                         // of the type; -1 otherwise
} DeclTypeInfo;

typedef struct ParsedStructMember
{
    DeclTypeInfo               ty;
    utf8str                    name;
    struct ParsedStructMember* next;
} ParsedStructMember;

typedef struct
{
    DeclHeader          header;
    ParsedStructMember* members; // linked list
} ParsedStruct;

// function ========================================================================

typedef struct ParsedFnArg
{
    DeclTypeInfo        ty;
    utf8str             name;
    struct ParsedFnArg* next;
} ParsedFnArg;

typedef struct
{
    DeclHeader   header;
    b8           isDelegate;
    DeclTypeInfo retTy;
    ParsedFnArg* args; // linked list
} ParsedFunction;

// overall =========================================================================

typedef struct
{
    utf8str     name;
    utf8str     doc;
    DeclHeader* declarations; // polymorphic linked list
} ParsedFileContents;

typedef struct
{
    ArraySlice(DeclTypeInfo) types;
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

b8 ProcessFile(utf8str pathRel, ArraySlice(u8) contents, PNSLR_Allocator allocator);

#endif
