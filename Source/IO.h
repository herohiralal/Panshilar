#ifndef PNSLR_IO // ================================================================
#define PNSLR_IO

#include "__Prelude.h"

ENUM_START(PNSLR_PathCheckType, u8)
#define PNSLR_PathCheckType_Either    ((PNSLR_PathCheckType) 0)
#define PNSLR_PathCheckType_File      ((PNSLR_PathCheckType) 1)
#define PNSLR_PathCheckType_Directory ((PNSLR_PathCheckType) 2)
ENUM_END

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

#endif // PNSLR_IO =================================================================
