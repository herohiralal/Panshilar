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
    KEEP THE FUCKING ORDER IN SYNC WITH `PNSLR_GET_LOC()` MACRO
    */

    utf8str file;     // File name
    i32     line;     // Line number
    i32     column;   // Column number
    utf8str function; // Function name
} PNSLR_SourceCodeLocation;

//+skipreflect

#ifdef __cplusplus

    /**
     * Get the current source code location.
     */
    #define PNSLR_GET_LOC() PNSLR_SourceCodeLocation \
    { \
        PNSLR_StringLiteral(__FILE__), \
        __LINE__, \
        0, /* not available but might get forwarded from somewhere */ \
        PNSLR_StringLiteral(__FUNCTION__) \
    }

#else

    /**
     * Get the current source code location.
     */
    #define PNSLR_GET_LOC() (PNSLR_SourceCodeLocation) \
    { \
        .file     = PNSLR_StringLiteral(__FILE__), \
        .line     = __LINE__, \
        .column   = 0, /* not available but might get forwarded from somewhere */ \
        .function = PNSLR_StringLiteral(__FUNCTION__) \
    }

#endif

//-skipreflect

EXTERN_C_END
#endif // PNSLR_RUNTIME_H ==========================================================
