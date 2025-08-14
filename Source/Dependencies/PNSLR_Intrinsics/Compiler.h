#ifdef __cplusplus
    #error "C++ is not supported for these headers. Please use one of the auto-generated headers."
#endif

#ifndef PNSLR_COMPILER_INTRINSICS_H // =============================================
#define PNSLR_COMPILER_INTRINSICS_H

#if defined(__clang__)

    #define PRAGMA_SUPPRESS_WARNINGS \
        _Pragma("clang diagnostic push")  \
        _Pragma("clang diagnostic ignored \"-Weverything\"")

    #define PRAGMA_REENABLE_WARNINGS \
        _Pragma("clang diagnostic pop")

    #define PNSLR_CLANG 1

#elif defined(__GNUC__)

    #define PRAGMA_SUPPRESS_WARNINGS \
        _Pragma("GCC diagnostic push")  \
        _Pragma("GCC diagnostic ignored \"-Weverything\"")

    #define PRAGMA_REENABLE_WARNINGS \
        _Pragma("GCC diagnostic pop")

    #define PNSLR_GCC 1

#elif defined(_MSC_VER)

    #define PRAGMA_SUPPRESS_WARNINGS \
        __pragma(warning(push, 0))

    #define PRAGMA_REENABLE_WARNINGS \
        __pragma(warning(pop))

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

#endif // PNSLR_COMPILER_INTRINSICS_H ==============================================
