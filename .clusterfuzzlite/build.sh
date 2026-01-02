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
    qtbase5-dev \
    qttools5-dev \
    qttools5-dev-tools \
    ninja-build \
    python3-pip
fi

# Install Qt6 toolchain via aqt (not available from focal apt repos)
QT_VERSION=6.5.3
QT_ROOT="/opt/Qt/${QT_VERSION}/gcc_64"
if [ ! -d "${QT_ROOT}" ]; then
  python3 -m pip install --no-cache-dir "aqtinstall==3.1.*"
  python3 -m aqt install-qt --outputdir /opt/Qt linux desktop "${QT_VERSION}" gcc_64 -m qttools qt5compat qtnetworkauth
fi
export PATH="${QT_ROOT}/bin:${PATH}"
export CMAKE_PREFIX_PATH="${QT_ROOT}/lib/cmake:${CMAKE_PREFIX_PATH:-}"

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_TESTING=OFF \
  -DBUILD_FUZZERS=ON

cmake --build build --target fuzz_nbt_reader fuzz_qjson_parse fuzz_gzip

cp build/fuzz_nbt_reader "$OUT/"
cp build/fuzz_qjson_parse "$OUT/"
cp build/fuzz_gzip "$OUT/"
