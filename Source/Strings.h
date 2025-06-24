#ifndef PNSLR_STRINGS_H // =========================================================
#define PNSLR_STRINGS_H

#include "__Prelude.h"

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
i32 PNSLR_GetStringLength(cstring str);

/**
 * Clone a C-style string into a new allocated string.
 */
utf8str PNSLR_StringFromCString(cstring str);

#endif // PNSLR_STRINGS_H ==========================================================
