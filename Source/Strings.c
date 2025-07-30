#include "Strings.h"

static b8 AreStringsEqualInternal(char* str1, i32 len1, char* str2, i32 len2, PNSLR_StringComparisonType comparisonType)
{
    if (str1 == nil) { str1 = ""; len1 = 0; }
    if (str2 == nil) { str2 = ""; len2 = 0; }

    if (len1 != len2) { return false; } // different lengths, cannot be equal
    if (str1 == str2) { return true;  } // same pointer, same string

    for (i32 i = 0; i < len1; ++i)
    {
        char c1 = str1[i];
        char c2 = str2[i];

        if (comparisonType == PNSLR_StringComparisonType_CaseInsensitive)
        {
            if (c1 >= 'A' && c1 <= 'Z') { c1 += ('a' - 'A'); }
            if (c2 >= 'A' && c2 <= 'Z') { c2 += ('a' - 'A'); }
        }

        if (c1 != c2) { return false; }
    }

    return true;
}

static b8 StringStartsWithInternal(char* str, i32 strLen, char* prefix, i32 prefixLen, PNSLR_StringComparisonType comparisonType)
{
    if (str    == nil) { str = "";    strLen = 0;    }
    if (prefix == nil) { prefix = ""; prefixLen = 0; }

    if (strLen < prefixLen) { return false; }

    for (i32 i = 0; i < prefixLen; ++i)
    {
        char c1 = str[i];
        char c2 = prefix[i];

        if (comparisonType == PNSLR_StringComparisonType_CaseInsensitive)
        {
            if (c1 >= 'A' && c1 <= 'Z') { c1 += ('a' - 'A'); }
            if (c2 >= 'A' && c2 <= 'Z') { c2 += ('a' - 'A'); }
        }

        if (c1 != c2) { return false; }
    }

    return true;
}

static b8 StringEndsWithInternal(char* str, i32 strLen, char* suffix, i32 suffixLen, PNSLR_StringComparisonType comparisonType)
{
    if (str == nil || suffix == nil || strLen < suffixLen) { return false; }

    for (i32 i = 0; i < suffixLen; ++i)
    {
        char c1 = str[strLen - suffixLen + i];
        char c2 = suffix[i];

        if (comparisonType == PNSLR_StringComparisonType_CaseInsensitive)
        {
            if (c1 >= 'A' && c1 <= 'Z') { c1 += ('a' - 'A'); }
            if (c2 >= 'A' && c2 <= 'Z') { c2 += ('a' - 'A'); }
        }

        if (c1 != c2) { return false; }
    }

    return true;
}

b8 PNSLR_AreStringsEqual(utf8str str1, utf8str str2, PNSLR_StringComparisonType comparisonType)
{
    return AreStringsEqualInternal(str1.data, (i32) str1.count, str2.data, (i32) str2.count, comparisonType);
}

b8 PNSLR_AreStringAndCStringEqual(utf8str str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return AreStringsEqualInternal(str1.data, (i32) str1.count, str2, (i32) PNSLR_GetStringLength(str2), comparisonType);
}

b8 PNSLR_AreCStringsEqual(cstring str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return AreStringsEqualInternal(str1, (i32) PNSLR_GetStringLength(str1), str2, (i32) PNSLR_GetStringLength(str2), comparisonType);
}

b8 PNSLR_StringStartsWith(utf8str str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal(str.data, (i32) str.count, prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_StringEndsWith(utf8str str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal(str.data, (i32) str.count, suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_StringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal(str.data, (i32) str.count, prefix, (i32) PNSLR_GetStringLength(prefix), comparisonType);
}

b8 PNSLR_StringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal(str.data, (i32) str.count, suffix, (i32) PNSLR_GetStringLength(suffix), comparisonType);
}

b8 PNSLR_CStringStartsWith(cstring str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal(str, (i32) PNSLR_GetStringLength(str), prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_CStringEndsWith(cstring str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal(str, (i32) PNSLR_GetStringLength(str), suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_CStringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal(str.data, (i32) str.count, prefix, (i32) PNSLR_GetStringLength(prefix), comparisonType);
}

b8 PNSLR_CStringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal(str.data, (i32) str.count, suffix, (i32) PNSLR_GetStringLength(suffix), comparisonType);
}

i32 PNSLR_GetStringLength(cstring str)
{
    if (str == nil) { return 0; }

    i32 length = 0;
    while (str[length] != '\0') { ++length;}
    return length;
}

utf8str PNSLR_StringFromCString(cstring str)
{
    if (str == nil) { str = ""; }
    i32 length = PNSLR_GetStringLength(str);
    return (utf8str) {.count = length, .data = (utf8ch*)str};
}

cstring PNSLR_CStringFromString(utf8str str, PNSLR_Allocator allocator)
{
    if (PNSLR_AreStringsEqual(str, PNSLR_STRING_LITERAL(""), PNSLR_StringComparisonType_CaseSensitive))
    {
        str = PNSLR_STRING_LITERAL("");
    }

    cstring result = PNSLR_Allocate(allocator, false, (str.count + 1) * sizeof(char), alignof(char), CURRENT_LOC(), nil);
    if (result == nil) { return nil; } // allocation failed

    PNSLR_Intrinsic_MemCopy(result, str.data, (i32) str.count);
    result[str.count] = '\0'; // null-terminate the C-style string

    return result;
}

utf8str PNSLR_CloneString(utf8str str, PNSLR_Allocator allocator)
{
    if (PNSLR_AreStringsEqual(str, PNSLR_STRING_LITERAL(""), PNSLR_StringComparisonType_CaseSensitive))
    {
        str = PNSLR_STRING_LITERAL("");
    }

    utf8str result = PNSLR_MakeSlice(utf8ch, str.count, false, allocator, nil);
    if (result.data == nil) { return (utf8str) {0}; } // allocation failed

    PNSLR_Intrinsic_MemCopy(result.data, str.data, (i32) str.count);
    return result;
}
