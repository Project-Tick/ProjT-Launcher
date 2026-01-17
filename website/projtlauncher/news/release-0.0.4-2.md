---
title: ProjT Launcher Update 0.0.4-2, now available
description: Our official ProjT Launcher 0.0.4-2 release!
date: 2026-01-13
release_version: "0.0.4-2"
minimum_macos_version: 11.0.0
macos_file_extension: zip
macos_signature: Dp/ZmYNgilj/2DkxTFzrced88mc+Mr5NpKXneieB+PL99DvuGZZqEuNbDxJx+heL2KlTIsTbDJJkasloUuULDw==

tags:
  - release
---

We are excited to announce the **official release of ProjT Launcher: version 0.0.4-2**.

This release is a **structural consolidation and infrastructure refinement update** following the 0.0.4-1 transition.  
It focuses on stabilizing versioning logic, modernizing CI workflows, unifying documentation under a single handbook, and improving project automation policies.

User-facing behavior remains unchanged, while internal systems are now significantly cleaner, more deterministic, and easier to maintain.

## Changelog

### Added

- **Unified handbook documentation system**
  - Migrated all existing wiki content into a single, structured handbook with consistent layout and navigation (#271)
  - Added comprehensive user help pages covering launcher settings, platforms, Java configuration, mod management, and tools (#271)
  - Introduced initial developer documentation including CI/CD workflows and third-party library notes (#271)

- **Pull request automation**
  - Enabled pull request auto-merge with updated workflow references and safeguards (#272)

### Changed

- **Versioning and updater improvements**
  - Normalized version tags across the project (#271)
  - Improved version comparison logic in the updater and UI to prevent incorrect update ordering (#271)

- **CI and workflow modernization**
  - Updated CI workflows to align with revised versioning and release handling logic (#271)
  - Cleaned up legacy CI paths and removed obsolete workflow references (#271)
  - Updated GitHub Actions dependencies:
    - `actions/checkout` from v2 to v6 (#268)
    - `github/codeql-action` from v3 to v4 (#269)

- **Documentation and website alignment**
  - Unified handbook generation across `docs/` and `website-root/handbook` (#271)
  - Updated `.eleventyignore` to reflect the new handbook layout (#271)

### Fixed

- **Handbook and site build issues**
  - Removed obsolete handbook sync step now that the site builds directly from `website-root/handbook` (#271)
  - Removed unused `handbook.11tydata.js` and corrected Eleventy ignore rules (#271)

- **Release consistency**
  - Consolidated post-0.0.4-1 cleanup fixes to ensure metadata and version state correctness (#264, #265)

### Removed

- **Obsolete components**
  - Cleaned up legacy website wiki artifacts no longer part of the unified handbook (#271)

### Maintenance

- **Dependency updates**
  - Automated GitHub Actions dependency bumps via Dependabot (#268, #269)

- **Project metadata**
  - Automated contributor list update (#267)

---

### Important Notes

- This release intentionally focuses on **internal consistency and long-term maintainability**, not new end-user features.
- Documentation is now **single-source-of-truth**, eliminating wiki drift and duplicated content.
- CI workflows are aligned with the new versioning model and prepared for future automation.
- No vendored third-party code was introduced; all external dependencies remain maintained forks.

**Full Changelog**: <https://github.com/Project-Tick/ProjT-Launcher/compare/0.0.4-1...0.0.4-2>

You can [grab the latest download here](/projtlauncher/download).
