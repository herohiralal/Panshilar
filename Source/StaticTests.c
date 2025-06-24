/**
 * This file is supposed to contain some static tests for the project.
 * These are meant to be very basic that check some of the assumptions made in the codebase.
 * Good idea to avoid functions here, or at least keep them very simple.
 * Important to also prefix the declarations with `PNSLR_StaticTests_` to avoid conflicts with other code.
 * These tests will be included the last in the compilation process,
 * so they can be used to verify that the code is working as expected.
 */

#include "__Prelude.h"

// auto alignment check
typedef struct { i32 a; i32 b; } PNSLR_StaticTests_AutoAlignCheck;
static_assert(offsetof(PNSLR_StaticTests_AutoAlignCheck, a) == 0, "Offset of 'a' should be 0");
static_assert(offsetof(PNSLR_StaticTests_AutoAlignCheck, b) == 4, "Offset of 'b' should be 4");

// manual alignment check
typedef struct alignas(16) { i32 a; i32 b; } PNSLR_StaticTests_ManualAlignCheck;
static_assert(alignof(PNSLR_StaticTests_ManualAlignCheck) == 16, "Custom alignment should be 16 bytes");
static_assert(alignof(PNSLR_StaticTests_ManualAlignCheck) == 16, "Custom alignment should be 16 bytes");

// ensure all compiler macros are defined, and exactly one is set
static_assert((PNSLR_CLANG + PNSLR_GCC + PNSLR_MSVC) == 1, "Exactly one compiler must be defined.");

// ensure all platform macros are defined, and exactly one is set
static_assert((PNSLR_WINDOWS + PNSLR_LINUX + PNSLR_OSX + PNSLR_ANDROID + PNSLR_IOS + PNSLR_PS5 + PNSLR_XSERIES + PNSLR_SWITCH) == 1, "Exactly one platform must be defined.");

// ensure all architecture macros are defined, and exactly one is set
static_assert((PNSLR_X64 + PNSLR_ARM64) == 1, "Exactly one architecture must be defined.");

// primitive type size assertions
static_assert(sizeof(b8)     == 1, " b8 must be 1 byte ");
static_assert(sizeof(u8)     == 1, " u8 must be 1 byte ");
static_assert(sizeof(i8)     == 1, " i8 must be 1 byte ");
static_assert(sizeof(u16)    == 2, "u16 must be 2 bytes");
static_assert(sizeof(i16)    == 2, "i16 must be 2 bytes");
static_assert(sizeof(b32)    == 4, "b32 must be 4 bytes");
static_assert(sizeof(u32)    == 4, "u32 must be 4 bytes");
static_assert(sizeof(i32)    == 4, "i32 must be 4 bytes");
static_assert(sizeof(f32)    == 4, "f32 must be 4 bytes");
static_assert(sizeof(u64)    == 8, "u64 must be 8 bytes");
static_assert(sizeof(i64)    == 8, "i64 must be 8 bytes");
static_assert(sizeof(f64)    == 8, "f64 must be 8 bytes");
static_assert(sizeof(rawptr) == 8, "ptr must be 8 bytes");
static_assert(PNSLR_PTR_SIZE == 8, "ptr must be 8 bytes"); // keep in sync with sizeof(rawptr)

// assert msvc toolchain for windows
// will not be supporting MinGW or anything else because so much Windows-specific
// stuff relies very directly on MSVC toolchain features
// eventually also want to add some kind of rendering etc. in a future library
// for this to link with that, MSVC is the only option
#if PNSLR_WINDOWS != defined(_MSC_VER)
    #error "MSVC toolchain is required for Windows platform."
#endif
