#define PNSLR_IMPLEMENTATION
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

b8 PNSLR_ConvertNanosecondsSinceUnixEpochToDateTime(
    i64  ns,
    i16* outYear,
    u8*  outMonth,
    u8*  outDay,
    u8*  outHour,
    u8*  outMinute,
    u8*  outSecond
) {
    // Convert nanoseconds to seconds
    i64 seconds = ns / 1000000000;

    // Handle negative timestamps (before epoch)
    if (seconds < 0) {
        return false;
    }

    // Calculate time components first (these are straightforward)
    if (outHour)   *outHour   = (u8)((seconds / 3600) % 24);
    if (outMinute) *outMinute = (u8)((seconds / 60) % 60);
    if (outSecond) *outSecond = (u8)(seconds % 60);

    // Calculate days since epoch
    i64 days = seconds / 86400; // 86400 seconds in a day

    // Start from Unix epoch: January 1, 1970
    i64 year = 1970;
    i64 month = 1;
    i64 day = 1;

    // Add the days to get the actual date
    day += days;

    // Days in each month (non-leap year)
    static const u8 daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Normalize the date
    while (true) {
        // Check if current year is a leap year
        b8 isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        u8 daysThisMonth = daysInMonth[month - 1];
        if (month == 2 && isLeapYear) {
            daysThisMonth = 29;
        }

        if (day <= daysThisMonth) {
            break; // Date is valid
        }

        // Move to next month
        day -= daysThisMonth;
        month++;

        if (month > 12) {
            month = 1;
            year++;
        }

        // Sanity check to prevent infinite loops
        if (year > 9999) {
            return false;
        }
    }

    // Set output values
    if (outYear)  *outYear  = (i16)year;
    if (outMonth) *outMonth = (u8)month;
    if (outDay)   *outDay   = (u8)day;

    return true;
}
