#ifndef PNSLR_PLATFORM_INTRINSICS_H // =============================================
#define PNSLR_PLATFORM_INTRINSICS_H
//+skipreflect

// platform

#ifndef PNSLR_WINDOWS
    #define PNSLR_WINDOWS 0
#endif
#ifndef PNSLR_LINUX
    #define PNSLR_LINUX 0
#endif
#ifndef PNSLR_OSX
    #define PNSLR_OSX 0
#endif
#ifndef PNSLR_ANDROID
    #define PNSLR_ANDROID 0
#endif
#ifndef PNSLR_IOS
    #define PNSLR_IOS 0
#endif
#ifndef PNSLR_PS5
    #define PNSLR_PS5 0
#endif
#ifndef PNSLR_XSERIES
    #define PNSLR_XSERIES 0
#endif
#ifndef PNSLR_SWITCH
    #define PNSLR_SWITCH 0
#endif

// architecture

#ifndef PNSLR_X64
    #define PNSLR_X64 0
#endif
#ifndef PNSLR_ARM64
    #define PNSLR_ARM64 0
#endif

// derived

#define PNSLR_UNIX    (PNSLR_LINUX || PNSLR_OSX || PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_APPLE   (PNSLR_OSX || PNSLR_IOS)
#define PNSLR_DESKTOP (PNSLR_WINDOWS || PNSLR_LINUX || PNSLR_OSX)
#define PNSLR_MOBILE  (PNSLR_ANDROID || PNSLR_IOS)
#define PNSLR_CONSOLE (PNSLR_PS5 || PNSLR_XSERIES || PNSLR_SWITCH)

#if (PNSLR_WINDOWS + PNSLR_LINUX + PNSLR_OSX + PNSLR_ANDROID + PNSLR_IOS + PNSLR_PS5 + PNSLR_XSERIES + PNSLR_SWITCH) != 1
    #error "Exactly one platform must be defined."
#endif

#if (PNSLR_X64 + PNSLR_ARM64) != 1
    #error "Exactly one architecture must be defined."
#endif

//-skipreflect
#endif // PNSLR_PLATFORM_INTRINSICS_H ==============================================
