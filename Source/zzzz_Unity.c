#ifndef PNSLR_UNITY_C
#define PNSLR_UNITY_C

#define PNSLR_IMPLEMENTATION
#include "__Prelude.h"

#include "RadDbgMarkup.c"
#include "Memory.c"
#include "Environment.c"
#include "Runtime.c"
#include "Sync.c"
#include "Allocators.c"
#include "Chrono.c"
#include "Strings.c"
#include "IO.c"
#include "Console.c"
#include "Process.c"
#include "Network.c"
#include "Stream.c"

#if PNSLR_ANDROID
    PNSLR_SUPPRESS_WARN
    #include <android_native_app_glue.c>
    PNSLR_UNSUPPRESS_WARN
#endif

#endif//PNSLR_UNITY_C
