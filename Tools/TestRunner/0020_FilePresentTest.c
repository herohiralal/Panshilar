#include "zzzz_TestRunner.h"

typedef struct
{
    PNSLR_Allocator                 allocator;
    PNSLR_ArraySlice(PNSLR_UTF8STR) paths;
    PNSLR_U64                       pathsCount;
} DirectoryStuffListerForFilePresentTestPayload;

PNSLR_B8 DirectoryStuffListerForFilePresentTest(void* payload, PNSLR_Path path, PNSLR_B8 directory, PNSLR_B8* exploreCurrentDirectory)
{
    DirectoryStuffListerForFilePresentTestPayload* data = (DirectoryStuffListerForFilePresentTestPayload*) payload;

    if (data->pathsCount >= (PNSLR_U64) data->paths.count)
    {
        AssertMsg(false, "Too many paths found.");
        return false;
    }

    PNSLR_B8 skippedDirectory = false;

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

    #undef SKIP_DIR

    if (skippedDirectory)
    {
        *exploreCurrentDirectory = false;
    }

    if (directory)
    {
        return true;
    }

    PNSLR_UTF8STR clonedStr = PNSLR_CloneString(path.path, data->allocator);
    data->paths.data[data->pathsCount] = clonedStr;
    data->pathsCount++;
    return true;
}

MAIN_TEST_FN(ctx)
{
    PNSLR_Platform     plt  = PNSLR_GetPlatform();
    PNSLR_Architecture arch = PNSLR_GetArchitecture();

    if (plt == PNSLR_Platform_Windows || plt == PNSLR_Platform_Linux || plt == PNSLR_Platform_OSX)
    {
        PNSLR_UTF8STR executableName = {0};

        if (plt == PNSLR_Platform_Windows)
        {
            executableName = PNSLR_StringLiteral("Binaries\\TestRunner-windows-x64.exe");
        }
        else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_X64)
        {
            executableName = PNSLR_StringLiteral("Binaries/TestRunner-linux-x64");
        }
        else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_ARM64)
        {
            executableName = PNSLR_StringLiteral("Binaries/TestRunner-linux-arm64");
        }
        else if (plt == PNSLR_Platform_OSX)
        {
            executableName = PNSLR_StringLiteral("Binaries/TestRunner-osx-arm64");
        }
        else if (plt == PNSLR_Platform_OSX && arch == PNSLR_Architecture_X64)
        {
            executableName = PNSLR_StringLiteral("Binaries/TestRunner-osx-x64");
        }
        else
        {
            AssertMsg(false, "Unsupported platform or architecture.");
            return;
        }

        if (!AssertMsg(ctx->args.count, "Need at least one arg to extract location.")) { return; }

        PNSLR_B8 firstArgIsExecutable = PNSLR_StringEndsWith(ctx->args.data[0], executableName, PNSLR_StringComparisonType_CaseInsensitive);
        if (!AssertMsg(firstArgIsExecutable, "First argument must be the executable name.")) { return; }

        PNSLR_UTF8STR dirRaw = ctx->args.data[0];
        dirRaw.count -= executableName.count;
        PNSLR_Path dir = PNSLR_NormalisePath(dirRaw, PNSLR_PathNormalisationType_Directory, ctx->testAllocator);

        DirectoryStuffListerForFilePresentTestPayload data = {0};
        data.allocator = PNSLR_NewAllocator_Stack(PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nullptr);
        data.paths = PNSLR_MakeSlice(PNSLR_UTF8STR, 2048, false, ctx->testAllocator, PNSLR_GET_LOC(), nullptr);
        PNSLR_IterateDirectory(dir, true, &data, DirectoryStuffListerForFilePresentTest);

        PNSLR_B8 pnslrHeaderFound = false, pnslrUnityFound = false;
        for (PNSLR_I32 i = (PNSLR_I32) data.pathsCount - 1; i >= 0; --i)
        {
            PNSLR_UTF8STR path = data.paths.data[i];

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

            PNSLR_UTF8STR pollute = PNSLR_CloneString(PNSLR_StringLiteral("POLLUTION_TEST"), data.allocator);
            AssertMsg(pollute.data && pollute.count, "Failed to pollute.");
            PNSLR_FreeString(pollute, data.allocator, PNSLR_GET_LOC(), &err);
            AssertMsg(err == PNSLR_AllocatorError_None, "Error freeing pollution string.");
            err = PNSLR_AllocatorError_None;
        }

        PNSLR_DestroyAllocator_Stack(data.allocator, PNSLR_GET_LOC(), nullptr);

        Assert(pnslrHeaderFound);
        Assert(pnslrUnityFound);
    }
}
