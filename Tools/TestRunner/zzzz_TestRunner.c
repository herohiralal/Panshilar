#include "zzzz_TestRunner.h"

typedef struct {
    PNSLR_Allocator     allocator;
    ArraySlice(utf8str) paths;
    u64                 pathsCount;
} DirectoryStuffListerPayload;

b8 DirectoryStuffLister(void* payload, utf8str path, b8 directory)
{
    DirectoryStuffListerPayload* data = (DirectoryStuffListerPayload*) payload;

    if (data->pathsCount > (u64) data->paths.count)  // Prevent overflow
    {
        printf("Too many paths collected, stopping iteration.\n");
        return false;
    }

    utf8str clonedStr = PNSLR_CloneString(path, data->allocator);
    data->paths.data[data->pathsCount] = clonedStr;
    data->pathsCount++;
    return true;
}

void TestRunnerMain(ArraySlice(utf8str) args)
{
    for (i32 i = 0; i < args.count; ++i)
    {
        utf8str arg = args.data[i];
        printf("Argument %d (length: %d): %.*s\n", i, (i32) arg.count, (i32) arg.count, arg.data);
    }

    DirectoryStuffListerPayload data = {0};
    data.allocator = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), nil);
    data.paths = PNSLR_MakeSlice(utf8str, 2048, false, PNSLR_DEFAULT_HEAP_ALLOCATOR, nil);
    PNSLR_IterateDirectory(PNSLR_STRING_LITERAL("D:/Projects/Panshilar/Source"), true, &data, DirectoryStuffLister);

    for (i32 i = 0; i < (i32) data.pathsCount; ++i)
    {
        utf8str path = data.paths.data[i];
        printf("Collected path %d (len: %d): %.*s\n", i, (i32) path.count, (i32) path.count, path.data);

        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_FreeString(path, data.allocator, &err);
        if (err != PNSLR_AllocatorError_None) { printf("Error freeing last path.\n"); }
        err = PNSLR_AllocatorError_None;

        utf8str pollute = PNSLR_CloneString(PNSLR_STRING_LITERAL("POLLUTION_TEST"), data.allocator);
        if (!pollute.data || !pollute.count) { printf("Failed to pollute.\n"); }
        PNSLR_FreeString(pollute, data.allocator, &err);
        if (err != PNSLR_AllocatorError_None) { printf("Error freeing pollution string.\n"); }
        err = PNSLR_AllocatorError_None;
    }
}

PNSLR_EXECUTABLE_ENTRY_POINT(TestRunnerMain)
