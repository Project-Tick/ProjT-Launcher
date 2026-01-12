# Fuzzing

This project ships libFuzzer targets for key parsers and data paths.

## Targets

- `fuzz_nbt_reader`: NBT stream reader (libnbt++)
- `fuzz_qjson_parse`: Qt JSON parser
- `fuzz_gzip`: GZip inflate/deflate helpers
- `fuzz/meta/fuzz_meta_models.py`: Python Atheris targets for metadata models
- `bot/fuzz/fastcheck.test.js`: JS property fuzzing for bot helpers

## Local build (Linux/macOS)

```bash
cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_TESTING=OFF \
  -DBUILD_FUZZERS=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++

cmake --build build-fuzz --target fuzz_nbt_reader fuzz_qjson_parse fuzz_gzip
```

Run a fuzzer:

```bash
./build-fuzz/fuzz_nbt_reader -runs=1000
```

## CI (ClusterFuzzLite)

The `ClusterFuzzLite` workflow builds and runs these targets on schedule and on PRs.

## CI (Python/JS fuzzing)

- `Python Atheris Fuzz` runs `fuzz/meta/fuzz_meta_models.py`
- `JS Fast-Check Fuzz` runs `bot/fuzz/fastcheck.test.js`
