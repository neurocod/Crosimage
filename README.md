# Crosimage

## Building on Windows

Install Python 3, Visual Studio with the C++ desktop workload, and Qt VS Tools.
The compiler toolset and Qt version come from `Crosimage/Crosimage.vcxproj`.
Run from a regular terminal:

```powershell
python build.py                         # incremental Debug|x64 build
python build.py --release               # Release|x64
python build.py --rebuild               # clean and rebuild
python build.py --errors                # only errors and the result
python build.py --quiet                 # log only; check the exit code
python build.py --qt C:\Qt\6.11.2\msvc2022_64
python build.py --dry-run               # show the resolved command
python build.py --help
```

MSBuild is discovered through Visual Studio's `vswhere`, with a PATH fallback.
Qt/MSBuild is resolved from `--qt-msbuild`, the `QtMsBuild` environment variable,
the project's `QtMsBuild` directory, or `%LOCALAPPDATA%\QtMsBuild`.
`--qt` (or `QT_PREFIX`) overrides the Qt installation for this build only;
otherwise the project's Qt VS Tools settings are used.
The full UTF-8 build output goes to `build.log` beside the script. Use `--log PATH`
to change it; relative paths are resolved from the caller's current directory.
The script can be invoked from any directory and returns MSBuild's exit code.

## Overview

Cross-platform C++/Qt image viewer with some GUI improvements, which I missed in other programs like XnView. For example, intellectual folder thumbnails, three-picture slideshows etc. Also aimed to big speed when working with big folders.

Main window with thumbnails view. You can see how thumbnails are placed close to each other to fit maximum visual information on the screen, up to 6 thumbnails per folder.
![09](https://user-images.githubusercontent.com/2953741/39309260-3e57fb2a-4970-11e8-857e-afed6d4b3e77.png)

Fullscreen view. Main image is on the center, and free space at the sides is used to show next and previous images in the folder. Of course this can be switched off.
![13slideshow](https://user-images.githubusercontent.com/2953741/39309261-3e756688-4970-11e8-81fc-2251ce56a091.png)

When there are many top-level windows and you want to locate window with proper folder, press `Ctrl+tab` to show `Window manager` - with preview of each window.
![15manager](https://user-images.githubusercontent.com/2953741/39309259-3e3a613c-4970-11e8-9f77-faa742a8efc6.png)

It automatically adjusts for any windows count, so in situations like 5+ windows it is very useful.
![14manager](https://user-images.githubusercontent.com/2953741/39309263-3e987e2a-4970-11e8-934a-d4391e5f3ad7.png)
