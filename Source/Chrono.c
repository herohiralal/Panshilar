#include "Chrono.h"

i64 PNSLR_NanosecondsSinceUnixEpoch(void)
{
    #if PNSLR_WINDOWS
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        u64 dt = ((u64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        return (i64)((dt - 116444736000000000ULL) * 100);
    #elif PNSLR_UNIX
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            return -1; // Error handling
        }
        return (i64)ts.tv_sec * 1000000000 + ts.tv_nsec;
    #endif
}
