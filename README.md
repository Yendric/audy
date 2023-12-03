# Audy

![Issues](https://img.shields.io/github/issues/Yendric/audy)

A Win32 application that enables you to modify your audio output device using a keyboard shortcut.

## Building from source

You can set the keyboard shortcut using the constants defined in [main.h](src/main.h).

1. Clone: `git clone git@github.com:Yendric/audy`
2. Run `make`
3. You can now run the executable from `/bin/audy.exe`. If you want audy to run on startup you can put it inside of the `shell:startup` folder.

## Audy

Audy is a Win32 application that enables you to modify your default audio output device using a keyboard shortcut. The default keyboard shortcut is `Shift+Alt+ArrowUp`.
Being written in C with the Win32 API, it has an incredibly small memory and CPU footprint. As such, it will have a negligible effect on your PC's performance when running in the background. The application can be controlled from the system tray.

## Todo

-   User configurable keyboard shortcut
-   Installer
-   Automated build system
-   ... please let me know what you want

## Contribute

Feel free to create an issue/PR if you have suggestions or find mistakes.
