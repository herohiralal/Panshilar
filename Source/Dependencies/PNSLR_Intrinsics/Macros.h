#ifndef PNSLR_MACRO_INTRINSICS_H // ================================================
#define PNSLR_MACRO_INTRINSICS_H
//+skipreflect
#include "Compiler.h"

#if PNSLR_MSVC

    #define PNSLR_NOINLINE                    __declspec(noinline)
    #define PNSLR_FORCEINLINE                 __forceinline
    #define PNSLR_NORETURN                    __declspec(noreturn)
    // offsetof impl depends on c/c++

#elif (PNSLR_CLANG || PNSLR_GCC)

    #define PNSLR_NOINLINE                    __attribute__((noinline))
    #define PNSLR_FORCEINLINE                 inline __attribute__((always_inline))
    #define PNSLR_NORETURN                    __attribute__((noreturn))
    #define PNSLR_OFFSETOF(type, member)      __builtin_offsetof(type, member)

#else
    #error "Required features not supported by this compiler."
#endif

#ifdef __cplusplus

    // thread_local is used as-is
    // inline       is used as-is
    // alignas      is used as-is
    // alignof      is used as-is
    #define PNSLR_DEPRECATED                  [[deprecated]]

    #if PNSLR_MSVC
        #define PNSLR_OFFSETOF(type, member)  ((u64)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
    #endif

    // static_assert is used as-is

#else

    #if PNSLR_MSVC

        #define thread_local                  __declspec(thread)
        #define inline                        __inline
        #define alignas(x)                    __declspec(align(x))
        #define alignof(type)                 __alignof(type)
        #define PNSLR_DEPRECATED              __declspec(deprecated)
        #define PNSLR_OFFSETOF(type, member)  ((unsigned __int64)&(((type*)0)->member))

    #elif (PNSLR_CLANG || PNSLR_GCC)

        #define thread_local                  __thread
        #define inline                        __inline__
        #define alignas(x)                    __attribute__((aligned(x)))
        #define alignof(type)                 __alignof__(type)
        #define PNSLR_DEPRECATED              __attribute__((deprecated))
        // offsetof declared previously

    #else
        #error "Required features not supported by this compiler."
    #endif

    #define static_assert                    _Static_assert

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

#ifdef __cplusplus
    #define OPT_ARG = { }
#else
    #define OPT_ARG
#endif

#if PNSLR_MSVC

    #define FORCE_TRAP     __fastfail(FAST_FAIL_FATAL_APP_EXIT)
    #define FORCE_DBG_TRAP __debugbreak()

#elif (PNSLR_CLANG || PNSLR_GCC)

    #define FORCE_TRAP     __builtin_trap()
    #define FORCE_DBG_TRAP __builtin_debugtrap()

#else
    #error "Required features not supported by this compiler."
#endif

#define COUNTVARARGS_INTERNAL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, COUNT, ...) COUNT
#define COUNT_VARARGS(...) COUNTVARARGS_INTERNAL(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

//-skipreflect
#endif // PNSLR_MACRO_INTRINSICS_H =================================================
