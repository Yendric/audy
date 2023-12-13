# Audy

![Issues](https://img.shields.io/github/issues/Yendric/audy)

A Win32 application that enables you to modify your audio output device using a keyboard shortcut.

## Audy

Audy is a Win32 application that enables you to modify your default audio output device using a keyboard shortcut. The default keyboard shortcut is `Shift+Alt+ArrowUp`.
Being written in C with the Win32 API, it has an incredibly small memory and CPU footprint. As such, it will have a negligible effect on your PC's performance when running in the background. The application can be controlled from the system tray.

## Building from source

You can set the keyboard shortcut using the constants defined in [main.h](src/main.h).

### Linux

While the application only runs on Windows, it can be built on Linux using mingw-w64. The makefile is configured to do this automatically.
This is useful if you're used to developing in a WSL environment, for example.

1. Clone: `git clone git@github.com:Yendric/audy`
2. Run `make`
3. You can now run the executable from `/bin/audy.exe`. If you want audy to run on startup you can put it inside of the `shell:startup` folder.

### Windows

On Windows the application can be built using MSBuild.

1. Clone: `git clone git@github.com:Yendric/audy`
2. Open the folder in a VS tools command prompt (or something else that has access to msbuild.exe)
3. Run `msbuild /property:Configuration=Release`
4. The executable can be found in the `bin` folder. If you want audy to run on startup you can put it inside of the `shell:startup` folder.

## Todo

- User configurable keyboard shortcut
- Installer
- Versioning system
- ... please let me know what you want

## Contribute

Feel free to create an issue/PR if you have suggestions or find mistakes.
