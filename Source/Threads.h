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

EXTERN_C_END
#endif // PNSLR_THREADS_H ==========================================================
