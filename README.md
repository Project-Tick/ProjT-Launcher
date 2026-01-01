<p align="center">
  <picture>
    <source srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
    <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
  </picture>
</p>

<p align="center">
  <img alt="ProjT Launcher Main Menu" src="https://projtlauncher.yongdohyun.org.tr/img/screenshots/projtlauncher_dark_main_window.png" width="80%">
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
- **Forked & Bundled libraries**: `bzip2/`, `quazip/`, `zlib/`

> [!Warning]
> Forks are expected; large rebundling or CI changes should clearly document intent.

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

## Third-party libraries

This section lists third‑party/external libraries required by other components in this repository. Each entry includes the library name, its purpose in the repo, license information, and a source URL.

### bzip2

Lossless compression library used for BZip2 support.
This repository contains a fork of the upstream project and it now advances in its own tree; see upstream for the original source.
License: bzip2 license (BSD-like). See `bzip2/COPYING`.
Forked Source: [https://gitlab.com/bzip2/bzip2](https://gitlab.com/bzip2/bzip2)
Local docs: `bzip2/README.md`, `bzip2/COMPILING.md`, `bzip2/NEWS.md`, `bzip2/code-of-conduct.md`

### quazip

Qt wrapper around minizip used for ZIP read/write support.
This repository contains a fork of the upstream project and it now advances in its own tree; see upstream for the original source.
License: LGPL-2.1 with static linking exception; minizip parts are zlib license. See `quazip/COPYING`.
Forked Source: [https://github.com/stachenov/quazip](https://github.com/stachenov/quazip)
Local docs: `quazip/README.md`, `quazip/CONTRIBUTING.md`, `quazip/SECURITY.md`, `quazip/QuaZip-1.x-migration.md`

### zlib

General-purpose lossless data-compression library used by ZIP/GZip flows.
This repository contains an integrated copy of the upstream project.
License: zlib license. See `zlib/LICENSE`.
Source: [https://zlib.net](https://zlib.net)
Forked Github Repository: [https://github.com/madler/zlib](https://github.com/madler/zlib)
Local docs: `zlib/README`, `zlib/FAQ`, `zlib/ChangeLog`

### gamemode

A performance optimization daemon.

See [github repo](https://github.com/FeralInteractive/gamemode).

[![License: BSD-3-Clause](https://img.shields.io/badge/License-BSD--3--Clause-AB2B28?logo=bsd)] licensed

### javacheck

Simple Java tool that prints the JVM details - version and platform bitness.

Do what you want with it. It is so trivial that noone cares.

### launcherjava

Java launcher part for Minecraft.

It does the following:

- Waits for a launch script on stdin.
- Consumes the launch script you feed it.
- Proceeds with launch when it gets the `launcher` command.

If "abort" is sent, the process will exit.

This means the process is essentially idle until the final command is sent. You can, for example, attach a profiler before you send it.

The `standard` and `legacy` launchers are available.

- `standard` can handle launching any Minecraft version, at the cost of some extra features `legacy` enables (custom window icon and title).
- `legacy` is intended for use with Minecraft versions < 1.6 and is deprecated.

Example (some parts have been censored):

```text
mod legacyjavafixer-1.0
mainClass net.minecraft.launchwrapper.Launch
param --username
param CENSORED
param --version
param ProjT Launcher
param --gameDir
param /home/peterix/minecraft/FTB/17ForgeTest/minecraft
param --assetsDir
param /home/peterix/minecraft/mmc5/assets
param --assetIndex
param 1.7.10
param --uuid
param CENSORED
param --accessToken
param CENSORED
param --userProperties
param {}
param --userType
param mojang
param --tweakClass
param cpw.mods.fml.common.launcher.FMLTweaker
windowTitle ProjT Launcher: 172ForgeTest
windowParams 854x480
userName CENSORED
sessionId token:CENSORED:CENSORED
launcher standard
```

Available under [![GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-C4282D?logo=gnu)](LICENSE) (with classpath exception), sublicensed from its original [![Apache-2.0](https://img.shields.io/badge/license-Apache--2.0-D22128?logo=apache)](LICENSE) codebase

### libnbtplusplus

libnbt++ is a free C++ library for Minecraft's file format Named Binary Tag (NBT). It can read and write compressed and uncompressed NBT files and provides a code interface for working with NBT data.

See [github repo](https://github.com/ljfa-ag/libnbtplusplus).

Available either under [![GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-C4282D?logo=gnu)](libnbtplusplus/COPYING).

### LocalPeer

Library for making only one instance of the application run at all times.

![License: BSD](https://img.shields.io/badge/License-BSD-blue.svg) licensed, derived from [QtSingleApplication](https://github.com/qtproject/qt-solutions/tree/master/qtsingleapplication).

Changes are made to make the code more generic and useful in less usual conditions.

### murmur2

Canonical implementation of the murmur2 hash, taken from [SMHasher](https://github.com/aappleby/smhasher).

Public domain (the author disclaimed the copyright).

### rainbow

Color functions extracted from [KGuiAddons](https://inqlude.org/libraries/kguiaddons.html). Used for adaptive text coloring.

Available either under [![LGPL-2.1-or-later](https://img.shields.io/badge/license-LGPL--2.1--or--later-C4282D?logo=gnu)].

### systeminfo

A ProjT Launcher-specific library for probing system information.

[![Apache-2.0](https://img.shields.io/badge/license-Apache--2.0-D22128?logo=apache)]

### qdcss

A quick and dirty css parser, used by NilLoader to store mod metadata.

Translated (and heavily trimmed down) from [the original Java code](https://github.com/unascribed/NilLoader/blob/trunk/website/main/java/nilloader/api/lib/qdcss/QDCSS.java) from NilLoader

Licensed under [![LGPL-3.0](https://img.shields.io/badge/license-LGPL--3.0-C4282D?logo=gnu)].

## License for ProjT Launcher

Code: [![GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-C4282D?logo=gnu)](LICENSE) (Launcher) / [![AGPL-3.0](https://img.shields.io/badge/license-AGPL--3.0-C4282D?logo=gnu)](website/LICENSE) (Website) / [![MS-PL](https://img.shields.io/badge/license-MS--PL-blue.svg)](meta/LICENSE) (Metadata generation scripts)<br>
Assets: CC BY-SA 4.0

---

<p align="center">Maintained by the <a href="https://github.com/Project-Tick">Project Tick</a> contributors.</p>
