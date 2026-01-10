## Tests

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

### Running the Tests

Run the tests using CMake or Meson's test commands.

For CMake:

```sh
ctest -V
```

For Meson:

```sh
meson test -C builddir --print-errorlogs
```

### Quick Test Suite

The quick test suite is a small set of `.bz2` compressed files and original
reference files.

BZip2 must be able to:

1. Compress the reference files without error and decompress the newly created
   compressed version into a file that matches the original reference file.
   Multiple compression modes are tested.

2. Decompress the `.bz2` files without error. The decompressed file must match
   the original reference file.
