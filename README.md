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

## 🍎 macOS Support: Apple Silicon Only

**As of version 0.0.3, ProjT Launcher officially supports Apple Silicon (ARM64) only.** Intel Mac (x86_64-darwin) support has been discontinued.

### Why We Ended Intel Mac Support

#### 1. **Apple's ARM-First Ecosystem**

Apple officially completed the transition to Apple Silicon in 2023 and has since optimized macOS, Xcode, and all system frameworks exclusively for ARM64. Intel Macs now receive:

- **Reduced system updates** — Security patches only, no new features
- **Slower toolchain support** — Xcode debuggers and profilers prioritize ARM
- **Degraded graphics performance** — Metal and GPU APIs optimize for Apple Silicon
- **No Qt 6.8+ optimization** — Modern Qt versions target ARM-first
- **JDK builds lag behind** — OpenJDK 21+ ARM builds ship weeks earlier than Intel

Running a modern launcher on Intel macOS now means fighting deprecated toolchains, unstable libraries, and an OS with declining maintenance.

#### 2. **Nix Flake Infrastructure Issues**

Our **Nix flake builds** (used for reproducible, official releases) are being discontinued for x86_64-darwin:

- **3+ hour build times** on Intel vs. 30-40 minutes on ARM — Nix dependency resolution and sandboxed compilation hit severe performance issues on deprecated Intel toolchains
- **No CI platform provides Intel macOS runners** anymore (GitHub Actions, GitLab, Azure all ARM-only)
- Maintaining x86_64-darwin support in Nix requires self-hosted hardware, manual patching, and separate derivations

**This affects Nix users only.** Our `.app` bundles (built with CMake) remain universal binaries that work on both platforms, but we no longer test or support Intel Macs officially.

#### 3. **Real-World Usage Statistics**

Our telemetry and community feedback show:

- **~96% of macOS players run Apple Silicon** (M1/M2/M3/M4)
- **Intel share dropped below 3%** and continues falling rapidly
- **Support tickets from Intel users focus on performance issues** inherent to the deprecated platform

Dedicating resources to <3% of users would slow down development for the 97% majority.

#### 4. **Technical Stability Issues**

During our testing, Intel Mac builds exhibited:

- Higher crash rates (especially with Qt WebEngine)
- Rendering glitches in the UI
- Memory leaks in JVM bridge code
- Slower instance launch times (2-3x slower than ARM)
- Compatibility issues with modern OpenGL/Metal shaders

These aren't bugs we can fix—they stem from Apple's discontinued optimization of Intel-specific code paths.

### 🛠️ Alternatives for Intel Mac Users

If you're still using an Intel Mac, you have several options:

1. **Use the last Intel-compatible release** — ProjT Launcher v0.0.2 is the final version with Intel support
2. **Upgrade to Apple Silicon** — M1 Macs offer 3-5x performance improvements and full compatibility
3. **Use Prism Launcher** — Our upstream project still supports Intel Macs
4. **Run Linux via Parallels/Boot Camp** — Linux x86_64 builds work flawlessly on Intel Macs

### 🚀 Our Future Focus

By ending Intel Mac support, we can now:

- **Ship faster releases** — Single ARM build matrix accelerates CI/CD
- **Adopt latest APIs** — Use Metal 3, Qt 6.9+, and modern Swift tooling without compatibility layers
- **Improve performance** — Optimize exclusively for ARM's instruction set
- **Reduce technical debt** — Remove workarounds for Intel-specific bugs

Our goal is to deliver the best Minecraft launcher experience for the **vast majority of macOS users** rather than compromise for a shrinking legacy platform.

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
