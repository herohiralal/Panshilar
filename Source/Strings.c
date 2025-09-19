#define PNSLR_IMPLEMENTATION
#include "Strings.h"
#include "Memory.h"

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

static b8 PNSLR_Internal_ResizeStringBuilderIfRequired(PNSLR_StringBuilder* builder, i64 lengthHint)
{
    if (builder->cursorPos + lengthHint <= builder->buffer.count) return true; // enough space already

    i64 newSize = builder->buffer.count == 0 ? 64 : builder->buffer.count;
    while (builder->cursorPos + lengthHint > newSize) { newSize *= 2; } // double until it fits

    PNSLR_AllocatorError err = PNSLR_AllocatorError_None;
    PNSLR_ResizeSlice(u8, &(builder->buffer), newSize, false, (builder->allocator), PNSLR_GET_LOC(), &err);
    return err == PNSLR_AllocatorError_None;
}

b8 PNSLR_AppendByteToStringBuilder(PNSLR_StringBuilder* builder, u8 byte)
{
    if (!builder || !PNSLR_Internal_ResizeStringBuilderIfRequired(builder, 1)) return false;

    builder->buffer.data[builder->cursorPos] = byte;
    builder->cursorPos += 1;
    if (builder->cursorPos > builder->writtenSize) { builder->writtenSize = builder->cursorPos; }
    return true;
}

b8 PNSLR_AppendStringToStringBuilder(PNSLR_StringBuilder* builder, utf8str str)
{
    if (!builder || !PNSLR_Internal_ResizeStringBuilderIfRequired(builder, str.count)) return false;

    PNSLR_MemCopy(builder->buffer.data + builder->cursorPos, str.data, (i32) str.count);
    builder->cursorPos += str.count;
    if (builder->cursorPos > builder->writtenSize) { builder->writtenSize = builder->cursorPos; }
    return true;
}

b8 PNSLR_AppendCStringToStringBuilder(PNSLR_StringBuilder* builder, cstring str)
{
    return PNSLR_AppendStringToStringBuilder(builder, PNSLR_StringFromCString(str));
}

b8 PNSLR_AppendRuneToStringBuilder(PNSLR_StringBuilder* builder, u32 rune)
{
    PNSLR_EncodedRune r = PNSLR_EncodeRune(rune);
    utf8str rStr = {.count = (i64) r.length, .data = &(r.data[0])};
    return PNSLR_AppendStringToStringBuilder(builder, rStr);
}

b8 PNSLR_AppendB8ToStringBuilder(PNSLR_StringBuilder* builder, b8 value)
{
    if (value) return PNSLR_AppendStringToStringBuilder(builder, PNSLR_StringLiteral("true" ));
    else       return PNSLR_AppendStringToStringBuilder(builder, PNSLR_StringLiteral("false"));
}

b8 PNSLR_AppendF32ToStringBuilder(PNSLR_StringBuilder* builder, f32 value, i32 decimalPlaces)
{
    return PNSLR_AppendF64ToStringBuilder(builder, (f64) value, decimalPlaces);
}

