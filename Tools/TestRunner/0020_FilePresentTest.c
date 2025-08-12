#include "zzzz_TestRunner.h"

typedef struct
{
    PNSLR_Allocator     allocator;
    ArraySlice(utf8str) paths;
    u64                 pathsCount;
} DirectoryStuffListerForFilePresentTestPayload;

b8 DirectoryStuffListerForFilePresentTest(void* payload, utf8str path, b8 directory, b8* exploreCurrentDirectory)
{
    DirectoryStuffListerForFilePresentTestPayload* data = (DirectoryStuffListerForFilePresentTestPayload*) payload;

    if (data->pathsCount >= (u64) data->paths.count)
    {
        AssertMsg(false, "Too many paths found.");
        return false;
    }

    b8 skippedDirectory = false;

    #define SKIP_DIR(x) \
        if (!skippedDirectory && directory && PNSLR_StringEndsWith(path, PNSLR_STRING_LITERAL("/" x "/"), PNSLR_StringComparisonType_CaseInsensitive)) \
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

    utf8str clonedStr = PNSLR_CloneString(path, data->allocator);
    data->paths.data[data->pathsCount] = clonedStr;
    data->pathsCount++;
    return true;
}

MAIN_TEST_FN(ctx)
{
    #if PNSLR_DESKTOP
    {
        utf8str executableName = {0};

        #if PNSLR_WINDOWS
        {
            executableName = PNSLR_STRING_LITERAL("Binaries\\TestRunner-windows-x64.exe");
        }
        #elif PNSLR_LINUX && PNSLR_X64
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-linux-x64");
        }
        #elif PNSLR_LINUX && PNSLR_ARM64
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-linux-arm64");
        }
        #elif PNSLR_OSX
        {
            executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-osx-arm64");
        }
        #else
        {
            #error "Unsupported desktop platform."
        }
        #endif

        if (!AssertMsg(ctx->args.count, "Need at least one arg to extract location.")) { return; }

        b8 firstArgIsExecutable = PNSLR_StringEndsWith(ctx->args.data[0], executableName, PNSLR_StringComparisonType_CaseInsensitive);
        if (!AssertMsg(firstArgIsExecutable, "First argument must be the executable name.")) { return; }

        utf8str dir = ctx->args.data[0];
        dir.count -= executableName.count;

        DirectoryStuffListerForFilePresentTestPayload data = {0};
        data.allocator = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), nil);
        data.paths = PNSLR_MakeSlice(utf8str, 2048, false, ctx->testAllocator, nil);
        PNSLR_IterateDirectory(dir, true, &data, DirectoryStuffListerForFilePresentTest);

        b8 pnslrHeaderFound = false, pnslrUnityFound = false;
        for (i32 i = (i32) data.pathsCount - 1; i >= 0; --i)
        {
            utf8str path = data.paths.data[i];

            if (!pnslrHeaderFound)
            {
                pnslrHeaderFound = PNSLR_StringEndsWith(path, PNSLR_STRING_LITERAL("Source/Panshilar.h"), PNSLR_StringComparisonType_CaseInsensitive);
            }

            if (!pnslrUnityFound)
            {
                pnslrUnityFound = PNSLR_StringEndsWith(path, PNSLR_STRING_LITERAL("Source/zzzz_Unity.c"), PNSLR_StringComparisonType_CaseInsensitive);
            }

            PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
            PNSLR_FreeString(path, data.allocator, &err);
            AssertMsg(err == PNSLR_AllocatorError_None, "Error freeing last path.");
            err = PNSLR_AllocatorError_None;

            utf8str pollute = PNSLR_CloneString(PNSLR_STRING_LITERAL("POLLUTION_TEST"), data.allocator);
            AssertMsg(pollute.data && pollute.count, "Failed to pollute.");
            PNSLR_FreeString(pollute, data.allocator, &err);
            AssertMsg(err == PNSLR_AllocatorError_None, "Error freeing pollution string.");
            err = PNSLR_AllocatorError_None;
        }

        PNSLR_DestroyAllocator_Stack(data.allocator, CURRENT_LOC(), nil);

        Assert(pnslrHeaderFound);
        Assert(pnslrUnityFound);
    }
    #endif
}
