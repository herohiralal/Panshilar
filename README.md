# Panshilar

The name means nothing, it just sounds good.

It's a C11 standard library for games with a heavy focus on minimalism, simplicity (to the point of dumbness), ease of interoperability (between different languages) and cross-platform compatibility.

## Tools

Currently the repository has two tools:
- Bindings Generator - parse the headers and generate bindings for different languages; some ideas include (in the order of priority):
  - Odin - primary target; for a personal project
  - Jai - because it's a short detour from Odin
  - C++ - because most people use that
  - C - because it's better to have a single consumable header than a whole repo to download
  - C# - because it sucks to deal with GC, which all Unity users have to
- Test Runner - run tests for the library, print results
  - However, in its current state, there aren't a lot of tests.
  - It also currently only builds/runs for the desktop platforms (Windows, OSX, Linux). Yet to add appropriate 'building' pipelines for iOS, iOS Simulator and Android.

Both meant to utilise the library and iron out its quirks.

## Style

Listing some ideas here as soft constraints or styling guide.
- No conditional compilation allowed in _most_ header files.
  - This maintains a clear, consistent and reliable API across different environments and platforms.
  - It also prevents linking hell.
  - Keeps Bindings Generator simple.
- C11 only.
  - C++ ABI is unstable and it's easier to port.
- No exceptions, no crashes. Return errors as values.
  - In some cases, feel free to return boolean result only.
- Avoid delegate passing.
  - While a lot of languages are compatible with C calling convention, it's not the default.
  - For languages like Jai/Odin, that use an implicit context system, this is even more annoying.
- No storing delegates (and by extension 'allocators') in structs or global variables.
  - Pain when it comes to hot-reloading.
- Avoid globals, but not outright banned, if they're localised properly.
  - Currently the repository has 'internal allocators' for certain modules (strings, paths, etc.), that are thread-local globals. But it should be fine, as any 'returning' function ensures that the allocators are cleaned up appropriately. They've also been designed in the way that the thread 'exiting' the thread does not need any sort of customized cleanup.
- Avoid Std C Lib dependencies.
  - This isn't a hard rule, because functionality comes first, but avoid as much as possible.

## Supported Platforms

You can find binaries for the following platforms in `Libraries/` directory:
```plaintext
- Windows (X64)   Good ol' gaming primary.
- Linux   (X64)   Up 'n comin gaming primary.
- Linux   (ARM64) Mostly for embedded.
- OSX     (ARM64) MacOS (Apple Silicon)
- OSX     (X64)   MacOS (Intel)
- Android (ARM64) Most accessible gaming hardware.
- Android (X64)   Helps with emulator/AVD compat.
- iOS     (ARM64) Most inaccessible game development platform.
- iOS Sim (ARM64)
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
- Windows (x64)   -> Android (X64)
- OSX     (ARM64) -> OSX     (ARM64)
- OSX     (ARM64) -> OSX     (X64)
- OSX     (ARM64) -> iOS     (ARM64)
- OSX     (ARM64) -> iOS Sim (ARM64)
```

The batch files (`build.bat`/`build.sh`) for both host platforms are set up, which in turn call `build.py`.

Build Depndencies:
- Python 3.
- Visual Studio Build Tools + Windows SDK
- Android NDK (something that can build API level 28, it'll pick the first one available)
- Linux cross-compilers ([get here](https://dev.epicgames.com/documentation/en-us/unreal-engine/linux-development-requirements-for-unreal-engine#version-history))
  - Thanks Epic Games!
- Xcode Build Tools
- OSX toolchain
- iPhone Simulator Toolchain
- iPhone Toolchain

If any toolchain is not found, the build scripts are set to ignore that platform when building.

Also, never switching to a build system. Unity build, with a single build command invocation is how it'll remain forever.

## How to use

The repository includes prebuilt static libraries for all platforms inside `Libraries/` directory. Link against them and you're good to go.

There's bindings (with extensive LSP-compatible API docs) available for the following languages:
- C
- C++
- Odin

## Made by HeroHiralal
