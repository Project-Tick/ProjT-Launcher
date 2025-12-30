---
title: ProjT Launcher Update 0.0.3-4, now available
description: Our official ProjT Launcher 0.0.3-4 release!
date: 2025-12-16
tags:
  - release
---

We are excited to announce the **official release of ProjT Launcher: version 0.0.3-4**!

This patch release focuses on **CI reliability and release automation**, along with a few important runtime/test fixes.

## Changelog

### Added

- **CI workflow improvements for PR processing and security** (#70, #75)  
  - Improved permissions and PR summary/comment behavior  
  - Better ref/SHA handling for workflow coordination  
- **Backport automation enhancements** (#75)  
  - Backport command handling in PR comments  
  - Backport-on-close job for labeled PRs when merged

### Changed

- **Release pipeline adjustments** to improve stability of create/release steps (#70)
- **README installation guidance** to better explain official releases and stable options (#75)
- **Removed unused Flatpak shared-modules submodule** and updated submodule configuration (#75)

### Fixed

- **CI issues affecting release creation and review automation** (#70, #75)
- **Resource loading edge cases** (#75)  
  - Excluded `.index` directory from resource loading  
  - Adjusted timeout behavior for Windows  
- **Test reliability** in `ResourceFolderModel` timeout handling (#75)

### Removed

- `flatpak/shared-modules` submodule (unused) (#75)

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/compare/0.0.3-3...0.0.3-4>

You can [grab the latest download here](/download) for your respective platform.
