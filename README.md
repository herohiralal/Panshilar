# Panshilar

The name means nothing, it just sounds good.

It's a C11 standard library for games with a heavy focus on minimalism, readability and cross-platform compatibility.

One of the stretch goals is to also support _multiple languages_ and provide a common base so that different libraries (in different languages) that use Panshilar, can interoperate a bit easier.

For now, even the idea of it, only revolves around:
- A common UTF-8 string type.
- Conversions to UTF-16 as required.
- Common internally-compatible allocators.
- Common slice and array types.

## Tools

Currently the repository has two tools (neither of them even at their first version):
- Test Runner - run tests for the library, print results
- Bindings Generator - parse the headers and generate bindings for different languages; some ideas include (in the order of priority):
  - Jai - primary target; for a personal project
  - Odin - because it's a short detour from Jai
  - C++ - because most people use that
    - currently hard-disabled from using via a static assert
    - the idea is to build better APIs that are more suited for C++
  - C# - because it sucks to deal with GC, which all Unity users have to

Both meant to utilise the library and iron out its quirks.

## Style

Listing some ideas here as soft constraints or styling guide.
- No conditional compilation allowed in _most_ header files.
  - This maintains a clear, consistent and reliable API across different environments and platforms.
  - This would also help with reflection-based code generation for different languages.
- C11 only.
  - C++ ABI is unstable and it's easier to port.
- No exceptions, no crashes. Return errors as values.
- Avoid delegate passing.
  - While a lot of languages are compatible with C calling convention, it's not the default.
  - For languages like Jai/Odin, that use an implicit context system, this is even more annoying.
- No storing delegates (and by extension 'allocators') in structs or global variables.
  - Pain when it comes to hot-reloading.
- Avoid globals, but not outright banned, if they're localised properly.
- Avoid UCRT dependencies.
  - This isn't a hard rule, because functionality comes first, but avoid as much as possible.

## Supported Platforms

You can find binaries for the following platforms in `Libraries/` directory:
```plaintext
- Windows (X64)   - primary platform
- Linux   (X64)
- Linux   (ARM64) - mostly for embedded; might drop it
- OSX     (ARM64)
- Android (ARM64)
- iOS     (ARM64)
```

However, this library is in very initial stages and there might not be a lot of feature parity around different platforms.

As a library centered around games, Windows will probably be getting the most attention.

There are also some placeholders for console platforms. I will, at a later date figure out how to build for it and share the static libraries to the licensees.

## How to Build

Currently it supposrts two hosts that can build different subsets of the targets:
```plaintext
- Windows (x64)   -> Windows (x64)
- Windows (x64)   -> Linux   (x64)
- Windows (x64)   -> Linux   (ARM64)
- Windows (x64)   -> Android (ARM64)
- OSX     (ARM64) -> OSX     (ARM64)
- OSX     (ARM64) -> iOS     (ARM64)
```

The batch files (`*.bat`/`*.sh`) for both are set up, although at the time of writing this, the OSX shell file is untested (never been run at all).
Hate Apple platforms for being closed. Love their tooling sometimes though.

Windows host will require running `setup.bat` once. This will set up toolchains, which by the way will need to be downloaded:
- Windows Build Tools + Windows Development Kit
- Android NDK (something that can build API level 28, it'll pick the first one available)
- Linux cross-compilers ([get here](https://dev.epicgames.com/documentation/en-us/unreal-engine/cross-compiling-for-linux?application_version=4.27))
  - Thanks Epic Games!

If any toolchain is not found, the batch files are set to ignore that platform when building.

Also, never switching to a build system. Unity build, with a batch file command invocation is how it'll remain forever.

## How to use

Idk why anyone would want to at this stage, but check out how `TestRunner
