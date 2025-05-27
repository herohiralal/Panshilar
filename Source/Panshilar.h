#ifndef PNSLR_MAIN_HEADER // =======================================================
#define PNSLR_MAIN_HEADER

#include "__Prelude.h"
#include "Allocators.h"
#include "Strings.h"
#include "IO.h"

//+skipreflect

#define PNSLR_EXECUTABLE_ENTRY_POINT(entryPointFunctionName__) \
    i32 main(i32 argc, cstring* argv) \
    { \
        ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_DEFAULT_HEAP_ALLOCATOR); \
        for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); } \
        entryPointFunctionName__(args); \
        return 0; \
    }

#if PNSLR_WINDOWS && defined(_MSC_VER)

#define PNSLR_LIBRARY_ENTRY_POINT \
    b32 __stdcall DllMain(void* hInstDLL, u32 fdwReason, void* lpvReserved) { return true; }

#else

#define PNSLR_LIBRARY_ENTRY_POINT \
    i32 main(i32 argc, cstring* argv) { return 0; }

#endif

//-skipreflect

#endif // PNSLR_MAIN_HEADER ========================================================
