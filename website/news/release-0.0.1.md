---
title: ProjT Launcher Update 0.0.1, now available
description: Our first official ProjT Launcher release!
date: 2025-11-16
tags:
  - release
---
We are excited to announce the **first official release of ProjT Launcher: version 0.0.1**!

This marks a major milestone for the project, as it represents the beginning of our fully independent development path after moving away from our Prism Launcher origin.

> **Important Notice:**  
> Microsoft/Mojang authentication is **not yet functional** in this release.  
> The API approval process is still in progress, and required application signatures (code-signing certificates & macOS notarization) have **not been issued yet**.  
> Once API approval and signing certificates are finalized, MSA login will become fully available in a future update.

## Changelog

### Added

- Introduced full ProjT-branded SPDX header system for all newly created source files.
- Added complete codebase restructuring to separate upstream (MultiMC/PolyMC/Prism) code from ProjT-specific modules.
- Implemented custom Qt6-based build configuration with support for QtWayland and Kirigami.
- Added cross-platform build support for **Windows (MSVC & MinGW), Linux, and macOS**.
- Added initial `.app` bundle structure for macOS builds.
- Integrated early OAuth flow preparation for Microsoft/Mojang account login (pending API approval).
- Added detection of `qtlogging.ini` in user and application directories.
- Added full ProjT branding (logos, names, identifiers, contributor credits).
- Introduced Inkscape → PNG → ICO icon pipeline for Windows assets.
- Added custom NixOS development environment (Hyprland, Qt, OBS, JetBrains tools).
- Added initial remote Nix binary cache server infrastructure.
- Added GitHub Pages deployment workflow for the ProjT Launcher website.

### Changed

- Replaced all outdated upstream license headers with preserved upstream blocks + ProjT extensions.
- Migrated build system to a fully Qt6-native toolchain, removing older Qt5/legacy logic.
- Updated the entire CMake tree with modernized target linking and static dependency rules.
- Improved logging during launcher startup to better detect configuration and platform issues.
- Updated project folder naming, identifiers, and contributor credits to ProjT standards.
- Optimized CI to generate artifacts for every commit, following a continuous-release approach.
- Enhanced Windows build pipeline with improved MSVC runtime bundling.
- Reorganized branding assets and icons into a structured ProjT theme.
- Fully detached repository identity from Prism Launcher upstream.

### Fixed

- Fixed zlib module path resolution in the build system.
- Fixed multiple Qt6 dependency issues across Linux and macOS environments.
- Fixed macOS build breaks caused by SDK version mismatches after updating the base image.
- Fixed launcher startup issue where `Platform: unknown` was displayed.
- Fixed GitHub Actions cache misconfigurations leading to failed builds.
- Fixed incorrect Wayland plugin loading order under Hyprland.
- Fixed Windows MinGW builds failing due to missing static libraries.

### Removed

- Removed all Prism Launcher branding and upstream-specific metadata.
- Removed deprecated translation files, unused assets, and outdated modules.
- Removed old CMake configuration fragments incompatible with Qt6.
- Removed upstream CI definitions and replaced them with custom ProjT workflows.

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/commits/0.0.1>

You can [grab the latest download here](/download) for your respective platform.
