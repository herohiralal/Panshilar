#ifndef PNSLR_IO_H // ==============================================================
#define PNSLR_IO_H

#include "__Prelude.h"

/**
 * Represents a normalised path.
 * This is used to ensure that paths are in a consistent format across different platforms.
 * It is a simple wrapper around a UTF-8 string.
 * It's always an absolute path, and the path separators are always forward slashes ('/').
 * For directories, it always ends with a trailing slash.
 */
typedef struct PNSLR_Path { utf8str path; } PNSLR_Path;

/**
 * Represents the type of path normalisation to perform.
 */
ENUM_START(PNSLR_PathNormalisationType, u8)
    #define PNSLR_PathNormalisationType_File      ((PNSLR_PathNormalisationType) 0)
    #define PNSLR_PathNormalisationType_Directory ((PNSLR_PathNormalisationType) 1)
ENUM_END

/**
 * Normalises a path to a consistent format, as specified in `PNSLR_PathNormalisationType`.
 */
PNSLR_Path PNSLR_NormalisePath(utf8str path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator);

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
typedef b8 (*PNSLR_DirectoryIterationVisitorDelegate)(void* payload, PNSLR_Path path, b8 isDirectory, b8* exploreCurrentDirectory);

/**
 * Iterates over a directory and calls the visitor function for each file/directory found.
 * If `recursive` is true, it will also iterate over subdirectories.
 */
void PNSLR_IterateDirectory(PNSLR_Path path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc);

/**
 * Represents the type of path check to perform when checking if a path exists.
 */
ENUM_START(PNSLR_PathExistsCheckType, u8)
    #define PNSLR_PathExistsCheckType_Either    ((PNSLR_PathExistsCheckType) 0)
    #define PNSLR_PathExistsCheckType_File      ((PNSLR_PathExistsCheckType) 1)
    #define PNSLR_PathExistsCheckType_Directory ((PNSLR_PathExistsCheckType) 2)
ENUM_END

/**
 * Checks if a file/directory exists at the specified path.
 */
b8 PNSLR_PathExists(PNSLR_Path path, PNSLR_PathExistsCheckType type);

/**
 * Deletes a file/directory at a path, if it exists.
 */
b8 PNSLR_DeletePath(PNSLR_Path path);

/**
 * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
 */
i64 PNSLR_GetFileTimestamp(PNSLR_Path path);

/**
 * Gets the size of a file at the specified path in bytes.
 */
i64 PNSLR_GetFileSize(PNSLR_Path path);

/**
 * Creates a directory tree, if it doesn't exist.
 * Note that if the path doesn't have a trailing slash, it'll assume it's a file.
 * So, the last component of the path (if is a directory) will not be created.
 */
b8 PNSLR_CreateDirectoryTree(PNSLR_Path path);

/**
 * Represents an opened file.
 */
typedef struct PNSLR_File { rawptr handle; } PNSLR_File;

/**
 * Opens a file for reading, optionally allow writing to the stream.
 * If the file does not exist, this function will fail.
 */
PNSLR_File PNSLR_OpenFileToRead(PNSLR_Path path, b8 allowWrite);

/**
 * Opens a file for writing (or appending), optionally allow reading from the stream.
 * If the file does not exist, it will be created.
 */
PNSLR_File PNSLR_OpenFileToWrite(PNSLR_Path path, b8 append, b8 allowRead);

/**
 * Gets the size of an opened file.
 */
i64 PNSLR_GetSizeOfFile(PNSLR_File handle);

/**
 * Seeks to a specific position in an opened file.
 * If not relative, it's absolute from the start.
 */
b8 PNSLR_SeekPositionInFile(PNSLR_File handle, i64 newPos, b8 relative);

/**
 * Reads data from an opened file at the current position.
 */
b8 PNSLR_ReadFromFile(PNSLR_File handle, ArraySlice(u8) dst);

/**
 * Writes data to an opened file at the current position.
 */
b8 PNSLR_WriteToFile(PNSLR_File handle, ArraySlice(u8) src);

/**
 * Truncates an opened file to a specific size.
 */
b8 PNSLR_TruncateFile(PNSLR_File handle, i64 newSize);

/**
 * Flushes any buffered data to the file.
 */
b8 PNSLR_FlushFile(PNSLR_File handle);

/**
 * Closes an opened file.
 */
void PNSLR_CloseFileHandle(PNSLR_File handle);

/**
 * Reads a file fully end-to-end and stores in a buffer. Won't work if dst is nil.
 * Provided allocator is used for creating the buffer.
 */
b8 PNSLR_ReadFromFile(PNSLR_Path path, ArraySlice(u8)* dst, PNSLR_Allocator allocator);

/**
 * Dump a bunch of data into a file. Optionally append it instead of overwriting.
 */
b8 PNSLR_WriteToFile(PNSLR_Path path, ArraySlice(u8) src, b8 append);

#endif // PNSLR_IO_H ===============================================================
