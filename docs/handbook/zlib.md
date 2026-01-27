# Zlib `zlib/`

> **Type**: Compression Library  
> **License**: zlib License  
> **Fork Origin**: [zlib.net](https://zlib.net) | [GitHub](https://github.com/madler/zlib)  
> **Status**: Detached Fork (independently maintained)  
> **Base Version**: 1.3.1.2
> **Latest Version**: 0.0.5-1

---

## Overview

Zlib is a general-purpose, lossless data-compression library used for ZIP and GZip functionality. It implements the DEFLATE compression algorithm and is one of the most widely used compression libraries in the world.

This repository contains a maintained fork of the upstream zlib project. The fork exists to allow controlled integration, CI validation, and long-term maintenance within the ProjT Launcher monorepo.

---

## Fork Policy

This is a **detached fork** that is independently maintained by Project Tick:

- ✅ Original codebase used as foundation
- ✅ Independent development and maintenance
- ✅ Custom CI/CD integration for ProjT Launcher
- ⚠️ May diverge from original project over time

This fork is not synchronized with the original repository.

---

## Usage in ProjT Launcher

Zlib is used for:

- **ZIP archive handling** via QuaZip wrapper
- **PNG image compression** for textures and assets
- **Network compression** for mod downloads
- **NBT file compression** for Minecraft world data

---

## Documentation

| Resource | Location |
|----------|----------|
| API Reference | `zlib/zlib.h` |
| FAQ | `zlib/FAQ` |
| Change History | `zlib/ChangeLog` |
| [Original CMake README](../../zlib/README-cmake.md) | Upstream CMake documentation |
| Upstream Manual | [zlib.net/manual.html](https://zlib.net/manual.html) |

---

## Build Integration

Zlib is built as part of the ProjT Launcher build system. The upstream build systems are preserved for reference, but the primary build path uses our integrated CMake configuration.

### CMake Options

```cmake
# In the main CMakeLists.txt, zlib is included as a subdirectory
add_subdirectory(zlib)

# Link against zlib
target_link_libraries(your_target PRIVATE ZLIB::ZLIB)
```

### Standalone Build

For development or testing purposes:

```bash
cd zlib
mkdir build && cd build
cmake ..
cmake --build .
```

For upstream-specific build instructions, see [zlib.net](https://zlib.net).

---

## Testing

Zlib includes its own test suite that is run as part of CI:

```bash
cd zlib/build
ctest -V
```

See [ci-zlib.yml](../../.github/workflows/ci-zlib.yml) for CI configuration.

---

## Licensing

Zlib is licensed under the **zlib License**, a permissive free software license.

The full license text is included unmodified in `zlib/LICENSE`.

### Copyright

**Original Work:**
```
Copyright © 1995–2025
Jean-loup Gailly, Mark Adler
```

**Modifications:**
```
Copyright © 2026
Project Tick contributors
```

---

## Related Documentation

- [QuaZip](./quazip.md) — C++ wrapper that uses zlib
- [bzip2](./bzip2.md) — Alternative compression library
- [Third-party Libraries](./third-party.md) — Overview of all dependencies

---

## External Links

- [zlib Official Website](https://zlib.net)
- [zlib Official GitHub Repository](https://github.com/madler/zlib)
- [zlib Project Tick Github Repository](https://github.com/Project-Tick/ProjT-Launcher/tree/main/zlib)
- [RFC 1950 - ZLIB Specification](https://tools.ietf.org/html/rfc1950)
- [RFC 1951 - DEFLATE Specification](https://tools.ietf.org/html/rfc1951)
