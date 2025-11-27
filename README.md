<p align="center">
  <picture>
    <source srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
    <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
  </picture>
</p>

<p align="center">
  <strong>ProjT Launcher</strong><br>
  Custom Minecraft launcher for managing multiple installations with ease.<br>
  <em>A fork of Prism Launcher, evolving into an independent project.</em>
</p>

<p align="center">
  <a href="#installation">Installation</a> •
  <a href="#building">Building</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a>
</p>

---

## What is ProjT Launcher?

ProjT Launcher is a powerful, user-friendly launcher for Minecraft that lets you manage multiple game instances effortlessly. Originally forked from Prism Launcher, it's now an independent project with its own vision and features.

### Key Features

- **Backup System**: Granular backups for saves, configs, mods, and more.
- **Performance Optimized**: Faster UI, quicker launches, lower memory usage.
- **Apple Silicon Native**: ARM64 builds for macOS without Rosetta.
- **Future-Ready**: Mod profiles, cloud sync, and performance profiling coming soon.
- **Prism Compatible**: Import and use existing Prism instances seamlessly.

### Goals

- Establish independent infrastructure and branding.
- Migrate to QML-based UI for better performance and modern design.
- Expand modding support and community features.
- Ensure cross-platform compatibility and native builds.

> **Note**: Infrastructure is still being built. Some links/assets may point to Prism Launcher temporarily.

## Installation

<a href="https://repology.org/project/projtlauncher/versions">
  <img src="https://repology.org/badge/vertical-allrepos/projtlauncher.svg" alt="Packaging status" align="right">
</a>

Official releases aren't available yet. Use development builds or build from source.

### Development Builds

These are unstable and for testing only.

- **Local Build**: `cmake -S . -B build && cmake --build build`
- **Nix Flake**: `nix build .#projtlauncher`
- **CI Artifacts**: Check GitHub Actions for builds.

## Building from Source

### Prerequisites

- CMake 3.22+
- Qt 6.x
- C++20 compiler
- Git submodules

### Quick Build

```bash
git clone --recursive https://github.com/YongDo-Hyun/ProjT-Launcher.git
cd ProjT-Launcher
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

For detailed instructions:

- [Nix Flake](nix/README.md)
- [CMake Guide](CMakeLists.txt)
- [GitHub Actions](.github/workflows/)

## macOS Notes

- ARM64 (Apple Silicon) is prioritized.
- Universal binaries work on Intel macOS but are deprecated.
- See [APPLE_SILICON_RATIONALE.md](docs/APPLE_SILICON_RATIONALE.md) for details.

## Community & Support

- **Issues**: Report bugs or suggest features on GitHub.
- **Translations**: Use upstream Prism strings for now; own workflow coming.
- **Community Spaces**: Discord/Matrix channels announced soon.

## Forking & Redistribution

You can fork and redistribute freely under the GPL-3.0 license. For custom builds:

- Clearly state it's not official ProjT Launcher.
- Change API keys in `CMakeLists.txt` to your own or disable them.
- Set `Launcher_BUILD_PLATFORM` for distributions (e.g., `archlinux`).

Building with default API keys implies acceptance of:

- [Microsoft Identity Platform Terms](https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use)
- [CurseForge API Terms](https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions)

## License

[![GPL-3.0-or-later](https://img.shields.io/badge/license-GPL--3.0--or--later-C4282D?logo=gnu)](LICENSE)

Code: GPL-3.0-or-later / GPL-3.0-only  
Assets: CC BY-SA 4.0

---

<p align="center">Made with love for the Minecraft community.</p>
