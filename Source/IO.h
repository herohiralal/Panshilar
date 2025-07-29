#ifndef PNSLR_IO_H // ==============================================================
#define PNSLR_IO_H

#include "__Prelude.h"

EXTERN_C_BEGIN

ENUM_START(PNSLR_PathCheckType, u8)
    #define PNSLR_PathCheckType_Either    ((PNSLR_PathCheckType) 0)
    #define PNSLR_PathCheckType_File      ((PNSLR_PathCheckType) 1)
    #define PNSLR_PathCheckType_Directory ((PNSLR_PathCheckType) 2)
ENUM_END

/**
 * The signature of the delegate that's supposed to be called for iterating over a directory.
 */
typedef b8 (*PNSLR_DirectoryIterationVisitorDelegate)(void* payload, utf8str path, b8 isDirectory);

/**
 * Iterates over a directory and calls the visitor function for each file/directory found.
 * If `recursive` is true, it will also iterate over subdirectories.
 */
void PNSLR_IterateDirectory(utf8str path, b8 recursive, rawptr visitorPayload, PNSLR_DirectoryIterationVisitorDelegate visitorFunc);

/**
 * Checks if a file/directory exists at the specified path.
 */
b8 PNSLR_PathExists(utf8str path, PNSLR_PathCheckType type);

/**
 * Get the timestamp of a file at the specified path as nanoseconds since unix epoch.
 */
i64 PNSLR_GetFileTimestamp(utf8str path);

/**
 * Represents a file handle. Used with a size buffer to ensure cross-platform compatibility.
 */
typedef struct alignas(8) PNSLR_FileHandle {
    u8 buffer[PNSLR_PTR_SIZE];
} PNSLR_FileHandle;

EXTERN_C_END

#endif // PNSLR_IO_H ===============================================================
