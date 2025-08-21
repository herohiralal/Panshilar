#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "Panshilar.h"
#include "TokenMatch.c"
#include "Lexer.c"

// ========================================================================================================================================================================
// Gathering files ========================================================================================================================================================

typedef struct
{
    utf8str        pathRel;
    ArraySlice(u8) contents;
} CollectedFile;

DECLARE_ARRAY_SLICE(CollectedFile);

void GatherSourceFilesInternal(ArraySlice(CollectedFile)* collectedFiles, i64* numCollectedFiles, PNSLR_Path srcDir, utf8str pathRel, PNSLR_Allocator globalAllocator)
{
    b8 collected = false;
    for (i64 i = 0; i < *numCollectedFiles; i++)
    {
        if (PNSLR_AreStringsEqual(collectedFiles->data[i].pathRel, pathRel, PNSLR_StringComparisonType_CaseInsensitive))
        {
            collected = true;
            break;
        }
    }
    if (collected) return;

    PNSLR_Path srcFile = PNSLR_GetPathForChildFile(srcDir, pathRel, globalAllocator);
    if (!PNSLR_PathExists(srcFile, PNSLR_PathExistsCheckType_File))
    {
        printf("Source file '%.*s' doesn't exist.\n", (i32) pathRel.count, pathRel.data);
        FORCE_DBG_TRAP;
    }

    ArraySlice(u8) contents;
    if (!PNSLR_ReadAllContentsFromFile(srcFile, &contents, globalAllocator))
    {
        printf("Failed to read source file '%.*s'.\n", (i32) pathRel.count, pathRel.data);
        FORCE_DBG_TRAP;
    }

    FileIterInfo iter = {0};
    iter.contents     = contents;

    i32 lineStart = 0, lineEnd = 0;
    while (DequeueNextLineSpan(&iter, &lineStart, &lineEnd))
    {
        ArraySlice(u8) line = (ArraySlice(u8)) {.data = contents.data + lineStart, .count = lineEnd - lineStart};

        FileIterInfo lineIterInfo = {0};
        lineIterInfo.contents     = line;

        b8 hasInclude = false, hasSpace = false;

        TokenSpan nextTokenInCurrentLine = {0};
        while (DequeueNextTokenSpan(&lineIterInfo, false, &nextTokenInCurrentLine))
        {
            if (!hasInclude)
            {
                if (nextTokenInCurrentLine.type == TokenType_PreprocessorInclude) { hasInclude = true; continue; }
                else                                                              {                    break;    }
            }

            if (!hasSpace)
            {
                if (nextTokenInCurrentLine.type == TokenType_Spaces) { hasSpace = true; continue; }
                else                                                 {                  break;    }
            }

            if (nextTokenInCurrentLine.type == TokenType_String)
            {
                utf8str fileNameStr = (utf8str) {.data = line.data + nextTokenInCurrentLine.start + 1, .count = nextTokenInCurrentLine.end - nextTokenInCurrentLine.start - 2};
                GatherSourceFilesInternal(collectedFiles, numCollectedFiles, srcDir, fileNameStr, globalAllocator);
            }
            else break;
        }
    }

    if (*numCollectedFiles >= collectedFiles->count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(CollectedFile, (*collectedFiles), (*numCollectedFiles) + 16, true, globalAllocator, &err);
        if (err != PNSLR_AllocatorError_None)
        {
            printf("Failed to resize collected files array.\n");
            FORCE_DBG_TRAP;
        }
    }

    collectedFiles->data[*numCollectedFiles] = (CollectedFile) {.pathRel = pathRel, .contents = contents};
    (*numCollectedFiles)++;
}

ArraySlice(CollectedFile) GatherSourceFiles(PNSLR_Path srcDir, utf8str startingPath, PNSLR_Allocator globalAllocator)
{
    ArraySlice(CollectedFile) collectedFiles    = PNSLR_MakeSlice(CollectedFile, 64, true, globalAllocator, nil);
    i64                       numCollectedFiles = 0;
    GatherSourceFilesInternal(&collectedFiles, &numCollectedFiles, srcDir, startingPath, globalAllocator);
    collectedFiles.count = numCollectedFiles;
    return collectedFiles;
}

// ========================================================================================================================================================================
// Parsing files ==========================================================================================================================================================

