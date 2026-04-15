#define PNSLR_IMPLEMENTATION
#include "DynaLib.h"

PNSLR_CREATE_INTERNAL_ARENA_ALLOCATOR(DynaLib, 2);

PNSLR_DynamicLibrary PNSLR_GetDynamicLibrary(PNSLR_Path path, b8 noLoad)
{
    if (!PNSLR_PathExists(path, PNSLR_PathExistsCheckType_File))
        return (PNSLR_DynamicLibrary) {0};

    PNSLR_INTERNAL_ALLOCATOR_INIT(DynaLib, internalAllocator);

    cstring pathCStr = PNSLR_CStringFromString(path.path, internalAllocator);

    rawptr handle = nil;

#if PNSLR_WINDOWS

    if (noLoad) handle = (rawptr) GetModuleHandle(pathCStr);
    else        handle = (rawptr) LoadLibraryA(pathCStr);

    if (handle == nil)
        return (PNSLR_DynamicLibrary) {0};

#elif PNSLR_UNIX

    handle = dlopen(pathCStr, RTLD_NOW | RTLD_LOCAL | (noLoad ? RTLD_NOLOAD : 0));
    if (handle == nil)
        return (PNSLR_DynamicLibrary) {0};

#else
    #error "PNSLR_DynLib: unsupported platform"
#endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(DynaLib, internalAllocator);

    return (PNSLR_DynamicLibrary) {.handle = handle};
}

rawptr PNSLR_GetDynamicLibraryFunction(PNSLR_DynamicLibrary lib, utf8str name)
{
    if (!name.data || !name.count || !lib.handle)
        return nil;

    PNSLR_INTERNAL_ALLOCATOR_INIT(DynaLib, internalAllocator);

    cstring nameCStr = PNSLR_CStringFromString(name, internalAllocator);

    rawptr handle = nil;
#if PNSLR_WINDOWS
    handle = (rawptr) GetProcAddress((HMODULE) lib.handle, nameCStr);
#elif PNSLR_UNIX
    handle = dlsym(lib.handle, nameCStr);
#else
    #error "unsupported platform"
#endif

    PNSLR_INTERNAL_ALLOCATOR_RESET(DynaLib, internalAllocator);

    return handle;
}

void PNSLR_UnloadDynamicLibrary(PNSLR_DynamicLibrary lib)
{
    if (!lib.handle)
        return;

#if PNSLR_WINDOWS
    FreeLibrary((HMODULE) lib.handle);
#elif PNSLR_UNIX
    dlclose(lib.handle);
#else
    #error "unsupported platform"
#endif
}
