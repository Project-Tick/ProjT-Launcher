<p align="center">
  <picture>
    <source srcset="/program_info/tr.org.projecttick.ProjTLauncher.logo-darkmode.svg">
    <img alt="ProjT Launcher" src="/program_info/tr.org.projecttick.ProjTLauncher.logo.svg" width="40%">
  </picture>
</p>

<p align="center">
  <img alt="ProjT Launcher Main Menu" src="https://projtlauncher.projecttick.org.tr/img/screenshots/projtlauncher_dark_main_window.png" width="80%">
</p>

<p align="center">
  <strong>ProjT Launcher</strong><br>
  A Minecraft launcher and automation stack that helps maintainers avoid missed updates,
  security drift, and broken metadata in long-lived projects.<br>
  <em>A fork of Prism Launcher, diverging on purpose to keep maintenance and infrastructure healthy over time.</em>
</p>

<p align="center">
  Built for projects that rely on CI, automation, and long-term maintenance,
  often maintained by small teams or individuals.
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

This repo is a monorepo with the launcher and the pieces that keep it running:

- **Launcher app (C++/Qt)**: `launcher/`, `CMakeLists.txt`
- **Website (Eleventy)**: `website/`, `public/`, `package.json`
- **Automation bot (Cloudflare Workers)**: `bot/`
- **Metadata generator (Python/Poetry)**: `meta/`, `pyproject.toml`
- **Docs/CI/Tools**: `docs/`, `ci/`, `.github/workflows/`, `scripts/`, `tools/`
- **Forked libraries**: `bzip2/`, `quazip/`, `zlib/`, `libnbtplusplus/`
- **Bundled libraries**: `gamemode/`, `LocalPeer/`, `murmur2/`, `qdcss/`, `rainbow/`, `systeminfo/`
- **Internal libraries**: `launcherjava/`

> [!Warning]
> Forks are expected; large rebundling or CI changes should clearly document intent.

## Reading Guide

> This README serves as a high-level entry point to the ProjT Launcher monorepo.
> Detailed technical and maintainer documentation lives under `docs/handbook/`.

This README serves multiple audiences and use cases.

- **Users**:  
  If you are looking to install or use ProjT Launcher, focus on:
  - *Launcher Releases*
  - *Quick Starts*
  - *Community & Support*

- **Contributors**:  
  If you plan to contribute code or documentation, read:
  - *What is ProjT Launcher?*
  - *Project Philosophy*
  - *Design Goals*
  - *Documentation*
  - *Contributing*

- **Packagers / Maintainers / Reviewers**:  
  If you are reviewing this repository for distribution, licensing,
  or long-term maintenance, the following sections are relevant:
  - *Forked Libraries (Detailed)*
  - *CI & Automation*
  - *License for ProjT Launcher*

Sections documenting forked upstream projects are intentionally
detailed and may be skipped by readers only interested in usage.

## What is ProjT Launcher?

ProjT Launcher is a fork of Prism Launcher with a deliberate focus on long-term maintainability, structural clarity, and respect for upstream ecosystems. We care more about boring reliability than shipping features fast. Clean architecture, reproducible builds, disciplined CI, and packaging correctness across Linux, Windows, macOS, Nix, and Flatpak come first.

We keep the core experience familiar and diverge only when the original architecture or maintenance model makes long-term upkeep harder. That divergence is a practical choice, not a statement about upstream quality.

### Project Philosophy

- Stability, clarity, and auditability come before new features.
- Changes should be traceable and maintainable years later.
- Infrastructure, tooling, CI, and refactors are first-class work.
- Packaging correctness and reproducible builds are non-negotiable.
- We prioritize long-term maintainability over short-term feature velocity.

### Design Goals

- Maintain a modular architecture with clear boundaries and minimal hidden coupling.
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
- Those expecting this fork to track upstream decisions, timelines, or design shortcuts.
- If you want the fastest new features, upstream may be a better fit.

## Launcher Releases

<a href="https://repology.org/project/projtlauncher/versions">
  <img src="https://repology.org/badge/vertical-allrepos/projtlauncher.svg" alt="Packaging status" align="right">
</a>

ProjT Launcher is available in various package repositories. Check the badge above for current distributions and versions.

> [!NOTE]
> Distribution availability varies and is maintained independently by downstream packagers.

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

The website source lives in `website/`, and the production output is built into `public/`.

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
- [**Code Style**](docs/contributing/CODE_STYLE.md): Strict rules for C++ and Qt Widgets.
- [**Project Structure**](docs/contributing/PROJECT_STRUCTURE.md): Where to put your files.
- [**Architecture**](docs/contributing/ARCHITECTURE.md): Understanding MVVM and the Task system.
- [**Architecture (Detailed)**](docs/architecture/OVERVIEW.md): A guided tour of modules, data flow, and contracts.
- [**Workflow**](docs/contributing/WORKFLOW.md): How to submit a Pull Request.
- [**Testing**](docs/contributing/TESTING.md): How to write and run tests.

## Handbook

Detailed maintainer and infrastructure documentation lives under:

**[`docs/handbook/`](docs/handbook)**

This includes:

- Forked libraries (bzip2, quazip, zlib, tomlplusplus, libnbt++)
- CI & automation internals
- Bot infrastructure
- Packaging and distribution details

## Contributing

Start with [`CONTRIBUTING.md`](CONTRIBUTING.md) for engineering standards and workflow. Project-specific notes:

- **Launcher**: `docs/contributing/` and `CMakeLists.txt`
- **Website**: `.eleventy.js`, `website/`, `package.json`
  - To sync the website wiki to GitHub Wiki, use: `./scripts/sync-wiki.sh`
- **Bot**: `bot/README.md`
- **Metadata generator**: `meta/`, `pyproject.toml`

## macOS Notes

- ARM64 (Apple Silicon) is prioritized.
- Universal binaries work on Intel macOS but are deprecated.
- See [APPLE_SILICON_RATIONALE.md](docs/APPLE_SILICON_RATIONALE.md) for details.

## Community & Support

- **Issues**: Report bugs or suggest features on GitHub.
- **Translations**: Use upstream Prism strings for now; own workflow coming.
- **Community Spaces**: Community channels may be announced in the future.

## Forking & Redistribution

You can fork and redistribute freely under the GPL-3.0 license. For custom builds:

- Clearly state it's not official ProjT Launcher.
- Change API keys in `CMakeLists.txt` to your own or disable them.
- Set `Launcher_BUILD_PLATFORM` for distributions (e.g., `archlinux`).

Building with default API keys implies acceptance of:

- [Microsoft Identity Platform Terms](https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use)
- [CurseForge API Terms](https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions)

## Third-party Libraries (Summary)

ProjT Launcher embeds or bundles several third-party libraries
for performance, compatibility, and long-term maintenance.

See `docs/handbook/third-party.md` for the full list and licenses.

## License for ProjT Launcher

Code: [![GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-C4282D?logo=gnu)](LICENSE)
<br>
Assets: CC BY-SA 4.0

## License for Website

Code: [![AGPL-3.0](https://img.shields.io/badge/license-AGPL--3.0-C4282D?logo=gnu)](website/LICENSE)
Assets: CC BY-SA 4.0

## License for Metadata Generator

Code: [![MS-PL](https://img.shields.io/badge/license-MS--PL-blue.svg)](meta/LICENSE)

---

<p align="center">Maintained by the <a href="https://github.com/Project-Tick">Project Tick</a> contributors.</p>
