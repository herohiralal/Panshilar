#ifdef _MSC_VER
    #pragma warning(disable: 4464) // relative include path contains '..'
#endif

#define PNSLR_IMPLEMENTATION
#include "../../Source/Dependencies/PNSLR_Intrinsics/Platforms.h"
#include "../../Source/Dependencies/PNSLR_Intrinsics/Compiler.h"
#include "../../Source/Dependencies/PNSLR_Intrinsics/Warnings.h"
PNSLR_SUPPRESS_WARN
#if PNSLR_UNIX
    #define _POSIX_C_SOURCE 200809L
    #define _XOPEN_SOURCE 700
#endif
#include <stdio.h>
PNSLR_UNSUPPRESS_WARN
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

PNSLR_DECLARE_ARRAY_SLICE(BufferedMessage);

static thread_local PNSLR_ArraySlice(BufferedMessage) G_BufferedMessages           = {0};
static thread_local u64                               G_NumBufferedMessages        = {0};
static thread_local PNSLR_Allocator                   G_CurrentTestRunnerAllocator = {0};

static inline void BufferMessage(const BufferedMessage* msg)
{
    if (G_NumBufferedMessages >= (u64) G_BufferedMessages.count)
    {
        PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
        PNSLR_ResizeSlice(BufferedMessage, &G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_CurrentTestRunnerAllocator, CURRENT_LOC(), &err);

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
void TestRunnerMain(PNSLR_ArraySlice(utf8str) args)
{
    setvbuf(stdout, NULL, _IONBF, 0); // disable stdout buffering

    PNSLR_Path tgtDir = {0};
    {
        PNSLR_Platform     plt  = PNSLR_GetPlatform();
        PNSLR_Architecture arch = PNSLR_GetArchitecture();

        if (plt == PNSLR_Platform_Windows || plt == PNSLR_Platform_Linux || plt == PNSLR_Platform_OSX)
        {
            utf8str executableName = {0};

            if (plt == PNSLR_Platform_Windows) { executableName = PNSLR_STRING_LITERAL("Binaries\\TestRunner-windows-x64.exe"); }
            else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_X64) { executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-linux-x64"); }
            else if (plt == PNSLR_Platform_Linux && arch == PNSLR_Architecture_ARM64) { executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-linux-arm64"); }
            else if (plt == PNSLR_Platform_OSX) { executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-osx-arm64"); }
            else if (plt == PNSLR_Platform_OSX && arch == PNSLR_Architecture_X64) { executableName = PNSLR_STRING_LITERAL("Binaries/TestRunner-osx-x64"); }
            else { printf("Unsupported platform or architecture."); PNSLR_ExitProcess(1); return; }

            if (!args.count) { printf("Need at least one arg to extract location."); PNSLR_ExitProcess(1); return; }

            b8 firstArgIsExecutable = PNSLR_StringEndsWith(args.data[0], executableName, PNSLR_StringComparisonType_CaseInsensitive);

            if (!firstArgIsExecutable) { printf("First argument must be the executable name."); PNSLR_ExitProcess(1); return; }

            utf8str dirRaw = args.data[0];
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
        u64                                testsCount = ZZZZ_GetTestsCount();
        PNSLR_AllocatorError               err        = PNSLR_AllocatorError_None;
        PNSLR_ArraySlice(TestFunctionInfo) tests2     = PNSLR_MakeSlice(TestFunctionInfo, testsCount, false, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), &err);

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

    G_CurrentTestRunnerAllocator = PNSLR_NewAllocator_Arena(PNSLR_GetAllocator_DefaultHeap(), 8 * 1024 * 1024, CURRENT_LOC(), nullptr);

    for (i32 i = 0; i < (i32) tests.count; ++i)
    {
        TestContext      ctx  = {.tgtDir = tgtDir,.testAllocator = G_CurrentTestRunnerAllocator, .args = args};
        TestFunctionInfo info = tests.data[i];

        PNSLR_FreeAll(ctx.testAllocator, CURRENT_LOC(), nullptr);
        G_BufferedMessages    = PNSLR_MakeSlice(BufferedMessage, 128, true, G_CurrentTestRunnerAllocator, CURRENT_LOC(), nullptr);
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

    PNSLR_DestroyAllocator_Arena(G_CurrentTestRunnerAllocator, CURRENT_LOC(), nullptr);

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

i32 main(i32 argc, char** argv)
{
    PNSLR_ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_GetAllocator_DefaultHeap(), CURRENT_LOC(), nullptr);
    for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); }
    TestRunnerMain(args);
    return 0;
}

#include "../../Source/zzzz_Unity.c"
