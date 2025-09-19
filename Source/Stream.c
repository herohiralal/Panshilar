#define PNSLR_IMPLEMENTATION
#include "Stream.h"

static b8 PNSLR_Internal_FileStreamProcedure(rawptr streamData, PNSLR_StreamMode mode, PNSLR_ArraySlice(u8) data, i64 offset, i64* extraRet)
{
    if (!streamData) { return false; }

    PNSLR_File f = {.handle = streamData};

    b8 success = true;

    i64 retAlt = 0;
    if (!extraRet) { extraRet = &retAlt; }

    *extraRet = 0;

    switch (mode)
    {
        case PNSLR_StreamMode_GetSize:
            *extraRet = PNSLR_GetSizeOfFile(f);
            if (*extraRet < 0) { success = false; }
            break;

        case PNSLR_StreamMode_GetCurrentPos:
            *extraRet = PNSLR_GetCurrentPositionInFile(f);
            if (*extraRet < 0) { success = false; }
            break;

        case PNSLR_StreamMode_SeekAbsolute:
            success = PNSLR_SeekPositionInFile(f, offset, false);
            break;

        case PNSLR_StreamMode_SeekRelative:
            success = PNSLR_SeekPositionInFile(f, offset, true);
            break;

        case PNSLR_StreamMode_Read:
            success = PNSLR_ReadFromFile(f, data);
            if (success) { *extraRet = (i64) data.count; }
            break;

        case PNSLR_StreamMode_Write:
            success = PNSLR_WriteToFile(f, data);
            if (success) { *extraRet = (i64) data.count; }
            break;

        case PNSLR_StreamMode_Truncate:
            success = PNSLR_TruncateFile(f, offset);
            break;

        case PNSLR_StreamMode_Flush:
            success = PNSLR_FlushFile(f);
            break;

        case PNSLR_StreamMode_Close:
            success = true;
            PNSLR_CloseFileHandle(f);
            break;

        default:
            success = false; // unknown mode
            break;
    }

    return success;
}

static b8 PNSLR_Internal_StringBuilderStreamProcedure(rawptr streamData, PNSLR_StreamMode mode, PNSLR_ArraySlice(u8) data, i64 offset, i64* extraRet)
{
    if (!streamData) { return false; }

    PNSLR_StringBuilder* sb = (PNSLR_StringBuilder*) streamData;

    b8 success = true;

    i64 retAlt = 0;
    if (!extraRet) { extraRet = &retAlt; }

    *extraRet = 0;

    switch (mode)
    {
        case PNSLR_StreamMode_GetSize:
            *extraRet = sb->writtenSize;
            success = true;
            break;

        case PNSLR_StreamMode_GetCurrentPos:
            *extraRet = sb->cursorPos;
            success = true;
            break;

        case PNSLR_StreamMode_SeekAbsolute:
            if (offset < 0 || offset > sb->writtenSize) { success = false; }
            else { sb->cursorPos = offset; success = true; }
            break;

        case PNSLR_StreamMode_SeekRelative:
            if ((sb->cursorPos + offset) < 0 || (sb->cursorPos + offset) > sb->writtenSize) { success = false; }
            else { sb->cursorPos += offset; success = true; }
            break;

        case PNSLR_StreamMode_Read:
            if (!data.data || !data.count) { success = false; }

            // offset not relevant for this function
            // read from current position
            if (!sb->buffer.data || !sb->buffer.count) { success = false; } // no data
            else if (sb->cursorPos >= sb->writtenSize) { success = false; } // at or past the end
            else
            {
                i64 toRead = data.count;
                if (sb->cursorPos + toRead > sb->writtenSize)
                {
                    toRead = sb->writtenSize - sb->cursorPos; // clamp to available data
                }

                PNSLR_MemCopy(data.data, sb->buffer.data + sb->cursorPos, (i32) toRead);
                sb->cursorPos += toRead;
                success = true;
            }
            break;

        case PNSLR_StreamMode_Write:
            PNSLR_AppendStringToStringBuilder(sb, data);
            break;

        case PNSLR_StreamMode_Truncate:
            if (offset < 0 || offset > sb->writtenSize) { success = false; }
            else
            {
                sb->writtenSize = offset;
                if (sb->cursorPos > sb->writtenSize) { sb->cursorPos = sb->writtenSize; }
                success = true;
            }
            break;

        case PNSLR_StreamMode_Flush:
            success = true; // nothing to do
            break;

        case PNSLR_StreamMode_Close:
            success = true;
            PNSLR_FreeStringBuilder(sb);
            break;

        default:
            success = false; // unknown mode
            break;
    }

    return success;
}

