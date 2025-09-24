#ifndef PNSLR_LOGGER_H // ==========================================================
#define PNSLR_LOGGER_H
#include "__Prelude.h"
#include "Runtime.h"
#include "Stream.h"
#include "Sync.h"
EXTERN_C_BEGIN

// Types ===========================================================================

/**
 * Defines the logging levels.
 */
ENUM_START(PNSLR_LoggerLevel, u8)
    #define PNSLR_LoggerLevel_Debug    ((PNSLR_LoggerLevel) 0)
    #define PNSLR_LoggerLevel_Info     ((PNSLR_LoggerLevel) 1)
    #define PNSLR_LoggerLevel_Warn     ((PNSLR_LoggerLevel) 2)
    #define PNSLR_LoggerLevel_Error    ((PNSLR_LoggerLevel) 3)
    #define PNSLR_LoggerLevel_Critical ((PNSLR_LoggerLevel) 4)
ENUM_END

/**
 * Defines options for logging output.
 */
ENUM_FLAGS_START(PNSLR_LogOption, u8)
    #define PNSLR_LogOption_None             ((PNSLR_LogOption) (     0))
    #define PNSLR_LogOption_IncludeLevel     ((PNSLR_LogOption) (1 << 0))
    #define PNSLR_LogOption_IncludeDate      ((PNSLR_LogOption) (1 << 1))
    #define PNSLR_LogOption_IncludeTime      ((PNSLR_LogOption) (1 << 2))
    #define PNSLR_LogOption_IncludeFile      ((PNSLR_LogOption) (1 << 3))
    #define PNSLR_LogOption_IncludeFn        ((PNSLR_LogOption) (1 << 4))
    #define PNSLR_LogOption_IncludeColours   ((PNSLR_LogOption) (1 << 5))
ENUM_END

/**
 * Defines the delegate type for the logger function.
 */
typedef void (*PNSLR_LoggerProcedure)(
    rawptr                   loggerData,
    PNSLR_LoggerLevel        level,
    utf8str                  data,
    PNSLR_LogOption         options,
    PNSLR_SourceCodeLocation location
);

/**
 * Defines a generic logger structure that can be used to log messages.
 */
typedef struct PNSLR_Logger
{
    PNSLR_LoggerProcedure procedure;
    rawptr                data;
    PNSLR_LoggerLevel     minAllowedLvl;
    PNSLR_LogOption      options;
} PNSLR_Logger;

// Default Logger Control ==========================================================

/**
 * Sets the default logger FOR THE CURRENT THREAD.
 * By default, every thread gets a thread-safe default logger that:
 * - logs to stdout on desktop platforms
 * - logs to logcat on Android
 */
void PNSLR_SetDefaultLogger(PNSLR_Logger logger);

/**
 * Disables the default logger FOR THE CURRENT THREAD.
 */
void PNSLR_DisableDefaultLogger(void);

// Default Logger Non-Format Log Functions =========================================

void PNSLR_LogD(utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogI(utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogW(utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogE(utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogC(utf8str msg, PNSLR_SourceCodeLocation loc);

// Default Logger Formatted Log Functions ==========================================

void PNSLR_LogDf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogIf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogWf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogEf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogCf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);

// Custom Logger Non-Format Log Functions =========================================

void PNSLR_LogLD(PNSLR_Logger logger, utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLI(PNSLR_Logger logger, utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLW(PNSLR_Logger logger, utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLE(PNSLR_Logger logger, utf8str msg, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLC(PNSLR_Logger logger, utf8str msg, PNSLR_SourceCodeLocation loc);

// Custom Logger Formatted Log Functions ==========================================

void PNSLR_LogLDf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLIf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLWf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLEf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogLCf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);

// Logger functions with explicit level ============================================

void PNSLR_Log(                       PNSLR_LoggerLevel level, utf8str msg,                                                      PNSLR_SourceCodeLocation loc);
void PNSLR_Logf(                      PNSLR_LoggerLevel level, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);
void PNSLR_LogL( PNSLR_Logger logger, PNSLR_LoggerLevel level, utf8str msg,                                                      PNSLR_SourceCodeLocation loc);
void PNSLR_LogLf(PNSLR_Logger logger, PNSLR_LoggerLevel level, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc);

// Logger Casts ====================================================================

/**
 * Creates a logger that writes to the given file.
 * The file must be opened and valid.
 */
PNSLR_Logger PNSLR_LoggerFromFile(PNSLR_File f, PNSLR_LoggerLevel minAllowedLevel, PNSLR_LogOption options OPT_ARG);

/**
 * Creates a logger that uses the default outputs (see `PNSLR_SetDefaultLogger()`).
 * The returned logger is thread-safe and can be used from any thread.
 * This can be used along with `PNSLR_SetDefaultLogger()` to customize
 * the behaviour of the default in-built logger.
 */
PNSLR_Logger PNSLR_GetDefaultLoggerWithOptions(PNSLR_LoggerLevel minAllowedLevel, PNSLR_LogOption options OPT_ARG);

/**
 * Creates a nil logger that does nothing.
 * This can be used to disable logging in certain parts of the code.
 */
PNSLR_Logger PNSLR_GetNilLogger(void);

EXTERN_C_END
#endif // PNSLR_LOGGER_H ===========================================================
