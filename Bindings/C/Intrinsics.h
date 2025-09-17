#ifndef PNSLR_INTRINSICS_MAIN_H
#define PNSLR_INTRINSICS_MAIN_H

/** An array slice of type 'ty'. */
#define PNSLR_ArraySlice(ty) PNSLR_ArraySlice_##ty

/** Declare an array slice of type 'ty'. */
#define PNSLR_DECLARE_ARRAY_SLICE(ty) \
    typedef union PNSLR_ArraySlice(ty) { struct { ty* data; i64 count; }; PNSLR_RawArraySlice raw; } PNSLR_ArraySlice(ty);

typedef unsigned char       b8;
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
typedef char*               cstring;
typedef void*               rawptr;


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus)
    #define PNSLR_ALIGNAS(x) alignas(x)
#elif defined(_MSC_VER)
    #define PNSLR_ALIGNAS(x) __declspec(align(x))
#elif defined(__clang__) || defined(__GNUC__)
    #define PNSLR_ALIGNAS(x) __attribute__((aligned(x)))
#else
    #error "UNSUPPORTED COMPILER!";
#endif

// #######################################################################################
// Collections
// #######################################################################################

/**
 * A raw type-unspecific array slice.
 */
typedef struct PNSLR_RawArraySlice
{
    rawptr data;
    i64 count;
} PNSLR_RawArraySlice;

PNSLR_DECLARE_ARRAY_SLICE(b8);

PNSLR_DECLARE_ARRAY_SLICE(u8);

PNSLR_DECLARE_ARRAY_SLICE(u16);

PNSLR_DECLARE_ARRAY_SLICE(u32);

PNSLR_DECLARE_ARRAY_SLICE(u64);

PNSLR_DECLARE_ARRAY_SLICE(i8);

PNSLR_DECLARE_ARRAY_SLICE(i16);

PNSLR_DECLARE_ARRAY_SLICE(i32);

PNSLR_DECLARE_ARRAY_SLICE(i64);

PNSLR_DECLARE_ARRAY_SLICE(f32);

PNSLR_DECLARE_ARRAY_SLICE(f64);

PNSLR_DECLARE_ARRAY_SLICE(char);

/**
 * UTF-8 string type, with length info (not necessarily null-terminated).
 */
typedef PNSLR_ArraySlice(u8) utf8str;

PNSLR_DECLARE_ARRAY_SLICE(utf8str);

#undef PNSLR_ALIGNAS

#ifdef __cplusplus
} // extern c
#endif


#endif//PNSLR_INTRINSICS_MAIN_H

#ifndef PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
#define PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
    #if !defined(__cplusplus) && !defined(static_assert)
        #define static_assert _Static_assert
        #define PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
    #endif

    static_assert(sizeof(b8 ) == 1, "Size mismatch.");
    static_assert(sizeof(u8 ) == 1, "Size mismatch.");
    static_assert(sizeof(u16) == 2, "Size mismatch.");
    static_assert(sizeof(u32) == 4, "Size mismatch.");
    static_assert(sizeof(u64) == 8, "Size mismatch.");
    static_assert(sizeof(i8 ) == 1, "Size mismatch.");
    static_assert(sizeof(i16) == 2, "Size mismatch.");
    static_assert(sizeof(i32) == 4, "Size mismatch.");
    static_assert(sizeof(i64) == 8, "Size mismatch.");
    static_assert(sizeof(f32) == 4, "Size mismatch.");
    static_assert(sizeof(f64) == 8, "Size mismatch.");

    #ifdef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
        #undef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
        #undef static_assert
    #endif
#endif//PNSLR_SKIP_PRIMITIVE_SIZE_TESTS
