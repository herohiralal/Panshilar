#ifndef PNSLR_STRINGS_H // =========================================================
#define PNSLR_STRINGS_H
#include "__Prelude.h"
#include "Allocators.h"
EXTERN_C_BEGIN

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

// Advanced comparisons ============================================================

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

/**
 * Replaces all occurrences of a substring within a string with a new value.
 * The returned string is allocated using the specified allocator.
 */
utf8str PNSLR_ReplaceInString(utf8str str, utf8str oldValue, utf8str newValue, PNSLR_Allocator allocator, PNSLR_StringComparisonType comparisonType);

// UTF-8 functionalities ===========================================================

/**
 * Result structure for UTF-8 rune encoding.
 * Contains the encoded bytes and the number of bytes used.
 */
typedef struct PNSLR_EncodedRune { u8 data[4]; i32 length; } PNSLR_EncodedRune;

/**
 * Result structure for UTF-8 rune decoding.
 * Contains the decoded rune and the number of bytes consumed.
 */
typedef struct PNSLR_DecodedRune { u32 rune; i32 length; } PNSLR_DecodedRune;

/**
 * Returns the number of bytes required to encode the given rune in UTF-8.
 */
i32 PNSLR_GetRuneLength(u32 r);

/**
 * Encodes a rune into UTF-8 byte sequence and returns the structure containing encoded bytes/length.
 * Invalid runes or surrogates are replaced with the error rune (U+FFFD).
 */
PNSLR_EncodedRune PNSLR_EncodeRune(u32 c);

/**
 * Decodes a UTF-8 byte sequence into a rune and returns the structure containing the rune/length.
 * Returns error rune (U+FFFD) for invalid sequences.
 */
PNSLR_DecodedRune PNSLR_DecodeRune(PNSLR_ArraySlice(u8) s);

// Windows-specific bs for UTF-16 conversions ======================================

/**
 * Converts a UTF-8 string to a UTF-16 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
PNSLR_ArraySlice(u16) PNSLR_UTF16FromUTF8WindowsOnly(utf8str str, PNSLR_Allocator allocator);

/**
 * Converts a UTF-16 string to a UTF-8 string.
 * The returned string is allocated using the specified allocator.
 * Only available on Windows. Bad decision to use UTF-16 on Windows, but it's a legacy thing.
 */
utf8str PNSLR_UTF8FromUTF16WindowsOnly(PNSLR_ArraySlice(u16) utf16str, PNSLR_Allocator allocator);

// String Builder ==================================================================

/**
 * A basic string builder. Can accept strings and characters,
 * and build a single string from them.
 *
 * Create by setting the allocator and zeroing the rest of the fields.
 */
typedef struct PNSLR_StringBuilder
{
    PNSLR_Allocator      allocator;
    PNSLR_ArraySlice(u8) buffer;
    i64                  writtenSize;
    i64                  cursorPos;
} PNSLR_StringBuilder;

/**
 * Append a single byte to the string builder. Could be an ANSI/ASCII character,
 * or not. The function does not check for validity.
 */
b8 PNSLR_AppendByteToStringBuilder(PNSLR_StringBuilder* builder, u8 byte);

/**
 * Append a UTF-8 string to the string builder.
 */
b8 PNSLR_AppendStringToStringBuilder(PNSLR_StringBuilder* builder, utf8str str);

/**
 * Append a C-style null-terminated string to the string builder.
 */
b8 PNSLR_AppendCStringToStringBuilder(PNSLR_StringBuilder* builder, cstring str);

/**
 * Append a single character (rune) to the string builder.
 */
b8 PNSLR_AppendRuneToStringBuilder(PNSLR_StringBuilder* builder, u32 rune);

/**
 * Append a boolean value to the string builder.
 */
b8 PNSLR_AppendBooleanToStringBuilder(PNSLR_StringBuilder* builder, b8 value);

/**
 * Append a 32-bit floating-point number to the string builder.
 */
b8 PNSLR_AppendF32ToStringBuilder(PNSLR_StringBuilder* builder, f32 value, i32 decimalPlaces);

/**
 * Append a 64-bit floating point number to the string builder.
 */
b8 PNSLR_AppendF64ToStringBuilder(PNSLR_StringBuilder* builder, f64 value, i32 decimalPlaces);

/**
 * The base to use when appending integer numbers to the string builder.
 */
ENUM_START(PNSLR_IntegerBase, u8)
    #define PNSLR_IntegerBase_Decimal     ((PNSLR_IntegerBase) 0) // first cuz good default
    #define PNSLR_IntegerBase_Binary      ((PNSLR_IntegerBase) 1)
    #define PNSLR_IntegerBase_HexaDecimal ((PNSLR_IntegerBase) 2)
    #define PNSLR_IntegerBase_Octal       ((PNSLR_IntegerBase) 3)
