#!/usr/bin/env bash
set -euo pipefail

if command -v apt-get >/dev/null 2>&1; then
  apt-get update
  apt-get install -y --no-install-recommends \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev-tools \
    ninja-build
fi

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_TESTING=OFF \
  -DBUILD_FUZZERS=ON

cmake --build build --target fuzz_nbt_reader fuzz_qjson_parse fuzz_gzip

cp build/fuzz_nbt_reader "$OUT/"
cp build/fuzz_qjson_parse "$OUT/"
cp build/fuzz_gzip "$OUT/"
