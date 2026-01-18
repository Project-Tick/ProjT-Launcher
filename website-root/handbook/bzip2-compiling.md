# Compiling Bzip2

> **Directory**: `bzip2/`  
> **Build Systems**: Meson (preferred), CMake
> **Latest Version**: 0.0.5-1

---

## Overview

Bzip2 supports two build systems. **Meson** is preferred for Unix-like systems, while **CMake** provides cross-platform support.

---

## Build Systems Comparison

| Feature | Meson | CMake |
|---------|-------|-------|
| Unix Support | ✅ Excellent | ✅ Good |
| Windows Support | ✅ Good | ✅ Excellent |
| Preferred For | Unix-like | Cross-platform |

---

## Prerequisites

### All Platforms

- Python 3.6+
- C compiler (GCC, Clang, or MSVC)

### Meson Builds

- Meson 0.56+
- Ninja
- pkg-config

### CMake Builds

- CMake 3.12+

---

## Meson Build

### Linux/macOS

```bash
cd bzip2

# Configure
meson setup builddir --prefix=/usr

# Build
ninja -C builddir

# Test
meson test -C builddir --print-errorlogs

# Install (optional)
sudo ninja -C builddir install
```

### Windows (MSVC)

```cmd
cd bzip2

REM From VS Developer Command Prompt
meson setup builddir
ninja -C builddir
meson test -C builddir --print-errorlogs
```

### Meson Options

| Option | Default | Description |
|--------|---------|-------------|
| `default_library` | `shared` | `static`, `shared`, or `both` |
| `--backend` | `ninja` | Use `vs` for MSBuild |
| `--unity` | off | Enable unity build |
| `buildtype` | `debug` | `release`, `debug`, etc. |

---

## CMake Build

### Linux/macOS

```bash
cd bzip2
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Test
ctest -V

# Install (optional)
cmake --install .
```

### Windows

```powershell
cd bzip2
mkdir build; cd build

# Configure
cmake ..

# Build (Release)
cmake --build . --config Release

# Test
ctest -C Release -V
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_APP` | `ON` | Build bzip2 program |
| `ENABLE_LIB_ONLY` | `OFF` | Only build library |
| `ENABLE_STATIC_LIB` | `OFF` | Build static library |
| `ENABLE_SHARED_LIB` | `ON` | Build shared library |
| `ENABLE_TESTS` | `ON` | Build tests |
| `ENABLE_DOCS` | `OFF` | Generate documentation |
| `ENABLE_EXAMPLES` | `OFF` | Build examples |
| `USE_OLD_SONAME` | `OFF` | Use old SONAME (distro compat) |

---

## SONAME Compatibility (Linux)

The SONAME changed from `libbz2.so.1.0` to `libbz2.so.1` in version 1.1.

For distro compatibility:

```bash
# Option 1: CMake flag
cmake .. -DUSE_OLD_SONAME=ON

# Option 2: Manual patchelf
patchelf --set-soname libbz2.so.1.0 libbz2.so.1.0.9
```

Check SONAME:
```bash
objdump -p libbz2.so.1.0.9 | grep SONAME
```

---

## Related Documentation

- [Bzip2 Overview](./bzip2.md) — Main documentation
- [Bzip2 Tests](./bzip2-tests.md) — Test suite
- [Bzip2 Test Files](./bzip2-testfiles.md) — Test file collection
