# Crosimage

## Building with CMake

The build uses CMake 3.21 or newer, a C++17 compiler, and Qt 6 with the Core,
Gui, Widgets, Sql, Svg, and Xml modules. On Windows, install Visual Studio with
Desktop development with C++ and the matching MSVC Qt kit. Visual Studio 2026
requires CMake 4.2 or newer. Qt VS Tools and Qt/MSBuild are no longer required.

### Windows build helper

With Python 3 installed, run from a regular PowerShell terminal:

```powershell
python build.py                         # configure and build Debug|x64
python build.py --release               # Release|x64
python build.py --rebuild               # clean and rebuild
python build.py --errors                # only errors and the result
python build.py --quiet                 # log only; check the exit code
python build.py --qt C:\Qt\6.11.2\msvc2022_64
python build.py --dry-run               # show configure/build commands
python build.py --help
```

The helper discovers CMake on PATH or in Visual Studio. Select Qt with `--qt`
or `QT_PREFIX`; an existing CMake cache or `CMAKE_PREFIX_PATH` is also respected.
For a new build without these settings, the helper looks for an installed MSVC
Qt 6 kit under `C:\Qt`. Use a matching 32-bit Qt kit with `--platform Win32`.

Build files go to `build/x64` (or `build/Win32`); use `--build-dir PATH` to
change this. The executable is `build/x64/bin/Debug/Crosimage.exe` or
`build/x64/bin/Release/Crosimage.exe`. On Windows, the build runs
[windeployqt](https://doc.qt.io/qt-6/windows-deployment.html) to copy Qt DLLs
and plugins beside the executable, including image formats and SQL drivers.
The compiler runtime must already be installed (as it is on the build machine).
To manage Qt runtime deployment yourself, configure with
`-DCROSIMAGE_DEPLOY_QT=OFF`.

The full UTF-8 configure/build output goes to `build.log` beside the script.
Use `--log PATH` to change it. Relative option paths are resolved from the
caller's current directory; the script can be invoked from any directory.
The exit code is the failing CMake command's exit code, or zero on success.
The old `--msbuild` and `--qt-msbuild` overrides have been replaced by `--cmake`
and standard CMake generator selection (`--generator`).

### Direct CMake commands and IDEs

```powershell
cmake -S . -B build/x64 -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.2/msvc2022_64
cmake --build build/x64 --config Debug --parallel
cmake --build build/x64 --config Release --parallel
```

CMake selects an installed Visual Studio by default on Windows. Add, for
example, `-G "Visual Studio 17 2022"` to choose a specific generator. Open the
solution generated in `build/x64`, or open the repository folder directly in
Visual Studio / Qt Creator and configure the Qt prefix in the IDE. Generated
solutions and projects are build outputs and are not committed.

The Visual Studio filters are preserved in `Crosimage/CMakeLists.txt` using
`source_group`: `src`, `src/settings`, `Thumb`, `ImageView`, `AltTab`, `Qqt`,
`Qqt/DB`, and `Resource Files`. A `Solution Items` target exposes the root
project documents. Add new files to the relevant `crosimage_sources` group;
CMake handles moc, resources, and the precompiled header automatically.

For a single-configuration generator such as Ninja, omit `-A` and select the
configuration at configure time:

```sh
cmake -S . -B build/ninja -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/compiler_kit
cmake --build build/ninja --parallel
```

On Windows, Ninja needs a Visual Studio developer shell. The migration has
been verified with MSVC on Windows; other compiler/platform combinations
may require changes to the existing C++ sources.

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
