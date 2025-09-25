#ifndef PNSLR_COLLECTIONS_INTRINSICS_H // ==========================================
#define PNSLR_COLLECTIONS_INTRINSICS_H
#include "Primitives.h"
#include "Compiler.h"
#include "Macros.h"

//+skipreflect
#define PNSLR_ArraySlice(ty) PNSLR_ArraySlice_##ty

#ifdef __cplusplus

    template <typename T> struct ArraySlice { T* data; i64 count; };

    #define PNSLR_DECLARE_ARRAY_SLICE(ty) \
        typedef struct { ty* data; i64 count; } PNSLR_ArraySlice(ty); \
        EXTERN_C_END \

        template<> struct ArraySlice<ty> \
        { \
            ty* data; \
            i64 count; \
            ArraySlice<ty>() = default; \
            ArraySlice<ty>(i64 inCount, ty* inData) : count(inCount), data(inData) { } \
            ArraySlice<ty>(const PNSLR_ArraySlice(ty)& other) : count(other.count), data(other.data) { } \
            operator PNSLR_ArraySlice(ty)() const { return {data, count}; } \
        }; \

        EXTERN_C_BEGIN

#else

    #define PNSLR_DECLARE_ARRAY_SLICE(ty) \
        typedef union { struct { ty* data; i64 count; }; PNSLR_RawArraySlice raw; } PNSLR_ArraySlice(ty);

#endif
//-skipreflect

EXTERN_C_BEGIN

/**
 * A raw type-unspecific array slice.
 */
typedef struct PNSLR_RawArraySlice { rawptr data; i64 count; } PNSLR_RawArraySlice;

PNSLR_DECLARE_ARRAY_SLICE(  b8);
PNSLR_DECLARE_ARRAY_SLICE(  u8);
PNSLR_DECLARE_ARRAY_SLICE( u16);
PNSLR_DECLARE_ARRAY_SLICE( u32);
PNSLR_DECLARE_ARRAY_SLICE( u64);
PNSLR_DECLARE_ARRAY_SLICE(  i8);
PNSLR_DECLARE_ARRAY_SLICE( i16);
PNSLR_DECLARE_ARRAY_SLICE( i32);
PNSLR_DECLARE_ARRAY_SLICE( i64);
PNSLR_DECLARE_ARRAY_SLICE( f32);
PNSLR_DECLARE_ARRAY_SLICE( f64);
PNSLR_DECLARE_ARRAY_SLICE(char);

/**
 * UTF-8 string type, with length info (not necessarily null-terminated).
 */
typedef PNSLR_ArraySlice(u8) utf8str;
PNSLR_DECLARE_ARRAY_SLICE(utf8str);

//+skipreflect
#ifdef __cplusplus
    EXTERN_C_END

    namespace Panshilar
    {
        // Create a utf8str from a string literal.
        template <u64 N>
        constexpr utf8str StringLiteral(const char (&str)[N])
        {
            utf8str output;
            output.count = (i64) (N - 1);
            output.data = (u8*) str;
            return output;
        }
    }

    EXTERN_C_BEGIN
#else
    // Create a utf8str from a string literal.
    #define PNSLR_StringLiteral(str) \
        (utf8str) {.count = sizeof(str) - 1, .data = (u8*) str}
#endif
//-skipreflect

EXTERN_C_END

#endif // PNSLR_COLLECTIONS_INTRINSICS_H ===========================================
