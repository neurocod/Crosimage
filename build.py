"""Build Crosimage.sln from a regular Windows terminal (Debug|x64 by default).

Uses the installed Visual Studio and Qt VS Tools; no Python packages needed.
The console shows diagnostics and a summary, while build.log keeps the full
MSBuild output in UTF-8. Run `python build.py --help` for all options.
"""

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parent
SOLUTION = ROOT / "Crosimage.sln"
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
    parser.add_argument("--rebuild", action="store_true", help="clean and rebuild the selected target")
    parser.add_argument("--platform", choices=("x64", "Win32"), default="x64")
    parser.add_argument("--project", type=str.strip, choices=("", "ALL_BUILD", "Crosimage"),
                        default="", help="build Crosimage only; empty or ALL_BUILD builds the solution")
    parser.add_argument("--jobs", type=positive_int, metavar="N",
                        help="maximum parallel MSBuild nodes (default: available CPUs)")
    parser.add_argument("--errors", action="store_true", help="show errors only, plus the build result")
    parser.add_argument("--quiet", action="store_true", help="suppress output; still write the full log")
    parser.add_argument("--log", type=nonblank, default=str(DEFAULT_LOG), metavar="PATH",
                        help="UTF-8 build log (default: build.log beside this script)")
    parser.add_argument("--msbuild", type=nonblank, metavar="PATH",
                        help="MSBuild.exe override (default: discover Visual Studio with vswhere)")
    parser.add_argument("--qt", type=nonblank, metavar="PATH",
                        help="Qt installation prefix; default: QT_PREFIX, then project Qt settings")
    parser.add_argument("--qt-msbuild", type=nonblank, metavar="PATH",
                        help="Qt/MSBuild directory containing qt.props and qt.targets")
    parser.add_argument("--dry-run", action="store_true", help="resolve tools and print command without building or writing a log")
    args = parser.parse_args(argv)
    # MSBuild file logger parameters use semicolons as separators.
    if any(char in args.log for char in ';"\r\n'):
        parser.error('--log must not contain semicolons, quotes or newlines')
    return args


def find_msbuild(override=None):
    if override:
        found = shutil.which(override)
        if found:
            return Path(found).resolve()
        raise RuntimeError(f"MSBuild executable not found: {override}")

    installer = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere = installer / "Microsoft Visual Studio/Installer/vswhere.exe"
    if vswhere.is_file():
        result = subprocess.run(
            [str(vswhere), "-latest", "-prerelease", "-products", "*",
             "-requires", "Microsoft.Component.MSBuild", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
             "-property", "installationPath", "-utf8"],
            capture_output=True, text=True, encoding="utf-8", errors="replace", check=True,
        )
        installation = result.stdout.strip()
        if installation:
            for relative in ("MSBuild/Current/Bin/amd64/MSBuild.exe", "MSBuild/Current/Bin/MSBuild.exe"):
                candidate = Path(installation) / relative
                if candidate.is_file():
                    return candidate
    found = shutil.which("MSBuild.exe")
    if found:
        return Path(found).resolve()
    raise RuntimeError("MSBuild not found. Install Visual Studio with Desktop development with C++, or use --msbuild PATH.")


def find_qt_msbuild(override=None):
    configured = override or os.environ.get("QtMsBuild", "").strip()
    if configured:
        candidates = [Path(configured).expanduser().resolve()]
    else:
        candidates = [ROOT / "Crosimage/QtMsBuild"]
        if os.environ.get("LOCALAPPDATA"):
            candidates.append(Path(os.environ["LOCALAPPDATA"]) / "QtMsBuild")
    for candidate in candidates:
        if all((candidate / name).is_file() for name in ("qt.props", "qt.targets")):
            return candidate
    raise RuntimeError("Qt/MSBuild not found. Install Qt VS Tools, or set --qt-msbuild PATH to the directory containing qt.props and qt.targets.")


def property_arg(name, value):
    # Escape MSBuild syntax, independently of subprocess's shell quoting.
    escaped = str(value)
    for char in "%$@();'?*":
        escaped = escaped.replace(char, f"%{ord(char):02X}")
    return f"/p:{name}={escaped}"


def build_command(args, log_path):
    if os.name != "nt":
        raise RuntimeError("This script builds the Visual Studio solution and requires Windows.")
    if not SOLUTION.is_file():
        raise RuntimeError(f"Solution not found: {SOLUTION}")
    msbuild = find_msbuild(args.msbuild)
    qt_msbuild = find_qt_msbuild(args.qt_msbuild)
    target = "Rebuild" if args.rebuild else "Build"
    if args.project == "Crosimage":
        target = f"Crosimage:{target}"
    command = [str(msbuild), str(SOLUTION), "/nologo", f"/t:{target}",
               property_arg("Configuration", "Release" if args.release else "Debug"),
               property_arg("Platform", args.platform), property_arg("QtMsBuild", qt_msbuild),
               f"/m:{args.jobs}" if args.jobs else "/m", "/nr:false"]
    qt = args.qt or os.environ.get("QT_PREFIX", "").strip()
    if qt:
        prefix = Path(qt).expanduser().resolve()
        if not (prefix / "bin/qmake.exe").is_file():
            raise RuntimeError(f"Qt prefix must contain bin/qmake.exe: {prefix}")
        command.append(property_arg("QtInstall", prefix))
    # Native loggers retain localized/multiline diagnostics and UTF-8 paths.
    command += ["/fl", f"/flp:LogFile={log_path};Encoding=UTF-8;Verbosity=normal;Append"]
    if args.quiet:
        command.append("/noconsolelogger")
    else:
        command += ["/v:quiet", "/clp:ErrorsOnly;Summary" if args.errors else "/clp:Summary"]
    return command


def main(argv=None):
    args = parse_args(argv)
    log_path = Path(args.log).expanduser().resolve()
    started = time.monotonic()
    try:
        command = build_command(args, log_path)
        command_text = subprocess.list2cmdline(command)
        if args.dry_run:
            if not args.quiet:
                print(command_text)
            return 0
        log_path.parent.mkdir(parents=True, exist_ok=True)
        log_path.write_text(f"Command: {command_text}\n\n", encoding="utf-8")
        if not args.quiet:
            action = "Rebuild" if args.rebuild else "Build"
            config = "Release" if args.release else "Debug"
            print(f"{action}: [{config}|{args.platform}] ({args.project or 'solution'})", flush=True)
            print(f"Full log: {log_path}", flush=True)
        result = subprocess.run(command, cwd=ROOT,
                                stdout=subprocess.DEVNULL if args.quiet else None,
                                stderr=subprocess.DEVNULL if args.quiet else None)
        code = result.returncode
    except KeyboardInterrupt:
        code = 130
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        message = f"Build error: {error}"
        if not args.quiet:
            print(message, file=sys.stderr)
        # Setup failures also leave an explanation in --log, when writable.
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
