<p align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
  <source media="(prefers-color-scheme: light)" srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg">
  <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
</picture>
</p>

<p align="center">
  ProjT Launcher is a custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once.<br />
  <br />This project started as a <b>fork</b> of Prism Launcher/MultiMC and is <b>not</b> endorsed by either upstream project.
</p>

> **Heads-up**  
> ProjT Launcher is still establishing its own infrastructure. Some assets and links may temporarily point to upstream Prism Launcher resources until we finish migrating.

## Installation
<!--
<a href="https://repology.org/project/prismlauncher/versions">
    <img src="https://repology.org/badge/vertical-allrepos/prismlauncher.svg" alt="Packaging status" align="right">
</a>
 -->
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

## ⚠️ Deprecation Notice: End of Support for macOS Intel (x86_64-darwin)

Beginning with upcoming releases, ProjT Launcher will no longer provide builds or official support for macOS Intel (x86_64-darwin).
Only Apple Silicon (aarch64-darwin) builds will continue to be maintained.

This decision aligns with both industry trends and Apple's official roadmap, where macOS 27 is expected to be the final version supporting Intel-based Macs.

### 🛑 Why Intel macOS support is being discontinued

1. Apple has shifted entirely to ARM

Apple ended the transition period and now focuses all macOS, Xcode, and system-level optimizations on Apple Silicon.
Intel Macs receive reduced testing, slower updates, and soon no OS-level innovation.

1. Xcode and modern toolchains are ARM-first

The latest development ecosystem is optimized for ARM64:

Xcode toolchains

Swift shader pipelines

Metal APIs

Qt 6.6+

JDK 21 (LTS)

OpenJDK builds

WebEngine / Chromium dependencies

Intel builds are becoming unstable, slower, and increasingly unsupported.

1. CI environments no longer provide Intel macOS builders

GitHub Actions, GitLab, Azure Pipelines, and most cloud CI platforms support only ARM-based macOS runners.
Maintaining Intel builds now requires:

Private, self-hosted machines

Manual patching

Non-standard environment setup

This greatly slows down development and increases maintenance cost.

1. Performance and stability issues on Intel Macs

During testing, Intel builds showed:

Higher crash rates

Inconsistent Qt rendering

Lower performance in WebEngine

Slower JVM startup

Memory pressure issues with GPU acceleration

Apple Silicon builds avoid all of these.

1. Extremely low usage share

Globally, over 95% of active macOS players run Apple Silicon hardware (M1, M2, M3, M4).
Intel user share is now below 3–4%, continuing to fall rapidly.

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
