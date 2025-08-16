#ifndef PNSLR_STRINGS_H // =========================================================
#define PNSLR_STRINGS_H

#include "__Prelude.h"
#include "Allocators.h"

// Basics/Conversions ==============================================================

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

// Basic/Manipulation ==============================================================

/**
 * Concatenates two UTF-8 strings into a new allocated string.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_ConcatenateStrings(utf8str str1, utf8str str2, PNSLR_Allocator allocator);

/**
 * Replaces all occurrences of a substring within a string with a new value.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_ReplaceInString(utf8str str, utf8str oldValue, utf8str newValue, PNSLR_Allocator allocator);

// Casing ==========================================================================

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

// Comparisons =====================================================================

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

// Search ==========================================================================

/**
 * Searches for the first occurrence of a substring within a string.
 * Returns the index of the first occurrence, or -1 if not found.
 */
i32 PNSLR_SearchFirstIndexInString(utf8str str, utf8str substring, PNSLR_StringComparisonType comparisonType);

/**
 * Searches for the last occurrence of a substring within a string.
 * Returns the index of the last occurrence, or -1 if not found.
 */
i32 PNSLR_SearchLastIndexInString(utf8str str, utf8str substring, PNSLR_StringComparisonType comparisonType);

// Windows-specific bs for UTF-16 conversions ======================================

//+skipreflect

#if PNSLR_WINDOWS

/**
 * Converts a UTF-8 string to a UTF-16 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
ArraySlice(utf16ch) PNSLR_UTF16FromUTF8WindowsOnly(utf8str str, PNSLR_Allocator allocator);

/**
 * Converts a UTF-16 string to a UTF-8 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
utf8str PNSLR_UTF8FromUTF16WindowsOnly(ArraySlice(utf16ch) utf16str, PNSLR_Allocator allocator);

#endif

//-skipreflect

#endif // PNSLR_STRINGS_H ==========================================================
