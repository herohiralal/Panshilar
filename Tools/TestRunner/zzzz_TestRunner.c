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

#if PNSLR_ANDROID
    #define TR_PRINTI(f)       ((void) __android_log_print(ANDROID_LOG_INFO,  "TestRunner", f             ))
    #define TR_PRINTW(f)       ((void) __android_log_print(ANDROID_LOG_WARN,  "TestRunner", f             ))
    #define TR_PRINTE(f)       ((void) __android_log_print(ANDROID_LOG_ERROR, "TestRunner", f             ))
    #define TR_PRINTFI(f, ...) ((void) __android_log_print(ANDROID_LOG_INFO,  "TestRunner", f, __VA_ARGS__))
    #define TR_PRINTFW(f, ...) ((void) __android_log_print(ANDROID_LOG_WARN,  "TestRunner", f, __VA_ARGS__))
    #define TR_PRINTFE(f, ...) ((void) __android_log_print(ANDROID_LOG_ERROR, "TestRunner", f, __VA_ARGS__))
#else
    #define TR_PRINTI(f)       ((void) printf("\033[32m" f "\033[0m\n"             ))
    #define TR_PRINTW(f)       ((void) printf("\033[33m" f "\033[0m\n"             ))
    #define TR_PRINTE(f)       ((void) printf("\033[31m" f "\033[0m\n"             ))
    #define TR_PRINTFI(f, ...) ((void) printf("\033[32m" f "\033[0m\n", __VA_ARGS__))
    #define TR_PRINTFW(f, ...) ((void) printf("\033[33m" f "\033[0m\n", __VA_ARGS__))
    #define TR_PRINTFE(f, ...) ((void) printf("\033[31m" f "\033[0m\n", __VA_ARGS__))
#endif

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
        PNSLR_ResizeSlice(BufferedMessage, &G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), &err);

        if (err != PNSLR_AllocatorError_None)
        {
            TR_PRINTE("Error resizing buffered messages array.");
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
        PNSLR_Platform plt  = PNSLR_GetPlatform();

        if (plt == PNSLR_Platform_Windows || plt == PNSLR_Platform_Linux)
        {
            if (!args.count) { TR_PRINTE("Need at least one arg to extract location."); PNSLR_ExitProcess(1); return; }

            PNSLR_Path execPath = PNSLR_NormalisePath(args.data[0], PNSLR_PathNormalisationType_File, PNSLR_GetAllocator_DefaultHeap());
            PNSLR_Path execParent = {0};
            if (!PNSLR_SplitPath(execPath, &execParent, nil, nil, nil)) { TR_PRINTE("Failed to split executable path."); PNSLR_ExitProcess(1); return; }

            utf8str execParentName = {0};
            if (!PNSLR_SplitPath(execParent, &tgtDir, &execParentName, nil, nil)) { TR_PRINTE("Failed to split executable parent path."); PNSLR_ExitProcess(1); return; }
            if (!PNSLR_AreStringsEqual(execParentName, PNSLR_StringLiteral("Binaries"), 0)) { TR_PRINTE("Executable not in expected 'Binaries' directory."); PNSLR_ExitProcess(1); return; }
        }
        else
        {
            tgtDir = (PNSLR_Path) {0};
        }
    }

    PNSLR_ArraySlice(TestFunctionInfo) tests = {0};
    {
        u64                                testsCount = ZZZZ_GetTestsCount();
        PNSLR_AllocatorError               err        = PNSLR_AllocatorError_None;
        PNSLR_ArraySlice(TestFunctionInfo) tests2     = PNSLR_MakeSlice(TestFunctionInfo, testsCount, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), &err);

        tests = tests2;
    }

    if (!tests.count || !tests.data)
    {
        TR_PRINTW("No tests found.");
        PNSLR_ExitProcess(1);
        return;
    }

    ZZZZ_GetAllTests(tests);
    b8 success = true;

    G_CurrentTestRunnerAllocator = PNSLR_NewAllocator_Arena(PNSLR_GetAllocator_DefaultHeap(), 8 * 1024 * 1024, PNSLR_GET_LOC(), nullptr);

    for (i32 i = 0; i < (i32) tests.count; ++i)
    {
        TestContext      ctx  = {.tgtDir = tgtDir,.testAllocator = G_CurrentTestRunnerAllocator, .args = args};
        TestFunctionInfo info = tests.data[i];

        PNSLR_FreeAll(ctx.testAllocator, PNSLR_GET_LOC(), nullptr);
        G_BufferedMessages    = PNSLR_MakeSlice(BufferedMessage, 128, true, G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), nullptr);
        G_NumBufferedMessages = 0;

        info.fn(&ctx);

        TR_PRINTFI("==== [%.*s] ====", (i32) info.name.count, info.name.data);

        i32 checkCount = 0, passCount = 0;
        for (i32 j = 0; j < (i32) G_NumBufferedMessages; ++j)
        {
            BufferedMessage* msg = &G_BufferedMessages.data[j];

            switch (msg->type)
            {
                case BufferedMessageType_TestFnLog:
                    TR_PRINTFI("INFO  : %.*s", (i32) msg->msg.count, msg->msg.data);
                    TR_PRINTFI("        from %.*s:%d", (i32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
                    break;
                case BufferedMessageType_AssertPass:
                    checkCount++;
                    passCount++;
                    break;
                case BufferedMessageType_AssertFail:
                    success = false;
                    checkCount++;
                    TR_PRINTFE("ERROR : %.*s", (i32) msg->msg.count, msg->msg.data);
                    TR_PRINTFE("        from %.*s:%d", (i32) msg->loc.file.count, msg->loc.file.data, msg->loc.line);
                    break;
                default:
                    TR_PRINTE("ERROR_UNKNOWN_MESSAGE");
                    break;
            }
        }

        TR_PRINTFI("RESULT: (%d/%d).", passCount, checkCount);
        if (checkCount == passCount) { TR_PRINTI("All tests passed. (^_^)        "); }
        else                         { TR_PRINTE("One or more tests failed. (>_<)"); }

        utf8str tmpEndStr = PNSLR_MakeString(info.name.count, false, ctx.testAllocator, PNSLR_GET_LOC(), nil);
        for (i64 j = 0; j < info.name.count; ++j) { tmpEndStr.data[j] = '='; }
        TR_PRINTFI("======%.*s======\n", (i32) tmpEndStr.count, tmpEndStr.data);
        PNSLR_FreeString(tmpEndStr, ctx.testAllocator, PNSLR_GET_LOC(), nil);
    }

    PNSLR_DestroyAllocator_Arena(G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), nullptr);

    if (!success)
    {
        TR_PRINTE("=============== One or more tests failed. (>_<) ===============");
        PNSLR_ExitProcess(1);
    }
    else
    {
        TR_PRINTI("=================== All tests passed. (^_^) ===================");
    }
}

#if PNSLR_DESKTOP

    i32 main(i32 argc, char** argv)
    {
        PNSLR_ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nullptr);
        for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); }
        TestRunnerMain(args);
        return 0;
    }

#elif PNSLR_ANDROID

    extern void android_main(struct android_app* app)
    {
        app->onAppCmd = nil;
        TestRunnerMain((PNSLR_ArraySlice(utf8str)) {0});
        ANativeActivity_finish(app->activity);
    }

#elif PNSLR_IOS

// no op here, we'll do it in objc

#else
    #error "Unsupported platform for test runner."
#endif

#include "../../Source/zzzz_Unity.c"
