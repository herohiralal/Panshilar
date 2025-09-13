#ifndef PNSLR_TOKEN_MATCH_H
#define PNSLR_TOKEN_MATCH_H
#include "../../Source/Panshilar.h"

b8 IsSpace(u32 r);
b8 IsSymbol(u32 r);
b8 IsValidName(utf8str str);
b8 IsValidStringToken(utf8str str);
b8 IsValidNumber(utf8str str);
b8 IsValidHexNumber(utf8str str);

#endif
