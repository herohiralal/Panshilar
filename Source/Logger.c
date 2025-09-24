#define PNSLR_IMPLEMENTATION
#include "Logger.h"

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(Logger, 16);
static thread_local PNSLR_Logger G_PNSLR_Internal_DefaultLogger = {0};

void PNSLR_SetDefaultLogger(PNSLR_Logger logger) { G_PNSLR_Internal_DefaultLogger = logger; }

void PNSLR_LogD( utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogLD(G_PNSLR_Internal_DefaultLogger,     msg,       loc); }
void PNSLR_LogI( utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogLI(G_PNSLR_Internal_DefaultLogger,     msg,       loc); }
void PNSLR_LogW( utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogLW(G_PNSLR_Internal_DefaultLogger,     msg,       loc); }
void PNSLR_LogE( utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogLE(G_PNSLR_Internal_DefaultLogger,     msg,       loc); }
void PNSLR_LogC( utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogLC(G_PNSLR_Internal_DefaultLogger,     msg,       loc); }

void PNSLR_LogDf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLDf(G_PNSLR_Internal_DefaultLogger, fmtMsg, args, loc); }
void PNSLR_LogIf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLIf(G_PNSLR_Internal_DefaultLogger, fmtMsg, args, loc); }
void PNSLR_LogWf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLWf(G_PNSLR_Internal_DefaultLogger, fmtMsg, args, loc); }
void PNSLR_LogEf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLEf(G_PNSLR_Internal_DefaultLogger, fmtMsg, args, loc); }
void PNSLR_LogCf(utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLCf(G_PNSLR_Internal_DefaultLogger, fmtMsg, args, loc); }

void PNSLR_LogLDf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(logger, PNSLR_LoggerLevel_Debug,    fmtMsg, args, loc); }
void PNSLR_LogLIf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(logger, PNSLR_LoggerLevel_Info,     fmtMsg, args, loc); }
void PNSLR_LogLWf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(logger, PNSLR_LoggerLevel_Warn,     fmtMsg, args, loc); }
void PNSLR_LogLEf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(logger, PNSLR_LoggerLevel_Error,    fmtMsg, args, loc); }
void PNSLR_LogLCf(PNSLR_Logger logger, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(logger, PNSLR_LoggerLevel_Critical, fmtMsg, args, loc); }

void PNSLR_LogLD( PNSLR_Logger logger, utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogL(logger,  PNSLR_LoggerLevel_Debug,       msg,       loc); }
void PNSLR_LogLI( PNSLR_Logger logger, utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogL(logger,  PNSLR_LoggerLevel_Info,        msg,       loc); }
void PNSLR_LogLW( PNSLR_Logger logger, utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogL(logger,  PNSLR_LoggerLevel_Warn,        msg,       loc); }
void PNSLR_LogLE( PNSLR_Logger logger, utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogL(logger,  PNSLR_LoggerLevel_Error,       msg,       loc); }
void PNSLR_LogLC( PNSLR_Logger logger, utf8str msg,                                                      PNSLR_SourceCodeLocation loc) { PNSLR_LogL(logger,  PNSLR_LoggerLevel_Critical,    msg,       loc); }

void PNSLR_Log(PNSLR_LoggerLevel level, utf8str msg, PNSLR_SourceCodeLocation loc) { PNSLR_LogL(G_PNSLR_Internal_DefaultLogger, level, msg, loc); }

void PNSLR_Logf(PNSLR_LoggerLevel level, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc) { PNSLR_LogLf(G_PNSLR_Internal_DefaultLogger, level, fmtMsg, args, loc); }

void PNSLR_LogL(PNSLR_Logger logger, PNSLR_LoggerLevel level, utf8str msg, PNSLR_SourceCodeLocation loc)
{
    // all overloads converge here

    if (!logger.procedure)
        return;

    if (level < logger.minAllowedLvl)
        return;

    logger.procedure(logger.data, level, msg, logger.options, loc);
}

void PNSLR_LogLf(PNSLR_Logger logger, PNSLR_LoggerLevel level, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Logger, internalAllocator);
    utf8str msg = PNSLR_FormatString(fmtMsg, args, internalAllocator);
    PNSLR_LogL(logger, level, msg, loc);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Logger, internalAllocator);
}