i64 PNSLR_GetSizeOfStream(PNSLR_Stream stream)
{
    if (!stream.procedure) { return 0; }

    i64 size = 0;
    if (stream.procedure(stream.data, PNSLR_StreamMode_GetSize, (PNSLR_ArraySlice(u8)) {0}, 0, &size))
    {
        return size;
    }
    return 0;
}

i64 PNSLR_GetCurrentPositionInStream(PNSLR_Stream stream)
{
    if (!stream.procedure) { return 0; }

    i64 pos = -1;
    if (stream.procedure(stream.data, PNSLR_StreamMode_GetCurrentPos, (PNSLR_ArraySlice(u8)) {0}, 0, &pos))
    {
        return pos;
    }
    return -1;
}

b8 PNSLR_SeekPositionInStream(PNSLR_Stream stream, i64 newPos, b8 relative)
{
    if (!stream.procedure) { return false; }

    return stream.procedure(
        stream.data,
        relative ? PNSLR_StreamMode_SeekRelative : PNSLR_StreamMode_SeekAbsolute,
        (PNSLR_ArraySlice(u8)) {0},
        newPos,
        nil
    );
}

b8 PNSLR_ReadFromStream(PNSLR_Stream stream, PNSLR_ArraySlice(u8) dst)
{
    if (!stream.procedure) { return false; }

    return stream.procedure(
        stream.data,
        PNSLR_StreamMode_Read,
        dst,
        0,
        nil
    );
}

b8 PNSLR_WriteToStream(PNSLR_Stream stream, PNSLR_ArraySlice(u8) src)
{
    if (!stream.procedure) { return false; }

    return stream.procedure(
        stream.data,
        PNSLR_StreamMode_Write,
        src,
        0,
        nil
    );
}

