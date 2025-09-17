#ifndef PNSLR_PRIMITIVE_INTRINSICS_H // ============================================
#define PNSLR_PRIMITIVE_INTRINSICS_H
//+skipreflect

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
#define I32_MIN ((i32) ((-2147483647 - 1)))
#define I32_MAX ((i32) (2147483647))
#define I64_MIN ((i64) ((-9223372036854775807LL - 1)))
#define I64_MAX ((i64) (9223372036854775807LL))
#define F32_MIN ((f32) ((-3.402823466e+38F)))
#define F32_MAX ((f32) (3.402823466e+38F))
#define F64_MIN ((f64) ((-1.7976931348623157e+308)))
#define F64_MAX ((f64) (1.7976931348623157e+308))

#define PNSLR_PTR_SIZE 8

#if !defined(__cplusplus) && !defined(static_assert)
    #define static_assert _Static_assert
    #define PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
#endif

static_assert(sizeof(b8)      == 1, " b8 must be 1 byte ");
static_assert(sizeof(u8)      == 1, " u8 must be 1 byte ");
static_assert(sizeof(i8)      == 1, " i8 must be 1 byte ");
static_assert(sizeof(u16)     == 2, "u16 must be 2 bytes");
static_assert(sizeof(i16)     == 2, "i16 must be 2 bytes");
static_assert(sizeof(u32)     == 4, "u32 must be 4 bytes");
static_assert(sizeof(i32)     == 4, "i32 must be 4 bytes");
static_assert(sizeof(f32)     == 4, "f32 must be 4 bytes");
static_assert(sizeof(u64)     == 8, "u64 must be 8 bytes");
static_assert(sizeof(i64)     == 8, "i64 must be 8 bytes");
static_assert(sizeof(f64)     == 8, "f64 must be 8 bytes");
static_assert(sizeof(rawptr)  == 8, "ptr must be 8 bytes");
static_assert(PNSLR_PTR_SIZE  == 8, "ptr must be 8 bytes"); // keep in sync with sizeof(rawptr)

#ifdef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
    #undef PNSLR_INTRINSIC_CUSTOM_TEMP_STATIC_ASSERT
    #undef static_assert
#endif

//-skipreflect
#endif // PNSLR_PRIMITIVE_INTRINSICS_H =============================================
