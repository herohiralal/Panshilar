#ifndef PNSLR_TOKEN_MATCH_H
#define PNSLR_TOKEN_MATCH_H
#include "Panshilar.h"

b8 PNSLR_IsSpace(u32 r);
b8 PNSLR_IsSymbol(u32 r);
b8 PNSLR_IsValidName(utf8str str);
b8 PNSLR_IsValidString(utf8str str);
b8 PNSLR_IsValidNumber(utf8str str);
b8 PNSLR_IsValidHexNumber(utf8str str);

#endif