static b8 PNSLR_Internal_WriteFmtOptionsToStream(PNSLR_Stream stream, PNSLR_PrimitiveFmtOptions fmtOpt)
{
    switch (fmtOpt.type)
    {
        case PNSLR_PrimitiveFmtType_CString:
        {
            utf8str data = PNSLR_StringFromCString(*(cstring*) &fmtOpt.valueBufferA);
            return PNSLR_WriteToStream(stream, data);
        }
        case PNSLR_PrimitiveFmtType_String:
        {
            utf8str data = {.data = *(u8**) &fmtOpt.valueBufferA, .count = *(i64*) &fmtOpt.valueBufferB};
            return PNSLR_WriteToStream(stream, data);
        }
        case PNSLR_PrimitiveFmtType_Rune:
        {
            u32 rune = (u32) fmtOpt.valueBufferA;
            PNSLR_EncodedRune encoded = PNSLR_EncodeRune(rune);
            utf8str rStr = {.count = (i64) encoded.length, .data = &(encoded.data[0])};
            return PNSLR_WriteToStream(stream, rStr);
        }
        case PNSLR_PrimitiveFmtType_B8:
        {
            b8 value = (b8) (!!fmtOpt.valueBufferA);
            if (value) return PNSLR_WriteToStream(stream, PNSLR_StringLiteral("true" ));
            else       return PNSLR_WriteToStream(stream, PNSLR_StringLiteral("false"));
        }
        case PNSLR_PrimitiveFmtType_F32:
        case PNSLR_PrimitiveFmtType_F64:
        case PNSLR_PrimitiveFmtType_U8:
        case PNSLR_PrimitiveFmtType_U16:
        case PNSLR_PrimitiveFmtType_U32:
        case PNSLR_PrimitiveFmtType_U64:
        case PNSLR_PrimitiveFmtType_I8:
        case PNSLR_PrimitiveFmtType_I16:
        case PNSLR_PrimitiveFmtType_I32:
        case PNSLR_PrimitiveFmtType_I64:
        {
            // all of these require formatting to a temporary buffer
            break;
        }
        default:
        {
            FORCE_DBG_TRAP;
            return false;
        }
    }

    u8 tempBuffer[128] = {0};
    PNSLR_StringBuilder tempBuilder = {
        .allocator   = {0},
        .buffer      = {.data = &(tempBuffer[0]), .count = sizeof(tempBuffer)},
    };

    switch (fmtOpt.type)
    {
        case PNSLR_PrimitiveFmtType_F32:
        {
            u32 tmpVal = (u32) fmtOpt.valueBufferA;
            f32 value = *(f32*) &tmpVal;
            i32 decimalPlaces = (i32) fmtOpt.valueBufferB;
            if (decimalPlaces <  0) decimalPlaces =  0;
            if (decimalPlaces > 30) decimalPlaces = 30; // clamp
            PNSLR_AppendF32ToStringBuilder(&tempBuilder, value, decimalPlaces);
            break;
        }
        case PNSLR_PrimitiveFmtType_F64:
        {
            f64 value = *(f64*) &fmtOpt.valueBufferA;
            i32 decimalPlaces = (i32) fmtOpt.valueBufferB;
            if (decimalPlaces <  0) decimalPlaces =  0;
            if (decimalPlaces > 30) decimalPlaces = 30; // clamp
            PNSLR_AppendF64ToStringBuilder(&tempBuilder, value, decimalPlaces);
            break;
        }
        case PNSLR_PrimitiveFmtType_U8:
        case PNSLR_PrimitiveFmtType_U16:
        case PNSLR_PrimitiveFmtType_U32:
        case PNSLR_PrimitiveFmtType_U64:
        {
            PNSLR_IntegerBase ib = (PNSLR_IntegerBase) fmtOpt.valueBufferB;
            if (ib != PNSLR_IntegerBase_Binary &&
                ib != PNSLR_IntegerBase_Octal  &&
                ib != PNSLR_IntegerBase_Decimal &&
                ib != PNSLR_IntegerBase_HexaDecimal)
            {
                ib = PNSLR_IntegerBase_Decimal; // default
            }

            PNSLR_AppendU64ToStringBuilder(
                &tempBuilder,
                fmtOpt.valueBufferA,
                ib
            );
        }
        case PNSLR_PrimitiveFmtType_I8:
        case PNSLR_PrimitiveFmtType_I16:
        case PNSLR_PrimitiveFmtType_I32:
        case PNSLR_PrimitiveFmtType_I64:
        {
            PNSLR_IntegerBase ib = (PNSLR_IntegerBase) fmtOpt.valueBufferB;
            if (ib != PNSLR_IntegerBase_Binary &&
                ib != PNSLR_IntegerBase_Octal  &&
                ib != PNSLR_IntegerBase_Decimal &&
                ib != PNSLR_IntegerBase_HexaDecimal)
            {
                ib = PNSLR_IntegerBase_Decimal; // default
            }

            PNSLR_AppendI64ToStringBuilder(
                &tempBuilder,
                *(i64*) &fmtOpt.valueBufferA,
                ib
            );
        }
        default:
        {
            FORCE_DBG_TRAP;
            return false;
        }
    }

    utf8str result = PNSLR_StringFromStringBuilder(&tempBuilder);
    return PNSLR_WriteToStream(stream, result);
}

