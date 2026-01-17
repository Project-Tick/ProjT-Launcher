# Bzip2 Tests

> **Directory**: `bzip2/`  
> **Test Suites**: Quick (default), Large
> **Latest Version**: 0.0.5-1

---

## Overview

Bzip2 has two test suites for validating compression and decompression functionality.

---

## Test Suites

| Suite | Files | Speed | Valgrind |
|-------|-------|-------|----------|
| **Quick** | Small set | Fast (~seconds) | ✅ Enabled |
| **Large** | Many files | Slow (~minutes) | ❌ Disabled |

---

## Quick Test Suite

The default test suite validates basic functionality:

### What It Tests

1. **Compression** — Compress reference files, decompress, verify match
2. **Decompression** — Decompress `.bz2` files, verify against reference
3. **Multiple modes** — Tests various compression levels

### Running

```bash
# CMake
cd bzip2/build
ctest -V

# Meson
meson test -C builddir --print-errorlogs
```

---

## Large Test Suite

Comprehensive test with files from various sources:

### Contents

- ✅ Good `.bz2` files — Should decompress correctly
- ❌ Bad `.bz2.bad` files — Should fail gracefully

### Running

Integrated with the build system test commands.

---

## Valgrind Integration

The quick tests run under Valgrind if available:

```bash
# Check if Valgrind is detected
cmake .. -DCMAKE_BUILD_TYPE=Debug
# Valgrind runs automatically during ctest
```

The large tests have Valgrind disabled (would take 35+ minutes).

---

## Related Documentation

- [Bzip2 Overview](./bzip2.md) — Main documentation
- [Compiling](./bzip2-compiling.md) — Build instructions
- [Test Files](./bzip2-testfiles.md) — Test file collection
