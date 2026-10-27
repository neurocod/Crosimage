# Crosimage

Crosimage is a C++/Qt image viewer with folder thumbnails,
slideshows, and window switching with previews.
See [README.md](README.md) for features, screenshots, and detailed build setup.

- Source code and Qt resources are in `Crosimage/`; helper classes are in `Crosimage/Qqt/`.
- The Windows build uses `Crosimage.sln` and `Crosimage/Crosimage.vcxproj`.
  The toolset and Qt versions are configured in `.vcxproj`.
- To build from a regular PowerShell session, use `python build.py`. By default,
  it performs an incremental `Debug|x64` build. The script automatically locates
  MSBuild and Qt/MSBuild; no third-party Python packages are required.
- `--release` selects Release, `--rebuild` performs a full rebuild,
  `--errors` shows only errors and the build result, and `--quiet` suppresses output.
  The full UTF-8 log is saved to `build.log`.
- `python build.py --help` lists all options; `--dry-run` prints the command
  without building. See README for details on overriding paths.
- After C++ changes, verify the build with `python build.py` and inspect
  `build.log` if errors occur. A successful build does not replace checking GUI behavior.
- Edit source files, not generated files in `GeneratedFiles/` or build directories.
- Before every nontrivial commit, run a subagent to review the changes.
  Wait for the review results and resolve any identified issues before committing.
- After major commits, consider running a separate subagent to identify refactoring
  opportunities, both within files (such as extracting duplicated code into a function)
  and across modules (such as changing data hierarchies or moving functions between modules).
