#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# In the OSS-Fuzz builder, the repo lives at /src/projt-launcher; locally this script sits in .clusterfuzzlite/
if [ -d "${SCRIPT_DIR}/projt-launcher" ]; then
  REPO_ROOT="$(cd "${SCRIPT_DIR}/projt-launcher" && pwd)"
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

# Qt6 not needed anymore - fuzz_qjson_parse removed to avoid glib dependency
# Fuzzing only needs: libnbt++ (for fuzz_nbt_reader) and zlib (for fuzz_gzip)

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

# Build only fuzzers (fuzz_qjson_parse removed - Qt6 adds glib dependency)
cmake --build build --parallel --target fuzz_nbt_reader fuzz_gzip

# Copy fuzzers to output directory
cp build/fuzz_nbt_reader "$OUT/"
cp build/fuzz_qjson_parse "$OUT/"
cp build/fuzz_gzip "$OUT/"

# Bundle only essential Qt6::Core runtime (needed by fuzz_qjson_parse)
cp "${QT_ROOT}/lib/libQt6Core.so"* "$OUT/" || true
# ICU libraries required by Qt6::Core
cp "${QT_ROOT}/lib/libicudata.so"* "$OUT/" || true
cp "${QT_ROOT}gzip "$OUT/"