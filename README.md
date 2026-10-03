# Audy

![Issues](https://img.shields.io/github/issues/Yendric/audy)

Audy is a Win32 application that enables you to modify your default audio output device using a keyboard shortcut. The default shortcut is `Shift+Alt+ArrowUp`, which can be changed from the tray icon's settings.
Being written in C with the Win32 API, it has a negligible effect on your PC's performance when running in the background.

## Building from source

Audy is built with CMake. From a Visual Studio developer prompt:

1. Clone: `git clone git@github.com:Yendric/audy`
2. Run `cmake -B build` and `cmake --build build --config Release`
3. The executable can be found at `build/Release/audy.exe`. If you want Audy to run on startup you can put it inside of the `shell:startup` folder.

It can also be cross-compiled from Linux with mingw-w64:

```sh
cmake -B build -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
cmake --build build
```

## Todo

- Installer
- Versioning system
- ... please let me know what you want

## Contribute

Feel free to create an issue/PR if you have suggestions or find mistakes.
