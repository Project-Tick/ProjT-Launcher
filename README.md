# ProjT Launcher

A Minecraft launcher engineered for long-term maintainability, architectural clarity,
and controlled ecosystem evolution.

ProjT Launcher is a structurally disciplined fork of Prism Launcher, diverging
intentionally to prevent maintenance decay, dependency drift, and architectural
erosion over time.

## Why ProjT Launcher?

- **Long-term maintainability**  
  Explicit architectural constraints and review rules prevent uncontrolled technical debt.

- **Controlled third-party integration**  
  External dependencies are maintained as detached forks with documented patch and update policies.

- **Deterministic CI and builds**  
  Exact dependency versions and constrained build inputs enable reproducible builds across environments.

- **Structural clarity**  
  Enforced MVVM boundaries and clearly separated modules simplify review, refactoring, and long-term contribution.

## Download

- [Releases](https://github.com/Project-Tick/ProjT-Launcher/releases) – Stable builds only. Nightly builds are not provided.
- [Website](https://projecttick.org/projtlauncher/)

[![Packaging status](https://repology.org/badge/vertical-allrepos/projtlauncher.svg)](https://repology.org/project/projtlauncher/versions)

## Build

Quick start for release builds:

```sh
git clone --recursive https://github.com/Project-Tick/ProjT-Launcher.git
cd ProjT-Launcher
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For development setup with presets and full tooling, see [GETTING_STARTED.md](docs/contributing/GETTING_STARTED.md).

### Requirements

| Tool | Version |
| ---- | ------- |
| CMake | 3.22+ |
| Qt | 6.10.x |
| Compiler | C++20 |

### Nix

```sh
nix build .#projtlauncher
```

## Structure

```yaml
launcher/       Application (C++/Qt)
website/        Website (Eleventy)
bot/            Automation (Cloudflare Workers)
meta/           Metadata generator (Python)
docs/           Documentation
```

### Detached Fork Libraries

```yaml
zlib/           Compression
bzip2/          Compression
quazip/         ZIP handling
cmark/          Markdown parsing
tomlplusplus/   TOML parsing
libqrencode/    QR codes
libnbtplusplus/ NBT format
```

> **Note**: These directories contain original upstream READMEs preserved for reference.
> For Project Tick–specific documentation, see [docs/handbook/](docs/handbook/).

### Vendored Libraries

```yaml
gamemode/       GameMode integration
LocalPeer/      Single instance
murmur2/        Hash functions
qdcss/          Dark CSS
rainbow/        Terminal colors
systeminfo/     System info
```

## Documentation

- [Contributing](CONTRIBUTING.md)
- [Getting Started](docs/contributing/GETTING_STARTED.md)
- [Code Style](docs/contributing/CODE_STYLE.md)
- [Architecture](docs/contributing/ARCHITECTURE.md)
- [Developer Handbook](docs/handbook/)

## License

Multiple licenses apply to different components:

- Launcher: [GPL-3.0-only](LICENSE)
- Website: [AGPL-3.0-only](website/LICENSE)
- Metadata: [MS-PL](meta/LICENSE)

Contributions to each component are licensed under its respective license. See [COPYING.md](COPYING.md) for details.

## Links

- [Website](https://projecttick.org/projtlauncher/)
- [Project Tick Website](https://projecttick.org/)
- [Issues](https://github.com/Project-Tick/ProjT-Launcher/issues)
- [Discussions](https://github.com/Project-Tick/ProjT-Launcher/discussions)

---

Maintained by [Project Tick](https://github.com/Project-Tick). Built with discipline, maintained for sustainability.
