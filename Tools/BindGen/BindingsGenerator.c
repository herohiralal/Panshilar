#include "Panshilar.h"

void BindGenMain(ArraySlice(utf8str) args)
{
    // get target path
    PNSLR_Path dir = {0};
    {
        utf8str executableName = {0};

        #if PNSLR_WINDOWS
        {
            executableName = PNSLR_STRING_LITERAL("Binaries\\BindingsGenerator-windows-x64.exe");
        }
        #elif PNSLR_LINUX && PNSLR_X64
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-linux-x64");
        }
        #elif PNSLR_LINUX && PNSLR_ARM64
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-linux-arm64");
        }
        #elif PNSLR_OSX
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-osx-arm64");
        }
        #else
        {
            #error "Unsupported desktop platform."
        }
        #endif

        if (!args.count) { return; }

        utf8str dirRaw = args.data[0];
        b8 firstArgIsExecutable = PNSLR_StringEndsWith(dirRaw, executableName, PNSLR_StringComparisonType_CaseInsensitive);
        if (!firstArgIsExecutable) { return; }

        dirRaw.count -= executableName.count;
        dir = PNSLR_NormalisePath(dirRaw, PNSLR_PathNormalisationType_Directory, PNSLR_DEFAULT_HEAP_ALLOCATOR);
    }

    // initialise global main thread allocator
    PNSLR_Allocator appArena = {0};
    {
        appArena = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 16 * 1024 * 1024 /* 16 MiB */, CURRENT_LOC(), nil);
        if (!appArena.data || !appArena.procedure) { PNSLR_ExitProcess(1); return; }
    }
}

PNSLR_EXECUTABLE_ENTRY_POINT(BindGenMain)
