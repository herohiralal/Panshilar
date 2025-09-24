#ifdef _MSC_VER
    #pragma warning(disable: 4464) // relative include path contains '..'
#endif

#define PNSLR_IMPLEMENTATION
#include "../../Source/__PrivateIncludes.h"
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
        PNSLR_ResizeSlice(BufferedMessage, &G_BufferedMessages, (G_BufferedMessages.count * 2), true, G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), &err);

        if (err != PNSLR_AllocatorError_None)
        {
            PNSLR_LogE(PNSLR_StringLiteral("Error resizing buffered messages array."), PNSLR_GET_LOC());
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
    PNSLR_Path tgtDir = {0};
    {
        PNSLR_Platform plt  = PNSLR_GetPlatform();

        if (plt == PNSLR_Platform_Windows || plt == PNSLR_Platform_Linux)
        {
            if (!args.count) { PNSLR_LogE(PNSLR_StringLiteral("Need at least one arg to extract location."), PNSLR_GET_LOC()); PNSLR_ExitProcess(1); return; }

            PNSLR_Path execPath = PNSLR_NormalisePath(args.data[0], PNSLR_PathNormalisationType_File, PNSLR_GetAllocator_DefaultHeap());
            PNSLR_Path execParent = {0};
            if (!PNSLR_SplitPath(execPath, &execParent, nil, nil, nil)) { PNSLR_LogE(PNSLR_StringLiteral("Failed to split executable path."), PNSLR_GET_LOC()); PNSLR_ExitProcess(1); return; }

            utf8str execParentName = {0};
            if (!PNSLR_SplitPath(execParent, &tgtDir, &execParentName, nil, nil)) { PNSLR_LogE(PNSLR_StringLiteral("Failed to split executable parent path."), PNSLR_GET_LOC()); PNSLR_ExitProcess(1); return; }
            if (!PNSLR_AreStringsEqual(execParentName, PNSLR_StringLiteral("Binaries"), 0)) { PNSLR_LogE(PNSLR_StringLiteral("Executable not in expected 'Binaries' directory."), PNSLR_GET_LOC()); PNSLR_ExitProcess(1); return; }
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
        PNSLR_LogW(PNSLR_StringLiteral("No tests found."), PNSLR_GET_LOC());
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

        PNSLR_LogIf(PNSLR_StringLiteral("==== [$] ===="), PNSLR_FmtArgs(PNSLR_FmtString(info.name)), PNSLR_GET_LOC());

        i32 checkCount = 0, passCount = 0;
        for (i32 j = 0; j < (i32) G_NumBufferedMessages; ++j)
        {
            BufferedMessage* msg = &G_BufferedMessages.data[j];

            switch (msg->type)
            {
                case BufferedMessageType_TestFnLog:
                    PNSLR_LogIf(PNSLR_StringLiteral("INFO  : $"), PNSLR_FmtArgs(PNSLR_FmtString(msg->msg)), msg->loc);
                    break;
                case BufferedMessageType_AssertPass:
                    checkCount++;
                    passCount++;
                    break;
                case BufferedMessageType_AssertFail:
                    success = false;
                    checkCount++;
                    PNSLR_LogEf(PNSLR_StringLiteral("ERROR : $"), PNSLR_FmtArgs(PNSLR_FmtString(msg->msg)), msg->loc);
                    break;
                default:
                    PNSLR_LogE(PNSLR_StringLiteral("ERROR_UNKNOWN_MESSAGE"), PNSLR_GET_LOC());
                    break;
            }
        }

        PNSLR_LogIf(PNSLR_StringLiteral("Result: ($/$)."), PNSLR_FmtArgs(PNSLR_FmtI32(passCount, PNSLR_IntegerBase_Decimal), PNSLR_FmtI32(checkCount, PNSLR_IntegerBase_Decimal)), PNSLR_GET_LOC());
        if (checkCount == passCount) { PNSLR_LogI(PNSLR_StringLiteral("All tests passed. (^_^)        "), PNSLR_GET_LOC()); }
        else                         { PNSLR_LogE(PNSLR_StringLiteral("One or more tests failed. (>_<)"), PNSLR_GET_LOC()); }

        utf8str tmpEndStr = PNSLR_MakeString(info.name.count, false, ctx.testAllocator, PNSLR_GET_LOC(), nil);
        for (i64 j = 0; j < info.name.count; ++j) { tmpEndStr.data[j] = '='; }
        PNSLR_LogIf(PNSLR_StringLiteral("======$======\n"), PNSLR_FmtArgs(PNSLR_FmtString(tmpEndStr)), PNSLR_GET_LOC());
        PNSLR_FreeString(tmpEndStr, ctx.testAllocator, PNSLR_GET_LOC(), nil);
    }

    PNSLR_DestroyAllocator_Arena(G_CurrentTestRunnerAllocator, PNSLR_GET_LOC(), nullptr);

    if (!success)
    {
        PNSLR_LogE(PNSLR_StringLiteral("=============== One or more tests failed. (>_<) ==============="), PNSLR_GET_LOC());
        PNSLR_ExitProcess(1);
    }
    else
    {
        PNSLR_LogI(PNSLR_StringLiteral("=================== All tests passed. (^_^) ==================="), PNSLR_GET_LOC());
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
