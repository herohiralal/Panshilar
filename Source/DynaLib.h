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
 * Loads a dynamic library from the given path.
 * Returns zero-value on failure.
 */
PNSLR_DynamicLibrary PNSLR_LoadDynamicLibrary(PNSLR_Path path);

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
