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
 * The `kvp` field contains the full "KEY=VALUE" string.
 * The 'key' field contains the key part.
 * The 'value' field contains the value part.
 */
typedef struct PNSLR_EnvVarKeyValuePair
{
    utf8str kvp;
    utf8str key;
    utf8str value;
} PNSLR_EnvVarKeyValuePair;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_EnvVarKeyValuePair);

/**
 * Retrieves all environment variables as an array slice of key-value pairs.
 * The returned array slice is allocated using the provided allocator.
 * The individual strings within the key-value pairs are also allocated using the same allocator.
 * For the key-value pairs, the `kvp` field contains the full "KEY=VALUE" string,
 * while the `key` and `value` fields are just 'views' into that string.
 */
PNSLR_ArraySlice(PNSLR_EnvVarKeyValuePair) PNSLR_GetEnvironmentVariables(PNSLR_Allocator allocator);

/**
 * Opaque handle to a pipe.
 * On Windows, this is a HANDLE.
 * On Unix-like systems, this is a file descriptor (int).
 * This is used for inter-process communication (IPC).
 */
typedef struct PNSLR_PipeHandle
{
    u64 platformHandle;
} PNSLR_PipeHandle;

/**
 * Creates a pipe and returns the read and write ends.
 * The read end is used for reading data from the pipe.
 * The write end is used for writing data to the pipe.
 */
b8 PNSLR_CreatePipe(PNSLR_PipeHandle* outR, PNSLR_PipeHandle* outW);

/**
 * Gets the number of bytes available to read from the read end of the pipe.
 * The size is stored in `outSize`.
 *
 * Note that this function does not block, and the size may change after
 * this function returns.
 */
b8 PNSLR_GetRemainingPipeReadSize(PNSLR_PipeHandle pipe, i64* outSize);

/**
 * Reads data from the read end of the pipe into the provided buffer.
 * The number of bytes read is stored in `readSize` if it's not null.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromPipe(PNSLR_PipeHandle pipe, PNSLR_ArraySlice(u8) dst, i64* readSize OPT_ARG);

/**
 * Writes data to the write end of the pipe from the provided buffer.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteToPipe(PNSLR_PipeHandle pipe, PNSLR_ArraySlice(u8) src);

/**
 * Closes the pipe handle, releasing any associated resources.
 * After calling this function, the pipe handle should not be used again.
 */
b8 PNSLR_ClosePipe(PNSLR_PipeHandle pipe);

/**
 * A handle to a process.
 * The `pid` field is the process ID.
 * On Windows, this is `dwProcessId`.
 * On Unix-like systems, this is the PID.
 * The `handle` field is a platform-specific handle to the process.
 * On Windows, this is a HANDLE.
 * On Unix-like systems, this is pidfd.
 */
typedef struct PNSLR_ProcessHandle
{
    i64 pid;
    u64 handle;
} PNSLR_ProcessHandle;

/**
 * Starts a new process with the specified executable and arguments.
 * Optionally, environment variables, working directory, and pipes for
 * standard output and error can be provided.
 *
 * If not provided, environment variables and working directory are inherited
 * from the current process. If provided, they must be in a 'KEY=VALUE' format.
 *
 * The pipe handles provided must be read ends for stdout and stderr respectively.
 * If null, the respective output is discarded.
 */
b8 PNSLR_StartProcess(
    PNSLR_ProcessHandle*      outProcessHandle,
    PNSLR_ArraySlice(utf8str) execAndArgs,
    PNSLR_ArraySlice(utf8str) environmentVariables OPT_ARG,
    utf8str                   workingDirectory     OPT_ARG,
    PNSLR_PipeHandle*         stdOutPipe           OPT_ARG,
    PNSLR_PipeHandle*         stdErrPipe           OPT_ARG
);

EXTERN_C_END
#endif // PNSLR_PROCESS_H ==========================================================
