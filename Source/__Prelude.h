//+skipreflect
#if defined(__cplusplus)
    #error "Panshilar is a C library and does not support C++ in this manner. Please use it only via the provided static libraries and headers."
#endif

#ifndef PNSLR_ENTRY_H // ===========================================================
#define PNSLR_ENTRY_H

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
        #include <Windows.h>
    #endif

    #if PNSLR_UNIX

        // since we're on C11
        #define _POSIX_C_SOURCE 200809L
        #define _XOPEN_SOURCE 700

        #include <unistd.h>
        #include <fcntl.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <sys/wait.h>
        #include <sys/mman.h>
        #include <errno.h>
        #include <dirent.h>
        #include <pthread.h>
        #include <signal.h>
        #include <time.h>
        #include <semaphore.h>
    #endif

    #if PNSLR_ANDROID
        #include <android/log.h>
        #include <android/asset_manager.h>
        #include <android/native_activity.h>
    #endif

    PRAGMA_REENABLE_WARNINGS

#endif

// always include this last and outside the implementation block
// it contains some important macros/typedefs that we'll use
#include "Dependencies/PNSLR_Intrinsics/Intrinsics.h"

#endif // PNSLR_ENTRY_H ============================================================
//-skipreflect
