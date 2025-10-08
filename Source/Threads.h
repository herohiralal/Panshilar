#ifndef PNSLR_THREADS_H // =========================================================
#define PNSLR_THREADS_H
#include "__Prelude.h"
#include "Allocators.h"
EXTERN_C_BEGIN

/**
 * An opaque handle to a thread.
 */
typedef struct PNSLR_ThreadHandle { u64 handle; } PNSLR_ThreadHandle;

/**
 * Checks if the handle to a thread is valid.
 */
b8 PNSLR_IsThreadHandleValid(PNSLR_ThreadHandle handle);

/**
 * Gets a handle to the current thread.
 */
PNSLR_ThreadHandle PNSLR_GetCurrentThreadHandle(void);

/**
 * Gets the name of a thread.
 * The returned string is allocated using the provided allocator.
 * If the thread has no name, an empty string is returned.
 */
utf8str PNSLR_GetThreadName(PNSLR_ThreadHandle handle, PNSLR_Allocator allocator);

/**
 * Sets the name of a thread.
 * The name is copied, so the provided string does not need to be valid after this call.
 * On some platforms, thread names may be truncated to a certain length.
 *
 * Thread lengths on platforms (excluding null terminator):
 *     Windows/OSX/iOS - 63 characters
 *     Linux/Android   - 15 characters
 */
void PNSLR_SetThreadName(PNSLR_ThreadHandle handle, utf8str name);

/**
 * Gets the name of the current thread.
 * Read more about `PNSLR_GetThreadName`.
 */
utf8str PNSLR_GetCurrentThreadName(PNSLR_Allocator allocator);

/**
 * Sets the name of the current thread.
 * Read more about `PNSLR_SetThreadName`.
 */
void PNSLR_SetCurrentThreadName(utf8str name);

/**
 * A procedure that can be run on a thread.
 * The `data` parameter is optional user data that can be passed to the thread.
 */
typedef void (*PNSLR_ThreadProcedure)(rawptr data);

/**
 * Start a new thread with the specified procedure and user data.
 */
PNSLR_ThreadHandle PNSLR_StartThread(PNSLR_ThreadProcedure procedure, rawptr data OPT_ARG, utf8str name OPT_ARG);

/**
 * Joins a thread, blocking the calling thread until the specified thread has finished.
 */
void PNSLR_JoinThread(PNSLR_ThreadHandle handle);

/**
 * Sleeps the current thread for the specified number of milliseconds.
 */
void PNSLR_SleepCurrentThread(u64 milliseconds);

EXTERN_C_END
#endif // PNSLR_THREADS_H ==========================================================
