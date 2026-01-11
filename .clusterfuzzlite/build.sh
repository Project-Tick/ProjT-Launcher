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

# Install Qt6 toolchain via aqt (minimal for fuzzing - only Qt6::Core needed)
QT_VERSION=6.5.3
QT_ROOT="/opt/Qt/${QT_VERSION}/gcc_64"
if [ ! -d "${QT_ROOT}" ]; then
  python3 -m pip install --no-cache-dir "aqtinstall==3.1.*"
  # Only install base Qt6 for fuzz_qjson_parse (needs Qt6::Core)
  python3 -m aqt install-qt --outputdir /opt/Qt linux desktop "${QT_VERSION}" gcc_64
fi
export PATH="${QT_ROOT}/bin:${PATH}"
export CMAKE_PREFIX_PATH="${QT_ROOT}/lib/cmake:${CMAKE_PREFIX_PATH:-}"
export LD_LIBRARY_PATH="${QT_ROOT}/lib:${LD_LIBRARY_PATH:-}"

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

# Build only fuzzers
cmake --build build --parallel --target fuzz_nbt_reader fuzz_qjson_parse fuzz_gzip

# Copy fuzzers to output directory
cp build/fuzz_nbt_reader "$OUT/"
cp build/fuzz_qjson_parse "$OUT/"
cp build/fuzz_gzip "$OUT/"

# Bundle only essential Qt6::Core runtime (needed by fuzz_qjson_parse)
cp "${QT_ROOT}/lib/libQt6Core.so"* "$OUT/" || true
# ICU libraries required by Qt6::Core
cp "${QT_ROOT}/lib/libicudata.so"* "$OUT/" || true
cp "${QT_ROOT}/lib/libicui18n.so"* "$OUT/" || true
cp "${QT_ROOT}/lib/libicuuc.so"* "$OUT/" || true

echo "✓ Fuzzer build complete. Built targets:"
ls -lh "$OUT/fuzz_"*
