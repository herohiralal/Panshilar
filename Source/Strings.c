#define PNSLR_IMPLEMENTATION
#include "Strings.h"

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(Strings, 60);

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
    return (utf8str) {.count = length, .data = (u8*) str};
}

cstring PNSLR_CStringFromString(utf8str str, PNSLR_Allocator allocator)
{
    cstring result = PNSLR_MakeCString(str.count, false, allocator, PNSLR_GET_LOC(), nil);
    if (result == nil) { return nil; } // allocation failed

    PNSLR_MemCopy(result, str.data, (i32) str.count);
    result[str.count] = '\0'; // null-terminate the C-style string

    return result;
}

utf8str PNSLR_CloneString(utf8str str, PNSLR_Allocator allocator)
{
    if (!str.data || !str.count)
    {
        return (utf8str) {0};
    }

    utf8str result = PNSLR_MakeString(str.count, false, allocator, PNSLR_GET_LOC(), nil);
    if (result.data == nil) { return (utf8str) {0}; } // allocation failed

    PNSLR_MemCopy(result.data, str.data, (i32) str.count);
    return result;
}

utf8str PNSLR_ConcatenateStrings(utf8str str1, utf8str str2, PNSLR_Allocator allocator)
{
    utf8str result = PNSLR_MakeString(str1.count + str2.count, false, allocator, PNSLR_GET_LOC(), nil);
    if (result.data == nil) { return (utf8str) {0}; } // allocation failed

    PNSLR_MemCopy(result.data,              str1.data, (i32) str1.count);
    PNSLR_MemCopy(result.data + str1.count, str2.data, (i32) str2.count);
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

static b8 PNSLR_Internal_AreStringsEqual(cstring str1, i32 len1, cstring str2, i32 len2, PNSLR_StringComparisonType comparisonType)
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

static b8 PNSLR_Internal_StringStartsWith(cstring str, i32 strLen, cstring prefix, i32 prefixLen, PNSLR_StringComparisonType comparisonType)
{
    if (str    == nil) { str = "";    strLen = 0;    }
    if (prefix == nil) { return false;               } // doesn't make sense to try for this case

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

static b8 PNSLR_Internal_StringEndsWith(cstring str, i32 strLen, cstring suffix, i32 suffixLen, PNSLR_StringComparisonType comparisonType)
{
    if (str    == nil) { str = "";    strLen = 0;    }
    if (suffix == nil) { return false;               } // doesn't make sense to try for this case

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
    return PNSLR_Internal_AreStringsEqual((cstring) str1.data, (i32) str1.count, (cstring) str2.data, (i32) str2.count, comparisonType);
}

b8 PNSLR_AreStringAndCStringEqual(utf8str str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_AreStringsEqual((cstring) str1.data, (i32) str1.count, str2, (i32) PNSLR_GetCStringLength(str2), comparisonType);
}

b8 PNSLR_AreCStringsEqual(cstring str1, cstring str2, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_AreStringsEqual(str1, (i32) PNSLR_GetCStringLength(str1), str2, (i32) PNSLR_GetCStringLength(str2), comparisonType);
}

b8 PNSLR_StringStartsWith(utf8str str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringStartsWith((cstring) str.data, (i32) str.count, (cstring) prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_StringEndsWith(utf8str str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringEndsWith((cstring) str.data, (i32) str.count, (cstring) suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_StringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringStartsWith((cstring) str.data, (i32) str.count, prefix, (i32) PNSLR_GetCStringLength(prefix), comparisonType);
}

b8 PNSLR_StringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringEndsWith((cstring) str.data, (i32) str.count, suffix, (i32) PNSLR_GetCStringLength(suffix), comparisonType);
}

b8 PNSLR_CStringStartsWith(cstring str, utf8str prefix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringStartsWith(str, (i32) PNSLR_GetCStringLength(str), (cstring) prefix.data, (i32) prefix.count, comparisonType);
}

b8 PNSLR_CStringEndsWith(cstring str, utf8str suffix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringEndsWith(str, (i32) PNSLR_GetCStringLength(str), (cstring) suffix.data, (i32) suffix.count, comparisonType);
}

b8 PNSLR_CStringStartsWithCString(utf8str str, cstring prefix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringStartsWith((cstring) str.data, (i32) str.count, prefix, (i32) PNSLR_GetCStringLength(prefix), comparisonType);
}

b8 PNSLR_CStringEndsWithCString(utf8str str, cstring suffix, PNSLR_StringComparisonType comparisonType)
{
    return PNSLR_Internal_StringEndsWith((cstring) str.data, (i32) str.count, suffix, (i32) PNSLR_GetCStringLength(suffix), comparisonType);
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
        if (PNSLR_Internal_AreStringsEqual((cstring) str.data + i, subLen, (cstring) substring.data, subLen, comparisonType))
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
        if (PNSLR_Internal_AreStringsEqual((cstring) str.data + i, subLen, (cstring) substring.data, subLen, comparisonType))
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

    PNSLR_ArraySlice(u32) replacementIndices    = PNSLR_MakeSlice(u32, 64, false, internalAllocator, PNSLR_GET_LOC(), nil);
    i64                   numReplacementIndices = 0;

    i64 searchSpaceOffset = 0;
    while (true)
    {
        utf8str searchSpace = (utf8str) {.count = (str.count - searchSpaceOffset), .data = (str.data + searchSpaceOffset)};
        i32 idx = PNSLR_SearchFirstIndexInString(searchSpace, oldValue, comparisonType);
        if (idx < 0 || (searchSpaceOffset + (i64) idx) >= str.count) { break; }

        if (numReplacementIndices >= replacementIndices.count)
        {
            PNSLR_ResizeSlice(u32, &replacementIndices, (numReplacementIndices + 64), false, internalAllocator, PNSLR_GET_LOC(), nil);
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
        output = PNSLR_MakeString(newSize, false, allocator, PNSLR_GET_LOC(), nil);
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

#define PNSLR_RUNE_ERROR 0xfffd
#define PNSLR_MAX_RUNE 0x0010ffff

#define PNSLR_SURROGATE_MIN 0xd800
#define PNSLR_SURROGATE_MAX 0xdfff

#define PNSLR_MASKX 0x3f
#define PNSLR_MASK2 0x1f
#define PNSLR_MASK3 0x0f
#define PNSLR_MASK4 0x07

#define PNSLR_RUNE1_MAX 127
#define PNSLR_RUNE2_MAX 2047
#define PNSLR_RUNE3_MAX 65535

#define PNSLR_LOCB 0x80
#define PNSLR_HICB 0xbf

typedef struct PNSLR_Internal_AcceptRange { u8 lo, hi; } PNSLR_Internal_AcceptRange;
static PNSLR_Internal_AcceptRange G_PNSLR_Internal_AcceptRanges[5] = { {0x80, 0xbf}, {0xa0, 0xbf}, {0x80, 0x9f}, {0x90, 0xbf}, {0x80, 0x8f} };

static u8 PNSLR_Internal_GetAcceptSize(u8 byte) {
    if (byte <= 0x7f) return 0xf0;
    if (byte <= 0xc1) return 0xf1;
    if (byte <= 0xdf) return 0x02;
    if (byte == 0xe0) return 0x13;
    if (byte <= 0xec) return 0x03;
    if (byte == 0xed) return 0x23;
    if (byte <= 0xef) return 0x03;
    if (byte == 0xf0) return 0x34;
    if (byte <= 0xf3) return 0x04;
    if (byte == 0xf4) return 0x44;
    return 0xf1;
}

i32 PNSLR_GetRuneLength(u32 r) {
    if (r <= PNSLR_RUNE1_MAX)                                 { return 1;  }
    if (r <= PNSLR_RUNE2_MAX)                                 { return 2;  }
    if (PNSLR_SURROGATE_MIN <= r && r <= PNSLR_SURROGATE_MAX) { return -1; }
    if (r <= PNSLR_RUNE3_MAX)                                 { return 3;  }
    if (r <= PNSLR_MAX_RUNE)                                  { return 4;  }
    return -1;
}

PNSLR_EncodedRune PNSLR_EncodeRune(u32 c) {
    u32 r = c;
    PNSLR_EncodedRune result = {0};
    u8 mask = 0x3f;

    if (r <= (1 << 7) - 1) {
        result.data[0] = (u8)r;
        result.length = 1;
        return result;
    }

    if (r <= (1 << 11) - 1) {
        result.data[0] = 0xc0 | (u8)(r >> 6);
        result.data[1] = 0x80 | ((u8)r & mask);
        result.length = 2;
        return result;
    }

    if (r > 0x0010ffff || (PNSLR_SURROGATE_MIN <= r && r <= PNSLR_SURROGATE_MAX)) {
        r = PNSLR_RUNE_ERROR;
    }

    if (r <= (1 << 16) - 1) {
        result.data[0] = 0xe0 | (u8)(r >> 12);
        result.data[1] = 0x80 | ((u8)(r >> 6) & mask);
        result.data[2] = 0x80 | ((u8)r & mask);
        result.length = 3;
        return result;
    }

    result.data[0] = 0xf0 | (u8)(r >> 18);
    result.data[1] = 0x80 | ((u8)(r >> 12) & mask);
    result.data[2] = 0x80 | ((u8)(r >> 6) & mask);
    result.data[3] = 0x80 | ((u8)r & mask);
    result.length = 4;
    return result;
}

PNSLR_DecodedRune PNSLR_DecodeRune(PNSLR_ArraySlice(u8) s) {
    i64 n = s.count;
    PNSLR_DecodedRune result;

    if (n < 1) {
        result.rune = PNSLR_RUNE_ERROR;
        result.length = 0;
        return result;
    }

    u8 s0 = s.data[0];
    u8 x = PNSLR_Internal_GetAcceptSize(s0);

    if (x >= 0xf0) {
        u32 mask = (u32) x << 31 >> 31;
        result.rune = ((u32) s.data[0] & ~mask) | (PNSLR_RUNE_ERROR & mask);
        result.length = 1;
        return result;
    }

    i32 sz = x & 7;
    PNSLR_Internal_AcceptRange accept = G_PNSLR_Internal_AcceptRanges[x >> 4];

    if (n < sz) {
        result.rune = PNSLR_RUNE_ERROR;
        result.length = 1;
        return result;
    }

    u8 b1 = s.data[1];
    if (b1 < accept.lo || accept.hi < b1) {
        result.rune = PNSLR_RUNE_ERROR;
        result.length = 1;
        return result;
    }

    if (sz == 2) {
        result.rune = ((u32) (s0 & PNSLR_MASK2) << 6) | (u32) (b1 & PNSLR_MASKX);
        result.length = 2;
        return result;
    }

    u8 b2 = s.data[2];
    if (b2 < PNSLR_LOCB || PNSLR_HICB < b2) {
        result.rune = PNSLR_RUNE_ERROR;
        result.length = 1;
        return result;
    }

    if (sz == 3) {
        result.rune = ((u32)(s0 & PNSLR_MASK3) << 12) |
                      ((u32)(b1 & PNSLR_MASKX) << 6) |
                      (u32)(b2 & PNSLR_MASKX);
        result.length = 3;
        return result;
    }

    u8 b3 = s.data[3];
    if (b3 < PNSLR_LOCB || PNSLR_HICB < b3) {
        result.rune = PNSLR_RUNE_ERROR;
        result.length = 1;
        return result;
    }

    result.rune = ((u32)(s0 & PNSLR_MASK4) << 18) |
                  ((u32)(b1 & PNSLR_MASKX) << 12) |
                  ((u32)(b2 & PNSLR_MASKX) << 6) |
                  (u32)(b3 & PNSLR_MASKX);
    result.length = 4;
    return result;
}

#undef PNSLR_RUNE_ERROR
#undef PNSLR_MAX_RUNE
#undef PNSLR_SURROGATE_MIN
#undef PNSLR_SURROGATE_MAX
#undef PNSLR_MASKX
#undef PNSLR_MASK2
#undef PNSLR_MASK3
#undef PNSLR_MASK4
#undef PNSLR_RUNE1_MAX
#undef PNSLR_RUNE2_MAX
#undef PNSLR_RUNE3_MAX
#undef PNSLR_LOCB
#undef PNSLR_HICB

PNSLR_ArraySlice(u16) PNSLR_UTF16FromUTF8WindowsOnly(utf8str str, PNSLR_Allocator allocator)
{
    #if PNSLR_WINDOWS
    {
        if (!str.data || !str.count)
        {
            return (PNSLR_ArraySlice(u16)) {0};
        }

        i32 n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (cstring) str.data, (i32) str.count, nil, 0);
        if (n <= 0) { return (PNSLR_ArraySlice(u16)) {0}; } // conversion failed

        PNSLR_ArraySlice(u16) output = PNSLR_MakeSlice(u16, (n + 1), false, allocator, PNSLR_GET_LOC(), nil);

        i32 n1 = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (cstring) str.data, (i32) str.count, (LPWSTR) output.data, (i32) n);
        if (n1 == 0)
        {
            PNSLR_FreeSlice(&output, allocator, PNSLR_GET_LOC(), nil);
            return (PNSLR_ArraySlice(u16)) {0}; // conversion failed
        }

        output.data[n] = 0; // null-terminate the UTF-16 string

        while (n >= 1 && output.data[n - 1] == 0)
        {
            --n; // remove trailing null characters
        }

        output.count = n; // update count to exclude trailing null characters
        return output;
    }
    #else
    {
        return (PNSLR_ArraySlice(u16)) {0};
    }
    #endif
}

utf8str PNSLR_UTF8FromUTF16WindowsOnly(PNSLR_ArraySlice(u16) utf16str, PNSLR_Allocator allocator)
{
    #if PNSLR_WINDOWS
    {
        if (!utf16str.data || !utf16str.count)
        {
            return (utf8str) {0};
        }

        i32 n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (LPCWSTR) utf16str.data, (i32) utf16str.count, nil, 0, nil, nil);
        if (n <= 0) { return (utf8str) {0}; } // conversion failed

        utf8str output = PNSLR_MakeString(n, false, allocator, PNSLR_GET_LOC(), nil);
        if (!output.data) { return (utf8str) {0}; } // allocation failed

        i32 n1 = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (LPCWSTR) utf16str.data, (i32) utf16str.count, (LPSTR) output.data, (i32) n, nil, nil);
        if (n1 == 0)
        {
            PNSLR_FreeString(output, allocator, PNSLR_GET_LOC(), nil);
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
    #else
    {
        return (utf8str) {0};
    }
    #endif
}
