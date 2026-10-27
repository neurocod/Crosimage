"""Configure and build Crosimage with CMake (Debug, x64 on Windows by default).

No third-party Python packages or Qt VS Tools are needed. The full configure
and build output is saved as UTF-8 in build.log. Run --help for all options.
"""

import argparse
import locale
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parent
DEFAULT_LOG = ROOT / "build.log"


def nonblank(value):
    if not value.strip():
        raise argparse.ArgumentTypeError("value must not be empty or blank")
    return value


def positive_int(value):
    try:
        number = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError("expected a positive integer") from None
    if number < 1:
        raise argparse.ArgumentTypeError("expected a positive integer")
    return number


def parse_args(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, allow_abbrev=False)
    parser.add_argument("--release", action="store_true", help="build Release (default: Debug)")
    parser.add_argument("--rebuild", action="store_true", help="clean before building")
    parser.add_argument("--platform", choices=("x64", "Win32"), default="x64",
                        help="Visual Studio architecture (default: x64)")
    parser.add_argument("--project", type=str.strip, choices=("", "ALL_BUILD", "Crosimage"),
                        default="", help="build Crosimage only; empty or ALL_BUILD builds all targets")
    parser.add_argument("--jobs", type=positive_int, metavar="N",
                        help="maximum parallel build jobs (default: available CPUs)")
    parser.add_argument("--errors", action="store_true", help="show errors only, plus the build result")
    parser.add_argument("--quiet", action="store_true", help="suppress output; still write the full log")
    parser.add_argument("--log", type=nonblank, default=str(DEFAULT_LOG), metavar="PATH",
                        help="UTF-8 build log (default: build.log beside this script)")
    parser.add_argument("--cmake", type=nonblank, metavar="PATH",
                        help="CMake executable override (default: PATH, then Visual Studio)")
    parser.add_argument("--build-dir", type=nonblank, metavar="PATH",
                        help="build directory (default: build/<platform> beside this script)")
    parser.add_argument("--generator", type=nonblank, metavar="NAME",
                        help="CMake generator (default: cached, CMAKE_GENERATOR, or CMake default)")
    parser.add_argument("--qt", type=nonblank, metavar="PATH",
                        help="Qt 6 prefix (default: QT_PREFIX, CMake settings, then C:/Qt MSVC kits)")
    parser.add_argument("--dry-run", action="store_true",
                        help="print configure/build commands without building or writing a log")
    return parser.parse_args(argv)


def find_cmake(override=None):
    found = shutil.which(override or "cmake")
    if found:
        return Path(found).resolve()
    if override:
        raise RuntimeError(f"CMake executable not found: {override}")
    installer = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere = installer / "Microsoft Visual Studio/Installer/vswhere.exe"
    if os.name == "nt" and vswhere.is_file():
        result = subprocess.run(
            [str(vswhere), "-latest", "-prerelease", "-products", "*",
             "-requires", "Microsoft.VisualStudio.Component.VC.CMake.Project",
             "-property", "installationPath", "-utf8"],
            capture_output=True, text=True, encoding="utf-8", errors="replace", check=True,
        )
        installation = result.stdout.strip()
        if installation:
            candidate = Path(installation) / "Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
            if candidate.is_file():
                return candidate
    raise RuntimeError("CMake not found. Install CMake or Visual Studio's C++ CMake tools, or use --cmake PATH.")


def read_cache(build_dir):
    cache = build_dir / "CMakeCache.txt"
    values = {}
    if cache.is_file():
        for line in cache.read_text(encoding="utf-8", errors="replace").splitlines():
            if not line.startswith(("#", "//")) and ":" in line and "=" in line:
                name, value = line.split("=", 1)
                values[name.split(":", 1)[0]] = value
    return values


def find_qt(args, cache, generator):
    configured = args.qt or os.environ.get("QT_PREFIX", "").strip()
    if configured:
        prefix = Path(configured).expanduser().resolve()
        if not (prefix / "lib/cmake/Qt6/Qt6Config.cmake").is_file():
            raise RuntimeError(f"Qt prefix must contain lib/cmake/Qt6/Qt6Config.cmake: {prefix}")
        return prefix
    # Let CMake retain its selected Qt and respect user-provided search paths.
    if any((cache.get(name) and not cache[name].endswith("-NOTFOUND"))
           or os.environ.get(name) for name in ("Qt6_DIR", "Qt6_ROOT", "CMAKE_PREFIX_PATH")):
        return None
    if os.name == "nt" and (not generator or generator.startswith("Visual Studio")):
        kit = "msvc*_64" if args.platform == "x64" else "msvc*"
        candidates = [path for path in Path("C:/Qt").glob(f"6.*/{kit}")
                      if (path / "lib/cmake/Qt6/Qt6Config.cmake").is_file()
                      and (args.platform == "x64" or not path.name.endswith(("_64", "_arm64")))]
        if candidates:
            return max(candidates, key=lambda path: (tuple(map(int, re.findall(r"\d+", path.parent.name))), path.name))
    return None


