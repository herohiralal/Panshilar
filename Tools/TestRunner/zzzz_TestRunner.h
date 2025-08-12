#ifndef PNSLR_TEST_RUNNER_H // =====================================================
#define PNSLR_TEST_RUNNER_H

#include "Panshilar.h"

typedef struct
{
    PNSLR_Allocator     testAllocator;
    ArraySlice(utf8str) args;
} TestContext;

inline b8 AssertInternal(b8 condition, utf8str message, PNSLR_SourceCodeLocation location);

#define Assert(cond) \
    AssertInternal((cond), PNSLR_STRING_LITERAL("Assertion failed: " #cond), CURRENT_LOC())

#define AssertMsg(cond, msg) \
    AssertInternal((cond), PNSLR_STRING_LITERAL("Assertion failed: " msg), CURRENT_LOC())

inline void LogInternal(utf8str message, PNSLR_SourceCodeLocation location);

#define Log(msg) \
    LogInternal(PNSLR_STRING_LITERAL(msg), CURRENT_LOC())

typedef void (*TestFunction)(const TestContext* ctx);

typedef struct
{
    utf8str      name;
    TestFunction fn;
} TestFunctionInfo;

DECLARE_ARRAY_SLICE(TestFunctionInfo);

#endif // PNSLR_TEST_RUNNER_H ======================================================

/**
 * This section helps with LSP compatibility and makaing it compile without any real problems.
 * In general, `MAIN_TEST_FN` is added by the codegen done by the build script.
 */
#ifndef MAIN_TEST_FN
#define MAIN_TEST_FN(ctxArgName) void ZZZZ_Test_THIS_IS_FOR_LSP_COMPATIBILITY_ONLY_CHECK_BUILD_SCRIPT(const TestContext* ctxArgName)
#endif
