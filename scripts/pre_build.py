#!/usr/bin/env python3
"""
PlatformIO Pre-Build Script
Runs flipper_to_cpp.py before each build.
"""

import subprocess
import sys
from pathlib import Path

Import("env")  # PlatformIO magic  # noqa: F405


def run_flipper_converter(source, target, env):
    """Run the Flipper to C++ converter before building."""
    script_dir = Path(__file__).parent.resolve()
    converter_script = script_dir / "flipper_to_cpp.py"

    print("=" * 60)
    print("Running Flipper SubGHz to C++ converter...")
    print("=" * 60)

    result = subprocess.run(
        [sys.executable, str(converter_script)], cwd=script_dir.parent, capture_output=False
    )

    if result.returncode != 0:
        print("[WARN] Flipper converter returned non-zero exit code")


# Register pre-build action
env.AddPreAction("buildprog", run_flipper_converter)
