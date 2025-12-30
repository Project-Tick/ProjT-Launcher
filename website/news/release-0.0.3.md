---
title: ProjT Launcher Update 0.0.3, now available
description: Our official ProjT Launcher 0.0.3 release!
date: 2025-11-27
tags:
  - release
---

We are excited to announce the **official release of ProjT Launcher: version 0.0.3**!

This release is the largest update since the project began, completing the full **rebranding from Prism Launcher**, introducing an entirely new **Backup System**, and delivering major improvements across packaging, CI, documentation, and platform compatibility.

## Changelog

### Added

- **Full Backup System**  
  A complete backup/restore implementation for managing Minecraft instances safely.  
  Includes:
  - BackupManager core system  
  - BackupPage integration for instance settings  
  - BackupDialog UI  
  - Backup metadata & workflow  
  - Instance list backup indicators  
- **New CMakePresets.json** replacing multiple per-platform preset files, simplifying the build pipeline.  
- **generate_todo_report.py** script for automated TODO scanning and reporting.  
- **APPLE_SILICON_RATIONALE.md** explaining support considerations and technical details for Apple Silicon.

### Changed

- **Complete Rebranding to ProjT Launcher (#5)**  
  - Replaced all Prism references with ProjT branding  
  - Updated CMake project metadata  
  - Updated Application.cpp / Application.h naming and UI elements  
  - New application icons, Big Sur style logos, and updated instance icons  
- **Updated Flatpak Manifest (tr.org.yongdohyun.ProjTLauncher.yml)**  
  - Improved structure, removed legacy modules  
  - Updated runtime integrations and packaging paths  
- **Debian packaging improvements**  
  - Updated control files, install files, and package name transitions  
- **CI/CD Enhancements**  
  - Release builds now clean build directories for consistency  
  - Improved Windows, Linux, macOS build and packaging workflows  
- **Launcher JAR inclusion fix**  
  - Ensured `ProjTLaunch.jar` and `ProjTLaunchLegacy.jar` are included in macOS & Linux packages  
- **General code quality upgrades** across Application startup, updater, assets, auth, network caching, and UI behavior.

### Fixed

- **macOS/Linux missing JAR issue** causing runtime failures  
- **Various UI glitches** in LabeledToolButton, InstanceDelegate, BackupPage  
- **HTTP Meta Cache reliability issues**  
- **Json parsing, MMCZip, copy task stability & asset loading**  
- **Security documentation formatting issues** (SECURITY.md)  
- **Multiple CI workflow failures** due to outdated steps and incorrect dependencies  
- **Nix Flake dependency issues**, updated lockfile & configuration

### Removed

- Legacy build preset files:  
  - `cmake/linuxPreset.json`  
  - `cmake/macosPreset.json`  
  - `cmake/windowsPreset.json`  
- Deprecated Flatpak modules:  
  - `flite.json`  
  - `libdecor.json`  
- Old/incompatible Prism branding assets  
- Obsolete CMake and README fragments from previous architecture

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/compare/0.0.2...0.0.3>

You can [grab the latest download here](/download) for your respective platform.
