#ifndef PNSLR_PROCESS_H // =========================================================
#define PNSLR_PROCESS_H
#include "__Prelude.h"
#include "Allocators.h"
EXTERN_C_BEGIN

/**
 * Exits the current process immediately with the specified exit code.
 */
void PNSLR_ExitProcess(i32 exitCode);

/**
 * A key-value pair representing an environment variable.
 */
typedef struct PNSLR_EnvVarKeyValuePair
{
    utf8str key;
    utf8str value;
} PNSLR_EnvVarKeyValuePair;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_EnvVarKeyValuePair);

/**
 * Retrieves all environment variables as an array slice of key-value pairs.
 * The returned array slice is allocated using the provided allocator.
 * The individual strings within the key-value pairs are also individually
 * allocated using the same allocator.
 */
PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) PNSLR_GetEnvironmentVariables(PNSLR_Allocator allocator);

EXTERN_C_END
#endif // PNSLR_PROCESS_H ==========================================================
