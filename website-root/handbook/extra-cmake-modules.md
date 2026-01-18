# Extra CMake Modules `extra-cmake-modules/`

> **Type**: CMake Module Collection  
> **License**: BSD-3-Clause  
> **Fork Origin**: [KDE ECM](https://api.kde.org/ecm/)
> **Status**: Detached Fork (independently maintained)
> **Latest Version**: 0.0.5-1

---

## Overview

Extra CMake Modules (ECM) provides additional CMake modules beyond what CMake ships by default. This includes:

- `find_package()` modules for common software
- Utility modules for common CMake tasks
- Toolchain files for cross-compilation
- Common build settings used by KDE software

While ECM originates from the KDE project, it is useful for any CMake-based project.

---

## Usage in ProjT Launcher

ECM is used for:

- **Qt integration** - Finding Qt components
- **Build utilities** - Common CMake patterns
- **Cross-platform support** - Toolchain configurations

---

## Key Modules

| Module | Purpose |
|--------|---------|
| `ECMQueryQt` | Query Qt installation details |
| `ECMInstallIcons` | Install icon themes |
| `ECMGenerateHeaders` | Generate forwarding headers |
| `ECMSetupVersion` | Configure project versioning |

---

## Build Integration

ECM is included as a subdirectory:

```cmake
add_subdirectory(extra-cmake-modules)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extra-cmake-modules/modules")
```

---

## Documentation

- [ECM API Documentation](https://api.kde.org/ecm/)
- [KDE Community Wiki](https://community.kde.org/Policies/CMake_Coding_Style)
- [Original README](../../extra-cmake-modules/README.md) — Upstream library documentation

---

## Related Documentation

- [Third-party Libraries](./third-party.md)
- [Workflows](./workflows.md)
