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
ENUM_START(PNSLR_LoggerLevel, i8)
    #define PNSLR_LoggerLevel_Debug    ((PNSLR_LoggerLevel) -1)
    #define PNSLR_LoggerLevel_Info     ((PNSLR_LoggerLevel)  0)
    #define PNSLR_LoggerLevel_Warn     ((PNSLR_LoggerLevel)  1)
    #define PNSLR_LoggerLevel_Error    ((PNSLR_LoggerLevel)  2)
    #define PNSLR_LoggerLevel_Critical ((PNSLR_LoggerLevel)  3)
ENUM_END

/**
 * Defines options for logging output.
 */
ENUM_FLAGS_START(PNSLR_LogOptions, u8)
    #define PNSLR_LogOption_None             ((PNSLR_LogOptions) (     0))
    #define PNSLR_LogOption_IncludeLevel     ((PNSLR_LogOptions) (1 << 0))
    #define PNSLR_LogOption_IncludeDate      ((PNSLR_LogOptions) (1 << 1))
    #define PNSLR_LogOption_IncludeTime      ((PNSLR_LogOptions) (1 << 2))
    #define PNSLR_LogOption_IncludeLocation  ((PNSLR_LogOptions) (1 << 3))
    #define PNSLR_LogOption_IncludeColours   ((PNSLR_LogOptions) (1 << 4))
ENUM_END

/**
 * Defines the delegate type for the logger function.
 */
typedef void (*PNSLR_LoggerProcedure)(
    rawptr                   loggerData,
    PNSLR_LoggerLevel        level,
    utf8str                  data,
    PNSLR_LogOptions         options,
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
    PNSLR_LogOptions      options;
} PNSLR_Logger;

// Default Logger Control ==========================================================

/**
 * Sets the default logger FOR THE CURRENT THREAD.
 * WILL NOT BE CARRIED OVER TO OTHER THREADS.
 */
void PNSLR_SetDefaultLogger(PNSLR_Logger logger);

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

EXTERN_C_END
#endif // PNSLR_LOGGER_H ===========================================================
