#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "zzzz_TestRunner.h"
#include "zzzz_GeneratedCombinedTests.c"

// typedef struct
// {
//     PNSLR_Allocator     allocator;
//     ArraySlice(utf8str) paths;
//     u64                 pathsCount;
// } DirectoryStuffListerPayload;

// b8 DirectoryStuffLister(void* payload, utf8str path, b8 directory)
// {
//     DirectoryStuffListerPayload* data = (DirectoryStuffListerPayload*) payload;

//     if (data->pathsCount > (u64) data->paths.count)  // Prevent overflow
//     {
//         printf("Too many paths collected, stopping iteration.\n");
//         return false;
//     }

//     utf8str clonedStr = PNSLR_CloneString(path, data->allocator);
//     data->paths.data[data->pathsCount] = clonedStr;
//     data->pathsCount++;
//     return true;
// }

// void UnusedForNow(ArraySlice(utf8str) args)
// {
//     for (i32 i = 0; i < args.count; ++i)
//     {
//         utf8str arg = args.data[i];
//         printf("Argument %d (length: %d): %.*s\n", i, (i32) arg.count, (i32) arg.count, arg.data);
//     }

//     DirectoryStuffListerPayload data = {0};
//     data.allocator = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), nil);
//     data.paths = PNSLR_MakeSlice(utf8str, 2048, false, PNSLR_DEFAULT_HEAP_ALLOCATOR, nil);
//     PNSLR_IterateDirectory(PNSLR_STRING_LITERAL("D:/Projects/Panshilar/Source"), true, &data, DirectoryStuffLister);

//     for (i32 i = (i32) data.pathsCount - 1; i >= 0; --i)
//     {
//         utf8str path = data.paths.data[i];
//         printf("Collected path %d (len: %d): %.*s\n", i, (i32) path.count, (i32) path.count, path.data);

//         PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
//         PNSLR_FreeString(path, data.allocator, &err);
//         if (err != PNSLR_AllocatorError_None) { printf("Error freeing last path.\n"); }
//         err = PNSLR_AllocatorError_None;

//         utf8str pollute = PNSLR_CloneString(PNSLR_STRING_LITERAL("POLLUTION_TEST"), data.allocator);
//         if (!pollute.data || !pollute.count) { printf("Failed to pollute.\n"); }
//         PNSLR_FreeString(pollute, data.allocator, &err);
//         if (err != PNSLR_AllocatorError_None) { printf("Error freeing pollution string.\n"); }
//         err = PNSLR_AllocatorError_None;
//     }
// }

ENUM_START(BufferedMessageType, u8)
    #define BufferedMessageType_Invalid     ((BufferedMessageType) 0)
    #define BufferedMessageType_TestFnStart ((BufferedMessageType) 1)
    #define BufferedMessageType_TestFnEnd   ((BufferedMessageType) 2)
    #define BufferedMessageType_TestFnLog   ((BufferedMessageType) 3)
    #define BufferedMessageType_AssertPass  ((BufferedMessageType) 4)
    #define BufferedMessageType_AssertFail  ((BufferedMessageType) 5)
ENUM_END

typedef struct
{
    BufferedMessageType      type;
    utf8str                  msg;
    PNSLR_SourceCodeLocation loc;
} BufferedMessage;

DECLARE_ARRAY_SLICE(BufferedMessage);

static thread_local ArraySlice(BufferedMessage) G_BufferedMessages          = {0};
static thread_local u64                         G_NumBufferedMessages       = {0};
static thread_local PNSLR_Allocator             G_BufferedMessagesAllocator = {0};

void BufferMessage(const BufferedMessage* msg)
{
    if (G_NumBufferedMessages >= G_BufferedMessages.count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(BufferedMessage, G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_BufferedMessagesAllocator, &err);

        if (err != PNSLR_AllocatorError_None)
        {
            printf("Error resizing buffered messages array.\n");
            PNSLR_ExitProcess(1);
            return;
        }
    }

    G_BufferedMessages.data[G_NumBufferedMessages] = *msg;
    G_NumBufferedMessages++;
}

void LogInternal(utf8str message, PNSLR_SourceCodeLocation location)
{
    BufferedMessage msg =
    {
        .type = BufferedMessageType_TestFnLog,
        .msg  = PNSLR_CloneString(message, G_BufferedMessagesAllocator),
        .loc  = location,
    };

    BufferMessage(&msg);
}

b8 AssertInternal(b8 condition, utf8str message, PNSLR_SourceCodeLocation location)
{
    BufferedMessage msg =
    {
        .type = condition ? BufferedMessageType_AssertPass : BufferedMessageType_AssertFail,
        .msg = {0},
        .loc = location,
    };

    if (!condition) { msg.msg = PNSLR_CloneString(message, G_BufferedMessagesAllocator); }

    BufferMessage(&msg);
    return condition;
}

// TODO: make the test runner multi-threaded
void TestRunnerMain(ArraySlice(utf8str) args)
{
    ArraySlice(TestFunctionInfo) tests = {0};
    {
        u64                          testsCount = ZZZZ_GetTestsCount();
        PNSLR_AllocatorError         err        = PNSLR_AllocatorError_None;
        ArraySlice(TestFunctionInfo) tests2     = PNSLR_MakeSlice(TestFunctionInfo, testsCount, false, PNSLR_DEFAULT_HEAP_ALLOCATOR, &err);

        tests = tests2;
    }

    if (!tests.count || !tests.data)
    {
        printf("No tests found.\n");
        PNSLR_ExitProcess(1);
        return;
    }

    PNSLR_Allocator testAllocator = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 8 * 1024 * 1024, CURRENT_LOC(), nil);
    G_BufferedMessagesAllocator   = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 8 * 1024 * 1024, CURRENT_LOC(), nil);
    G_BufferedMessages            = PNSLR_MakeSlice(BufferedMessage, 1024, true, G_BufferedMessagesAllocator, nil);
    G_NumBufferedMessages         = 0;

    for (i32 i = 0; i < (i32) tests.count; ++i)
    {
        TestContext      ctx  = {.testAllocator = testAllocator, .args = args};
        TestFunctionInfo info = tests.data[i];

        PNSLR_FreeAll(ctx.testAllocator, CURRENT_LOC(), nil);

        BufferedMessage startMsg = {.loc = {0}, .type = BufferedMessageType_TestFnStart, .msg = info.name};
        BufferMessage(&startMsg);

        info.fn(&ctx);

        BufferedMessage endMsg = {.loc = {0}, .type = BufferedMessageType_TestFnEnd, .msg = info.name};
        BufferMessage(&endMsg);
    }

    PNSLR_DestroyAllocator_Arena(G_BufferedMessagesAllocator, CURRENT_LOC(), nil);
    PNSLR_DestroyAllocator_Arena(testAllocator,               CURRENT_LOC(), nil);
}

PNSLR_EXECUTABLE_ENTRY_POINT(TestRunnerMain)
