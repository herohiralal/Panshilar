#ifndef PNSLR_STREAM_H // ==========================================================
#define PNSLR_STREAM_H
#include "__Prelude.h"
#include "Strings.h"
#include "IO.h"
EXTERN_C_BEGIN

// Stream Declaration ==============================================================

/**
 * Defines the mode to be used when calling the stream function.
 */
ENUM_START(PNSLR_StreamMode, u8)
    #define PNSLR_StreamMode_GetSize           ((PNSLR_StreamMode) 0)
    #define PNSLR_StreamMode_GetCurrentPos     ((PNSLR_StreamMode) 1)
    #define PNSLR_StreamMode_SeekAbsolute      ((PNSLR_StreamMode) 2)
    #define PNSLR_StreamMode_SeekRelative      ((PNSLR_StreamMode) 3)
    #define PNSLR_StreamMode_Read              ((PNSLR_StreamMode) 4)
    #define PNSLR_StreamMode_Write             ((PNSLR_StreamMode) 5)
    #define PNSLR_StreamMode_Truncate          ((PNSLR_StreamMode) 6)
    #define PNSLR_StreamMode_Flush             ((PNSLR_StreamMode) 7)
    #define PNSLR_StreamMode_Close             ((PNSLR_StreamMode) 8)
ENUM_END

/**
 * Defines the delegate type for the stream function
 */
typedef b8 (*PNSLR_StreamProcedure)(
    rawptr                streamData,
    PNSLR_StreamMode      mode,
    PNSLR_ArraySlice(u8)  data,
    i64                   offset,
    i64*                  extraRet
);

/**
 * Defines a generic stream, that can be used for reading/writing data.
 */
typedef struct PNSLR_Stream
{
    PNSLR_StreamProcedure procedure;
    rawptr                data;
} PNSLR_Stream;

PNSLR_DECLARE_ARRAY_SLICE(PNSLR_Stream);

// Stream ease-of-use functions ====================================================

/**
 * Gets the size of the stream.
 * Returns 0 on error.
 */
i64 PNSLR_GetSizeOfStream(PNSLR_Stream stream);

/**
 * Gets the current position in the stream.
 * Returns -1 on error.
 */
i64 PNSLR_GetCurrentPositionInStream(PNSLR_Stream stream);

/**
 * Seeks to a new position in the stream.
 * If 'relative' is true, the new position is relative to the current position.
 * If 'relative' is false, the new position is absolute from the start.
 * Returns true on success, false on failure.
 */
b8 PNSLR_SeekPositionInStream(PNSLR_Stream stream, i64 newPos, b8 relative);

/**
 * Reads data from the stream into the provided buffer.
 * Returns true on success, false on failure.
 */
b8 PNSLR_ReadFromStream(PNSLR_Stream stream, PNSLR_ArraySlice(u8) dst);

/**
 * Writes data from the provided buffer into the stream.
 * Returns true on success, false on failure.
 */
b8 PNSLR_WriteToStream(PNSLR_Stream stream, PNSLR_ArraySlice(u8) src);

/**
 * Truncates the stream to the specified size.
 * Returns true on success, false on failure.
 */
b8 PNSLR_TruncateStream(PNSLR_Stream stream, i64 newSize);

/**
 * Flushes any buffered data to the stream.
 * Returns true on success, false on failure.
 */
b8 PNSLR_FlushStream(PNSLR_Stream stream);

/**
 * Closes the stream and frees any associated resources.
 */
void PNSLR_CloseStream(PNSLR_Stream stream);

// Stream casts ====================================================================

/**
 * Creates a stream from a file handle.
 */
PNSLR_Stream PNSLR_StreamFromFile(PNSLR_File* file);

/**
 * Creates a stream from a string builder.
 */
PNSLR_Stream PNSLR_StreamFromStringBuilder(PNSLR_StringBuilder* builder);

EXTERN_C_END
#endif // PNSLR_STREAM_H ===========================================================