void PrintParseError(utf8str pathRel, ArraySlice(u8) contents, i32 start, i32 end, utf8str err)
{
    i32 errLineStart = -1, errLineEnd = -1;
    i32 lineIdx = 0;
    for (i32 j = 0, w = 0; j < start; j += w)
    {
        rune r;
        PNSLR_DecodedRune rDecoded = PNSLR_DecodeRune((ArraySlice(u8)) {.data = contents.data + j, .count = contents.count - (i64) j});
        r = rDecoded.rune; w = rDecoded.length;
        if (r == '\n')
        {
            errLineStart = j + 1;
            lineIdx++;
        }
    }

    for (i32 j = start, w = 0; j < contents.count; j += w)
    {
        rune r;
        PNSLR_DecodedRune rDecoded = PNSLR_DecodeRune((ArraySlice(u8)) {.data = contents.data + j, .count = contents.count - (i64) j});
        r = rDecoded.rune; w = rDecoded.length;
        if (r == '\n')
        {
            errLineEnd = j;
            break;
        }
    }

    if (errLineStart == -1) errLineStart = 0;                    // error is in the first line
    if (errLineEnd   == -1) errLineEnd   = (i32) contents.count; // error is in the  last line

    printf("\n\n");
    printf(
        "Error in file \033[1;3m%.*s:%d:%d\033[22;23m: %.*s\n",
        (i32) pathRel.count, pathRel.data,
        lineIdx + 1, start - errLineStart + 1,
        (i32) err.count, err.data
    );

    printf(">   ");
    printf("\033[1;36m%.*s", errLineStart - start, contents.data + errLineStart);
    printf("\033[31m%.*s",   end - start,          contents.data + start       );
    printf("\033[36m%.*s",   errLineEnd - end,     contents.data + end         );
    printf("\033[0m\n");

    // highlight bad part
    {
        printf(">   ");
        for (i32 j = errLineStart; j < start; j++) { printf(" "); }
        printf("^");
        for (i32 j = end - start - 1; j > 0; j--) { printf("^"); }
        printf("\n");
    }
}

void ProcessFile(utf8str pathRel, ArraySlice(u8) contents)
{
    FileIterInfo iter = {0};
    iter.contents     = contents;

    // utf8str fileDoc = {0};

    b8 skipping = false, isInIncludeGuard = false;
    TokenSpan span = {0};
    while (DequeueNextTokenSpan(&iter, true, &span))
    {
        if (span.type == TokenType_LineEndComment) continue;

        utf8str tokenStr = (utf8str) {.count = span.end - span.start, .data = contents.data + span.start};

        // skipping handling
        {
            if (!skipping && span.type == TokenType_MetaSkipReflectBegin) { skipping = true;            }
            if (skipping && span.type == TokenType_MetaSkipReflectEnd)    { skipping = false; continue; }
            if (skipping)                                                 {                   continue; }
        }

        if (!isInIncludeGuard)
        {
            // if (span.type == TokenType_BlockComment)
            // {
            //     if (!fileDoc.count && !fileDoc.data) fileDoc = tokenStr;
            //     else continue; // meaningless
            // }

            // if (span.type == TokenType_SymbolHash)
            // {
            // }
        }

        utf8str tokenTypeStr = GetTokenTypeString(span.type);
        printf("[%.*s]", (i32) tokenTypeStr.count, tokenTypeStr.data);
        for (i32 j = 0; j < (32 - (i32) tokenTypeStr.count); ++j) { printf(" "); }
        printf("<%.*s>\n", (i32) tokenStr.count, tokenStr.data);
    }
}

// ========================================================================================================================================================================
// Main ===================================================================================================================================================================

void BindGenMain(ArraySlice(utf8str) args)
{
    setvbuf(stdout, NULL, _IONBF, 0);

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
        appArena = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 16 * 1024 * 1024 /* 64 MiB */, CURRENT_LOC(), nil);
        if (!appArena.data || !appArena.procedure) { printf("Failed to initialise app memory."); FORCE_DBG_TRAP; }
    }

    PNSLR_Path srcDir        = PNSLR_GetPathForSubdirectory(dir, PNSLR_STRING_LITERAL("Source"), appArena);
    utf8str    pnslrFileName = PNSLR_ConcatenateStrings(dirName, PNSLR_STRING_LITERAL(".h"), appArena);

    ArraySlice(CollectedFile) files = GatherSourceFiles(srcDir, pnslrFileName, appArena);

    for (i64 i = 0; i < files.count; i++)
    {
        CollectedFile file = files.data[i];
        ProcessFile(file.pathRel, file.contents);
    }

    PNSLR_ArenaAllocatorPayload* pl = (PNSLR_ArenaAllocatorPayload*) appArena.data;
    printf("used mem: %u / %u\n", pl->totalUsed, pl->totalCapacity);
}

PNSLR_EXECUTABLE_ENTRY_POINT(BindGenMain)
