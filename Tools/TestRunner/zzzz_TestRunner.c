#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
#if PNSLR_MSVC
    #define thread_local            __declspec(thread)
    #define alignof(type)           __alignof(type)
#elif (PNSLR_CLANG || PNSLR_GCC)
    #define thread_local            __thread
    #define alignof(type)           __alignof__(type)
#else
    #error "Required features not supported by this compiler."
#endif
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "zzzz_TestRunner.h"
#include "zzzz_GeneratedCombinedTests.c"

typedef PNSLR_U8 BufferedMessageType;
#define BufferedMessageType_Invalid     ((BufferedMessageType) 0)
#define BufferedMessageType_TestFnLog   ((BufferedMessageType) 1)
#define BufferedMessageType_AssertPass  ((BufferedMessageType) 2)
#define BufferedMessageType_AssertFail  ((BufferedMessageType) 3)

typedef struct
{
    BufferedMessageType      type;
    PNSLR_UTF8STR                  msg;
    PNSLR_SourceCodeLocation loc;
} BufferedMessage;

PNSLR_DECLARE_ARRAY_SLICE(BufferedMessage);

static thread_local PNSLR_ArraySlice(BufferedMessage) G_BufferedMessages           = {0};
static thread_local PNSLR_U64                         G_NumBufferedMessages        = {0};
static thread_local PNSLR_Allocator             G_CurrentTestRunnerAllocator = {0};

