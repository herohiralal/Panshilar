#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
#include <stdlib.h>
PRAGMA_REENABLE_WARNINGS
#include "Panshilar.h"
#include "FilesGather.h"
#include "SrcParser.h"
#include "Generator.h"
#include "Generator_C.h"

void BindGenMain(ArraySlice(utf8str) args)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    // get target path
    PNSLR_Path dir     = {0};
    utf8str    dirName = {0};
    {
        utf8str executableName = {0};

        #if PNSLR_WINDOWS
            executableName = PNSLR_STRING_LITERAL("Binaries\\BindingsGenerator-windows-x64.exe");
        #elif PNSLR_LINUX && PNSLR_X64
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-linux-x64");
        #elif PNSLR_LINUX && PNSLR_ARM64
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-linux-arm64");
        #elif PNSLR_OSX && PNSLR_X64
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-osx-x64");
        #elif PNSLR_OSX && PNSLR_ARM64
            executableName = PNSLR_STRING_LITERAL("Binaries/BindingsGenerator-osx-arm64");
        #else
            #error "Unsupported desktop platform."
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
        appArena = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 16 * 1024 * 1024 /* 16 MiB */, CURRENT_LOC(), nil);
        if (!appArena.data || !appArena.procedure) { printf("Failed to initialise app memory."); FORCE_DBG_TRAP; }
    }

    PNSLR_Path srcDir        = PNSLR_GetPathForSubdirectory(dir, PNSLR_STRING_LITERAL("Source"), appArena);
    utf8str    pnslrFileName = PNSLR_ConcatenateStrings(dirName, PNSLR_STRING_LITERAL(".h"), appArena);

    ArraySlice(CollectedFile) files = GatherSourceFiles(srcDir, pnslrFileName, appArena);

    b8 parsingSuccessful = true;
    ParsedContent parsedStuff = {0};
    {
        InitialiseTypeTable(&parsedStuff, appArena);

        CachedLasts processingCache = {0};
        for (i64 i = 0; i < files.count; i++)
        {
            CollectedFile file = files.data[i];
            if (!ProcessFile(&parsedStuff, &processingCache, file.pathRel, file.contents, appArena))
            {
                parsingSuccessful = false;
                printf("Parsing failed. Stopping.\n");
                break;
            }
        }
    }

    if (parsingSuccessful) // TODO: make this multithreaded
    {
        // create main bindings dir
        PNSLR_Path bindingsDir = PNSLR_GetPathForSubdirectory(dir, PNSLR_STRING_LITERAL("Bindings"), appArena);
        if (!PNSLR_PathExists(bindingsDir, PNSLR_PathExistsCheckType_Directory)) { PNSLR_CreateDirectoryTree(bindingsDir); }

        RunGenerator(GenerateCBindings, bindingsDir, PNSLR_STRING_LITERAL("C"), &parsedStuff, appArena);
    }

    PNSLR_ArenaAllocatorPayload* pl = (PNSLR_ArenaAllocatorPayload*) appArena.data;
    printf("used mem: %u / %u\n", pl->totalUsed, pl->totalCapacity);
}

PNSLR_EXECUTABLE_ENTRY_POINT(BindGenMain)

// unity build
#include "TokenMatch.c"
#include "Lexer.c"
#include "FilesGather.c"
#include "SrcParser.c"
#include "Generator.c"
#include "Generator_C.c"