ENUM_END

/**
 * Append an unsigned 8-bit integer to the string builder.
 */
b8 PNSLR_AppendU8ToStringBuilder(PNSLR_StringBuilder* builder, u8 value, PNSLR_IntegerBase base);

/**
 * Append an unsigned 16-bit integer to the string builder.
 */
b8 PNSLR_AppendU16ToStringBuilder(PNSLR_StringBuilder* builder, u16 value, PNSLR_IntegerBase base);

/**
 * Append an unsigned 32-bit integer number to the string builder.
 */
b8 PNSLR_AppendU32ToStringBuilder(PNSLR_StringBuilder* builder, u32 value, PNSLR_IntegerBase base);

/**
 * Append an unsigned 64-bit integer to the string builder.
 */
b8 PNSLR_AppendU64ToStringBuilder(PNSLR_StringBuilder* builder, u64 value, PNSLR_IntegerBase base);

/**
 * Append a signed 8-bit integer to the string builder.
 */
b8 PNSLR_AppendI8ToStringBuilder(PNSLR_StringBuilder* builder, i8 value, PNSLR_IntegerBase base);

/**
 * Append a signed 16-bit integer to the string builder.
 */
b8 PNSLR_AppendI16ToStringBuilder(PNSLR_StringBuilder* builder, i16 value, PNSLR_IntegerBase base);

/**
 * Append a signed 32-bit integer number to the string builder.
 */
b8 PNSLR_AppendI32ToStringBuilder(PNSLR_StringBuilder* builder, i32 value, PNSLR_IntegerBase base);

/**
 * Append a signed 64-bit integer to the string builder.
 */
b8 PNSLR_AppendI64ToStringBuilder(PNSLR_StringBuilder* builder, i64 value, PNSLR_IntegerBase base);

/**
 * Return the string from the string builder.
 */
utf8str PNSLR_StringFromStringBuilder(PNSLR_StringBuilder* builder);

/**
 * Reset the string builder, clearing its contents but keeping the allocated buffer.
 */
void PNSLR_ResetStringBuilder(PNSLR_StringBuilder* builder);

/**
 * Free the resources used by the string builder.
 */
void PNSLR_FreeStringBuilder(PNSLR_StringBuilder* builder);

// String Formatting ===============================================================

/**
 * The possible primitive types that can be formatted.
 */
ENUM_START(PNSLR_PrimitiveFmtType, u8)
    #define PNSLR_PrimitiveFmtType_B8        ((PNSLR_PrimitiveFmtType) 0)
    #define PNSLR_PrimitiveFmtType_F32       ((PNSLR_PrimitiveFmtType) 1)
    #define PNSLR_PrimitiveFmtType_F64       ((PNSLR_PrimitiveFmtType) 2)
    #define PNSLR_PrimitiveFmtType_U8        ((PNSLR_PrimitiveFmtType) 3)
    #define PNSLR_PrimitiveFmtType_U16       ((PNSLR_PrimitiveFmtType) 4)
    #define PNSLR_PrimitiveFmtType_U32       ((PNSLR_PrimitiveFmtType) 5)
    #define PNSLR_PrimitiveFmtType_U64       ((PNSLR_PrimitiveFmtType) 6)
    #define PNSLR_PrimitiveFmtType_I8        ((PNSLR_PrimitiveFmtType) 7)
    #define PNSLR_PrimitiveFmtType_I16       ((PNSLR_PrimitiveFmtType) 8)
    #define PNSLR_PrimitiveFmtType_I32       ((PNSLR_PrimitiveFmtType) 9)
    #define PNSLR_PrimitiveFmtType_I64       ((PNSLR_PrimitiveFmtType) 10)
    #define PNSLR_PrimitiveFmtType_Rune      ((PNSLR_PrimitiveFmtType) 11)
    #define PNSLR_PrimitiveFmtType_CString   ((PNSLR_PrimitiveFmtType) 12)
    #define PNSLR_PrimitiveFmtType_String    ((PNSLR_PrimitiveFmtType) 13)
ENUM_END

/**
 * The internal encoding of a type-unspecific format specifier.
 * For booleans, valueBufferA is 0 or 1.
 * For floats, valueBufferA is the float value (reinterpret as relevant),
 *     and valueBufferB is the number of decimal places (cast to i32).
 * For integers, valueBufferA is the integer value (reinterpret as relevant),
 *     and the first half of valueBufferB is the base (cast to PNSLR_IntegerBase).
 * For runes, valueBufferA is the rune value (reinterpret as u32).
 * For C-style strings, valueBufferA is the pointer to the string.
 * For UTF-8 strings, valueBufferA is the pointer to the string,
 *     and valueBufferB is the length (reinterpret as i64).
 */
