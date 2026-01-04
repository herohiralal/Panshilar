#ifndef PNSLR_PLATFORM_INTRINSICS_H // =============================================
#define PNSLR_PLATFORM_INTRINSICS_H
//+skipreflect
#include "Warnings.h"

// configuration

#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
    #define PNSLR_DBG 1
#else
    #define PNSLR_REL 1
#endif

#ifndef PNSLR_DBG
    #define PNSLR_DBG 0
#endif
#ifndef PNSLR_REL
    #define PNSLR_REL 0
#endif

// platform

#if defined(_WIN32) || defined(_WIN64)
    #define PNSLR_WINDOWS 1
#elif defined(__APPLE__)
    PNSLR_SUPPRESS_WARN
        #include <TargetConditionals.h>
    PNSLR_UNSUPPRESS_WARN

    #if TARGET_OS_IPHONE
        #define PNSLR_IOS 1
    #else
        #define PNSLR_OSX 1
    #endif
#elif defined(__linux__)
    #if defined(__ANDROID__)
        #define PNSLR_ANDROID 1
    #else
        #define PNSLR_LINUX 1
    #endif
#endif

#ifndef PNSLR_WINDOWS
    #define PNSLR_WINDOWS 0
#endif
#ifndef PNSLR_LINUX
    #define PNSLR_LINUX 0
#endif
#ifndef PNSLR_OSX
    #define PNSLR_OSX 0
#endif
#ifndef PNSLR_ANDROID
    #define PNSLR_ANDROID 0
#endif
#ifndef PNSLR_IOS
    #define PNSLR_IOS 0
#endif
#ifndef PNSLR_PS5
    #define PNSLR_PS5 0
#endif
#ifndef PNSLR_XSERIES
    #define PNSLR_XSERIES 0
#endif
#ifndef PNSLR_SWITCH
    #define PNSLR_SWITCH 0
#endif

// architecture

#if defined(__x86_64__) || defined(_M_X64)
    #define PNSLR_X64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PNSLR_ARM64 1
#endif

#ifndef PNSLR_X64
    #define PNSLR_X64 0
#endif
#ifndef PNSLR_ARM64
    #define PNSLR_ARM64 0
#endif

// derived

#define PNSLR_UNIX    (PNSLR_LINUX || PNSLR_OSX || PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_APPLE   (PNSLR_OSX || PNSLR_IOS)
#define PNSLR_DESKTOP (PNSLR_WINDOWS || PNSLR_LINUX || PNSLR_OSX)
#define PNSLR_MOBILE  (PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_CONSOLE (PNSLR_PS5 || PNSLR_XSERIES || PNSLR_SWITCH)

#if (PNSLR_DBG + PNSLR_REL) != 1
    #error "Exactly one configuration must be defined."
#endif

#if (PNSLR_WINDOWS + PNSLR_LINUX + PNSLR_OSX + PNSLR_ANDROID + PNSLR_IOS + PNSLR_PS5 + PNSLR_XSERIES + PNSLR_SWITCH) != 1
    #error "Exactly one platform must be defined."
#endif

#if (PNSLR_X64 + PNSLR_ARM64) != 1
    #error "Exactly one architecture must be defined."
#endif

//-skipreflect
#endif // PNSLR_PLATFORM_INTRINSICS_H ==============================================
