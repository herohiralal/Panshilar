/**
 * The goal of this file is to provide a set of intrinsic functions that are yoinked
 * from the standard library, so that we do not have to directly include the
 * standard library headers in our code.
 */

#ifndef PNSLR_INTRINSICS_H // ======================================================
#define PNSLR_INTRINSICS_H
//+skipreflect

// Primitives ======================================================================

#ifdef __cplusplus
    typedef bool            b8;
#else
    typedef _Bool           b8;
#endif

typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef signed char         i8;
typedef signed short int    i16;
typedef signed int          i32;
typedef signed long long    i64;
typedef float               f32;
typedef double              f64;
typedef void*               rawptr;
typedef char*               cstring;

#ifdef __cplusplus
    #undef  nil

    #define nil   (   nullptr)
#else

    #undef  nil
    #undef  bool
    #undef  false
    #undef  true

    #define nil   ((rawptr) 0)
    #define false ((b8)     0)
    #define true  ((b8)     1)

#endif

#define U8_MIN  ((u8)  (0))
#define U8_MAX  ((u8)  (255))
#define U16_MIN ((u16) (0))
#define U16_MAX ((u16) (65535))
#define U32_MIN ((u32) (0))
#define U32_MAX ((u32) (4294967295U))
#define U64_MIN ((u64) (0))
#define U64_MAX ((u64) (18446744073709551615ULL))
#define I8_MIN  ((i8)  (-128))
#define I8_MAX  ((i8)  (127))
#define I16_MIN ((i16) ((-32768)))
#define I16_MAX ((i16) (32767))
#define I32_MIN ((i32) ((-2147483648)))
#define I32_MAX ((i32) (2147483647))
#define I64_MIN ((i64) ((-9223372036854775807LL - 1)))
#define I64_MAX ((i64) (9223372036854775807LL))
#define F32_MIN ((f32) ((-3.402823466e+38F)))
#define F32_MAX ((f32) (3.402823466e+38F))
#define F64_MIN ((f64) ((-1.7976931348623157e+308)))
#define F64_MAX ((f64) (1.7976931348623157e+308))

// Macros ==========================================================================

#include "Macros.h"

#define PNSLR_PTR_SIZE                      8

// Collections =====================================================================

#define ArraySlice(ty) ArraySlice_##ty

#ifdef __cplusplus

    template <typename T> struct ArraySlice { T* data; i64 count; };

    #define DECLARE_ARRAY_SLICE(ty) \
        typedef struct { ty* data; i64 count; } ArraySlice(ty); \
        EXTERN_C_END \
        template<> struct ArraySlice<ty> \
        { \
            ty* data; \
            i64 count; \
            ArraySlice<ty>() = default; \
            ArraySlice<ty>(i64 inCount, ty* inData) : count(inCount), data(inData) { } \
            ArraySlice<ty>(const ArraySlice(ty)& other) : count(other.count), data(other.data) { } \
            operator ArraySlice(ty)() const { return {data, count}; } \
        }; \
        EXTERN_C_BEGIN

#else

    #define DECLARE_ARRAY_SLICE(ty) \
        typedef union { struct { ty* data; i64 count; }; PNSLR_RawArraySlice raw; } ArraySlice(ty);

#endif

//-skipreflect
EXTERN_C_BEGIN

// Primitive Collections  ==========================================================

/**
 * A raw type-unspecific array slice.
 */
typedef struct PNSLR_RawArraySlice { rawptr data; i64 count; } PNSLR_RawArraySlice;

DECLARE_ARRAY_SLICE(     b8);
DECLARE_ARRAY_SLICE(     u8);
DECLARE_ARRAY_SLICE(    u16);
DECLARE_ARRAY_SLICE(    u32);
DECLARE_ARRAY_SLICE(    u64);
DECLARE_ARRAY_SLICE(     i8);
DECLARE_ARRAY_SLICE(    i16);
DECLARE_ARRAY_SLICE(    i32);
DECLARE_ARRAY_SLICE(    i64);
DECLARE_ARRAY_SLICE(    f32);
DECLARE_ARRAY_SLICE(    f64);
DECLARE_ARRAY_SLICE(   char);

/**
 * UTF-8 string type, with length info (not necessarily null-terminated).
 */
typedef ArraySlice(u8) utf8str;
DECLARE_ARRAY_SLICE(utf8str);

//+skipreflect

#ifdef __cplusplus
    #define PNSLR_STRING_LITERAL(str) \
        utf8str {(u8*) str, sizeof(str) - 1}
#else
    // Create a utf8str from a string literal.
    #define PNSLR_STRING_LITERAL(str) \
        (utf8str) {.count = sizeof(str) - 1, .data = (u8*) str}
#endif

//-skipreflect

// Memory Management ===============================================================

/**
 * Allocate memory with the specified alignment and size.
 */
rawptr PNSLR_Intrinsic_Malloc(i32 alignment, i32 size);

/**
 * Free memory allocated with PNSLR_Intrinsic_Malloc.
 */
void PNSLR_Intrinsic_Free(rawptr memory);

/**
 * Set a block of memory to a specific value.
 */
void PNSLR_Intrinsic_MemSet(rawptr memory, i32 value, i32 size);

/**
 * Copy a block of memory from source to destination.
 */
void PNSLR_Intrinsic_MemCopy(rawptr destination, rawptr source, i32 size);

/**
 * Copy a block of memory from source to destination, handling overlapping regions.
 */
void PNSLR_Intrinsic_MemMove(rawptr destination, rawptr source, i32 size);

EXTERN_C_END

#endif // PNSLR_INTRINSICS_H =======================================================
