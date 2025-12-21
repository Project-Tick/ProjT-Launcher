---
title: ProjT Launcher Update 0.0.3-3, now available
description: Our official ProjT Launcher 0.0.3-3 release!
date: 2025-12-15
tags:
  - release
---

We are excited to announce the **official release of ProjT Launcher: version 0.0.3-3**!

This update focuses on **CI/automation stability and maintainability**, plus a few critical fixes for **Linux dependency setup**.

## Changelog

### Added

- **Large CI system refresh** (#56, #61)  
  - New evaluation workflows, checks, and reporting structure  
  - Consolidated CI scripts under `ci/` for reuse and consistency  
- **New contribution documentation set** (#27)  
  - Architecture, code style, workflow, testing, and project structure guides  
- **Repository automation configs** (#56, #57, #49)  
  - Issue templates, Dependabot scheduling/labels, labeler rules, and stale-bot documentation  
- **`force-prepare-yes` workflow input** for conditional prepare/test/build behavior (#66)

### Changed

- **Backport automation improvements** (#62, #64)  
  - Better backport label extraction  
  - Backport workflow now correctly creates backport branches and PRs  
- **Periodic merge workflow reliability** (#51, #52, #53, #54, #50)  
  - Safer conflict resolution and branch handling for automated merges  
- **Documentation and build config cleanup** (#65)  
  - README clarity improvements (including QML migration notes)  
  - Removed redundant Linux ARM build configuration  
- **Dependency updates via Dependabot** (GitHub Actions + CI JS deps) (#28, #29, #30, #31, #32, #36, #37, #38, #39, #42, #43, #44, #45, #46, #47)

### Fixed

- **Linux dependency setup: libtiff mismatch issues** (#67, #69)  
  - Corrected package selection and added a compatibility symlink where needed  
- **Workflow and template polish** (#61)  
  - Naming consistency and small syntax/template fixes

### Removed

- Deprecated/older CI workflows replaced by the new evaluation framework (#56)
- Unused lint script/devDependencies from `ci/github-script` (#35)
- Flatpak helper script no longer used (`flatpak/prime-run`) (#20)

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/compare/0.0.3-2...0.0.3-3>

You can [grab the latest download here](/download) for your respective platform.
