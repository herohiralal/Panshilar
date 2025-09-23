#include "zzzz_TestRunner.h"

typedef struct
{
    PNSLR_Allocator           allocator;
    PNSLR_ArraySlice(utf8str) paths;
    u64                       pathsCount;
} DirectoryStuffListerForFilePresentTestPayload;

b8 DirectoryStuffListerForFilePresentTest(void* payload, PNSLR_Path path, b8 directory, b8* exploreCurrentDirectory)
{
    DirectoryStuffListerForFilePresentTestPayload* data = (DirectoryStuffListerForFilePresentTestPayload*) payload;

    if (data->pathsCount >= (u64) data->paths.count)
    {
        AssertMsg(false, "Too many paths found.");
        return false;
    }

    b8 skippedDirectory = false;

    #define SKIP_DIR(x) \
        if (!skippedDirectory && directory && PNSLR_StringEndsWith(path.path, PNSLR_StringLiteral("/" x "/"), PNSLR_StringComparisonType_CaseInsensitive)) \
        { \
            skippedDirectory = true; \
        }

    SKIP_DIR(".git");
    SKIP_DIR("Binaries");
    SKIP_DIR("Libraries");
    SKIP_DIR("Prebuilt");
    SKIP_DIR("Temp");
    SKIP_DIR("ProjectFiles");

    #undef SKIP_DIR

    if (skippedDirectory)
    {
        *exploreCurrentDirectory = false;
    }

    if (directory)
    {
        return true;
    }

    utf8str clonedStr = PNSLR_CloneString(path.path, data->allocator);
    data->paths.data[data->pathsCount] = clonedStr;
    data->pathsCount++;
    return true;
}

MAIN_TEST_FN(ctx)
{
    if (!ctx->tgtDir.path.data || !ctx->tgtDir.path.count)
    {
        return;
    }

    DirectoryStuffListerForFilePresentTestPayload data = {0};
    data.allocator = PNSLR_NewAllocator_Stack(PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nullptr);
    data.paths = PNSLR_MakeSlice(utf8str, 2048, false, ctx->testAllocator, PNSLR_GET_LOC(), nullptr);
    PNSLR_IterateDirectory(ctx->tgtDir, true, &data, DirectoryStuffListerForFilePresentTest);

    b8 pnslrHeaderFound = false, pnslrUnityFound = false;
    for (i32 i = (i32) data.pathsCount - 1; i >= 0; --i)
    {
        utf8str path = data.paths.data[i];

        if (!pnslrHeaderFound)
        {
            pnslrHeaderFound = PNSLR_StringEndsWith(path, PNSLR_StringLiteral("Source/Panshilar.h"), PNSLR_StringComparisonType_CaseInsensitive);
        }

        if (!pnslrUnityFound)
        {
            pnslrUnityFound = PNSLR_StringEndsWith(path, PNSLR_StringLiteral("Source/zzzz_Unity.c"), PNSLR_StringComparisonType_CaseInsensitive);
        }

        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_FreeString(path, data.allocator, PNSLR_GET_LOC(), &err);
        AssertMsg(err == PNSLR_AllocatorError_None, "Error freeing last path.");
        err = PNSLR_AllocatorError_None;

        utf8str pollute = PNSLR_CloneString(PNSLR_StringLiteral("POLLUTION_TEST"), data.allocator);
        AssertMsg(pollute.data && pollute.count, "Failed to pollute.");
        PNSLR_FreeString(pollute, data.allocator, PNSLR_GET_LOC(), &err);
        AssertMsg(err == PNSLR_AllocatorError_None, "Error freeing pollution string.");
        err = PNSLR_AllocatorError_None;
    }

    PNSLR_DestroyAllocator_Stack(data.allocator, PNSLR_GET_LOC(), nullptr);

    Assert(pnslrHeaderFound);
    Assert(pnslrUnityFound);
}
