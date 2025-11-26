<p align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
  <source media="(prefers-color-scheme: light)" srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg">
  <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
</picture>
</p>

<p align="center">
  ProjT Launcher is a custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once.<br />
  <br />This project <b>originated as a fork</b> of Prism Launcher but is now <b>evolving into an independent project</b> with its own roadmap and features.
</p>

## 🚀 Why ProjT Launcher?

As we move forward as an independent project, ProjT Launcher is diverging from Prism Launcher with features designed for modern workflows:

- **Backup System** — Create, restore, and manage instance backups with granular control (saves, configs, mods, etc.)
- **Performance-First** — Optimized UI rendering, faster instance launching, reduced memory overhead
- **Apple Silicon Priority** — Native ARM64 builds for macOS, no Rosetta translation overhead
- **Future Features** — Mod profile system, cloud sync, performance profiler (coming soon)

While we maintain compatibility with Prism instances, **ProjT Launcher is charting its own path** to deliver unique value to the community.

> **Heads-up**  
> ProjT Launcher is still establishing its own infrastructure. Some assets and links may temporarily point to upstream Prism Launcher resources until we finish migrating.

## Installation

<a href="https://repology.org/project/projtlauncher/versions">
    <img src="https://repology.org/badge/vertical-allrepos/projtlauncher.svg" alt="Packaging status" align="right">
</a>

- Official ProjT Launcher downloads are not published yet; please follow the build instructions below or use the provided Nix flake.
- Build and test status is available from this repository's GitHub Actions tab.

### Development Builds

Please understand that these builds are not intended for most users. There may be bugs and other instabilities. You have been warned.

Automated development builds for ProjT Launcher are still being wired up. In the meantime you can:

- Build locally via CMake (`cmake -S launcher -B build && cmake --build build`).
- Use Nix with `nix build .#projtlauncher` (the package name will be renamed once the rebranding work lands everywhere).
- Consume artifacts produced via this repository's GitHub Actions runs.

## Community & Support

Feel free to create a GitHub issue if you find a bug or want to suggest a new feature. Dedicated community spaces for ProjT Launcher (Discord, Matrix, etc.) will be announced once they are ready—thanks for your patience while we set them up.

## Translations

We are preparing our own translation workflow for ProjT Launcher. Until it is available you can temporarily rely on the upstream ProjT Launcher strings and contribute improvements through this repository.

## Building

If you want to build ProjT Launcher yourself, check out:

- [`nix/README.md`](nix/README.md) for flake-based builds.
- [`CMakeLists.txt`](CMakeLists.txt) plus the `cmake/` helpers for a traditional CMake toolchain.
- The GitHub Actions workflows (coming soon) for concrete build examples on each platform.

## 🍎 macOS Support

As of v0.0.3 the project targets Apple Silicon (ARM64) for official builds and day-to-day testing. Universal `.app` bundles produced via CMake remain generally compatible with Intel macOS (x86_64), but Intel-specific build pipelines (notably Nix flake builds) and active Intel testing are deprecated.

For the full technical rationale, migration notes, and alternatives for Intel users, see `docs/APPLE_SILICON_RATIONALE.md`.

## Sponsors & Partners

We are currently self-funding ProjT Launcher while things are being set up. If you're interested in sponsoring infrastructure or tooling, please reach out via GitHub issues so we can coordinate once public tiers are available.

## Forking/Redistributing/Custom builds policy

You are free to fork, redistribute and provide custom builds as long as you follow the terms of the [license](LICENSE) (this is a legal responsibility), and if you made code changes rather than just packaging a custom build, please do the following as a basic courtesy:

- Make it clear that your fork is not ProjT Launcher and is not endorsed by or affiliated with the ProjT Launcher project.
- Go through [CMakeLists.txt](CMakeLists.txt) and change ProjT Launcher's API keys to your own or set them to empty strings (`""`) to disable them (this way the program will still compile but the functionality requiring those keys will be disabled).

If you have any questions or want any clarification on the above conditions please make an issue and ask us.

If you are just building ProjT Launcher for your distribution, please make sure to set the `Launcher_BUILD_PLATFORM` to a slug representing your distribution. Examples are `archlinux`, `fedora` and `nixpkgs`.

Note that if you build this software without removing the provided API keys in [CMakeLists.txt](CMakeLists.txt) you are accepting the following terms and conditions:

- [Microsoft Identity Platform Terms of Use](https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use)
- [CurseForge 3rd Party API Terms and Conditions](https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions)

If you do not agree with these terms and conditions, then remove the associated API keys from the [CMakeLists.txt](CMakeLists.txt) file by setting them to an empty string (`""`).

## License [![GPL-3.0-only](https://img.shields.io/badge/license-GPL--3.0--only-C4282D?logo=gnu)](LICENSE)

All launcher code is available under the GPL-3.0-only license.

The logo and related assets are under the CC BY-SA 4.0 license.
