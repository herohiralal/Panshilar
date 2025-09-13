#define PNSLR_IMPLEMENTATION
#include "Panshilar.h"

#ifdef RADDBG_MARKUP_H

raddbg_type_view(b8, bool($));
raddbg_type_view(PNSLR_B8, bool($));
raddbg_type_view(ArraySlice_?, array(data, count));
raddbg_type_view(utf8str, array(data, count));
raddbg_type_view(PNSLR_UTF8STR, array(data, count));
raddbg_type_view(PNSLR_ArraySlice_?, array(data, count));

#endif
