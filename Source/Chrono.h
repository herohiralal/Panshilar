#ifndef PNSLR_CHRONO_H // ==========================================================
#define PNSLR_CHRONO_H
#include "__Prelude.h"
EXTERN_C_BEGIN

/**
 * Returns the current time in nanoseconds since the Unix epoch (January 1, 1970).
 */
i64 PNSLR_NanosecondsSinceUnixEpoch(void);

/**
 * Breaks down the given nanoseconds since the Unix epoch into its
 * date and time components.
 */
b8 PNSLR_ConvertNanosecondsSinceUnixEpochToDateTime(
    i64  ns,
    i16* outYear,
    u8*  outMonth,
    u8*  outDay,
    u8*  outHour,
    u8*  outMinute,
    u8*  outSecond
);

EXTERN_C_END
#endif // PNSLR_CHRONO_H ===========================================================
