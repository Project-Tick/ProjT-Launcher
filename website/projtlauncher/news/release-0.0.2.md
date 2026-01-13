---
title: ProjT Launcher Update 0.0.2, now available
description: Our official ProjT Launcher release!
date: 2025-11-16
tags:
  - release
---

We are excited to announce the **official release of ProjT Launcher: version 0.0.2**!

This marks a major milestone for the project, as it represents the beginning of our fully independent development path after moving away from our Prism Launcher origin.

## Changelog

### Added

- Debian packaging support: Added complete .deb package structure with control files, changelog, installation rules, and build scripts for Debian-based distributions.
- Pull Request template: Introduced structured PR template for improved contribution workflow.
- Git submodules for Java components: Migrated javacheck and launcher Java modules to dedicated Git submodules for cleaner repository structure.
- Enhanced Nix flake configuration with improved GitHub API authentication to prevent rate limiting.

### Changed

- Submodule architecture: Converted internal Java libraries (javacheck, launcher) from in-tree code to Git submodules, removing 3000+ lines of duplicated code.
- Library updates: Updated quazip, cmark, extra-cmake-modules, and tomlplusplus submodules to latest stable versions.
- Flatpak configuration: Updated Flatpak manifest with 47 improvements for better sandboxing and runtime compatibility.
- MetaInfo updates: Refreshed AppStream metadata with updated descriptions and compatibility information (38 changes).
- Improved GitHub Actions Nix workflow with proper token authentication to avoid API rate limits.
- Enhanced build system stability with better dependency resolution.

### Fixed

- Fixed launcher part launch compatibility issues in Minecraft integration layer.
- Fixed .gitignore patterns to better handle build artifacts and temporary files.
- Improved Nix binary cache configuration for faster builds.
- Fixed submodule reference issues in Flatpak shared-modules.

### Removed

- Removed in-tree javacheck Java source files (now submodule): JavaCheck.java, CMakeLists.txt.
- Removed in-tree launcher Java implementation (now submodule): Entire org.prismlauncher package structure including:
--Legacy Minecraft launcher compatibility layer (220+ lines)
--Legacy frame and proxy implementations (400+ lines)
--Online fixes and skin handling (400+ lines)
--JSON parser and utility classes (800+ lines)
--Entry point and launcher implementations (600+ lines)
--Total: 3000+ lines migrated to dedicated repositories for better version control.

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/compare/0.0.1...0.0.2>

You can [grab the latest download here](/projtlauncher/download) for your respective platform.
