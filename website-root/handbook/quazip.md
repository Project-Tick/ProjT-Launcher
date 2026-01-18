# QuaZip `quazip/`

> **Type**: ZIP Archive Library (Qt C++ Wrapper)  
> **License**: LGPL-2.1-or-later  
> **Fork Origin**: [GitHub](https://github.com/stachenov/quazip)  
> **Status**: Detached Fork (independently maintained)  
> **Dependencies**: Qt 5/6, zlib
> **Latest Version**: 0.0.5-1

[![CI](https://github.com/Project-Tick/ProjT-Launcher/actions/workflows/ci-new.yml/badge.svg?branch=develop)](https://github.com/Project-Tick/ProjT-Launcher/actions/workflows/ci-new.yml)

---

## Overview

QuaZip is the C++ wrapper for Gilles Vollant's ZIP/UNZIP package (Minizip) using the Qt library. If you need to write files to a ZIP archive or read files from one using the `QIODevice` API, QuaZip is the tool you need.

---

## Usage in ProjT Launcher

QuaZip is used for:

- **Mod installation** — Extracting mod archives
- **Modpack handling** — Processing CurseForge/Modrinth packs
- **Resource packs** — Managing Minecraft resource packs
- **Backup/restore** — Creating instance backups

---

## Features

| Feature | Status |
|---------|--------|
| Read/Write ZIP files | ✅ |
| Qt stream integration (`QIODevice`) | ✅ |
| Unicode filename support | ✅ |
| ZIP64 support (files > 4GB) | ✅ |
| BZIP2 compression | ✅ |
| Password protection (read) | ✅ |

---

## Documentation

| Resource | Description |
|----------|-------------|
| [Migration Guide](../../quazip/QuaZip-1.x-migration.md) | Migration guide to QuaZip 1.x |
| [Online Docs](https://projecttick.org/projtlauncher/wiki/) | Full documentation |
| `quazip/qztest/` | Test suite and examples |

---

## Build Instructions

### Prerequisites

- **Qt 5.12+** or **Qt 6.x**
- **zlib** (or Qt's bundled zlib)
- **CMake 3.15+**
- **libbz2** (optional, for BZIP2 support)

### Linux

```bash
sudo apt-get install libbz2-dev
cmake -B build
cmake --build build
```

### Windows

When using Qt online installer with MSVC, select `MSVC 20XY 64-bit` and under additional libraries, select `Qt 5 Compatibility Module`. Add `C:\Qt\6.8.2\msvc20XY_64` to your PATH.

#### Using vcpkg (x64)

```bat
cmake --preset vcpkg
cmake --build build --config Release
```

#### Using Conan v2 (x64)

```bat
conan install . -of build -s build_type=Release -o *:shared=False --build=missing
cmake --preset conan
cmake --build build --config Release
```

### Complete Build Workflow

```bash
cmake -B build -DQUAZIP_QT_MAJOR_VERSION=6 -DBUILD_SHARED_LIBS=ON -DQUAZIP_ENABLE_TESTS=ON
cmake --build build --config Release
sudo cmake --install build
cd build && ctest --verbose -C Release
```

---

## CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `QUAZIP_QT_MAJOR_VERSION` | Qt version to search for (5 or 6) | Auto-detect |
| `BUILD_SHARED_LIBS` | Build as shared library | `ON` |
| `QUAZIP_LIB_FILE_NAME` | Output library name | `quazip<ver>-qt<ver>` |
| `QUAZIP_INSTALL` | Enable installation | `ON` |
| `QUAZIP_USE_QT_ZLIB` | Use Qt's bundled zlib (not recommended) | `OFF` |
| `QUAZIP_ENABLE_TESTS` | Build test suite | `OFF` |
| `QUAZIP_BZIP2` | Enable BZIP2 compression | `ON` |
| `QUAZIP_BZIP2_STDIO` | Output BZIP2 errors to stdio | `ON` |
| `QUAZIP_ENABLE_QTEXTCODEC` | Enable QTextCodec on Qt6 | `ON` |

---

## Copyright & Licensing

```
Copyright (C) 2005-2020 Sergey A. Tachenov and contributors
```

Distributed under **LGPL-2.1-or-later**. See `quazip/COPYING` for details.

Original ZIP package is copyrighted by Gilles Vollant (zlib license).

---

## Contributing

- **Report bugs**: [GitHub Issues](https://github.com/Project-Tick/ProjT-Launcher/issues)
- **Contribute code**: See [CONTRIBUTING.md](../../CONTRIBUTING.md)
- **Migration guide**: `quazip/QuaZip-1.x-migration.md`

---

## Related Documentation

- [zlib](./zlib.md) — Underlying compression library
- [bzip2](./bzip2.md) — BZIP2 compression support
- [Third-party Libraries](./third-party.md) — All dependencies

---

## External Links

- [QuaZip GitHub](https://github.com/stachenov/quazip)
- [Minizip](http://www.winimage.com/zLibDll/minizip.html)
- [Qt Documentation](https://doc.qt.io/)
