#ifndef PNSLR_MAIN_HEADER_H // =====================================================
#define PNSLR_MAIN_HEADER_H
#include "__Prelude.h"
#include "Dependencies/PNSLR_Intrinsics/Compiler.h"   // technically included by prelude
#include "Dependencies/PNSLR_Intrinsics/Intrinsics.h" // technically included by prelude
#include "Environment.h"
#include "Runtime.h"
#include "Sync.h"
#include "Allocators.h"
#include "Chrono.h"
#include "Strings.h"
#include "IO.h"
#include "Console.h"
#include "Process.h"
EXTERN_C_BEGIN

//+skipreflect

#define PNSLR_EXECUTABLE_ENTRY_POINT(entryPointFunctionName__) \
    i32 main(i32 argc, cstring* argv) \
    { \
        ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_DEFAULT_HEAP_ALLOCATOR, nil); \
        for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); } \
        entryPointFunctionName__(args); \
        return 0; \
    }

#if PNSLR_WINDOWS

    #define PNSLR_LIBRARY_ENTRY_POINT \
        b32 __stdcall DllMain(rawptr hInstDLL, u32 fdwReason, rawptr lpvReserved) { return true; }

#else

    #define PNSLR_LIBRARY_ENTRY_POINT \
        i32 main(i32 argc, cstring* argv) { return 0; }

#endif

//-skipreflect

EXTERN_C_END
#endif // PNSLR_MAIN_HEADER_H ======================================================
