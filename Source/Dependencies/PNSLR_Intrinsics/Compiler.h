#ifndef PNSLR_COMPILER_INTRINSICS_H // =============================================
#define PNSLR_COMPILER_INTRINSICS_H
//+skipreflect

#if defined(__clang__)
    #define PNSLR_CLANG 1
#elif defined(__GNUC__)
    #define PNSLR_GCC 1
#elif defined(_MSC_VER)
    #define PNSLR_MSVC 1
#else
    #error "Unsupported compiler."
#endif

#ifndef PNSLR_CLANG
    #define PNSLR_CLANG 0
#endif
#ifndef PNSLR_GCC
    #define PNSLR_GCC 0
#endif
#ifndef PNSLR_MSVC
    #define PNSLR_MSVC 0
#endif

#if PNSLR_MSVC
    #pragma warning(disable: 4100) // unreferenced formal parameter
    #pragma warning(disable: 5045) // spectre mitigation
    #pragma warning(disable: 4324) // structure was padded due to alignment specifier
    #pragma warning(disable: 4820) // bytes padding added after data member
    #pragma warning(disable: 4127) // conditional expression is constant

    #define PRAGMA_SUPPRESS_WARNINGS \
        __pragma(warning(push, 0))

    #define PRAGMA_REENABLE_WARNINGS \
        __pragma(warning(pop))
#endif

#if PNSLR_GCC
    #pragma GCC diagnostic error   "-Wall"
    #pragma GCC diagnostic error   "-Wextra"
    #pragma GCC diagnostic error   "-Wshadow"
    #pragma GCC diagnostic error   "-Wconversion"
    #pragma GCC diagnostic error   "-Wsign-conversion"
    #pragma GCC diagnostic error   "-Wdouble-promotion"
    #pragma GCC diagnostic error   "-Wfloat-equal"
    #pragma GCC diagnostic error   "-Wundef"
    #pragma GCC diagnostic error   "-Wswitch-enum"
    #pragma GCC diagnostic error   "-Wstrict-prototypes"
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    #define PRAGMA_SUPPRESS_WARNINGS \
        _Pragma("GCC diagnostic push")  \
        _Pragma("GCC diagnostic ignored \"-Weverything\"")

    #define PRAGMA_REENABLE_WARNINGS \
        _Pragma("GCC diagnostic pop")
#endif

#if PNSLR_CLANG
    #pragma clang diagnostic error   "-Wall"
    #pragma clang diagnostic error   "-Wextra"
    #pragma clang diagnostic error   "-Wshadow"
    #pragma clang diagnostic error   "-Wconversion"
    #pragma clang diagnostic error   "-Wsign-conversion"
    #pragma clang diagnostic error   "-Wdouble-promotion"
    #pragma clang diagnostic error   "-Wfloat-equal"
    #pragma clang diagnostic error   "-Wundef"
    #pragma clang diagnostic error   "-Wswitch-enum"
    #pragma clang diagnostic error   "-Wstrict-prototypes"
    #pragma clang diagnostic ignored "-Wunused-parameter"

    #define PRAGMA_SUPPRESS_WARNINGS \
        _Pragma("clang diagnostic push")  \
        _Pragma("clang diagnostic ignored \"-Weverything\"")

    #define PRAGMA_REENABLE_WARNINGS \
        _Pragma("clang diagnostic pop")
#endif

//-skipreflect
#endif // PNSLR_COMPILER_INTRINSICS_H ==============================================
