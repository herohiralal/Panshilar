#ifndef PNSLR_PRIVATE_INLCUDES_H
#define PNSLR_PRIVATE_INLCUDES_H

#include "Dependencies/PNSLR_Intrinsics/Platforms.h"
#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
#include "Dependencies/PNSLR_Intrinsics/Warnings.h"

PNSLR_SUPPRESS_WARN

#if PNSLR_WINDOWS
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "Ws2_32.lib")
#endif

#if PNSLR_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WinSock2.h>
    #include <ws2ipdef.h>
    #include <iphlpapi.h>
    #include <intrin.h>
    #include <malloc.h>

    #if PNSLR_DBG
        #define RADDBG_MARKUP_IMPLEMENTATION
        #include "Dependencies/raddbg/raddbg_markup.h"
        #undef RADDBG_MARKUP_IMPLEMENTATION
    #endif

    #undef WIN32_LEAN_AND_MEAN
#endif

#if PNSLR_UNIX

    // since we're on C11
    #define _POSIX_C_SOURCE 200809L
    #define _XOPEN_SOURCE 700

    #include <stdlib.h>
    #include <string.h>
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
    #include <android_native_app_glue.h>
#endif

PNSLR_UNSUPPRESS_WARN

#endif//PNSLR_PRIVATE_INCLUDES_H
