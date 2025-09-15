#include "FilesGather.h"
#include "Lexer.h"

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

    PNSLR_Path relDir = {0};
    if (!PNSLR_SplitPath(srcFile, &relDir, nil, nil, nil))
    {
        printf("Failed to split source file path '%.*s'.\n", (i32) pathRel.count, pathRel.data);
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
        while (DequeueNextTokenSpan(&lineIterInfo, TokenIgnoreMask_None, &nextTokenInCurrentLine))
        {
            if (!hasInclude)
            {
                if (nextTokenInCurrentLine.type == TknTy_PreprocessorInclude) { hasInclude = true; continue; }
                else                                                          {                    break;    }
            }

            if (!hasSpace)
            {
                if (nextTokenInCurrentLine.type == TknTy_Spaces) { hasSpace = true; continue; }
                else                                             {                  break;    }
            }

            if (nextTokenInCurrentLine.type == TknTy_String)
            {
                utf8str fileNameStr = (utf8str) {.data = line.data + nextTokenInCurrentLine.start + 1, .count = nextTokenInCurrentLine.end - nextTokenInCurrentLine.start - 2};
                GatherSourceFilesInternal(collectedFiles, numCollectedFiles, relDir, fileNameStr, globalAllocator);
            }
            else break;
        }
    }

    if (*numCollectedFiles >= collectedFiles->count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(CollectedFile, collectedFiles, (*numCollectedFiles) + 16, true, globalAllocator, CURRENT_LOC(), &err);
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
    ArraySlice(CollectedFile) collectedFiles    = PNSLR_MakeSlice(CollectedFile, 64, true, globalAllocator, CURRENT_LOC(), nil);
    i64                       numCollectedFiles = 0;
    GatherSourceFilesInternal(&collectedFiles, &numCollectedFiles, srcDir, startingPath, globalAllocator);
    collectedFiles.count = numCollectedFiles;
    return collectedFiles;
}
