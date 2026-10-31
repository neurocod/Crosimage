"""Clean a regression fixture after its process releases SQLite connections."""

import subprocess
import sys
import tempfile


def main():
    with tempfile.TemporaryDirectory(prefix="crosimage-regression-") as fixture:
        return subprocess.run([sys.argv[1], fixture], timeout=240).returncode


if __name__ == "__main__":
    sys.exit(main())
