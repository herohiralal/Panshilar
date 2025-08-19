#include "TokenMatch.h"

b8 PNSLR_IsSpace(u32 r)
{
    if (r <= 0xFF)
    {
        switch (r)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\f':
            case '\r':
            case 0x85:   // Next Line
            case 0xA0:   // No-Break Space
                return true;
            default:
                return false;
        }
    }

    // High-valued ones
    if (0x2000 <= r && r <= 0x200A) { return true; }

    switch (r)
    {
        case 0x1680:  // Ogham Space Mark
        case 0x2028:  // Line Separator
        case 0x2029:  // Paragraph Separator
        case 0x202F:  // Narrow No-Break Space
        case 0x205F:  // Medium Mathematical Space
        case 0x3000:  // Ideographic Space
            return true;
        default:
            return false;
    }
}

b8 PNSLR_IsSymbol(u32 r)
{
    if (r <= 0xFF)
    {
        switch (r)
        {
            case '!':
            case '"':
            case '#':
            case '$':
            case '%':
            case '&':
            case '\'':
            case '(':
            case ')':
            case '*':
            case '+':
            case ',':
            case '-':
            case '.':
            case '/':
            case ':':
            case ';':
            case '<':
            case '=':
            case '>':
            case '?':
            case '@':
            case '[':
            case '\\':
            case ']':
            case '^':
            case '`':
            case '{':
            case '|':
            case '}':
            case '~':
                return true;
            default:
                return false;
        }
    }

    return false;
}

b8 PNSLR_IsValidName(utf8str str)
{
    if (str.count == 0) { return false; }

    i64 pos = 0;
    i32 charIdx = 0;

    while (pos < str.count)
    {
        PNSLR_DecodedRune decoded = PNSLR_DecodeRune((ArraySlice(u8)){.count = str.count - pos, .data = str.data + pos});
        if (decoded.length <= 0) { return false; }

        u32 r = decoded.rune;
        b8 allowNumeric = (charIdx != 0);
        b8 isNumeric = (r >= '0' && r <= '9');
        b8 isAlphabetical = (r >= 'a' && r <= 'z') || (r >= 'A' && r <= 'Z');
        b8 isUnderscore = (r == '_');

        if (!allowNumeric && isNumeric) { return false; }

        if (!(isAlphabetical || isUnderscore || isNumeric)) { return false; }

        pos += decoded.length;
        charIdx++;
    }

    return true;
}

b8 PNSLR_IsValidString(utf8str str)
{
    if (str.count < 2) { return false; }

    if (str.data[0] != '"' || str.data[str.count - 1] != '"') { return false; }

    return true;
}

b8 PNSLR_IsValidNumber(utf8str str)
{
    if (str.count == 0) { return false; }

    b8 hasDash = false;
    b8 hasDot = false;
    b8 hasAtLeastOneDigitBeforeDot = false;
    b8 hasAtLeastOneDigitAfterDot = false;

    i64 pos = 0;
    i32 charIdx = 0;

    while (pos < str.count)
    {
        PNSLR_DecodedRune decoded = PNSLR_DecodeRune((ArraySlice(u8)){.count = str.count - pos, .data = str.data + pos});
        if (decoded.length <= 0) { return false; }

        u32 r = decoded.rune;
        b8 isDash = (r == '-');
        b8 isNumeric = (r >= '0' && r <= '9');
        b8 isDot = (r == '.');

        b8 allowDash = !hasDash && (charIdx == 0);

        if (isDash)
        {
            if (!allowDash) { return false; }
            hasDash = true;
            pos += decoded.length;
            charIdx++;
            continue;
        }

        b8 allowDot = !hasDot && (hasDash ? (charIdx != 1) : (charIdx != 0));

        if (isDot)
        {
            if (!allowDot) { return false; }
            hasDot = true;
            pos += decoded.length;
            charIdx++;
            continue;
        }

        if (!isNumeric) { return false; }

        if (hasDot) { hasAtLeastOneDigitAfterDot = true; }
        else { hasAtLeastOneDigitBeforeDot = true; }

        pos += decoded.length;
        charIdx++;
    }

    return hasAtLeastOneDigitBeforeDot && (!hasDot || hasAtLeastOneDigitAfterDot);
}

b8 PNSLR_IsValidHexNumber(utf8str str)
{
    i64 pos = 0;
    while (pos < str.count)
    {
        PNSLR_DecodedRune decoded = PNSLR_DecodeRune((ArraySlice(u8)){.count = str.count - pos, .data = str.data + pos});
        if (decoded.length <= 0) { return false; }

        u32 r = decoded.rune;
        b8 isNumeric = (r >= '0' && r <= '9');
        b8 isAlpha = (r >= 'A' && r <= 'F');

        if (!isNumeric && !isAlpha) { return false; }

        pos += decoded.length;
    }

    return true;
}
