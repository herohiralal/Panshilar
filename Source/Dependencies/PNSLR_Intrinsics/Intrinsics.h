/**
 * The goal of this file is to provide a set of intrinsic functions that are yoinked
 * from the standard library, so that we do not have to directly include the
 * standard library headers in our code.
 */

#ifndef PNSLR_INTRINSICS_H // ======================================================
#define PNSLR_INTRINSICS_H
#include "Primitives.h"
#include "Macros.h"
//+skipreflect

#define PNSLR_PTR_SIZE 8

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

DECLARE_ARRAY_SLICE(  b8);
DECLARE_ARRAY_SLICE(  u8);
DECLARE_ARRAY_SLICE( u16);
DECLARE_ARRAY_SLICE( u32);
DECLARE_ARRAY_SLICE( u64);
DECLARE_ARRAY_SLICE(  i8);
DECLARE_ARRAY_SLICE( i16);
DECLARE_ARRAY_SLICE( i32);
DECLARE_ARRAY_SLICE( i64);
DECLARE_ARRAY_SLICE( f32);
DECLARE_ARRAY_SLICE( f64);
DECLARE_ARRAY_SLICE(char);

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
