#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "Panshilar.h"

void BindGenMain(ArraySlice(utf8str) args)
{
    // get target path
    PNSLR_Path dir     = {0};
    utf8str    dirName = {0};
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

        if (!args.count) { printf("No args to extract dir path from."); FORCE_DBG_TRAP; }

        utf8str dirRaw = args.data[0];
        b8 firstArgIsExecutable = PNSLR_StringEndsWith(dirRaw, executableName, PNSLR_StringComparisonType_CaseInsensitive);
        if (!firstArgIsExecutable) { printf("First argument is not the executable name."); FORCE_DBG_TRAP; }

        dirRaw.count -= executableName.count;
        dir = PNSLR_NormalisePath(dirRaw, PNSLR_PathNormalisationType_Directory, PNSLR_DEFAULT_HEAP_ALLOCATOR);

        if (!PNSLR_SplitPath(dir, nil, nil, &dirName, nil)) { printf("Failed to split path."); FORCE_DBG_TRAP; }
    }

    // initialise global main thread allocator
    PNSLR_Allocator appArena = {0};
    {
        appArena = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 64 * 1024 * 1024 /* 64 MiB */, CURRENT_LOC(), nil);
        if (!appArena.data || !appArena.procedure) { printf("Failed to initialise app memory."); FORCE_DBG_TRAP; }
    }

    PNSLR_Path srcDir        = PNSLR_GetPathForSubdirectory(dir, PNSLR_STRING_LITERAL("Source"), appArena);
    utf8str    pnslrFileName = PNSLR_ConcatenateStrings(dirName, PNSLR_STRING_LITERAL(".h"), appArena);
    PNSLR_Path pnslrFile     = PNSLR_GetPathForChildFile(srcDir, pnslrFileName, appArena);

    if (!PNSLR_PathExists(pnslrFile, PNSLR_PathExistsCheckType_File))
    {
        printf("Entry file doesn't exist.");
        FORCE_DBG_TRAP;
    }

    ArraySlice(u8) pnslrFileContents;
    if (!PNSLR_ReadAllContentsFromFile(pnslrFile, &pnslrFileContents, appArena))
    {
        printf("Failed to read entry file.");
        FORCE_DBG_TRAP;
    }

    printf("contents: %.*s\n", (i32) pnslrFileContents.count, pnslrFileContents.data);
}

PNSLR_EXECUTABLE_ENTRY_POINT(BindGenMain)
