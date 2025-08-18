#include "Strings.h"

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(Strings, 1024);

i32 PNSLR_GetCStringLength(cstring str)
{
    if (str == nil) { return 0; }

    i32 length = 0;
    while (str[length] != '\0') { ++length;}
    return length;
}

utf8str PNSLR_StringFromCString(cstring str)
{
    if (str == nil) { str = ""; }
    i32 length = PNSLR_GetCStringLength(str);
    return (utf8str) {.count = length, .data = (utf8ch*)str};
}

cstring PNSLR_CStringFromString(utf8str str, PNSLR_Allocator allocator)
{
    cstring result = PNSLR_MakeCString(str.count, false, allocator, nil);
    if (result == nil) { return nil; } // allocation failed

    PNSLR_Intrinsic_MemCopy(result, str.data, (i32) str.count);
    result[str.count] = '\0'; // null-terminate the C-style string

    return result;
}

utf8str PNSLR_CloneString(utf8str str, PNSLR_Allocator allocator)
{
    if (!str.data || !str.count)
    {
        return (utf8str) {0};
    }

    utf8str result = PNSLR_MakeString(str.count, false, allocator, nil);
    if (result.data == nil) { return (utf8str) {0}; } // allocation failed

    PNSLR_Intrinsic_MemCopy(result.data, str.data, (i32) str.count);
    return result;
}

utf8str PNSLR_ConcatenateStrings(utf8str str1, utf8str str2, PNSLR_Allocator allocator)
{
    utf8str result = PNSLR_MakeString(str1.count + str2.count, false, allocator, nil);
    if (result.data == nil) { return (utf8str) {0}; } // allocation failed

    PNSLR_Intrinsic_MemCopy(result.data,              str1.data, (i32) str1.count);
    PNSLR_Intrinsic_MemCopy(result.data + str1.count, str2.data, (i32) str2.count);
    return result;
}

utf8str PNSLR_UpperString(utf8str str, PNSLR_Allocator allocator)
{
    utf8str copy = PNSLR_CloneString(str, allocator);
    if (copy.data == nil) { return (utf8str) {0}; }

    for (i32 i = 0; i < copy.count; ++i)
    {
        if (copy.data[i] >= 'a' && copy.data[i] <= 'z')
        {
            copy.data[i] -= ('a' - 'A'); // convert to uppercase
        }
    }

    return copy;
}

utf8str PNSLR_LowerString(utf8str str, PNSLR_Allocator allocator)
{
    utf8str copy = PNSLR_CloneString(str, allocator);
    if (copy.data == nil) { return (utf8str) {0}; }

    for (i32 i = 0; i < copy.count; ++i)
    {
        if (copy.data[i] >= 'A' && copy.data[i] <= 'Z')
        {
            copy.data[i] += ('a' - 'A'); // convert to lowercase
        }
    }

    return copy;
}