static inline void BufferMessage(const BufferedMessage* msg)
{
    if (G_NumBufferedMessages >= (PNSLR_U64) G_BufferedMessages.count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(BufferedMessage, &G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), &err);

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

void LogInternal(PNSLR_UTF8STR message, PNSLR_SourceCodeLocation location)
{
    BufferedMessage msg =
    {
        .type = BufferedMessageType_TestFnLog,
        .msg  = message,
        .loc  = location,
    };

    BufferMessage(&msg);
}

PNSLR_B8 AssertInternal(PNSLR_B8 condition, PNSLR_UTF8STR message, PNSLR_SourceCodeLocation location)
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
void TestRunnerMain(PNSLR_ArraySlice(PNSLR_UTF8STR) args)
{
    setvbuf(stdout, NULL, _IONBF, 0); // disable stdout buffering

    PNSLR_Path tgtDir = {0};
    {
        PNSLR_Platform     plt  = PNSLR_GetPlatform();
        PNSLR_Architecture arch = PNSLR_GetArchitecture();

        if (plt == PNSLR_Platform_Windows || plt == PNSLR_Platform_Linux || plt == PNSLR_Platform_OSX)
        {
            PNSLR_UTF8STR executableName = {0};

            if (plt == PNSLR_Platform_Windows) { executableName = PNSLR_StringLiteral("Binaries\\TestRunner-windows-x64.exe"); }
            else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_X64) { executableName = PNSLR_StringLiteral("Binaries/TestRunner-linux-x64"); }
            else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_ARM64) { executableName = PNSLR_StringLiteral("Binaries/TestRunner-linux-arm64"); }
            else if (plt == PNSLR_Platform_OSX) { executableName = PNSLR_StringLiteral("Binaries/TestRunner-osx-arm64"); }
            else if (plt == PNSLR_Platform_OSX && arch == PNSLR_Architecture_X64) { executableName = PNSLR_StringLiteral("Binaries/TestRunner-osx-x64"); }
            else { printf("Unsupported platform or architecture."); PNSLR_ExitProcess(1); return; }

            if (!args.count) { printf("Need at least one arg to extract location."); PNSLR_ExitProcess(1); return; }

            PNSLR_B8 firstArgIsExecutable = PNSLR_StringEndsWith(args.data[0], executableName, PNSLR_StringComparisonType_CaseInsensitive);

            if (!firstArgIsExecutable) { printf("First argument must be the executable name."); PNSLR_ExitProcess(1); return; }

            PNSLR_UTF8STR dirRaw = args.data[0];
            dirRaw.count -= executableName.count;
            tgtDir = PNSLR_NormalisePath(dirRaw, PNSLR_PathNormalisationType_Directory, PNSLR_GetAllocator_DefaultHeap());
        }
        else if (plt == PNSLR_Platform_Android)
        {
            // TODO: find path
        }
        else if (plt == PNSLR_Platform_iOS)
        {
            // TODO: find path
        }
        else
        {
            printf("Unsupported platform.");
            PNSLR_ExitProcess(1); return;
        }
    }

    PNSLR_ArraySlice(TestFunctionInfo) tests = {0};
    {
        PNSLR_U64                          testsCount = ZZZZ_GetTestsCount();
        PNSLR_AllocatorError               err        = PNSLR_AllocatorError_None;
        PNSLR_ArraySlice(TestFunctionInfo) tests2     = PNSLR_MakeSlice(TestFunctionInfo, testsCount, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), &err);

        tests = tests2;
    }

    if (!tests.count || !tests.data)
    {
        printf("No tests found.\n");
        PNSLR_ExitProcess(1);
        return;
    }

    ZZZZ_GetAllTests(tests);
    PNSLR_B8 success = true;

    G_CurrentTestRunnerAllocator = PNSLR_NewAllocator_Arena(PNSLR_GetAllocator_DefaultHeap(), 8 * 1024 * 1024, PNSLR_GET_LOC(), nullptr);

    for (PNSLR_I32 i = 0; i < (PNSLR_I32) tests.count; ++i)
    {
        TestContext      ctx  = {.tgtDir = tgtDir,.testAllocator = G_CurrentTestRunnerAllocator, .args = args};
        TestFunctionInfo info = tests.data[i];

        PNSLR_FreeAll(ctx.testAllocator, PNSLR_GET_LOC(), nullptr);
        G_BufferedMessages    = PNSLR_MakeSlice(BufferedMessage, 128, true, G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), nullptr);
        G_NumBufferedMessages = 0;

        info.fn(&ctx);

        printf("==== [%.*s] ====\n", (PNSLR_I32) info.name.count, info.name.data);

        PNSLR_I32 checkCount = 0, passCount = 0;
        for (PNSLR_I32 j = 0; j < (PNSLR_I32) G_NumBufferedMessages; ++j)
        {
            BufferedMessage* msg = &G_BufferedMessages.data[j];

            switch (msg->type)
            {
                case BufferedMessageType_TestFnLog:
                    printf("INFO  : %.*s\n", (PNSLR_I32) msg->msg.count, msg->msg.data);
                    printf("        from %.*s:%d\n", (PNSLR_I32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
                    break;
                case BufferedMessageType_AssertPass:
                    checkCount++;
                    passCount++;
                    break;
                case BufferedMessageType_AssertFail:
                    success = false;
                    checkCount++;
                    printf("ERROR : %.*s\n", (PNSLR_I32) msg->msg.count, msg->msg.data);
                    printf("        from %.*s:%d\n", (PNSLR_I32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
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
        for (PNSLR_I32 j = 0; j < (PNSLR_I32) info.name.count; ++j) { printf("="); }
        printf("======\n\n");
    }

    PNSLR_DestroyAllocator_Arena(G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), nullptr);

    if (!success)
    {
        printf("=============== One or more tests failed. (>_<) ===============\n");
        PNSLR_ExitProcess(1);
    }
    else
    {
        printf("=================== All tests passed. (^_^) ===================\n");
    }
}

PNSLR_I32 main(PNSLR_I32 argc, char** argv)
{
    PNSLR_ArraySlice(PNSLR_UTF8STR) args = PNSLR_MakeSlice(PNSLR_UTF8STR, argc, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nullptr);
    for (PNSLR_I32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); }
    TestRunnerMain(args);
    return 0;
}
