#define PNSLR_IMPLEMENTATION
#include "Stream.h"

static b8 PNSLR_Internal_FileStreamProcedure(rawptr streamData, PNSLR_StreamMode mode, PNSLR_ArraySlice(u8) data, i64 offset, i64* extraRet)
{
    if (!streamData) { return false; }

    PNSLR_File* f = (PNSLR_File*) streamData;

    b8 success = true;

    i64 retAlt = 0;
    if (!extraRet) { extraRet = &retAlt; }

    *extraRet = 0;

    switch (mode)
    {
        case PNSLR_StreamMode_GetSize:
            *extraRet = PNSLR_GetSizeOfFile(*f);
            if (*extraRet < 0) { success = false; }
            break;

        case PNSLR_StreamMode_GetCurrentPos:
            *extraRet = PNSLR_GetCurrentPositionInFile(*f);
            if (*extraRet < 0) { success = false; }
            break;

        case PNSLR_StreamMode_SeekAbsolute:
            success = PNSLR_SeekPositionInFile(*f, offset, false);
            break;

        case PNSLR_StreamMode_SeekRelative:
            success = PNSLR_SeekPositionInFile(*f, offset, true);
            break;

        case PNSLR_StreamMode_Read:
            success = PNSLR_ReadFromFile(*f, data);
            if (success) { *extraRet = (i64) data.count; }
            break;

        case PNSLR_StreamMode_Write:
            success = PNSLR_WriteToFile(*f, data);
            if (success) { *extraRet = (i64) data.count; }
            break;

        case PNSLR_StreamMode_Truncate:
            success = PNSLR_TruncateFile(*f, offset);
            break;

        case PNSLR_StreamMode_Flush:
            success = PNSLR_FlushFile(*f);
            break;

        case PNSLR_StreamMode_Close:
            success = true;
            PNSLR_CloseFileHandle(*f);
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

PNSLR_Stream PNSLR_StreamFromFile(PNSLR_File* file)
{
    return (PNSLR_Stream) {
        .procedure = PNSLR_Internal_FileStreamProcedure,
        .data      = (rawptr) file
    };
}

PNSLR_Stream PNSLR_StreamFromStringBuilder(PNSLR_StringBuilder* builder)
{
    return (PNSLR_Stream) {
        .procedure = PNSLR_Internal_StringBuilderStreamProcedure,
        .data      = (rawptr) builder
    };
}
