/**
 * The goal of this file is to provide a set of intrinsic functions that are yoinked
 * from the standard library, so that we do not have to directly include the
 * standard library headers in our code.
 */

#ifndef PNSLR_INTRINSICS_H // ======================================================
#define PNSLR_INTRINSICS_H

//+skipreflect
// Primitives ======================================================================

#if !defined(__cplusplus)
    typedef _Bool           b8;
#else
    typedef bool            b8;
#endif

typedef unsigned int        b32; // mainly for interop purposes with certain other libraries
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
typedef unsigned char       utf8ch;
typedef char*               cstring;
typedef void*               rawptr;

#undef nil
#if !defined(__cplusplus)
    #undef bool
    #undef false
    #undef true
#endif

#define nil   ((rawptr) 0)
#define false ((b8)     0)
#define true  ((b8)     1)

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

// Collections =====================================================================

#define ArraySlice(ty)  ArraySlice_##ty

#define DECLARE_ARRAY_SLICE(ty) \
    typedef struct { i64 count; ty* data; } ArraySlice(ty);

#define EMPTY_ARRAY_SLICE(ty) \
    (ArraySlice(ty)) { .count = 0, .data = nil }

DECLARE_ARRAY_SLICE(     b8);
DECLARE_ARRAY_SLICE(    b32);
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
DECLARE_ARRAY_SLICE( utf8ch);
DECLARE_ARRAY_SLICE(cstring);

// UTF-8 string type, with length info (not necessarily null-terminated).
typedef ArraySlice(utf8ch) utf8str;
DECLARE_ARRAY_SLICE(utf8str);

// Create a utf8str from a string literal.
#define PNSLR_STRING_LITERAL(str) \
    (utf8str) \
    { \
        .count = sizeof(str) - 1, \
        .data = (utf8ch*) str \
    }

// Macros ==========================================================================

#if defined(offsetof)
    #undef offsetof // avoid conflict with stddef.h, if somehow inherited
#endif

#if PNSLR_MSVC

        #define thread_local            __declspec(thread)
        #define noinline                __declspec(noinline)
        #define forceinline             __forceinline
        #define noreturn                __declspec(noreturn)

#elif (PNSLR_CLANG || PNSLR_GCC)

        #define noinline                __attribute__((noinline))
        #define forceinline             inline __attribute__((always_inline))
        #define noreturn                __attribute__((noreturn))
        #define offsetof(type, member)  __builtin_offsetof(type, member)

#else
    #error "Required features not supported by this compiler."
#endif

#ifdef __cplusplus

    #if (PNSLR_CLANG || PNSLR_GCC)
        #define thread_local           thread_local
    #endif

    #define inline                     inline
    #define alignas(x)                 alignas(x)
    #define alignof(type)              alignof(type)
    #define deprecated                 [[deprecated]]

    #if PNSLR_MSVC
        #define offsetof(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
    #endif

    #define static_assert              static_assert

#else

    #if PNSLR_MSVC

        #define inline                  __inline
        #define alignas(x)              __declspec(align(x))
        #define alignof(type)           __alignof(type)
        #define deprecated              __declspec(deprecated)
        #define offsetof(type, member)  ((unsigned __int64)&(((type*)0)->member))

    #elif (PNSLR_CLANG || PNSLR_GCC)

        #define thread_local            __thread
        #define inline                  __inline__
        #define alignas(x)              __attribute__((aligned(x)))
        #define alignof(type)           __alignof__(type)
        #define deprecated              __attribute__((deprecated))

    #else
        #error "Required features not supported by this compiler."
    #endif

    #define static_assert _Static_assert

#endif

#define ENUM_START(name, backingType)       typedef backingType name;
#define ENUM_FLAGS_START(name, backingType) typedef backingType name;
#define ENUM_END
#define PNSLR_PTR_SIZE                      8

#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END   }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif

//-skipreflect

// Memory Management ===============================================================

EXTERN_C_BEGIN

/**
 * Delegate type for thread cleanup functions.
 * This is used to register cleanup functions that will be called when the thread exits.
 */
typedef rawptr (*PNSLR_Intrinsic_ThreadCleanupDelegate)(void);

/**
 * Register a thread cleanup function.
 * This function will be called when the thread exits.
 * There's a hard-limit of 8 thread cleanup functions that can be registered.
 */
void PNSLR_Intrinsic_RegisterThreadCleanup(PNSLR_Intrinsic_ThreadCleanupDelegate delegate);

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
void PNSLR_Intrinsic_MemCopy(rawptr destination, const rawptr source, i32 size);

/**
 * Copy a block of memory from source to destination, handling overlapping regions.
 */
void PNSLR_Intrinsic_MemMove(rawptr destination, const rawptr source, i32 size);

EXTERN_C_END

#endif // PNSLR_INTRINSICS_H =======================================================
