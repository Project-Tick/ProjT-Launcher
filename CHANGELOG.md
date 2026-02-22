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

### Added
- Launcher Hub feature (web-based panel).
- New unit tests for various launcher components.
- More complete packaging outputs across platforms, especially Linux/macOS artifact flow.

### Changed
- Component dependency resolution flow (`ComponentUpdateTask`) is now more stable.
- Qt/CMake-based build and preset flows are more consistent across Linux, Windows, and macOS.
- Improved MSYS2/MSVC/Clang compatibility for Windows builds.
- Reorganized packaging architecture (RPM/portable/AppImage/macOS artifacts).

### Fixed
- Fixed metadata and version compatibility issues related to Fabric/LWJGL.
- Fixed path/folder coverage and filesystem test issues.
- Fixed `DESTDIR` and library placement issues in AppImage/portable packages.
- Fixed multiple macOS/Windows build and linking incompatibilities.

### Internal / CI
- CI workflows were simplified and reorganized for GitLab/GitHub.
- Removed old/duplicated workflows; improved fuzzing and packaging steps.
- Updated maintenance automation for subtree/toolchain/Qt management.

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
