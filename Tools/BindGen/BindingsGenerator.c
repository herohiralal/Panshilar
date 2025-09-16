#ifdef _MSC_VER
    #pragma warning(disable: 4464) // relative include path contains '..'
#endif

#define PNSLR_IMPLEMENTATION
#include "../../Source/Dependencies/PNSLR_Intrinsics/Compiler.h"
#include "../../Source/Dependencies/PNSLR_Intrinsics/Warnings.h"
PNSLR_SUPPRESS_WARN
#include <stdio.h>
#include <stdlib.h>
PNSLR_UNSUPPRESS_WARN
#include "../../Source/Panshilar.h"
#include "FilesGather.h"
#include "SrcParser.h"
#include "Generator.h"
#include "BindMetaParser.h"

void BindGenMain(PNSLR_ArraySlice(utf8str) args)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    // get target path
    PNSLR_Path dir     = {0};
    utf8str    dirName = {0};
    {
        utf8str executableName = {0};

        #if PNSLR_WINDOWS
            executableName = PNSLR_StringLiteral("Binaries\\BindingsGenerator-windows-x64.exe");
        #elif PNSLR_LINUX && PNSLR_X64
            executableName = PNSLR_StringLiteral("Binaries/BindingsGenerator-linux-x64");
        #elif PNSLR_LINUX && PNSLR_ARM64
            executableName = PNSLR_StringLiteral("Binaries/BindingsGenerator-linux-arm64");
        #elif PNSLR_OSX && PNSLR_X64
            executableName = PNSLR_StringLiteral("Binaries/BindingsGenerator-osx-x64");
        #elif PNSLR_OSX && PNSLR_ARM64
            executableName = PNSLR_StringLiteral("Binaries/BindingsGenerator-osx-arm64");
        #else
            #error "Unsupported desktop platform."
        #endif

        if (!args.count) { printf("No args to extract dir path from."); FORCE_DBG_TRAP; }

        utf8str dirRaw = args.data[0];
        b8 firstArgIsExecutable = PNSLR_StringEndsWith(dirRaw, executableName, PNSLR_StringComparisonType_CaseInsensitive);
        if (!firstArgIsExecutable) { printf("First argument is not the executable name."); FORCE_DBG_TRAP; }

        dirRaw.count -= executableName.count;
        dir = PNSLR_NormalisePath(dirRaw, PNSLR_PathNormalisationType_Directory, PNSLR_GetAllocator_DefaultHeap());

        if (!PNSLR_SplitPath(dir, nil, nil, &dirName, nil)) { printf("Failed to split path."); FORCE_DBG_TRAP; }
    }

    // initialise global main thread allocator
    PNSLR_Allocator appArena = {0};
    {
        appArena = PNSLR_NewAllocator_Arena(PNSLR_GetAllocator_DefaultHeap(), 16 * 1024 * 1024 /* 16 MiB */, PNSLR_GET_LOC(), nil);
        if (!appArena.data || !appArena.procedure) { printf("Failed to initialise app memory."); FORCE_DBG_TRAP; }
    }

    PNSLR_Path srcDir        = PNSLR_GetPathForSubdirectory(dir, PNSLR_StringLiteral("Source"), appArena);
    utf8str    pnslrFileName = PNSLR_ConcatenateStrings(dirName, PNSLR_StringLiteral(".h"), appArena);
    BindMeta bindMeta = {0};
    if (!LoadBindMeta(srcDir, &bindMeta, appArena))
    {
        printf("No .bindmeta.txt file found in Source directory. Stopping.\n");
        return;
    }

    PNSLR_ArraySlice(CollectedFile) files = GatherSourceFiles(srcDir, pnslrFileName, appArena);

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
        PNSLR_Path bindingsDir = PNSLR_GetPathForSubdirectory(dir, PNSLR_StringLiteral("Bindings"), appArena);
        if (!PNSLR_PathExists(bindingsDir, PNSLR_PathExistsCheckType_Directory)) { PNSLR_CreateDirectoryTree(bindingsDir); }

        RunGenerator(GenerateCBindings,   bindingsDir, PNSLR_StringLiteral("C"),    &parsedStuff, appArena);
        RunGenerator(GenerateCxxBindings, bindingsDir, PNSLR_StringLiteral("Cxx"),  &parsedStuff, appArena);
        RunGenerator(GenerateOdnBindings, bindingsDir, PNSLR_StringLiteral("Odin"), &parsedStuff, appArena);
    }

    PNSLR_ArenaAllocatorPayload* pl = (PNSLR_ArenaAllocatorPayload*) appArena.data;
    printf("used mem: %u / %u\n", pl->totalUsed, pl->totalCapacity);
}

i32 main(i32 argc, cstring* argv)
{
    PNSLR_ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); }
    BindGenMain(args);
    return 0;
}

// unity build
#include "TokenMatch.c"
#include "Lexer.c"
#include "FilesGather.c"
#include "SrcParser.c"
#include "Generator.c"
#include "BindMetaParser.c"
#include "GeneratorC.c"
#include "GeneratorCxx.c"
#include "GeneratorOdn.c"
#include "../../Source/zzzz_Unity.c"
