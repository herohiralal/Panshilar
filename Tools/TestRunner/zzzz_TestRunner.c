#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "zzzz_TestRunner.h"
#include "zzzz_GeneratedCombinedTests.c"

ENUM_START(BufferedMessageType, u8)
    #define BufferedMessageType_Invalid     ((BufferedMessageType) 0)
    #define BufferedMessageType_TestFnLog   ((BufferedMessageType) 1)
    #define BufferedMessageType_AssertPass  ((BufferedMessageType) 2)
    #define BufferedMessageType_AssertFail  ((BufferedMessageType) 3)
ENUM_END

typedef struct
{
    BufferedMessageType      type;
    utf8str                  msg;
    PNSLR_SourceCodeLocation loc;
} BufferedMessage;

DECLARE_ARRAY_SLICE(BufferedMessage);

static thread_local ArraySlice(BufferedMessage) G_BufferedMessages           = {0};
static thread_local u64                         G_NumBufferedMessages        = {0};
static thread_local PNSLR_Allocator             G_CurrentTestRunnerAllocator = {0};

static inline void BufferMessage(const BufferedMessage* msg)
{
    if (G_NumBufferedMessages >= (u64) G_BufferedMessages.count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(BufferedMessage, G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_CurrentTestRunnerAllocator, &err);

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
        .msg  = message,
        .loc  = location,
    };

    BufferMessage(&msg);
}

b8 AssertInternal(b8 condition, utf8str message, PNSLR_SourceCodeLocation location)
{
    BufferedMessage msg =
    {
        .type = condition ? BufferedMessageType_AssertPass : BufferedMessageType_AssertFail,
        .msg  = message,
        .loc  = location,
    };

    BufferMessage(&msg);
    return condition;
}

// TODO: make the test runner multi-threaded
void TestRunnerMain(ArraySlice(utf8str) args)
{
    setvbuf(stdout, NULL, _IONBF, 0); // disable stdout buffering

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

    ZZZZ_GetAllTests(tests);
    b8 success = true;

    G_CurrentTestRunnerAllocator = PNSLR_NewAllocator_Arena(PNSLR_DEFAULT_HEAP_ALLOCATOR, 8 * 1024 * 1024, CURRENT_LOC(), nil);

    for (i32 i = 0; i < (i32) tests.count; ++i)
    {
        TestContext      ctx  = {.testAllocator = G_CurrentTestRunnerAllocator, .args = args};
        TestFunctionInfo info = tests.data[i];

        PNSLR_FreeAll(ctx.testAllocator, CURRENT_LOC(), nil);
        G_BufferedMessages    = PNSLR_MakeSlice(BufferedMessage, 128, true, G_CurrentTestRunnerAllocator, nil);
        G_NumBufferedMessages = 0;

        info.fn(&ctx);

        printf("==== [%.*s] ====\n", (i32) info.name.count, info.name.data);

        i32 checkCount = 0, passCount = 0;
        for (i32 j = 0; j < (i32) G_NumBufferedMessages; ++j)
        {
            BufferedMessage* msg = &G_BufferedMessages.data[j];

            switch (msg->type)
            {
                case BufferedMessageType_TestFnLog:
                    printf("INFO  : %.*s\n", (i32) msg->msg.count, msg->msg.data);
                    printf("        from %.*s:%d\n", (i32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
                    break;
                case BufferedMessageType_AssertPass:
                    checkCount++;
                    passCount++;
                    break;
                case BufferedMessageType_AssertFail:
                    success = false;
                    checkCount++;
                    printf("ERROR : %.*s\n", (i32) msg->msg.count, msg->msg.data);
                    printf("        from %.*s:%d\n", (i32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
                    break;
                default:
                    printf("ERROR_UNKNOWN_MESSAGE\n");
                    break;
            }
        }

        printf("RESULT: (%d/%d).\n", passCount, checkCount);
        if (checkCount == passCount) { printf("All tests passed. (^_^)        \n"); }
        else                         { printf("One or more tests failed. (>_<)\n"); }

        printf("======");
        for (i32 j = 0; j < (i32) info.name.count; ++j) { printf("="); }
        printf("======\n\n");
    }

    PNSLR_DestroyAllocator_Arena(G_CurrentTestRunnerAllocator, CURRENT_LOC(), nil);

    if (!success) { PNSLR_ExitProcess(1); }
}

PNSLR_EXECUTABLE_ENTRY_POINT(TestRunnerMain)
