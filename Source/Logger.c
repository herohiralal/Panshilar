#define PNSLR_IMPLEMENTATION
#include "Logger.h"
#include "Sync.h"
#include "Chrono.h"

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(Logger, 16);
static thread_local PNSLR_Logger G_PNSLR_Internal_DefaultLogger = {0};

#if PNSLR_DESKTOP
    static PNSLR_Mutex* G_PNSLR_Internal_DefaultLoggerMutex = nil;
#endif

static PNSLR_DoOnce G_PNSLR_Internal_DefaultLoggerInit  = {0};

static void PNSLR_Internal_InitialiseLoggerStateIfRequired(void)
{
    #if PNSLR_DESKTOP
        G_PNSLR_Internal_DefaultLoggerMutex = PNSLR_New(PNSLR_Mutex, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        *G_PNSLR_Internal_DefaultLoggerMutex = PNSLR_CreateMutex();
        setvbuf(stdout, nil, _IONBF, 0); // Disable stdout buffering
    #endif
}

static void PNSLR_Internal_LoggerFn_Default(rawptr loggerData, PNSLR_LoggerLevel level, utf8str data, PNSLR_LogOption options, PNSLR_SourceCodeLocation location)
{
    PNSLR_ExecuteDoOnce(&G_PNSLR_Internal_DefaultLoggerInit, PNSLR_Internal_InitialiseLoggerStateIfRequired);

    #if PNSLR_ANDROID // logcat takes care of these
        options &= ~(PNSLR_LogOption_IncludeLevel|PNSLR_LogOption_IncludeDate|PNSLR_LogOption_IncludeTime|PNSLR_LogOption_IncludeColours);
    #endif

    #if PNSLR_DESKTOP
        utf8str levelStr = {0};
        if (options & PNSLR_LogOption_IncludeLevel)
        {
            switch (level)
            {
                case PNSLR_LoggerLevel_Debug:    levelStr = PNSLR_StringLiteral("[DBG] "); break;
                case PNSLR_LoggerLevel_Info:     levelStr = PNSLR_StringLiteral("[INF] "); break;
                case PNSLR_LoggerLevel_Warn:     levelStr = PNSLR_StringLiteral("[WRN] "); break;
                case PNSLR_LoggerLevel_Error:    levelStr = PNSLR_StringLiteral("[ERR] "); break;
                case PNSLR_LoggerLevel_Critical: levelStr = PNSLR_StringLiteral("[XXX] "); break;
                default:                         levelStr = PNSLR_StringLiteral("[---] "); break;
            }
        }

        i16 yr = 0; u8 mo = 0, da = 0, hh = 0, mm = 0, ss = 0;
        if ((options & PNSLR_LogOption_IncludeDate) || (options & PNSLR_LogOption_IncludeTime))
        {
            i64 ns = PNSLR_NanosecondsSinceUnixEpoch();
            PNSLR_ConvertNanosecondsSinceUnixEpochToDateTime(ns, &yr, &mo, &da, &hh, &mm, &ss);
        }

        i64 dateLen = 0;
        if (options & PNSLR_LogOption_IncludeDate)
        {
            dateLen = 1 + 4 + 1 + 2 + 1 + 2 + 1 + 1; // "[YYYY-MM-DD] "; don't forget space at the end
        }

        i64 timeLen = 0;
        if (options & PNSLR_LogOption_IncludeTime)
        {
            timeLen = 1 + 2 + 1 + 2 + 1 + 2 + 1 + 1; // "[HH:MM:SS] "; don't forget space at the end
        }
    #endif

    i64 fnLen = 0;
    if (options & PNSLR_LogOption_IncludeFn && location.function.data && location.function.count)
    {
        fnLen = 1 + location.function.count + 2 + 1 + 1; // "[function()] "; don't forget space at the end
    }

    i64 locLen = 0;
    if (options & PNSLR_LogOption_IncludeFile && location.file.data && location.file.count)
    {
        i64 lineDigitCount = 0;
        {
            i32 line = location.line;
            if (line <= 0) { lineDigitCount = 1; }
            else
            {
                while (line != 0) { line /= 10; lineDigitCount++; }
            }
        }

        i64 columnDigitCount = 0;
        {
            i32 column = location.column;
            if (column <= 0) { columnDigitCount = 1; }
            else
            {
                while (column != 0) { column /= 10; columnDigitCount++; }
            }
        }

        locLen = 1 + 1 + 4 + 1 + location.file.count + 1 + lineDigitCount + 1 + columnDigitCount + 1; // " (from file:line:column)"; don't forget space at the start
    }

    #if PNSLR_DESKTOP
    {
        PNSLR_LockMutex(G_PNSLR_Internal_DefaultLoggerMutex);

        if (options & PNSLR_LogOption_IncludeColours)
        {
            switch (level)
            {
                case PNSLR_LoggerLevel_Debug:    fputs("\033[0m\033[1m",    stdout); break; // Reset, Bold
                case PNSLR_LoggerLevel_Info:     fputs("\033[0m\033[1;36m", stdout); break; // Reset, Bold Cyan
                case PNSLR_LoggerLevel_Warn:     fputs("\033[0m\033[1;33m", stdout); break; // Reset, Bold Yellow
                case PNSLR_LoggerLevel_Error:    fputs("\033[0m\033[1;31m", stdout); break; // Reset, Bold Red
                case PNSLR_LoggerLevel_Critical: fputs("\033[0m\033[1;41m", stdout); break; // Reset, Bold Red background
                default:                                                             break;
            }
        }

        if (levelStr.count)
            fwrite(levelStr.data, sizeof(u8), (size_t) levelStr.count, stdout);

        if (options & PNSLR_LogOption_IncludeColours)
        {
            switch (level)
            {
                case PNSLR_LoggerLevel_Debug:    fputs("\033[0m\033[90m",   stdout); break; // Reset, Dark Grey
                case PNSLR_LoggerLevel_Critical: fputs("\033[0m\033[1;31m", stdout); break; // Reset, Red
                default:                         fputs("\033[0m",           stdout); break; // Reset
            }
        }

        if (dateLen && yr >= 0) // i don't know if anybody be logging before 0 AD
            fprintf(stdout, "[%04d-%02d-%02d] ", (i32) yr, (i32) mo, (i32) da);

        if (timeLen)
            fprintf(stdout, "[%02d:%02d:%02d] ", (i32) hh, (i32) mm, (i32) ss);

        if (fnLen)
            fprintf(stdout, "[%.*s()] ", (i32) location.function.count, location.function.data);

        fwrite(data.data, sizeof(u8), (size_t) data.count, stdout);

        if (locLen)
        {
            if (options & PNSLR_LogOption_IncludeColours)
                fputs("\033[0m\033[90m", stdout); // Reset, Dark Grey

            fprintf(stdout, " (from %.*s:%d:%d)", (i32) location.file.count, location.file.data, (i32) location.line, (i32) location.column);
        }

        if (options & PNSLR_LogOption_IncludeColours)
            fputs("\033[0m", stdout);

        fputc('\n', stdout);

        PNSLR_UnlockMutex(G_PNSLR_Internal_DefaultLoggerMutex);
    }
    #elif PNSLR_ANDROID
    {
        PNSLR_INTERNAL_ALLOCATOR_INIT(Logger, internalAllocator);
        i64 totalLen = fnLen + data.count + locLen;
        PNSLR_StringBuilder sb = {.allocator = internalAllocator};
        PNSLR_ReserveSpaceInStringBuilder(&sb, totalLen + 1); // +1 for null terminator
        if (fnLen)
            PNSLR_FormatAndAppendToStringBuilder(&sb, PNSLR_StringLiteral("[$()] "),
                PNSLR_FmtArgs(
                    PNSLR_FmtString(location.function)
                )
            );

        PNSLR_AppendStringToStringBuilder(&sb, data);

        if (locLen)
            PNSLR_FormatAndAppendToStringBuilder(&sb, PNSLR_StringLiteral(" (from $:$:$)"),
                PNSLR_FmtArgs(
                    PNSLR_FmtString(location.file),
                    PNSLR_FmtI32(location.line, PNSLR_IntegerBase_Decimal),
                    PNSLR_FmtI32(location.column, PNSLR_IntegerBase_Decimal)
                )
            );

        PNSLR_AppendRuneToStringBuilder(&sb, '\0');
        utf8str outData = PNSLR_StringFromStringBuilder(&sb);

        int androidPrio = ANDROID_LOG_DEFAULT;
        switch (level)
        {
            case PNSLR_LoggerLevel_Debug:    androidPrio = ANDROID_LOG_DEBUG;    break;
            case PNSLR_LoggerLevel_Info:     androidPrio = ANDROID_LOG_INFO;     break;
            case PNSLR_LoggerLevel_Warn:     androidPrio = ANDROID_LOG_WARN;     break;
            case PNSLR_LoggerLevel_Error:    androidPrio = ANDROID_LOG_ERROR;    break;
            case PNSLR_LoggerLevel_Critical: androidPrio = ANDROID_LOG_FATAL;    break;
            default:                         androidPrio = ANDROID_LOG_DEFAULT;  break;
        }

        __android_log_write(androidPrio, "Panshilar-DefaultLogger", (cstring) outData.data);

        PNSLR_INTERNAL_ALLOCATOR_RESET(Logger, internalAllocator);
    }
    #endif
}


static void PNSLR_Internal_LoggerFn_File(rawptr loggerData, PNSLR_LoggerLevel level, utf8str data, PNSLR_LogOption options, PNSLR_SourceCodeLocation location)
{
    if (!loggerData) { return; }

    PNSLR_File f = {.handle = loggerData};

    if (options & PNSLR_LogOption_IncludeLevel)
    {
        utf8str levelStr = {0};
        switch (level)
        {
            case PNSLR_LoggerLevel_Debug:    levelStr = PNSLR_StringLiteral("[DBG] "); break;
            case PNSLR_LoggerLevel_Info:     levelStr = PNSLR_StringLiteral("[INF] "); break;
            case PNSLR_LoggerLevel_Warn:     levelStr = PNSLR_StringLiteral("[WRN] "); break;
            case PNSLR_LoggerLevel_Error:    levelStr = PNSLR_StringLiteral("[ERR] "); break;
            case PNSLR_LoggerLevel_Critical: levelStr = PNSLR_StringLiteral("[XXX] "); break;
            default:                         levelStr = PNSLR_StringLiteral("[---] "); break;
        }

        PNSLR_WriteToFile(f, levelStr);
    }

    i16 yr = 0; u8 mo = 0, da = 0, hh = 0, mm = 0, ss = 0;
    if ((options & PNSLR_LogOption_IncludeDate) || (options & PNSLR_LogOption_IncludeTime))
    {
        i64 ns = PNSLR_NanosecondsSinceUnixEpoch();
        PNSLR_ConvertNanosecondsSinceUnixEpochToDateTime(ns, &yr, &mo, &da, &hh, &mm, &ss);
    }

    if (options & PNSLR_LogOption_IncludeDate)
    {
        PNSLR_FormatAndWriteToFile(f, PNSLR_StringLiteral("[$-$-$] "),
            PNSLR_FmtArgs(
                PNSLR_FmtI16(yr, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtU8(mo, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtU8(da, PNSLR_IntegerBase_Decimal)
            )
        );
    }

    if (options & PNSLR_LogOption_IncludeTime)
    {
        PNSLR_FormatAndWriteToFile(f, PNSLR_StringLiteral("[$:$:$] "),
            PNSLR_FmtArgs(
                PNSLR_FmtU8(hh, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtU8(mm, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtU8(ss, PNSLR_IntegerBase_Decimal)
            )
        );
    }

    PNSLR_WriteToFile(f, data);

    if (options & PNSLR_LogOption_IncludeFn && location.function.data && location.function.count)
    {
        PNSLR_FormatAndWriteToFile(f, PNSLR_StringLiteral("\n\t\t\t\t[$()]"),
            PNSLR_FmtArgs(
                PNSLR_FmtString(location.function)
            )
        );
    }

    if (options & PNSLR_LogOption_IncludeFile && location.file.data && location.file.count)
    {
        PNSLR_FormatAndWriteToFile(f, PNSLR_StringLiteral("\n\t\t\t\t(from $:$:$)"),
            PNSLR_FmtArgs(
                PNSLR_FmtString(location.file),
                PNSLR_FmtI32(location.line,   PNSLR_IntegerBase_Decimal),
                PNSLR_FmtI32(location.column, PNSLR_IntegerBase_Decimal)
            )
        );
    }

    PNSLR_WriteToFile(f, PNSLR_StringLiteral("\n"));
}

static void PNSLR_Internal_LoggerFn_NoOp(rawptr loggerData, PNSLR_LoggerLevel level, utf8str data, PNSLR_LogOption options, PNSLR_SourceCodeLocation location)
{
    // do nothing
}

void PNSLR_SetDefaultLogger(PNSLR_Logger logger) { G_PNSLR_Internal_DefaultLogger = logger; }

void PNSLR_DisableDefaultLogger(void) { PNSLR_SetDefaultLogger(PNSLR_GetNilLogger()); }

void PNSLR_LogD(utf8str msg, PNSLR_SourceCodeLocation loc) { PNSLR_LogLD(G_PNSLR_Internal_DefaultLogger, msg, loc); }
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

    if (level < logger.minAllowedLvl)
        return;

    if (logger.procedure)
        logger.procedure(logger.data, level, msg, logger.options, loc);
    else if (!logger.data && logger.minAllowedLvl == PNSLR_LoggerLevel_Debug && logger.options == PNSLR_LogOption_None && level >= PNSLR_LoggerLevel_Info)
    {
        // basically nothing has been set
        PNSLR_Internal_LoggerFn_Default(
            nil,
            level,
            msg,
            PNSLR_LogOption_None |
                PNSLR_LogOption_IncludeLevel |
                PNSLR_LogOption_IncludeColours,
            loc
        );
    }
}

void PNSLR_LogLf(PNSLR_Logger logger, PNSLR_LoggerLevel level, utf8str fmtMsg, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args, PNSLR_SourceCodeLocation loc)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Logger, internalAllocator);
    utf8str msg = PNSLR_FormatString(fmtMsg, args, internalAllocator);
    PNSLR_LogL(logger, level, msg, loc);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Logger, internalAllocator);
}

PNSLR_Logger PNSLR_LoggerFromFile(PNSLR_File f, PNSLR_LoggerLevel minAllowedLevel, PNSLR_LogOption options)
{
    static_assert(sizeof(f) == sizeof(rawptr), "PNSLR_File must be the same size as rawptr");

    return (PNSLR_Logger)
    {
        .procedure     = PNSLR_Internal_LoggerFn_File,
        .data          = f.handle,
        .minAllowedLvl = minAllowedLevel,
        .options       = options & ~(PNSLR_LogOption_IncludeColours) // no colours in files
    };
}

PNSLR_Logger PNSLR_GetDefaultLoggerWithOptions(PNSLR_LoggerLevel minAllowedLevel, PNSLR_LogOption options)
{
    return (PNSLR_Logger)
    {
        .procedure     = PNSLR_Internal_LoggerFn_Default,
        .data          = nil,
        .minAllowedLvl = minAllowedLevel,
        .options       = options
    };
}

PNSLR_Logger PNSLR_GetNilLogger(void)
{
    return (PNSLR_Logger)
    {
        .procedure     = PNSLR_Internal_LoggerFn_NoOp,
        .data          = nil,
        .minAllowedLvl = PNSLR_LoggerLevel_Critical + 1, // effectively disables all logging
        .options       = PNSLR_LogOption_None
    };
}
