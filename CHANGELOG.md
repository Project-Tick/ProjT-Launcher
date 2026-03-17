# Changelog

## 0.0.5-1 DRAFT

**Date range:** 2026-01-16 to yyyy-mm-dd

We are excited to announce the official release of **ProjT Launcher: version 0.0.5-1**.

This is our first version on **GitLab**!

This release strengthens version compliance, particularly by improving Fabric/Quilt and LWJGL
component parsing; it also adds a Launcher Hub (web-based dashboard) and makes the packaging flow
(RPM, AppImage, portable, macOS) more consistent. The CI and build systems (Qt/CMake, MSYS2/MSVC
Clang) have been simplified and stabilized, while multi-platform compilation, DESTDIR placement,
and metadata-related incompatibilities have been addressed.

### Highlights
- Improved Fabric/Quilt component version resolution with better Minecraft-version alignment.
- Added Launcher Hub support (web-based dashboard).
- Strengthened version comparison logic, especially for release-candidate handling.
- Added a compatibility hotfix for LWJGL metadata variants.
- Added Modrinth collection import for existing instances.
- Switched the Linux Launcher Hub backend from QtWebEngine to CEF and added a native cockpit dashboard.
- Improved CEF packaging/runtime handling for Nix, AppDir, and multi-architecture builds.
- Reduced launcher/build warnings and resolved zlib/libpng symbol-conflict issues.

### Added
- Launcher Hub feature (web-based panel).
- New unit tests for various launcher components.
- More complete packaging outputs across platforms, especially Linux/macOS artifact flow.
- Modrinth collection import flow in the existing mod download dialog for current instances.
- Native cockpit dashboard for Launcher Hub with quick actions and Linux fallback support.
- Linux CEF Hub backend wiring, including local SDK detection and configure-time auto-download support.
- Additional standalone unit tests for utility, JSON, filtering, serialization, and exponential-series behavior.
- AI Usage Policy, refreshed licensing display/docs, and new GPLv3 program-info asset.

### Changed
- Component dependency resolution flow (`ComponentUpdateTask`) is now more stable.
- Qt/CMake-based build and preset flows are more consistent across Linux, Windows, and macOS.
- Improved MSYS2/MSVC/Clang compatibility for Windows builds.
- Reorganized packaging architecture (RPM/portable/AppImage/macOS artifacts).
- Linux Hub now uses CEF instead of QtWebEngine, while keeping platform-specific backends on Windows and macOS.
- CEF build and packaging flow is now architecture-aware (`x64`/`arm64`) and more reproducible on Nix.
- CEF source builds remain optional by default during configure.
- zlib symbol handling was refined to use libpng-targeted shim overrides instead of global prefixing.
- About dialog licensing/contributing content and related program-info metadata were refreshed.

### Fixed
- Fixed metadata and version compatibility issues related to Fabric/LWJGL.
- Fixed path/folder coverage and filesystem test issues.
- Fixed `DESTDIR` and library placement issues in AppImage/portable packages.
- Fixed multiple macOS/Windows build and linking incompatibilities.
- Fixed Linux CEF runtime installation so AppDir packaging includes the required binaries and resources.
- Fixed Nix-side CEF runtime linking, runtime dependency propagation, and translation model reset behavior.
- Fixed MinGW-specific build problems, including CFG flag incompatibility and `DataPackPage` LTO linker errors.
- Fixed Linux CEF command-line switch handling for `disable-features`.
- Fixed bundled zlib/libpng symbol conflicts and corrected installed `pkg-config` prefix resolution.
- Fixed remaining launcher null-dereference/container-access warnings and reduced general build-warning noise.
- Fixed NeoForge legacy URL normalization and `/releases` Maven path handling.

### Internal / CI
- CI workflows were simplified and reorganized for GitLab/GitHub.
- Removed old/duplicated workflows; improved fuzzing and packaging steps.
- Updated maintenance automation for subtree/toolchain/Qt management.
- Expanded targeted standalone test coverage and kept launcher test targets aligned with shared runtime deps.
- Refined Linux image/Flatpak/runner workflow details and cleaned up repository metadata/docs housekeeping.
- Debounced instance directory reloads to reduce noisy logs and unnecessary refresh churn.

### Why GitLab?

The decision to position GitLab SaaS as the primary development platform is grounded in operational efficiency, cost structure, and long-term control — not ideology.

**1. OSS Ultimate SaaS Program Approval**
Project Tick has been accepted into the GitLab OSS Ultimate SaaS Program. This provides:

* Access to Ultimate-tier features
* 50,000 CI minutes per month
* Advanced security and compliance tooling
  This materially reduces operational cost while expanding CI capacity.

**2. Merge Request-Centric Workflow Architecture**
GitLab provides a more integrated MR-driven workflow model:

* First-class MR pipeline controls
* Native approval rules and compliance gating
* Clearer repository governance boundaries

For a multi-repository, high-scale project, centralized MR orchestration is critical.

**3. CI Governance and Hybrid Control**
GitLab enables:

* Clear separation between repository management and compute execution
* Flexible runner orchestration (self-managed + hosted)
* Fine-grained CI control without vendor lock-in

GitHub remains a compute layer in the hybrid model, but GitLab is positioned as the canonical coordination layer.

**4. Platform Independence by Design**
GitLab is used as the central management layer — not as a single point of failure.
All repositories are:

* Backed up independently
* Architected for migration
* Designed to avoid provider entanglement

The objective is sovereignty with redundancy.

**5. Cost-to-Performance Optimization**
The hybrid design leverages:

* GitLab OSS CI allocation
* GitHub Actions compute elasticity
* Self-managed runner capacity

Workload placement is data-driven and capacity-driven, not preference-driven.