b8 PNSLR_FormatAndWriteToStream(PNSLR_Stream stream, utf8str fmtStr, PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions) args)
{
    if (!stream.procedure || !fmtStr.data || fmtStr.count <= 0) { return false; }

    // formatting rules:
    // - % is the placeholder for an argument
    // - %% is a literal %
    // - anything else is written to the stream as-is
    // - if there are more placeholders than args, add `{MISSING_ARG}`
    // - if there are more args than placeholders, print the extra args as {UNUSED_ARGS: a, b, c}

    i64 argIndex = 0;
    i64 i        = 0;
    i64 literalStart = 0; // start of pending literal run

    #define PNSLR_STRFMT_FLUSH_BUFFER() \
        do { \
            if (i > literalStart) { \
                if (!PNSLR_WriteToStream(stream, (utf8str){.data = fmtStr.data + literalStart, .count = i - literalStart})) { return false; } \
            } \
        } while (0)

    while (i < fmtStr.count)
    {
        if (fmtStr.data[i] == '%')
        {
            PNSLR_STRFMT_FLUSH_BUFFER();

            if (i + 1 < fmtStr.count && fmtStr.data[i + 1] == '%')
            {
                if (!PNSLR_WriteToStream(stream, PNSLR_StringLiteral("%%"))) { return false; }
                i += 2;
            }
            else
            {
                if (argIndex < args.count)
                {
                    if (!PNSLR_Internal_WriteFmtOptionsToStream(stream, args.data[argIndex++])) { return false; }
                }
                else
                {
                    if (!PNSLR_WriteToStream(stream, PNSLR_StringLiteral("{MISSING_ARG}"))) { return false; }
                }
                i += 1;
            }

            // reset literal run after special handling
            literalStart = i;
        }
        else i += 1;
    }

    PNSLR_STRFMT_FLUSH_BUFFER();

    #undef PNSLR_STRFMT_FLUSH_BUFFER

    // handle unused args
    if (argIndex < args.count)
    {
        if (!PNSLR_WriteToStream(stream, PNSLR_StringLiteral("{UNUSED_ARGS: "))) { return false; }

        for (i64 j = argIndex; j < args.count; ++j)
        {
            if (!PNSLR_Internal_WriteFmtOptionsToStream(stream, args.data[j])) { return false; }

            if (j + 1 < args.count)
            {
                if (!PNSLR_WriteToStream(stream, PNSLR_StringLiteral(", "))) { return false; }
            }
        }

        if (!PNSLR_WriteToStream(stream, PNSLR_StringLiteral("}"))) { return false; }
    }

    return true;
}

b8 PNSLR_TruncateStream(PNSLR_Stream stream, i64 newSize)
{
    if (!stream.procedure) { return false; }

    return stream.procedure(
        stream.data,
        PNSLR_StreamMode_Truncate,
        (PNSLR_ArraySlice(u8)) {0},
        newSize,
        nil
    );
}

b8 PNSLR_FlushStream(PNSLR_Stream stream)
{
    if (!stream.procedure) { return false; }

    return stream.procedure(
        stream.data,
        PNSLR_StreamMode_Flush,
        (PNSLR_ArraySlice(u8)) {0},
        0,
        nil
    );
}

void PNSLR_CloseStream(PNSLR_Stream stream)
{
    if (!stream.procedure) { return; }

    stream.procedure(
        stream.data,
        PNSLR_StreamMode_Close,
        (PNSLR_ArraySlice(u8)) {0},
        0,
        nil
    );
}

PNSLR_Stream PNSLR_StreamFromFile(PNSLR_File file)
{
    static_assert(sizeof(file) == sizeof(rawptr), "PNSLR_File must be the same size as rawptr");

    return (PNSLR_Stream) {
        .procedure = PNSLR_Internal_FileStreamProcedure,
        .data      = file.handle,
    };
}

PNSLR_Stream PNSLR_StreamFromStringBuilder(PNSLR_StringBuilder* builder)
{
    static_assert(sizeof(builder) == sizeof(rawptr), "PNSLR_File must be the same size as rawptr");

    return (PNSLR_Stream) {
        .procedure = PNSLR_Internal_StringBuilderStreamProcedure,
        .data      = (rawptr) builder
    };
}
