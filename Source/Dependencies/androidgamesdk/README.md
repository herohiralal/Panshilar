# Android Game SDK Source Inclusion

Primarily including this so I don't have to fucking deal with the horrible gradle setup and can work on stuff OUTSIDE having to deal with Android Studio.

## Where to Find?

[Here!](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/b2f7b2462b6969c79f6d6999e3e62816436b3585)

## How to Reimport?

- Select a specific commit (in this case, game-activity & game-text-input 3.0.5).
- Include all the relevant `*.h`/`*.hpp`/`*.c`/`*.cpp` files into the project.
  - will need to go through gamesdk, gametextinput and game-activity separately
  - something else as well, if you need
- Fix up any 'include' errors.
- Fix up any C11 incompatibilities, at least in headers

## C11 Compatibility

Following headers are c11 compatible:
- `android_native_app_glue.h`
- `ChoreographerShim.h`
- `GameActivity.h` // by itself, is compatible, but had to remove `enum` specifiers from a specific function (because of `gametextinput.h` changes)
- `GameActivityEvents.h`
- `gamecommon.h`
- `gamesdk_common.h`
- `gametextinput.h` // had to convert some enums to typedefs + macros

Following headers are NOT c11 compatible:
- `apk_utils.h`
- `jni_helper.h`
- `jni_wrap.h`
- `jnictx.h`
- `JNIUtil.h`
- `Log.h`
- `StringShim.h`
- `system_utils.h`
- `Trace.h`

> Regardless of whatever language you use, will still need C++ compiler because there are C++ source files. :'(
