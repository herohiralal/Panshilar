#ifndef PNSLR_RUNTIME // ===========================================================
#define PNSLR_RUNTIME

#include "__Prelude.h"

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

#endif // PNSLR_RUNTIME ============================================================
