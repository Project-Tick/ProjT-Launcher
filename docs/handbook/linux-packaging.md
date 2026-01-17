# Linux Packaging

> **Location**: `docs/packaging/os-specific/linux/`  
> **Platforms**: Nix, Flatpak, distribution packages
> **Latest Version**: 0.0.5-1

---

## Overview

ProjT Launcher supports multiple Linux packaging formats to reach different user bases and distribution requirements.

---

## Packaging Formats

### Nix / NixOS

The recommended method for reproducible builds.

**Location**: `nix/`, `flake.nix`

```sh
# Build
nix build .#projtlauncher

# Run without installing
nix run .#projtlauncher

# Development shell
nix develop
```

See [nix.md](./nix.md) for detailed instructions.

### Flatpak

Sandboxed application format.

**Location**: `docs/packaging/os-specific/linux/flathub/`

The Flatpak manifest is maintained separately for Flathub submission.

### Distribution Packages

ProjT Launcher is packaged for various distributions:

| Distribution | Package | Status |
|--------------|---------|--------|
| Arch Linux (AUR) | `projtlauncher` | Community |
| Fedora COPR | `projtlauncher` | Community |
| openSUSE OBS | `projtlauncher` | Community |

Check [Repology](https://repology.org/project/projtlauncher/versions) for current availability.

---

## Building for Distribution

### Requirements

- CMake 3.22+
- Qt 6.x
- C++20 compiler
- Ninja (recommended)

### Build Commands

```sh
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DLauncher_BUILD_PLATFORM=linux
cmake --build build
DESTDIR="$pkgdir" cmake --install build
```

### Platform Identifier

Set `Launcher_BUILD_PLATFORM` to identify your distribution:

```cmake
-DLauncher_BUILD_PLATFORM=archlinux
-DLauncher_BUILD_PLATFORM=fedora
-DLauncher_BUILD_PLATFORM=flatpak
```

---

## Desktop Integration

### Desktop File

Installed to `/usr/share/applications/`:

```
org.projecttick.ProjTLauncher.desktop
```

### Icon

Installed to `/usr/share/icons/hicolor/`:

```
org.projecttick.ProjTLauncher.svg
```

### AppStream Metadata

Installed to `/usr/share/metainfo/`:

```
org.projecttick.ProjTLauncher.metainfo.xml
```

---

## Related Documentation

- [Nix Packaging](./nix.md)
- [Program Info](./program_info.md) - Branding configuration