static b8 AreStringsEqualInternal(cstring str1, i32 len1, cstring str2, i32 len2, PNSLR_StringComparisonType comparisonType)
{
    if (str1 == nil) { str1 = ""; len1 = 0; }
    if (str2 == nil) { str2 = ""; len2 = 0; }

    if (len1 != len2) { return false; } // different lengths, cannot be equal
    if (len1 == 0)    { return true;  } // both are empty strings
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

static b8 StringStartsWithInternal(cstring str, i32 strLen, cstring prefix, i32 prefixLen, PNSLR_StringComparisonType comparisonType)
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

static b8 StringEndsWithInternal(cstring str, i32 strLen, cstring suffix, i32 suffixLen, PNSLR_StringComparisonType comparisonType)
{
    if (str    == nil) { str = "";    strLen = 0;    }
    if (suffix == nil) { suffix = ""; suffixLen = 0; }

    if (strLen < suffixLen) { return false; }

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
    return AreStringsEqualInternal((cstring) str1.data, (i32) str1.count, (cstring) str1.data, (i32) str2.count, comparisonType);
}

b8 PNSLR_AreStringAndCStringEqual(utf8str str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return AreStringsEqualInternal((cstring) str1.data, (i32) str1.count, str2, (i32) PNSLR_GetCStringLength(str2), comparisonType);
}

b8 PNSLR_AreCStringsEqual(cstring str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return AreStringsEqualInternal(str1, (i32) PNSLR_GetCStringLength(str1), str2, (i32) PNSLR_GetCStringLength(str2), comparisonType);
}

b8 PNSLR_StringStartsWith(utf8str str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal((cstring) str.data, (i32) str.count, (cstring) prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_StringEndsWith(utf8str str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal((cstring) str.data, (i32) str.count, (cstring) suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_StringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal((cstring) str.data, (i32) str.count, prefix, (i32) PNSLR_GetCStringLength(prefix), comparisonType);
}

b8 PNSLR_StringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal((cstring) str.data, (i32) str.count, suffix, (i32) PNSLR_GetCStringLength(suffix), comparisonType);
}

b8 PNSLR_CStringStartsWith(cstring str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal(str, (i32) PNSLR_GetCStringLength(str), (cstring) prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_CStringEndsWith(cstring str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal(str, (i32) PNSLR_GetCStringLength(str), (cstring) suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_CStringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return StringStartsWithInternal((cstring) str.data, (i32) str.count, prefix, (i32) PNSLR_GetCStringLength(prefix), comparisonType);
}

b8 PNSLR_CStringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return StringEndsWithInternal((cstring) str.data, (i32) str.count, suffix, (i32) PNSLR_GetCStringLength(suffix), comparisonType);
}

i32 PNSLR_SearchFirstIndexInString(utf8str str, utf8str substring, PNSLR_StringComparisonType comparisonType)
{
    if (str.data == nil || str.count == 0 || substring.data == nil || substring.count == 0)
    {
        return -1; // invalid input
    }

    i32 strLen = (i32) str.count;
    i32 subLen = (i32) substring.count;

    for (i32 i = 0; i <= strLen - subLen; ++i)
    {
        if (AreStringsEqualInternal((cstring) str.data + i, subLen, (cstring) substring.data, subLen, comparisonType))
        {
            return i; // found at index i
        }
    }

    return -1; // not found
}

i32 PNSLR_SearchLastIndexInString(utf8str str, utf8str substring, PNSLR_StringComparisonType comparisonType)
{
    if (str.data == nil || str.count == 0 || substring.data == nil || substring.count == 0)
    {
        return -1; // invalid input
    }

    i32 strLen = (i32) str.count;
    i32 subLen = (i32) substring.count;

    for (i32 i = strLen - subLen; i >= 0; --i)
    {
        if (AreStringsEqualInternal((cstring) str.data + i, subLen, (cstring) substring.data, subLen, comparisonType))
        {
            return i; // found at index i
        }
    }

    return -1; // not found
}

utf8str PNSLR_ReplaceInString(utf8str str, utf8str oldValue, utf8str newValue, PNSLR_Allocator allocator, PNSLR_StringComparisonType comparisonType)
{
    if (!str.data || !str.count || !oldValue.data || !oldValue.count) { return (utf8str) {0}; }

    utf8str output = {0};
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);

    ArraySlice(u32) replacementIndices    = PNSLR_MakeSlice(u32, 64, false, internalAllocator, nil);
    i64             numReplacementIndices = 0;

    i64 searchSpaceOffset = 0;
    while (true)
    {
        utf8str searchSpace = (utf8str) {.count = (str.count - searchSpaceOffset), .data = (str.data + searchSpaceOffset)};
        i32 idx = PNSLR_SearchFirstIndexInString(searchSpace, oldValue, comparisonType);
        if (idx < 0 || (searchSpaceOffset + (i64) idx) >= str.count) { break; }

        if (numReplacementIndices >= replacementIndices.count)
        {
            PNSLR_ResizeSlice(u32, replacementIndices, (numReplacementIndices + 64), false, internalAllocator, nil);
        }

        replacementIndices.data[numReplacementIndices] = (u32) (searchSpaceOffset + idx);
        numReplacementIndices++;

        searchSpaceOffset += (i64) idx + oldValue.count; // move past the found substring
    }

    if (numReplacementIndices == 0)
    {
        output = PNSLR_CloneString(str, allocator);
    }
    else
    {
        i64 newSize = str.count + ((newValue.count - oldValue.count) * numReplacementIndices);
        output = PNSLR_MakeString(newSize, false, allocator, nil);
        if (output.data && output.count)
        {
            i64 srcIndex = 0;
            i64 dstIndex = 0;
            for (i64 r = 0; r < numReplacementIndices; r++)
            {
                i64 repIndex = replacementIndices.data[r];
                i64 chunkSize = repIndex - srcIndex;
                for (i64 i = 0; i < chunkSize; i++) { output.data[dstIndex + i] = str.data[srcIndex + i]; }
                dstIndex += chunkSize;
                srcIndex += chunkSize;
                for (i64 i = 0; i < newValue.count; i++) { output.data[dstIndex + i] = newValue.data[i]; }
                dstIndex += newValue.count;
                srcIndex += oldValue.count;
            }
            i64 remaining = str.count - srcIndex;
            for (i64 i = 0; i < remaining; i++) { output.data[dstIndex + i] = str.data[srcIndex + i]; }
        }
    }

    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    return output;
}

#if PNSLR_WINDOWS

ArraySlice(utf16ch) PNSLR_UTF16FromUTF8WindowsOnly(utf8str str, PNSLR_Allocator allocator)
{
    if (!str.data || !str.count)
    {
        return EMPTY_ARRAY_SLICE(utf16ch);
    }

    i32 n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (cstring) str.data, (i32) str.count, nil, 0);
    if (n <= 0) { return EMPTY_ARRAY_SLICE(utf16ch); } // conversion failed

    ArraySlice(utf16ch) output = PNSLR_MakeSlice(utf16ch, (n + 1), false, allocator, nil);

    i32 n1 = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (cstring) str.data, (i32) str.count, (LPWSTR) output.data, (i32) n);
    if (n1 == 0)
    {
        PNSLR_FreeSlice(output, allocator, nil);
        return EMPTY_ARRAY_SLICE(utf16ch); // conversion failed
    }

    output.data[n] = 0; // null-terminate the UTF-16 string

    while (n >= 1 && output.data[n - 1] == 0)
    {
        --n; // remove trailing null characters
    }

    output.count = n; // update count to exclude trailing null characters
    return output;
}

utf8str PNSLR_UTF8FromUTF16WindowsOnly(ArraySlice(utf16ch) utf16str, PNSLR_Allocator allocator)
{
    if (!utf16str.data || !utf16str.count)
    {
        return (utf8str) {0};
    }

    i32 n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (LPCWSTR) utf16str.data, (i32) utf16str.count, nil, 0, nil, nil);
    if (n <= 0) { return (utf8str) {0}; } // conversion failed

    utf8str output = PNSLR_MakeString(n, false, allocator, nil);
    if (!output.data) { return (utf8str) {0}; } // allocation failed

    i32 n1 = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (LPCWSTR) utf16str.data, (i32) utf16str.count, (LPSTR) output.data, (i32) n, nil, nil);
    if (n1 == 0)
    {
        PNSLR_FreeString(output, allocator, nil);
        return (utf8str) {0}; // conversion failed
    }

    for (i32 i = 0; i < n; i++)
    {
        if (output.data[i] == 0)
        {
            n = i; // remove trailing null characters
            break;
        }
    }

    output.count = n; // update count to exclude trailing null characters
    return output;
}

#endif