typedef struct PNSLR_PrimitiveFmtOptions
{
    PNSLR_PrimitiveFmtType type;
    u64                    valueBufferA;
    u64                    valueBufferB;
} PNSLR_PrimitiveFmtOptions;

DECLARE_ARRAY_SLICE(PNSLR_PrimitiveFmtOptions);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtB8(b8 value);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtF32(f32 value, i32 decimalPlaces);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtF64(f64 value, i32 decimalPlaces);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU8(u8 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU16(u16 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU32(u32 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtU64(u64 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI8(i8 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI16(i16 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI32(i32 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtI64(i64 value, PNSLR_IntegerBase base);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtRune(u32 value);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtCString(cstring value);

/**
 * Use when formatting a string. Pass as one of the varargs.
 */
PNSLR_PrimitiveFmtOptions PNSLR_FmtString(utf8str value);

//+skipreflect

/**
 * Helper macro to create an array slice of format options from varargs.
 * Note that this macro creates a temporary array, so it should only be used
 * when passing arguments to a function, not for storing the result.
 *
 * Use as:
 *     PNSLR_FmtArgs(
 *         PNSLR_FmtI32(42, PNSLR_IntegerBase_Decimal),
 *         PNSLR_FmtString(myStr),
 *         PNSLR_FmtF64(3.14159, 3),
 *         ... // more args
 *     )
 */
#define PNSLR_FmtArgs(...) (PNSLR_ArraySlice(PNSLR_PrimitiveFmtOptions)) \
    { \
        .data = (PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__}, \
        .count = sizeof((PNSLR_PrimitiveFmtOptions[]){__VA_ARGS__})/sizeof(PNSLR_PrimitiveFmtOptions) \
    }

//-skipreflect

// Conversions to strings ==========================================================

/**
 * Convert a boolean value to a string ("true" or "false").
 */
utf8str PNSLR_StringFromBoolean(b8 value, PNSLR_Allocator allocator);

/**
 * Convert a 32-bit floating-point number to a string with specified decimal places.
 */
utf8str PNSLR_StringFromF32(f32 value, i32 decimalPlaces, PNSLR_Allocator allocator);

/**
 * Convert a 64-bit floating-point number to a string with specified decimal places.
 */
utf8str PNSLR_StringFromF64(f64 value, i32 decimalPlaces, PNSLR_Allocator allocator);

/**
 * Convert an unsigned 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU8(u8 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert an unsigned 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU16(u16 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert an unsigned 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU32(u32 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert an unsigned 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromU64(u64 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert a signed 8-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI8(i8 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert a signed 16-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI16(i16 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert a signed 32-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI32(i32 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

/**
 * Convert a signed 64-bit integer to a string in the specified base.
 */
utf8str PNSLR_StringFromI64(i64 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator);

// Conversions from strings ========================================================

/**
 * Convert a validstring (case-insensitive "true" or "false", or "1" or "0") to a boolean.
 */
b8 PNSLR_BooleanFromString(utf8str str, b8* value);

/**
 * Convert a valid string (numbers-only, with zero or one decimal points,
 * optional -/+ sign at the start) to a 32-bit floating-point number.
 */
b8 PNSLR_F32FromString(utf8str str, f32* value);

/**
 * Convert a valid string (numbers-only, with zero or one decimal points,
 * optional -/+ sign at the start) to a 64-bit floating-point number.
 */
b8 PNSLR_F64FromString(utf8str str, f64* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 8-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U8FromString(utf8str str, u8* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 16-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U16FromString(utf8str str, u16* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 32-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U32FromString(utf8str str, u32* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optionally
 * starting with 0b/0o/0x prefix for alternate bases) to an unsigned 64-bit integer.
 * Will be assumed to be hexadecimal if it contains A-F characters but no prefix.
 * By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_U64FromString(utf8str str, u64* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 8-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I8FromString(utf8str str, i8* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 16-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I16FromString(utf8str str, i16* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 32-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I32FromString(utf8str str, i32* value);

/**
 * Convert a valid string (numbers/A-F only, case-insensitive, optional -/+ sign
 * at the start, optionally starting with 0b/0o/0x prefix for alternate bases) to
 * a signed 64-bit integer. Will be assumed to be hexadecimal if it contains A-F
 * characters but no prefix. By default (no prefix), decimal base is assumed.
 */
b8 PNSLR_I64FromString(utf8str str, i64* value);

EXTERN_C_END
#endif // PNSLR_STRINGS_H ==========================================================
