#include "Environment.h"

PNSLR_Platform PNSLR_GetPlatform(void)
{
#if PNSLR_WINDOWS
    return PNSLR_Platform_Windows;
#elif PNSLR_LINUX
    return PNSLR_Platform_Linux;
#elif PNSLR_OSX
    return PNSLR_Platform_OSX;
#elif PNSLR_ANDROID
    return PNSLR_Platform_Android;
#elif PNSLR_IOS
    return PNSLR_Platform_iOS;
#elif PNSLR_PS5
    return PNSLR_Platform_PS5;
#elif PNSLR_XSERIES
    return PNSLR_Platform_XBoxSeries;
#elif PNSLR_SWITCH
    return PNSLR_Platform_Switch;
#else
    return PNSLR_Platform_Unknown;
#endif
}

PNSLR_Architecture PNSLR_GetArchitecture(void)
{
#if PNSLR_X64
    return PNSLR_Architecture_X64;
#elif PNSLR_ARM64
    return PNSLR_Architecture_ARM64;
#else
    return PNSLR_Architecture_Unknown;
#endif
}
