#ifndef PNSLR_MACRO_INTRINSICS_H // ================================================
#define PNSLR_MACRO_INTRINSICS_H
//+skipreflect
#include "Compiler.h"

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

    // thread_local is used as-is
    // inline       is used as-is
    // alignas      is used as-is
    // alignof      is used as-is
    #define deprecated                 [[deprecated]]

    #if PNSLR_MSVC
        #define offsetof(type, member) ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
    #endif

    // static_assert is used as-is

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

#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END   }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif

//-skipreflect
#endif // PNSLR_MACRO_INTRINSICS_H =================================================
