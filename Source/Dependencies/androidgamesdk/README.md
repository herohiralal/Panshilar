# Android Game SDK Source Inclusion

Primarily including this so I don't have to fucking deal with the horrible gradle setup and can work on stuff OUTSIDE having to deal with Android Studio.

## Where to Find?

[Here!](https://android.googlesource.com/platform/frameworks/opt/gamesdk/+/9af0c7683febf43997ff1bafcf29722c2c0c086c)

## How to Reimport?

- Select a specific commit (in this case, game-activity & game-text-input 4.0.0 stable).
- Include all the relevant `*.h`/`*.hpp`/`*.c`/`*.cpp` files into the project.
  - will need to go through gamesdk, gametextinput and game-activity separately
  - something else as well, if you need
- Fix up any 'include' errors.
