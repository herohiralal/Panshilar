#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) envVars = PNSLR_GetEnvironmentVariables(ctx->testAllocator);

    if (!AssertMsg(envVars.count > 0, "No environment variables found."))
        return;

    for (i64 i = 0; i < envVars.count; i++)
    {
        PNSLR_EnvVarKeyValuePair pair = envVars.data[i];

        utf8str logVal = PNSLR_FormatString(
            PNSLR_StringLiteral("Env Var [$]: '$' = '$'"),
            PNSLR_FmtArgs(
                PNSLR_FmtI64(i+1, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtString(pair.key),
                PNSLR_FmtString(pair.value)
            ), ctx->testAllocator);

        LogInternal(logVal, PNSLR_GET_LOC());
    }
}
