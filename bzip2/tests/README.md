# Tests

BZip2 has two test suites:

1. The quick test suite is the original test suite. It is small and runs very
   quickly to verify correct compression and decompression of simple files.

2. The large test suite is a large collection of test files gathered from
   various sources. It includes not only good `.bz2` files but also bad ones.

The quick tests will run under Valgrind if Valgrind is installed on the system
and was discovered by CMake/Meson at build time. If you installed Valgrind after
build time, you may have to do a clean build for the Valgrind to be detected.

The slow tests have Valgrind disabled, because with it enabled it takes upwards
of 35 minutes to run.

## Running the Tests

Run the tests using CMake or Meson's test commands.

For CMake:
  ```sh
  ctest -V
  ```

For Meson:
  ```sh
  meson test -C builddir --print-errorlogs
  ```

## Quick Test Suite

The quick test suite is a small set of `.bz2` compressed files and original
reference files.

BZip2 must be able to:

1. Compress the reference files without error and decompress the newly created
   compressed version into a file that matches the original reference file.
   Multiple compression modes are tested.

2. Decompress the `.bz2` files without error. The decompressed file must match
   the original reference file.

## Large Test Suite

The large test suite tests a collection of "interesting" `.bz2` files that can
be used to test bzip2 works correctly. They come from different projects.

The test files for the Large Test Suite are in a separate repository that was
added here as a Git submodule: https://gitlab.com/bzip2/bzip2-testfiles

To run the larger test suite, you must first pull down the submodule before
running `ctest`.

E.g.:
```sh
git submodule update --init --recursive
```

For each `.bz2` file found it is decompressed, recompressed and decompressed
again. Once with the default bzip2 settings and once in `--small` (`-s`) mode.
Each time after decompression, the resulting file is checked against the MD5
reference hash to verify that decompression worked correctly.

For each `.bz2.bad` file, decompression is also tried twice, first in default-
mode and again in small-mode. The bzip2 binary is expected to return either `1`
or `2` as exit status. Any other exit code is interpreted as failure.