def build_commands(args):
    cmake = str(find_cmake(args.cmake))
    build_dir = Path(args.build_dir).expanduser().resolve() if args.build_dir else ROOT / "build" / args.platform
    if build_dir == ROOT:
        raise RuntimeError("Use a separate build directory; in-source builds are not supported.")
    cache = read_cache(build_dir)
    config = "Release" if args.release else "Debug"
    generator = args.generator or cache.get("CMAKE_GENERATOR") or os.environ.get("CMAKE_GENERATOR", "")
    configure = [cmake, "-S", str(ROOT), "-B", str(build_dir)]
    multi_config = (bool(cache.get("CMAKE_CONFIGURATION_TYPES"))
                    or generator.startswith("Visual Studio")
                    or generator in ("Ninja Multi-Config", "Xcode")
                    or (os.name == "nt" and not generator))
    if not multi_config:
        configure.append(f"-DCMAKE_BUILD_TYPE={config}")
    if args.generator:
        configure += ["-G", args.generator]
    if os.name == "nt" and (not generator or generator.startswith("Visual Studio")):
        configure += ["-A", args.platform]
    qt = find_qt(args, cache, generator)
    if qt:
        # Clear module locations too: retaining them can mix two Qt installs.
        configure += ["-UQt6*_DIR", f"-DQt6_ROOT={qt}", f"-DQt6_DIR={qt / 'lib/cmake/Qt6'}"]
    build = [cmake, "--build", str(build_dir), "--config", config,
             "--parallel", str(args.jobs or os.cpu_count() or 1)]
    if args.project == "Crosimage":
        build += ["--target", "Crosimage"]
    if args.rebuild:
        build.append("--clean-first")
    return [configure, build]


def run_command(command, args, log):
    log.write(f"Command: {subprocess.list2cmdline(command)}\n\n")
    log.flush()
    # CMake uses UTF-8; MSBuild may still use the Windows system code page.
    with subprocess.Popen(command, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as process:
        for raw_line in process.stdout:
            try:
                line = raw_line.decode("utf-8")
            except UnicodeDecodeError:
                line = raw_line.decode(locale.getpreferredencoding(False), errors="replace")
            line = line.rstrip("\r\n") + "\n"
            log.write(line)
            log.flush()
            if not args.quiet and (not args.errors or re.search(r"error|failed|ошибк", line, re.IGNORECASE)):
                print(line, end="", flush=True)
        return process.wait()


def main(argv=None):
    args = parse_args(argv)
    log_path = Path(args.log).expanduser().resolve()
    started = time.monotonic()
    try:
        commands = build_commands(args)
        if args.dry_run:
            if not args.quiet:
                for command in commands:
                    print(subprocess.list2cmdline(command))
            return 0
        log_path.parent.mkdir(parents=True, exist_ok=True)
        if not args.quiet:
            print(f"Build: [{'Release' if args.release else 'Debug'}|{args.platform}]", flush=True)
            print(f"Full log: {log_path}", flush=True)
        with log_path.open("w", encoding="utf-8") as log:
            for command in commands:
                code = run_command(command, args, log)
                if code:
                    break
    except KeyboardInterrupt:
        code = 130
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        message = f"Build error: {error}"
        if not args.quiet:
            print(message, file=sys.stderr)
        if not args.dry_run:
            try:
                log_path.parent.mkdir(parents=True, exist_ok=True)
                with log_path.open("a", encoding="utf-8") as log:
                    log.write(message + "\n")
            except OSError:
                pass
        return 1
    if not args.quiet:
        print(f"Build {'succeeded' if code == 0 else 'FAILED'}. Exit code: {code}; elapsed: {time.monotonic() - started:.1f}s")
    return code


if __name__ == "__main__":
    sys.exit(main())
