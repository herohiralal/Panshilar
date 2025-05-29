//+skipreflect
#if defined(__cplusplus)
    #error "Panshilar is a C library and does not support C++ in this manner. Please use it only via the provided static libraries and headers."
#endif

#ifndef PNSLR_ENTRY // =============================================================
#define PNSLR_ENTRY

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
