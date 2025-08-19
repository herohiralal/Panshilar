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

#ifndef __cplusplus

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

#else

    inline PNSLR_SourceCodeLocation PNSLR_MakeSourceCodeLocation(const char* file, i64 fileLen, i32 line, const char* function, i64 functionLen)
    {
        PNSLR_SourceCodeLocation loc;
        loc.file     = PNSLR_MakeStringLiteral(file, fileLen);
        loc.line     = line;
        loc.column   = 0; // not available but might get forwarded from somewhere
        loc.function = PNSLR_MakeStringLiteral(function, functionLen);
        return loc;
    }

    /**
     * Get the current source code location.
     */
    #define CURRENT_LOC() \
        PNSLR_MakeSourceCodeLocation(__FILE__, sizeof(__FILE__) - 1, __LINE__, __FUNCTION__, sizeof(__FUNCTION__) - 1)

#endif

//-skipreflect

EXTERN_C_END
#endif // PNSLR_RUNTIME_H ==========================================================
