#ifndef PNSLR_ENVIRONMENT // =======================================================
#define PNSLR_ENVIRONMENT

#include "__Prelude.h"

/**
 * Defines the platforms supported by the library.
 */
typedef enum
{
    PNSLR_Platform_Unknown = 0,
    PNSLR_Platform_Windows,
    PNSLR_Platform_Linux,
    PNSLR_Platform_OSX,
    PNSLR_Platform_Android,
    PNSLR_Platform_iOS,
    PNSLR_Platform_PS5,
    PNSLR_Platform_XBoxSeries,
    PNSLR_Platform_Switch,
} PNSLR_Platform;

/**
 * Defines the architectures supported by the library.
 */
typedef enum
{
    PNSLR_Architecture_Unknown = 0,
    PNSLR_Architecture_X64,
    PNSLR_Architecture_ARM64,
} PNSLR_Architecture;

/**
 * Get the current platform.
 */
PNSLR_Platform PNSLR_GetPlatform(void);

/**
 * Get the current architecture.
 */
PNSLR_Architecture PNSLR_GetArchitecture(void);

#endif // PNSLR_ENVIRONMENT ========================================================