b8 PNSLR_AppendF64ToStringBuilder(PNSLR_StringBuilder* builder, f64 value, i32 decimalPlaces)
{
    if (!builder) return false;
    if (value != value) { return PNSLR_AppendCStringToStringBuilder(builder, "NaN"); } // NaN

    b8 status = true;
    if (value == 0.0)
    {
        status = PNSLR_AppendByteToStringBuilder(builder, '0');
        if (decimalPlaces > 0)
        {
            status = status && PNSLR_AppendByteToStringBuilder(builder, '.');
            for (i32 i = 0; i < decimalPlaces; i++)
            {
                status = status && PNSLR_AppendByteToStringBuilder(builder, '0');
            }
        }
        return status;
    }

    // Handle sign
    f64 absValue = value;
    if (value < 0.0)
    {
        status = PNSLR_AppendByteToStringBuilder(builder, '-');
        absValue = -value;
    }

    // Integer part
    u64 intPart = (u64) absValue;
    status = status && PNSLR_AppendU64ToStringBuilder(builder, intPart, PNSLR_IntegerBase_Decimal);

    // Fractional part
    if (decimalPlaces > 0)
    {
        status = status && PNSLR_AppendByteToStringBuilder(builder, '.');

        f64 fracPart = absValue - (f64) intPart;

        // Scale fractional part by 10^decimalPlaces
        u64 factor = 1;
        for (i32 i = 0; i < decimalPlaces; i++) factor *= 10;

        u64 scaledFrac = (u64)(fracPart * (f64)factor + 0.5); // round

        // Count digits of scaledFrac
        u64 temp = scaledFrac;
        i32 digitCount = 0;
        if (temp == 0) { digitCount = 1; }
        else while (temp > 0) { digitCount++; temp /= 10; }

        // trailing zeroes
        for (i32 i = 0; i < decimalPlaces - digitCount; i++) { status = status && PNSLR_AppendByteToStringBuilder(builder, '0'); }
        if (scaledFrac > 0) { status = status && PNSLR_AppendU64ToStringBuilder(builder, scaledFrac, PNSLR_IntegerBase_Decimal); }
    }

    return status;
}

b8 PNSLR_AppendU8ToStringBuilder(PNSLR_StringBuilder* builder, u8 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendU64ToStringBuilder(builder, (u64) value, base);
}

b8 PNSLR_AppendU16ToStringBuilder(PNSLR_StringBuilder* builder, u16 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendU64ToStringBuilder(builder, (u64) value, base);
}

b8 PNSLR_AppendU32ToStringBuilder(PNSLR_StringBuilder* builder, u32 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendU64ToStringBuilder(builder, (u64) value, base);
}

b8 PNSLR_AppendU64ToStringBuilder(PNSLR_StringBuilder* builder, u64 value, PNSLR_IntegerBase base)
{
    u8 prefix = 0;
    switch (base)
    {
        case PNSLR_IntegerBase_Binary:      prefix = 'b'; break;
        case PNSLR_IntegerBase_HexaDecimal: prefix = 'x'; break;
        case PNSLR_IntegerBase_Octal:       prefix = 'o'; break;
        case PNSLR_IntegerBase_Decimal:                   break;
        default: FORCE_DBG_TRAP; break;
    }

    if (prefix)
    {
        b8 prefixAdded = PNSLR_AppendByteToStringBuilder(builder, '0') && PNSLR_AppendByteToStringBuilder(builder, (u8) prefix);
        if (!prefixAdded) { return false; }
    }

    if (value == 0)
    {
        return PNSLR_AppendByteToStringBuilder(builder, '0');
    }

    u8 digits[64] = {0};
    i64 count = 0;

    u64 baseInt;
    switch (base)
    {
        case PNSLR_IntegerBase_Binary:      baseInt = 2;  break;
        case PNSLR_IntegerBase_HexaDecimal: baseInt = 16; break;
        case PNSLR_IntegerBase_Octal:       baseInt = 8;  break;
        case PNSLR_IntegerBase_Decimal:     baseInt = 10; break;
        default: FORCE_DBG_TRAP; return false;
    }

    u64 tempValue = value;
    while (tempValue > 0)
    {
        u64 digit = (tempValue % baseInt);
        if (digit < 10) { digits[count] = (u8) ('0' + digit);        }
        else            { digits[count] = (u8) ('a' + (digit - 10)); }

        tempValue /= baseInt;
        count++;
    }

    for (i64 i = count - 1; i >= 0; --i)
    {
        if (!PNSLR_AppendByteToStringBuilder(builder, digits[i])) { return false; }
    }

    return true;
}

b8 PNSLR_AppendI8ToStringBuilder(PNSLR_StringBuilder* builder, i8 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendI64ToStringBuilder(builder, (i64) value, base);
}

b8 PNSLR_AppendI16ToStringBuilder(PNSLR_StringBuilder* builder, i16 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendI64ToStringBuilder(builder, (i64) value, base);
}

