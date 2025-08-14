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
typedef struct PNSLR_NormalisedPath
{
    utf8str path;
} PNSLR_NormalisedPath;

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
PNSLR_NormalisedPath PNSLR_NormalisePath(utf8str path, PNSLR_PathNormalisationType type, PNSLR_Allocator allocator);

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
typedef b8 (*PNSLR_DirectoryIterationVisitorDelegate)(void* payload, utf8str path, b8 isDirectory, b8* exploreCurrentDirectory);

/**
 * Iterates over a directory and calls the visitor function for each file/directory found.
 * If `recursive` is true, it will also iterate over subdirectories.
 */
void PNSLR_IterateDirectory(utf8str path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc);

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
b8 PNSLR_PathExists(utf8str path, PNSLR_PathExistsCheckType type);

/**
 * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
 */
i64 PNSLR_GetFileTimestamp(utf8str path);

/**
 * Represents a file handle. Used with a size buffer to ensure cross-platform compatibility.
 */
typedef struct alignas(8) PNSLR_FileHandle
{
    u8 buffer[PNSLR_PTR_SIZE];
} PNSLR_FileHandle;

#endif // PNSLR_IO_H ===============================================================
