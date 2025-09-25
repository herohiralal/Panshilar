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
