#ifndef PNSLR_RUNTIME_H // =========================================================
#define PNSLR_RUNTIME_H

#include "__Prelude.h"

EXTERN_C_BEGIN

/**
 * Defines the source code location for debugging purposes.
 */
typedef struct PNSLR_SourceCodeLocation
{
    utf8str file;     // File name
    i32     line;     // Line number
    i32     column;   // Column number
    utf8str function; // Function name
} PNSLR_SourceCodeLocation;

//+skipreflect

/**
 * Get the current source code location.
 */
#define CURRENT_LOC() (PNSLR_SourceCodeLocation) \
{ \
    .file     = PNSLR_STRING_LITERAL(__FILE__), \
    .line     = __LINE__, \
    .column   = 0, /* not available but might get forwarded from somewhere */ \
    .function = PNSLR_STRING_LITERAL(__FUNCTION__) \
}

//-skipreflect

EXTERN_C_END

#endif // PNSLR_RUNTIME_H ==========================================================
