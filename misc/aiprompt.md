# Rules

- c11 code only
- `PascalCase` for structs/functions/enums/globals
- `ALL_CAPS` for macro constants
- `camelCase` for variables (local/member/arguments)
- `PNSLR_` prefix for functions/structs/enums
- `PNSLR_Internal_` prefix for any internal (source file) functions/structs/enums
  - they must also be static
- `G_PNSLR_Internal_` prefix for globals; don't declare them in headers; keep them static
- use `thread_local` after `static` and before type-name to make a thread-local var
- don't use macros
- curly braces on new lines everywhere
- b8/u8/i8/u16/i16/u32/i32/u64/i64 primitives available (use instead of c usuals)
- `PNSLR_ArraySlice(TYPE)` to use an array (has { TYPE* data; i64* count; })
- `PNSLR_DECLARE_ARRAY_SLICE(TYPE);` needed in a header once, if `PNSLR_ArraySlice(TYPE)` is to be used
- `utf8str` is `PNSLR_ArraySlice(u8)` (typedef'd) and is a UTF-8 string (with its length stored; not necessarily null-terminated / might actually include null terminators in it)
  - `PNSLR_StringLiteral("My String")` to create a `utf8str` from a string literal
- `OPT_ARG` after a function argument means it can be passed as NULL/0
  - use as `..., MyType* arg1 OPT_ARG, ...`
- `true`/`false` already defined
- `nil` to be used instead of `NULL`/`nullptr`, already defined
- `void*`/`char*` typedef'd as `rawptr`/`cstring` and those are to be used instead of originals
- `EXTERN_C_START`/`EXTERN_C_END` for C++ compatibility
- Enum declaration looks like this:
```
ENUM_START(PNSLR_ExampleEnum, u8 /* /u16/u32/u64/i8/i16/i32/i64 */)
    #define PNSLR_ExampleEnum_ValueA ((PNSLR_ExampleEnum) 0)
    #define PNSLR_ExampleEnum_ValueB ((PNSLR_ExampleEnum) 1)
    #define PNSLR_ExampleEnum_ValueC ((PNSLR_ExampleEnum) 2)
ENUM_END
```
- Enum flags declaration looks like this:
```
ENUM_FLAGS_START(PNSLR_ExampleEnumFlag /* singular name */, u8 /* /u16/u32/u64 - unsigned only */)
    #define PNSLR_ExampleEnumFlag_None   ((PNSLR_ExampleEnumFlag) (        0)) // needs the extra brackets
    #define PNSLR_ExampleEnumFlag_FlagA  ((PNSLR_ExampleEnumFlag) (1ULL << 0)) // needs 1ULL and not just 1
    #define PNSLR_ExampleEnumFlag_FlagB  ((PNSLR_ExampleEnumFlag) (1ULL << 1))
    #define PNSLR_ExampleEnumFlag_FlagC  ((PNSLR_ExampleEnumFlag) (1ULL << 2))
ENUM_FLAGS_END
```
- Struct declaration looks like this:
```
typedef struct PNSLR_ExampleStruct
{
    i32     fieldA; // tabular formatting like this preferred
    utf8str fieldB;
} PNSLR_ExampleStruct;
```
- for pointer declarations, do `type* x` and not `type *x`
- no `#include`s of standard library or other libraries in headers or source files
  - they're handled separately in a private includes file, that automatically gets included
  - if any `include`s are to be added, list them separately and i'll add them where needed
- use `PNSLR_WINDOWS`/`PNSLR_LINUX`/`PNSLR_OSX`/`PNSLR_ANDROID`/`PNSLR_IOS`/`PNSLR_PS5`/`PNSLR_XSERIES`/`PNSLR_SWITCH` for platform-specific code
  - `PNSLR_CONSOLE`/`PNSLR_UNIX`/`PNSLR_MOBILE`/`PNSLR_DESKTOP`/`PNSLR_APPLE` also available
  - `PNSLR_UNIX` includes linux/osx/android/ios
- use `PNSLR_X64`/`PNSLR_ARM64` for architecture-specific code
  - 32-bit stuff not supported
- use `PNSLR_MSVC`/`PNSLR_CLANG`/`PNSLR_GCC` for compiler-specific code
- use `PNSLR_DBG`/`PNSLR_REL` for debug/release specific code
