//+skipreflect
#if defined(__cplusplus)
    #error "Panshilar is a C library and does not support C++ in this manner. Please use it only via the provided static libraries and headers."
#endif

#ifndef PNSLR_ENTRY // =============================================================
#define PNSLR_ENTRY

// Environment setup ===============================================================

// platform

#if defined(PNSLR_WINDOWS)
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_LINUX)
    #define PNSLR_WINDOWS 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_OSX)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_ANDROID)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_IOS)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_PS5)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_XSERIES 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_XSERIES)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_SWITCH 0
#elif defined(PNSLR_SWITCH)
    #define PNSLR_WINDOWS 0
    #define PNSLR_LINUX 0
    #define PNSLR_OSX 0
    #define PNSLR_ANDROID 0
    #define PNSLR_IOS 0
    #define PNSLR_PS5 0
    #define PNSLR_XSERIES 0
#else
    #error "No platform defined."
#endif

// architecture

#if defined(PNSLR_X64)
    #define PNSLR_ARM64 0
#elif defined(PNSLR_ARM64)
    #define PNSLR_X64 0
#else
    #error "No architecture defined."
#endif

// derived

#define PNSLR_UNIX (PNSLR_LINUX || PNSLR_OSX || PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_DESKTOP (PNSLR_WINDOWS || PNSLR_LINUX || PNSLR_OSX)
#define PNSLR_MOBILE (PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_CONSOLE (PNSLR_PS5 || PNSLR_XSERIES || PNSLR_SWITCH)

// Includes ========================================================================

#ifdef PNSLR_IMPLEMENTATION

    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Weverything"
    #elif defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Weverything"
    #elif defined(_MSC_VER)
        #pragma warning(push, 0)
    #endif

    #if PNSLR_WINDOWS
        #include <Windows.h>
    #endif

    #if PNSLR_UNIX
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

    #if defined(__clang__)
        #pragma clang diagnostic pop
    #elif defined(__GNUC__)
        #pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
        #pragma warning(pop)
    #endif

#endif

// always include this last and outside the implementation block
// it contains some important macros/typedefs that we'll use
#include "Dependencies/PNSLR_Intrinsics/Intrinsics.h"

// Macros ==========================================================================

#if !defined(__cplusplus)

    #if defined(offsetof)
        #undef offsetof // avoid conflict with stddef.h, if somehow inherited
    #endif

    #if PNSLR_UNIX

        #define thread_local            __thread
        #define inline                  __inline__
        #define noinline                __attribute__((noinline))
        #define forceinline             inline __attribute__((always_inline))
        #define alignas(x)              __attribute__((aligned(x)))
        #define deprecated              __attribute__((deprecated))
        #define noreturn                __attribute__((noreturn))
        #define alignof(type)           __alignof__(type)
        #define offsetof(type, member)  __builtin_offsetof(type, member)

    #elif PNSLR_WINDOWS

        #define thread_local            __declspec(thread)
        #define inline                  __inline
        #define noinline                __declspec(noinline)
        #define forceinline             __forceinline
        #define alignas(x)              __declspec(align(x))
        #define deprecated              __declspec(deprecated)
        #define noreturn                __declspec(noreturn)
        #define alignof(type)           __alignof(type)
        #define offsetof(type, member)  ((unsigned __int64)&(((type*)0)->member))

    #else
        #error "Required features not supported by this compiler."
    #endif

    #define static_assert           _Static_assert

#endif

// Runtime =========================================================================

typedef struct SourceCodeLocation
{
    utf8str file;     // File name
    i32     line;     // Line number
    i32     column;   // Column number
    utf8str function; // Function name
} SourceCodeLocation;

#define CURRENT_LOC() (SourceCodeLocation) \
{ \
    .file = (utf8str){ .count = sizeof(__FILE__) - 1, .data = (utf8ch*)__FILE__ }, \
    .line = __LINE__, \
    .column = 0, /* not available but might get forwarded from somewhere */ \
    .function = (utf8str){ .count = sizeof(__FUNCTION__) - 1, .data = (utf8ch*)__FUNCTION__ } \
}

// Allocator modes
#define ALLOCATOR_ALLOCATE           ((u8) 0)
#define ALLOCATOR_RESIZE             ((u8) 1)
#define ALLOCATOR_FREE               ((u8) 2)
#define ALLOCATOR_FREE_ALL           ((u8) 3)
#define ALLOCATOR_ALLOCATE_NO_ZERO   ((u8) 4)
#define ALLOCATOR_RESIZE_NO_ZERO     ((u8) 5)
#define ALLOCATOR_QUERY_CAPABILITIES ((u8) 255)

// Allocator capabilities
#define ALLOCATOR_CAPABILITY_NONE        ((u64) (1 << 0))
#define ALLOCATOR_CAPABILITY_THREAD_SAFE ((u64) (1 << 1))
#define ALLOCATOR_CAPABILITY_RESIZE_FR   ((u64) (1 << 2))
#define ALLOCATOR_CAPABILITY_FREE        ((u64) (1 << 3))
#define ALLOCATOR_CAPABILITY_HINT_BUMP   ((u64) (1 << 17))
#define ALLOCATOR_CAPABILITY_HINT_HEAP   ((u64) (1 << 18))
#define ALLOCATOR_CAPABILITY_HINT_TEMP   ((u64) (1 << 19))
#define ALLOCATOR_CAPABILITY_HINT_DEBUG  ((u64) (1 << 20))

typedef void* (*AllocatorFn)(void* allocatorData, u8 mode, i32 size, i32 alignment, void* oldMemory, i32 oldSize, SourceCodeLocation location);

// ideally, don't want to use this in structs, only as function parameters
typedef struct Allocator
{
    AllocatorFn function;
    void*       data; // Optional data for the allocator function
} Allocator;

// Metadata ========================================================================

#define REFLECT(...)

#endif // PNSLR_ENTRY ==============================================================
//-skipreflect
