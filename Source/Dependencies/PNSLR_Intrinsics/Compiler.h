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

#if (PNSLR_CLANG + PNSLR_GCC + PNSLR_MSVC) != 1
    #error "Exactly one compiler must be defined."
#endif

//-skipreflect
#endif // PNSLR_COMPILER_INTRINSICS_H ==============================================
