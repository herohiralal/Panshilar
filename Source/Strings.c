#include "Strings.h"

i32 PNSLR_GetStringLength(cstring str)
{
    if (str == nil) { return 0; }

    i32 length = 0;
    while (str[length] != '\0') { ++length;}
    return length;
}

utf8str PNSLR_StringFromCString(cstring str)
{
    i32 length = PNSLR_GetStringLength(str);
    return (utf8str) {.count = length, .data = (utf8ch*)str};
}
