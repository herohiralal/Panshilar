#ifndef PNSLR_ENTRY_H // ===========================================================
#define PNSLR_ENTRY_H
//+skipreflect

// Environment setup ===============================================================

// platform

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

// Includes ========================================================================

// always include this first and outside the implementation block
// it contains some important macros that we'll use
#include "Dependencies/PNSLR_Intrinsics/Compiler.h"

#ifdef PNSLR_IMPLEMENTATION

    PRAGMA_SUPPRESS_WARNINGS

    #if PNSLR_WINDOWS
        #define WIN32_LEAN_AND_MEAN
        #include <Windows.h>
        #include <WinSock2.h>
        #include <ws2ipdef.h>
        #include <iphlpapi.h>
        #include <intrin.h>
        #undef WIN32_LEAN_AND_MEAN
    #endif

    #if PNSLR_UNIX

        // since we're on C11
        #define _POSIX_C_SOURCE 200809L
        #define _XOPEN_SOURCE 700

        #include <stdlib.h>
        #include <unistd.h>
        #include <fcntl.h>
        #include <ifaddrs.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <sys/stat.h>
        #include <sys/wait.h>
        #include <sys/mman.h>
        #include <netinet/in.h>
        #include <errno.h>
        #include <dirent.h>
        #include <pthread.h>
        #include <semaphore.h>
    #endif

    #if PNSLR_APPLE
        #include <dispatch/dispatch.h>
    #endif

    #if PNSLR_ANDROID
        #include <android/log.h>
        #include <android/asset_manager.h>
        #include <android/native_activity.h>
    #endif

    PRAGMA_REENABLE_WARNINGS

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


// always include this last and outside the implementation block
// it contains some important macros/typedefs that we'll use
#include "Dependencies/PNSLR_Intrinsics/Intrinsics.h"

// Static tests ====================================================================

// auto alignment check
typedef struct { i32 a; i32 b; } PNSLR_StaticTests_AutoAlignCheck;
static_assert(offsetof(PNSLR_StaticTests_AutoAlignCheck, a) == 0, "Offset of 'a' should be 0");
static_assert(offsetof(PNSLR_StaticTests_AutoAlignCheck, b) == 4, "Offset of 'b' should be 4");

// manual alignment check
typedef struct alignas(16) { i32 a; i32 b; } PNSLR_StaticTests_ManualAlignCheck;
static_assert(alignof(PNSLR_StaticTests_ManualAlignCheck) == 16, "Custom alignment should be 16 bytes");

// ensure all compiler macros are defined, and exactly one is set
static_assert((PNSLR_CLANG + PNSLR_GCC + PNSLR_MSVC) == 1, "Exactly one compiler must be defined.");

// ensure all platform macros are defined, and exactly one is set
static_assert((PNSLR_WINDOWS + PNSLR_LINUX + PNSLR_OSX + PNSLR_ANDROID + PNSLR_IOS + PNSLR_PS5 + PNSLR_XSERIES + PNSLR_SWITCH) == 1, "Exactly one platform must be defined.");

// ensure all architecture macros are defined, and exactly one is set
static_assert((PNSLR_X64 + PNSLR_ARM64) == 1, "Exactly one architecture must be defined.");

// primitive type size assertions
static_assert(sizeof(b8)      == 1, "     b8 must be 1 byte ");
static_assert(sizeof(u8)      == 1, "     u8 must be 1 byte ");
static_assert(sizeof(i8)      == 1, "     i8 must be 1 byte ");
static_assert(sizeof(u16)     == 2, "    u16 must be 2 bytes");
static_assert(sizeof(i16)     == 2, "    i16 must be 2 bytes");
static_assert(sizeof(u32)     == 4, "    u32 must be 4 bytes");
static_assert(sizeof(i32)     == 4, "    i32 must be 4 bytes");
static_assert(sizeof(f32)     == 4, "    f32 must be 4 bytes");
static_assert(sizeof(u64)     == 8, "    u64 must be 8 bytes");
static_assert(sizeof(i64)     == 8, "    i64 must be 8 bytes");
static_assert(sizeof(f64)     == 8, "    f64 must be 8 bytes");
static_assert(sizeof(rawptr)  == 8, "    ptr must be 8 bytes");
static_assert(PNSLR_PTR_SIZE  == 8, "    ptr must be 8 bytes"); // keep in sync with sizeof(rawptr)

// assert msvc toolchain for windows
// will not be supporting MinGW or anything else because so much Windows-specific
// stuff relies very directly on MSVC toolchain features
// eventually also want to add some kind of rendering etc. in a future library
// for this to link with that, MSVC is the only option
#if PNSLR_WINDOWS != defined(_MSC_VER)
    #error "MSVC toolchain is required for Windows platform."
#endif

//-skipreflect
#endif // PNSLR_ENTRY_H ============================================================
