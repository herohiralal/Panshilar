#ifndef PNSLR_STRINGS_H // =========================================================
#define PNSLR_STRINGS_H

#include "__Prelude.h"
#include "Allocators.h"

EXTERN_C_BEGIN

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
i32 PNSLR_GetCStringLength(cstring str);

/**
 * Clone a C-style string into a new allocated string.
 */
utf8str PNSLR_StringFromCString(cstring str);

/**
 * Clones a UTF-8 string to a C-style null-terminated string.
 * The returned string is allocated using the specified allocator.
 */
cstring PNSLR_CStringFromString(utf8str str, PNSLR_Allocator allocator);

/**
 * Clones a UTF-8 string to a new allocated UTF-8 string.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_CloneString(utf8str str, PNSLR_Allocator allocator);

/**
 * Converts a UTF-8 string to uppercase.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_UpperString(utf8str str, PNSLR_Allocator allocator);

/**
 * Converts a UTF-8 string to lowercase.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_LowerString(utf8str str, PNSLR_Allocator allocator);

/**
 * Represents the type of string comparison to perform.
 */
ENUM_START(PNSLR_StringComparisonType, u8)
    #define PNSLR_StringComparisonType_CaseSensitive   ((PNSLR_StringComparisonType) 0)
    #define PNSLR_StringComparisonType_CaseInsensitive ((PNSLR_StringComparisonType) 1)
ENUM_END

/**
 * Checks if two UTF-8 strings contain the same data.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreStringsEqual(utf8str str1, utf8str str2, PNSLR_StringComparisonType comparisonType);

/**
 * Asymmetric equality-check between a UTF-8 string and a C-style null-terminated string.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreStringAndCStringEqual(utf8str str1, cstring str2, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if two C-style null-terminated strings are equal.
 * Returns true if they are equal, false otherwise.
 */
b8 PNSLR_AreCStringsEqual(cstring str1, cstring str2, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a UTF-8 string starts with the specified prefix.
 * Returns true if it does, false otherwise.
 */
b8 PNSLR_StringStartsWith(utf8str str, utf8str prefix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a UTF-8 string ends with the specified suffix.
 * Returns true if it does, false otherwise.
 */
b8 PNSLR_StringEndsWith(utf8str str, utf8str suffix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a C-style null-terminated string starts with the specified prefix.
 */
b8 PNSLR_StringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a C-style null-terminated string ends with the specified suffix.
 */
b8 PNSLR_StringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType);

/**
 * Returns the length of the given C-style null-terminated string, excluding the null terminator.
 */
b8 PNSLR_CStringStartsWith(cstring str, utf8str prefix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
b8 PNSLR_CStringEndsWith(cstring str, utf8str suffix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a C-style null-terminated string starts with the specified UTF-8 prefix.
 */
b8 PNSLR_CStringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType);

/**
 * Checks if a C-style null-terminated string ends with the specified UTF-8 suffix.
 */
b8 PNSLR_CStringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType);

EXTERN_C_END

#endif // PNSLR_STRINGS_H ==========================================================
