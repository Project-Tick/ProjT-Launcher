#!/usr/bin/env bash
# ProjT Launcher - Update pinned dependency versions
# Updates ci/pinned.json with current recommended versions

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PINNED_FILE="$SCRIPT_DIR/pinned.json"

echo "Updating ProjT Launcher pinned dependencies..."

# Get current date
CURRENT_DATE=$(date +%Y-%m-%d)

# Create updated pinned.json
cat > "$PINNED_FILE" << EOF
{
  "dependencies": {
    "cmake": {
      "version": "3.28.0",
      "description": "Build system"
    },
    "qt6": {
      "version": "6.7.0",
      "description": "Qt framework for UI"
    },
    "gcc": {
      "version": "13.2.0",
      "description": "GCC compiler"
    },
    "clang": {
      "version": "17.0.0",
      "description": "Clang compiler"
    },
    "ninja": {
      "version": "1.11.1",
      "description": "Fast build system"
    },
    "gtest": {
      "version": "1.14.0",
      "description": "Google Test framework"
    }
  },
  "platforms": {
    "linux": {
      "runner": "ubuntu-24.04",
      "compiler": "gcc"
    },
    "macos": {
      "runner": "macos-14",
      "compiler": "clang"
    },
    "windows": {
      "runner": "windows-2022",
      "compiler": "msvc"
    }
  },
  "version": 1,
  "updated": "$CURRENT_DATE"
}
EOF

echo "Updated $PINNED_FILE"
echo "Date: $CURRENT_DATE"
