<p align="center">
  <picture>
    <source srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
    <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
  </picture>
</p>

<p align="center">
  <img alt="ProjT Launcher Main Menu" src="https://projtlauncher.yongdohyun.org.tr/img/screenshots/launcher.png" width="80%">
</p>

<p align="center">
  <strong>ProjT Launcher</strong><br>
  Minecraft launcher plus website and automation tools in one monorepo.<br>
  <em>A fork of Prism Launcher, intentionally diverging for long-term maintainability.</em>
</p>

<p align="center">
  This fork exists to keep a stable, maintainable launcher with deliberate architecture and long-lived infrastructure.
</p>

<p align="center">
  <a href="#projects">Projects</a> •
  <a href="#launcher-releases">Launcher Releases</a> •
  <a href="#quick-starts">Quick Starts</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a>
</p>

---

## Projects

This repository is a monorepo that hosts the launcher and its supporting services:

- **Launcher app (C++/Qt)**: `launcher/`, `libraries/`, `CMakeLists.txt`
- **Website (Eleventy)**: `src/`, `public/`, `package.json`
- **Automation bot (Cloudflare Workers)**: `bot/`
- **Metadata generator (Python/Poetry)**: `meta/`, `pyproject.toml`
- **Docs/CI/Tools**: `docs/`, `ci/`, `.github/workflows/`, `scripts/`, `tools/`

## What is ProjT Launcher?

ProjT Launcher is a fork of Prism Launcher with a deliberate focus on long-term maintainability, structural clarity, and respect for upstream ecosystems. This is not a race for features or popularity. It is an engineering-first project that prioritizes clean architecture, reproducible builds, disciplined CI, and packaging correctness across Linux, Windows, macOS, Nix, and Flatpak. Sustained maintenance and understandable code take priority over feature velocity.

It intentionally diverges where the original architecture or maintenance model no longer supports sustainable growth. That divergence is a choice, not a statement about upstream quality.

### Project Philosophy

- Stability, clarity, and auditability come before adding features.
- Every change must be traceable and maintainable years later.
- Infrastructure, tooling, CI, and refactoring are first-class contributions.
- Packaging correctness and reproducible builds are non-negotiable work items.
- Development decisions ignore popularity contests and vanity metrics.

### Design Goals

- Maintain a modular architecture with explicit boundaries and minimal hidden coupling.
- Keep CI deterministic, with consistent tooling across platforms and ecosystems.
- Prefer documented, repeatable solutions over clever but fragile shortcuts.
- Keep build and packaging workflows aligned with downstream expectations and policies.

### Who This Project Is For

- Developers who value maintainable codebases and long-lived infrastructure.
- Packagers and distro maintainers who need reproducible builds and clear conventions.
- Contributors who are comfortable with quiet, incremental work: refactors, CI, build tooling.
- Users who prefer stability and predictable behavior over rapid feature churn.

### Who This Project Is Not For

- Anyone prioritizing rapid feature churn over maintainability and clarity.
- Contributors driven by hype, vanity metrics, or appearances of activity.
- Those expecting this fork to track upstream decisions, timelines, or design shortcuts.

## Launcher Releases

<a href="https://repology.org/project/projtlauncher/versions">
  <img src="https://repology.org/badge/vertical-allrepos/projtlauncher.svg" alt="Packaging status" align="right">
</a>

ProjT Launcher is available in various package repositories. Check the badge above for current distributions and versions.

### Stable Options

- **Official Releases**: Download installers/binaries from [GitHub Releases](https://github.com/YongDo-Hyun/ProjT-Launcher/releases).
- **Package Manager**: Install your distro's `projtlauncher` package (see the Repology badge above for availability).
- **Build from Source**: Follow the launcher build steps below for a reproducible release build.

### Development Builds (Unstable)

These are for testing and contributors only:

- **CI Artifacts**: Check GitHub Actions for builds.
- **Nix Flake**: `nix build .#projtlauncher`
- **Local Build**: `cmake -S . -B build && cmake --build build`

## Quick Starts

### Launcher (C++/Qt)

#### Prerequisites

- CMake 3.22+
- Qt 6.x
- C++20 compiler
- Git submodules

#### Quick Build

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

### Website (Eleventy)

```bash
pnpm install
pnpm serve
```

The website source lives in `src/`, and the production output is built into `public/`.

### Bot (Cloudflare Workers)

See [`bot/README.md`](bot/README.md) for secrets, endpoints, and deployment details.

Local smoke test:

```bash
cd bot
wrangler dev
```

### Metadata Generator (Python/Poetry)

ProjT Launcher Meta by Yong Do-Hyun. Scripts to generate JSONs and jars that ProjT Launcher will access. The metadata generator lives in `meta/` and is wired via `pyproject.toml`.

#### Recommended Deployment (CI)

The old Flake-based NixOS deployment is removed. Use the GitHub Actions workflow in `.github/workflows/auto-update.yml` (hourly schedule + manual dispatch).

Secrets supported by the workflow:

- `DEPLOY_PAT` (preferred, must have access to `meta-upstream` and `meta-launcher`)
- `META_BOT_TOKEN` (fallback if no PAT)
- `DEPLOY_SSH_KEY_UPSTREAM` and `DEPLOY_SSH_KEY_LAUNCHER` (SSH alternative)
- `META_UPSTREAM_URL` and `META_LAUNCHER_URL` (optional custom repo URLs)

#### Local Run

Set `META_UPSTREAM_URL` and `META_LAUNCHER_URL` (or define them in `config.sh`), then:

```bash
python -m pip install -r requirements.txt
python -m pip install .
./init.sh
./update.sh
```

## Documentation

Launcher contributor docs:

- [**Getting Started**](docs/contributing/GETTING_STARTED.md): Setup your environment (Windows/Linux/macOS).
- [**Code Style**](docs/contributing/CODE_STYLE.md): Strict rules for C++ and QML.
- [**Project Structure**](docs/contributing/PROJECT_STRUCTURE.md): Where to put your files.
- [**Architecture**](docs/contributing/ARCHITECTURE.md): Understanding MVVM and the Task system.
- [**Workflow**](docs/contributing/WORKFLOW.md): How to submit a Pull Request.
- [**Testing**](docs/contributing/TESTING.md): How to write and run tests.

## Contributing

Start with [`CONTRIBUTING.md`](CONTRIBUTING.md) for engineering standards and workflow. Project-specific notes:

- **Launcher**: `docs/contributing/` and `CMakeLists.txt`
- **Website**: `.eleventy.js`, `src/`, `package.json`
- **Bot**: `bot/README.md`
- **Metadata generator**: `meta/`, `pyproject.toml`

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

[![GPL-3.0-only](https://img.shields.io/badge/license-GPL--3.0--only-C4282D?logo=gnu)](LICENSE)
[![AGPL-3.0-only](https://img.shields.io/badge/license-AGPL--3.0--only-C4282D?logo=gnu)](LICENSE)

Code: GPL-3.0-only (Launcher) / AGPL-3.0-only (Website) / MS-PL (Metadata generation scripts)<br>
Assets: CC BY-SA 4.0

---

<p align="center">Maintained by the <a href="https://github.com/Project-Tick">Project Tick</a> contributors.</p>
