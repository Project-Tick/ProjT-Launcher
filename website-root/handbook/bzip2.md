# Bzip2 `bzip2/`

> **Type**: Compression Library  
> **License**: bzip2 License (BSD-like)  
> **Fork Origin**: [GitLab](https://gitlab.com/bzip2/bzip2) | [Sourceware](https://sourceware.org/bzip2/)  
> **Status**: Detached Fork (independently maintained)
> **Latest Version**: 0.0.5-1

---

## Overview

Bzip2/libbz2 is a program and library for lossless, block-sorting data compression. It typically compresses files to within 10% to 15% of the best available techniques (PPM family), while being around twice as fast at compression and six times faster at decompression.

This repository contains a maintained fork that now advances in its own tree while staying compatible with upstream.

---

## Usage in ProjT Launcher

Bzip2 is used for:

- **Mod archive extraction** — Some modpacks use bzip2 compression
- **Legacy support** — Older Minecraft assets may use bzip2
- **Download optimization** — Efficient compression for large files

---

## Documentation

| Resource | Description |
|----------|-------------|
| [Compiling Guide](./bzip2-compiling.md) | Build instructions (Meson & CMake) |
| [Test Suite](./bzip2-tests.md) | Quick and large test suites |
| [Test Files](./bzip2-testfiles.md) | Test file collection |
| [Original Manual](../../bzip2/bzip2.txt) | Upstream plain-text manual |
| `bzip2/manual.html` | Full API documentation |

---

## Build Systems

Bzip2 supports two build systems:

| Build System | Platform Support | Recommended For |
|--------------|------------------|-----------------|
| **Meson** | Unix, Windows | Unix-like systems |
| **CMake** | Unix, Windows | Cross-platform builds |

For detailed build instructions, see [bzip2-compiling.md](./bzip2-compiling.md).

### Quick Build (CMake)

```bash
cd bzip2
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest -V
```

---

## Contributing

The bzip2 development happens in the ProjT Launcher repository:

| Branch | Purpose |
|--------|---------|
| `develop` | Main development branch |
| `release-*` | Stable release branches |
| Feature branches | Experimental work (may be rebased) |

### Report Issues

Please report bugs via [GitHub Issues](https://github.com/Project-Tick/ProjT-Launcher/issues).

---

## ⚠️ Important Notices

### Warning

This program and library compresses data by performing several non-trivial transformations. Unless you are 100% familiar with *all* the algorithms contained herein, you should **NOT** modify the compression or decompression machinery. Incorrect changes can lead to disastrous loss of data.

### Disclaimer

**NO RESPONSIBILITY IS TAKEN FOR ANY LOSS OF DATA ARISING FROM THE USE OF THIS PROGRAM/LIBRARY.**

The complexity of the algorithms makes it impossible to rule out the possibility of bugs. Do not compress critical data without backups.

### Patents

To the best of our knowledge, bzip2/libbz2 does not use any patented algorithms. However, no patent search has been conducted, so no guarantee can be given.

---

## Copyright & Licensing

```
Copyright (C) 1996-2010 Julian Seward <jseward@acm.org>
Copyright (C) 2019-2020 Federico Mena Quintero <federico@gnome.org>
Copyright (C) 2021-2025 Micah Snyder
Copyright (C) 2025 YongDo-Hyun
Copyright (C) 2025 grxtor
```

Licensed under the bzip2 license (BSD-like). See `bzip2/COPYING` for full text.

---

## Related Documentation

- [zlib](./zlib.md) — Alternative compression library
- [QuaZip](./quazip.md) — ZIP archive wrapper
- [Third-party Libraries](./third-party.md) — All external dependencies

---

## External Links

- [Bzip2 GitLab](https://gitlab.com/bzip2/bzip2)
- [Bzip2 Sourceware](https://sourceware.org/bzip2/)
- [Wikipedia: Bzip2](https://en.wikipedia.org/wiki/Bzip2)
