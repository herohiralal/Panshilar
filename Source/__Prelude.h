#ifndef PNSLR_ENTRY_H // ===========================================================
#define PNSLR_ENTRY_H
//+skipreflect
#include "Dependencies/PNSLR_Intrinsics/Platforms.h"
#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
#include "Dependencies/PNSLR_Intrinsics/Warnings.h"

#ifdef PNSLR_IMPLEMENTATION

    PNSLR_SUPPRESS_WARN

    #if PNSLR_WINDOWS
        #define WIN32_LEAN_AND_MEAN
        #include <Windows.h>
        #include <WinSock2.h>
        #include <ws2ipdef.h>
        #include <iphlpapi.h>
        #include <intrin.h>
        #include <malloc.h>
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

    #include <string.h>

    PNSLR_UNSUPPRESS_WARN

#endif

#include "Dependencies/PNSLR_Intrinsics/Primitives.h"
#include "Dependencies/PNSLR_Intrinsics/Macros.h"
#include "Dependencies/PNSLR_Intrinsics/Collections.h"

// Static tests ====================================================================

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