b8 PNSLR_AppendI32ToStringBuilder(PNSLR_StringBuilder* builder, i32 value, PNSLR_IntegerBase base)
{
    return PNSLR_AppendI64ToStringBuilder(builder, (i64) value, base);
}

b8 PNSLR_AppendI64ToStringBuilder(PNSLR_StringBuilder* builder, i64 value, PNSLR_IntegerBase base)
{
    b8 status = true;
    if (value < 0)
    {
        status = PNSLR_AppendByteToStringBuilder(builder, '-');
        value = -value;
    }

    return status && PNSLR_AppendU64ToStringBuilder(builder, (u64) value, base);
}

utf8str PNSLR_StringFromStringBuilder(PNSLR_StringBuilder* builder)
{
    if (!builder || builder->writtenSize == 0) { return (utf8str) {0}; }
    return (utf8str) {.data = builder->buffer.data, .count = builder->writtenSize};
}

void PNSLR_ResetStringBuilder(PNSLR_StringBuilder* builder)
{
    if (builder)
    {
        builder->writtenSize = 0;
        builder->cursorPos   = 0;
    }
}

void PNSLR_FreeStringBuilder(PNSLR_StringBuilder* builder)
{
    if (builder)
    {
        PNSLR_FreeSlice(&(builder->buffer), builder->allocator, PNSLR_GET_LOC(), nil);
        *builder = (PNSLR_StringBuilder) {0};
    }
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtB8(b8 value)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_B8,
        .valueBufferA = value ? 1 : 0,
        .valueBufferB = 0,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtF32(f32 value, i32 decimalPlaces)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_F32,
        .valueBufferA = (u64) (*(u32*) &value),
        .valueBufferB = (u64) decimalPlaces,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtF64(f64 value, i32 decimalPlaces)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_F64,
        .valueBufferA = *(u64*) &value,
        .valueBufferB = (u64) decimalPlaces,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtU8(u8 value, PNSLR_IntegerBase base)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_U8,
        .valueBufferA = (u64) value,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtU16(u16 value, PNSLR_IntegerBase base)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_U16,
        .valueBufferA = (u64) value,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtU32(u32 value, PNSLR_IntegerBase base)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_U32,
        .valueBufferA = (u64) value,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtU64(u64 value, PNSLR_IntegerBase base)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_U64,
        .valueBufferA = value,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtI8(i8 value, PNSLR_IntegerBase base)
{
    i64 tmpVal = (i64) value;
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_I8,
        .valueBufferA = *(u64*) &tmpVal,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtI16(i16 value, PNSLR_IntegerBase base)
{
    i64 tmpVal = (i64) value;
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_I16,
        .valueBufferA = *(u64*) &tmpVal,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtI32(i32 value, PNSLR_IntegerBase base)
{
    i64 tmpVal = (i64) value;
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_I32,
        .valueBufferA = *(u64*) &tmpVal,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtI64(i64 value, PNSLR_IntegerBase base)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_I64,
        .valueBufferA = *(u64*) &value,
        .valueBufferB = (u64) base,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtRune(u32 value)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_Rune,
        .valueBufferA = (u64) value,
        .valueBufferB = 0,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtCString(cstring value)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_CString,
        .valueBufferA = *(u64*) &value,
        .valueBufferB = 0,
    };
}

PNSLR_PrimitiveFmtOptions PNSLR_FmtString(utf8str value)
{
    return (PNSLR_PrimitiveFmtOptions)
    {
        .type         = PNSLR_PrimitiveFmtType_String,
        .valueBufferA = *(u64*) &value.data,
        .valueBufferB = *(u64*) &value.count,
    };
}

utf8str PNSLR_StringFromBoolean(b8 value, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendB8ToStringBuilder(&builder, value);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromF32(f32 value, i32 decimalPlaces, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendF32ToStringBuilder(&builder, value, decimalPlaces);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromF64(f64 value, i32 decimalPlaces, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendF64ToStringBuilder(&builder, value, decimalPlaces);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromU8(u8 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendU8ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromU16(u16 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendU16ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromU32(u32 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendU32ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromU64(u64 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendU64ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromI8(i8 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendI8ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromI16(i16 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendI16ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromI32(i32 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendI32ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

utf8str PNSLR_StringFromI64(i64 value, PNSLR_IntegerBase base, PNSLR_Allocator allocator)
{
    PNSLR_INTERNAL_ALLOCATOR_INIT(Strings, internalAllocator);
    PNSLR_StringBuilder builder = {.allocator = internalAllocator};
    PNSLR_AppendI64ToStringBuilder(&builder, value, base);
    utf8str result = PNSLR_CloneString(PNSLR_StringFromStringBuilder(&builder), allocator);
    PNSLR_INTERNAL_ALLOCATOR_RESET(Strings, internalAllocator);
    // no need to 'free' string builder, the internal allocator reset will take care
    return result;
}

b8 PNSLR_BooleanFromString(utf8str str, b8* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (b8) {0}; // zero by default

    if (PNSLR_AreStringsEqual(str, PNSLR_StringLiteral("true"), PNSLR_StringComparisonType_CaseInsensitive))
    {
        *value = true;
        return true;
    }
    else if (PNSLR_AreStringsEqual(str, PNSLR_StringLiteral("false"), PNSLR_StringComparisonType_CaseInsensitive))
    {
        *value = false;
        return true;
    }
    else if (str.count == 1 && *str.data == (u8) '1')
    {
        *value = true;
        return true;
    }
    else if (str.count == 1 && *str.data == (u8) '0')
    {
        *value = false;
        return true;
    }

    return false;
}

b8 PNSLR_F32FromString(utf8str str, f32* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (f32) {0}; // zero by default
    f64 tempVal = 0.0;
    if (!PNSLR_F64FromString(str, &tempVal)) return false;
    *value = (f32) tempVal;
    return true;
}

b8 PNSLR_F64FromString(utf8str str, f64* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (f64) {0}; // zero by default

    i64 i = 0;
    b8 negative = false;

    // Handle optional sign
    if (str.data[0] == '-')
    {
        negative = true;
        i = 1;
    }
    else if (str.data[0] == '+')
    {
        i = 1;
    }

    // Find decimal point (if any)
    i64 dotIndex = -1;
    for (i64 j = i; j < str.count; j++)
    {
        if (str.data[j] == '.')
        {
            if (dotIndex != -1) return false; // multiple dots
            dotIndex = j;
        }
    }

    // Integer part slice
    utf8str intStr;
    intStr.data  = str.data + i;
    intStr.count = (dotIndex == -1) ? (str.count - i) : (dotIndex - i);

    // Fractional part slice
    utf8str fracStr;
    if (dotIndex != -1)
    {
        fracStr.data  = str.data + dotIndex + 1;
        fracStr.count = str.count - (dotIndex + 1);
    }
    else
    {
        fracStr.data  = 0;
        fracStr.count = 0;
    }

    // Parse integer part
    i64 intVal = 0;
    if (intStr.count > 0)
    {
        if (!PNSLR_I64FromString(intStr, &intVal)) return false;
    }

    // Parse fractional part
    f64 fracVal = 0.0;
    if (fracStr.count > 0)
    {
        u64 fracDigits = 0;
        if (!PNSLR_U64FromString(fracStr, &fracDigits)) return false;

        f64 divisor = 1.0;
        for (i64 k = 0; k < fracStr.count; k++) divisor *= 10.0;

        fracVal = (f64)fracDigits / divisor;
    }

    f64 result = (f64)intVal + fracVal;
    if (negative) result = -result;

    *value = result;
    return true;
}

b8 PNSLR_U8FromString(utf8str str, u8* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (u8) {0}; // zero by default
    u64 tempVal = 0;
    if (!PNSLR_U64FromString(str, &tempVal)) return false;
    if (tempVal > U8_MAX) return false; // overflow
    *value = (u8) tempVal;
    return true;
}

b8 PNSLR_U16FromString(utf8str str, u16* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (u16) {0}; // zero by default
    u64 tempVal = 0;
    if (!PNSLR_U64FromString(str, &tempVal)) return false;
    if (tempVal > U16_MAX) return false; // overflow
    *value = (u16) tempVal;
    return true;
}

b8 PNSLR_U32FromString(utf8str str, u32* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (u32) {0}; // zero by default
    u64 tempVal = 0;
    if (!PNSLR_U64FromString(str, &tempVal)) return false;
    if (tempVal > U32_MAX) return false; // overflow
    *value = (u32) tempVal;
    return true;
}

b8 PNSLR_U64FromString(utf8str str, u64* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (u64) {0}; // zero by default

    i64 i = 0;
    u64 base = 10;

    // Prefix check
    if (str.count > 2 && str.data[0] == '0')
    {
        u8 p = (u8)str.data[1];
        if (p == 'b' || p == 'B') { base = 2;  i = 2; }
        else if (p == 'o' || p == 'O') { base = 8;  i = 2; }
        else if (p == 'x' || p == 'X') { base = 16; i = 2; }
    }

    // If no prefix: check if hex letters appear
    if (base == 10)
    {
        for (i64 j = 0; j < str.count; j++)
        {
            u8 c = (u8)str.data[j];
            if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
            {
                base = 16;
                break;
            }
        }
    }

    u64 result = 0;

    for (; i < str.count; i++)
    {
        u8 c = (u8)str.data[i];
        u64 digit;

        if (c >= '0' && c <= '9')       digit = (u64)(c - '0');
        else if (c >= 'A' && c <= 'F')  digit = (u64)(10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')  digit = (u64)(10 + (c - 'a'));
        else                            return false; // invalid char

        if (digit >= base) return false; // invalid digit for base

        result = result * base + digit;
    }

    *value = result;
    return true;
}

b8 PNSLR_I8FromString(utf8str str, i8* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (i8) {0}; // zero by default
    i64 tempVal = 0;
    if (!PNSLR_I64FromString(str, &tempVal)) return false;
    if (tempVal < I8_MIN || tempVal > I8_MAX) return false; // overflow
    *value = (i8) tempVal;
    return true;
}

b8 PNSLR_I16FromString(utf8str str, i16* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (i16) {0}; // zero by default
    i64 tempVal = 0;
    if (!PNSLR_I64FromString(str, &tempVal)) return false;
    if (tempVal < I16_MIN || tempVal > I16_MAX) return false; // overflow
    *value = (i16) tempVal;
    return true;
}

b8 PNSLR_I32FromString(utf8str str, i32* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (i32) {0}; // zero by default
    i64 tempVal = 0;
    if (!PNSLR_I64FromString(str, &tempVal)) return false;
    if (tempVal < I32_MIN || tempVal > I32_MAX) return false; // overflow
    *value = (i32) tempVal;
    return true;
}

b8 PNSLR_I64FromString(utf8str str, i64* value)
{
    if (!str.data || !str.count || !value) return false;
    *value = (i64) {0}; // zero by default

    i64 i = 0;
    b8 negative = false;

    // Handle optional sign
    if (str.data[0] == '-')
    {
        negative = true;
        i = 1;
    }
    else if (str.data[0] == '+')
    {
        i = 1;
    }

    // Slice after sign
    utf8str unsignedStr;
    unsignedStr.data  = str.data + i;
    unsignedStr.count = str.count - i;

    u64 uval = 0;
    if (!PNSLR_U64FromString(unsignedStr, &uval))
    {
        return false;
    }

    if (negative)
    {
        if (uval > (u64) I64_MAX + 1ULL) return false; // overflow
        *value = -(i64)uval;
    }
    else
    {
        if (uval > (u64) I64_MAX) return false; // overflow
        *value = (i64)uval;
    }

    return true;
}

