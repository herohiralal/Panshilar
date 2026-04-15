#ifndef PNSLR_DYNA_LIB_H // ========================================================
#define PNSLR_DYNA_LIB_H
#include "__Prelude.h"
#include "IO.h"
EXTERN_C_BEGIN

/**
 * Opaque handle to a loaded dynamic library.
 */
typedef struct PNSLR_DynamicLibrary
{
    rawptr handle;
} PNSLR_DynamicLibrary;

/**
 * Gets tthe handle to a dynamic library by its path.
 * By default, it'll load the library, unless noLoad argument is passed as true.
 * In that case, the library won't be loaded if it isn't already loaded.
 *
 * Returns zero-value on failure (invalid path, failed to load, etc.).
 * Returns zero-value if noLoad is passed as true, but the library is not loaded
 * already.
 */
PNSLR_DynamicLibrary PNSLR_GetDynamicLibrary(PNSLR_Path path, b8 noLoad OPT_ARG);

/**
 * Retrieves a function pointer from a loaded dynamic library by name.
 * Returns nil if the library handle is nil, or if the string is empty.
 * Returns nil if the symbol is not found.
 */
rawptr PNSLR_GetDynamicLibraryFunction(PNSLR_DynamicLibrary lib, utf8str name);

/**
 * Unloads a dynamic library and frees associated resources.
 */
void PNSLR_UnloadDynamicLibrary(PNSLR_DynamicLibrary lib);

EXTERN_C_END
#endif // PNSLR_DYNA_LIB_H =========================================================
