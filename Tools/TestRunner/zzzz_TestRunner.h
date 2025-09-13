#ifndef PNSLR_TEST_RUNNER_H // =====================================================
#define PNSLR_TEST_RUNNER_H

#include "../../Source/Dependencies/PNSLR_Intrinsics/Macros.h"
#include "../../Bindings/C/Panshilar.h"

#undef true
#undef false
#undef nullptr
#define true    ((PNSLR_B8) 1)
#define false   ((PNSLR_B8) 0)
#define nullptr ((void*)    0)

typedef struct
{
    PNSLR_Path                      tgtDir;
    PNSLR_Allocator                 testAllocator;
    PNSLR_ArraySlice(PNSLR_UTF8STR) args;
} TestContext;

PNSLR_B8 AssertInternal(PNSLR_B8 condition, PNSLR_UTF8STR message, PNSLR_SourceCodeLocation location);

#define Assert(cond) \
    AssertInternal((PNSLR_B8) (cond), PNSLR_StringLiteral("Assertion failed: " #cond), PNSLR_GET_LOC())

#define AssertMsg(cond, msg) \
    AssertInternal((PNSLR_B8) (cond), PNSLR_StringLiteral("Assertion failed: " msg), PNSLR_GET_LOC())

void LogInternal(PNSLR_UTF8STR message, PNSLR_SourceCodeLocation location);

#define Log(msg) \
    LogInternal(PNSLR_StringLiteral(msg), PNSLR_GET_LOC())

typedef void (*TestFunction)(const TestContext* ctx);

typedef struct
{
    PNSLR_UTF8STR      name;
    TestFunction fn;
} TestFunctionInfo;

PNSLR_DECLARE_ARRAY_SLICE(TestFunctionInfo);

#endif // PNSLR_TEST_RUNNER_H ======================================================

/**
 * This section helps with LSP compatibility and makaing it compile without any real problems.
 * In general, `MAIN_TEST_FN` is added by the codegen done by the build script.
 */
#ifndef MAIN_TEST_FN
#define MAIN_TEST_FN(ctxArgName) void ZZZZ_Test_THIS_IS_FOR_LSP_COMPATIBILITY_ONLY_CHECK_BUILD_SCRIPT(const TestContext* ctxArgName)
#endif
