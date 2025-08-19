#ifndef PNSLR_RUNTIME_H // =========================================================
#define PNSLR_RUNTIME_H
#include "__Prelude.h"
EXTERN_C_BEGIN

/**
 * Defines the source code location for debugging purposes.
 */
typedef struct PNSLR_SourceCodeLocation
{
    /*
    KEEP THE FUCKING ORDER IN SYNC WITH `CURRENT_LOC()` MACRO
    */

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
    PNSLR_ARG_ASSIGN(file    )  PNSLR_STRING_LITERAL(__FILE__), \
    PNSLR_ARG_ASSIGN(line    )  __LINE__, \
    PNSLR_ARG_ASSIGN(column  )  0, /* not available but might get forwarded from somewhere */ \
    PNSLR_ARG_ASSIGN(function) PNSLR_STRING_LITERAL(__FUNCTION__) \
}

//-skipreflect

EXTERN_C_END
#endif // PNSLR_RUNTIME_H ==========================================================
