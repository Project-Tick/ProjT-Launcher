#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# In the OSS-Fuzz builder, the repo lives at /src/projtlauncher; locally this script sits in .clusterfuzzlite/
if [ -d "${SCRIPT_DIR}/projtlauncher" ]; then
  REPO_ROOT="$(cd "${SCRIPT_DIR}/projtlauncher" && pwd)"
elif [ -f "${SCRIPT_DIR}/CMakeLists.txt" ]; then
  REPO_ROOT="${SCRIPT_DIR}"
else
  REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
fi
cd "${REPO_ROOT}"

if command -v apt-get >/dev/null 2>&1; then
  apt-get update
  apt-get install -y --no-install-recommends \
    ninja-build \
    python3-pip
fi

# Qt6 not available in OSS-Fuzz environment
# Only fuzz_nbt_reader is built (uses libnbt++ and zlib, no Qt dependency)
# fuzz_gzip requires Qt6::Core (for QByteArray/QFile in GZip.cpp) - skipped

export PATH="${PATH}"

# Configure with fuzzing flags
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF \
  -DBUILD_FUZZERS=ON \
  -DLAUNCHER_FUZZ_ONLY=ON \
  -DCMAKE_C_COMPILER="${CC:-clang}" \
  -DCMAKE_CXX_COMPILER="${CXX:-clang++}" \
  -DCMAKE_BUILD_RPATH="\$ORIGIN" \
  -DCMAKE_INSTALL_RPATH="\$ORIGIN" \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON

# Build only fuzz_nbt_reader (no Qt dependency)
# fuzz_gzip is conditionally built only when Qt6::Core is available
cmake --build build --parallel --target fuzz_nbt_reader

# Copy fuzzers to output directory
cp build/fuzz_nbt_reader "$OUT/"