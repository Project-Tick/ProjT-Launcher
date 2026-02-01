# Libraries

> **Purpose**: Overview of all external dependencies used by ProjT Launcher

---

## Overview

ProjT Launcher includes several third-party libraries for various functionality. This document provides a quick reference to all external dependencies.

---

## Detached Fork Libraries

These libraries are **detached forks** — independently maintained by Project Tick, not synchronized with original repositories:

### Compression

| Library | Purpose | License | Documentation |
|---------|---------|---------|---------------|
| **zlib** | DEFLATE compression | zlib | [zlib.md](./zlib.md) |
| **bzip2** | Block-sorting compression | BSD-like | [bzip2.md](./bzip2.md) |
| **QuaZip** | Qt ZIP wrapper | LGPL-2.1+ | [quazip.md](./quazip.md) |

### Data Formats

| Library | Purpose | License | Documentation |
|---------|---------|---------|---------------|
| **toml++** | TOML parser | MIT | [tomlplusplus.md](./tomlplusplus.md) |
| **libnbt++** | NBT format | GPL-3.0 | [libnbtplusplus.md](./libnbtplusplus.md) |
| **cmark** | Markdown parser | BSD-2-Clause | [cmark.md](./cmark.md) |

### Utilities

| Library | Purpose | License | Documentation |
|---------|---------|---------|---------------|
| **libqrencode** | QR code generation | LGPL-2.1+ | [libqrencode.md](./libqrencode.md) |

### Build System

| Library | Purpose | License | Documentation |
|---------|---------|---------|---------------|
| **Extra CMake Modules** | CMake utilities | BSD-3-Clause | [extra-cmake-modules.md](./extra-cmake-modules.md) |

---

## Original READMEs

Each detached fork retains its original upstream documentation. These are preserved for reference but are not actively maintained by Project Tick:

| Library | Documentation |
|---------|---------------|
| toml++ | `tomlplusplus/README.md` |
| libqrencode | `libqrencode/README.md` |
| ECM | `extra-cmake-modules/README.md` |
| zlib | `zlib/README-cmake.md`, `zlib/FAQ` |
| bzip2 | `bzip2/bzip2.txt`, `bzip2/manual.html` |
| cmark | `cmark/changelog.txt`, `cmark/man/` |
| QuaZip | `quazip/QuaZip-1.x-migration.md` |
| libnbt++ | `libnbtplusplus/include/` (headers) |

For Project Tick–specific documentation and usage, refer to the handbook pages linked above.

---

## Internal Libraries

These are ProjT Launcher-specific libraries:

### LocalPeer `LocalPeer/`

Single-instance application enforcement.

- **Origin**: Derived from QtSingleApplication
- **License**: BSD
- **Use**: Prevents multiple launcher instances

### murmur2 `murmur2/`

Hash function implementation.

- **Origin**: [SMHasher](https://github.com/aappleby/smhasher)
- **License**: Public Domain
- **Use**: Content hashing for mods

### rainbow `rainbow/`

Color manipulation functions.

- **Origin**: KGuiAddons
- **License**: LGPL-2.1+
- **Use**: Adaptive text coloring

### systeminfo `systeminfo/`

System information probing.

- **License**: Apache-2.0
- **Use**: Hardware/OS detection

### qdcss `qdcss/`

Quick and dirty CSS parser.

- **Origin**: NilLoader
- **License**: LGPL-3.0
- **Use**: Mod metadata parsing

---

## External Dependencies

These are system/Qt dependencies not bundled:

### Qt Framework

| Module | Purpose |
|--------|---------|
| Qt Core | Base functionality |
| Qt GUI | UI rendering |
| Qt Widgets | UI components |
| Qt Network | HTTP/networking |
| Qt Concurrent | Threading |

### System Libraries

| Library | Purpose | Platforms |
|---------|---------|-----------|
| OpenSSL | TLS/HTTPS | All |
| libpng | PNG images | All |
| zlib (system) | Alternative to bundled | Optional |

---

## License Summary

| License | Libraries |
|---------|-----------|
| **MIT** | toml++ |
| **BSD-2/3-Clause** | cmark, gamemode |
| **BSD-like** | bzip2, LocalPeer |
| **zlib** | zlib |
| **LGPL-2.1+** | QuaZip, rainbow, libqrencode |
| **LGPL-3.0** | qdcss |
| **GPL-3.0** | libnbt++ |
| **Apache-2.0** | systeminfo |
| **Public Domain** | murmur2 |

---

## Adding New Dependencies

When adding new libraries:

1. **Evaluate license compatibility** with GPL-3.0
2. **Create handbook documentation** in `docs/handbook/`
3. **Update this file** with the new entry
4. **Add CI workflow** if needed
5. **Update CMakeLists.txt** for build integration

---

## Related Documentation

- [Workflows](./workflows.md) — CI for libraries
- [README](./README.md) — Handbook index
