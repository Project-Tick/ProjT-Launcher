# ProjT Launcher

A Minecraft launcher focused on long-term maintainability and structural clarity.

Fork of [Prism Launcher](https://prismlauncher.org), diverging to keep maintenance healthy over time.

## Why ProjT Launcher?

- **Long-term maintainability**: Strict architectural discipline prevents technical debt accumulation
- **Controlled third-party handling**: All external dependencies are detached forks with documented patch policies
- **CI determinism**: Exact version requirements ensure reproducible builds across all environments
- **Structural clarity**: MVVM enforcement and clear module boundaries simplify contribution and review

## Download

- [Releases](https://github.com/YongDo-Hyun/ProjT-Launcher/releases) – Stable builds only. Nightly builds are not provided.
- [Website](https://projecttick.org/projtlauncher/)

[![Packaging status](https://repology.org/badge/vertical-allrepos/projtlauncher.svg)](https://repology.org/project/projtlauncher/versions)

## Build

Quick start for release builds:

```sh
git clone --recursive https://github.com/YongDo-Hyun/ProjT-Launcher.git
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
- [Issues](https://github.com/YongDo-Hyun/ProjT-Launcher/issues)
- [Discussions](https://github.com/YongDo-Hyun/ProjT-Launcher/discussions)

---

Maintained by [Project Tick](https://github.com/Project-Tick). Built with discipline, maintained for sustainability.
