#ifndef PNSLR_ENVIRONMENT // =======================================================
#define PNSLR_ENVIRONMENT

#include "__Prelude.h"

/**
 * Defines the platforms supported by the library.
 */
ENUM_START(PNSLR_Platform, u8)
    #define PNSLR_Platform_Unknown    ((PNSLR_Platform) 0)
    #define PNSLR_Platform_Windows    ((PNSLR_Platform) 1)
    #define PNSLR_Platform_Linux      ((PNSLR_Platform) 2)
    #define PNSLR_Platform_OSX        ((PNSLR_Platform) 3)
    #define PNSLR_Platform_Android    ((PNSLR_Platform) 4)
    #define PNSLR_Platform_iOS        ((PNSLR_Platform) 5)
    #define PNSLR_Platform_PS5        ((PNSLR_Platform) 6)
    #define PNSLR_Platform_XBoxSeries ((PNSLR_Platform) 7)
    #define PNSLR_Platform_Switch     ((PNSLR_Platform) 8)
ENUM_END

/**
 * Defines the architectures supported by the library.
 */
ENUM_START(PNSLR_Architecture, u8)
    #define PNSLR_Architecture_Unknown ((PNSLR_Architecture) 0)
    #define PNSLR_Architecture_X64     ((PNSLR_Architecture) 1)
    #define PNSLR_Architecture_ARM64   ((PNSLR_Architecture) 2)
ENUM_END

/**
 * Get the current platform.
 */
PNSLR_Platform PNSLR_GetPlatform(void);

/**
 * Get the current architecture.
 */
PNSLR_Architecture PNSLR_GetArchitecture(void);

#endif // PNSLR_ENVIRONMENT ========================================================
