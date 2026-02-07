# ProjT Launcher — Build System Documentation

> **Version:** 0.0.5-1  
> **Build System:** GNU Make + Kconfig  
> **Last Updated:** 2026-02-07

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Quick Start](#2-quick-start)
3. [Directory Layout](#3-directory-layout)
4. [Makefile Include Graph](#4-makefile-include-graph)
5. [Build Phases](#5-build-phases)
6. [Configuration System (Kconfig)](#6-configuration-system-kconfig)
7. [Toolchain Selection](#7-toolchain-selection)
8. [mk/ File Reference](#8-mk-file-reference)
9. [Module System](#9-module-system)
10. [Qt Integration](#10-qt-integration)
11. [Test Infrastructure](#11-test-infrastructure)
12. [Packaging](#12-packaging)
13. [Cross-Compilation](#13-cross-compilation)
14. [CI/CD Integration](#14-cicd-integration)
15. [Make Targets Reference](#15-make-targets-reference)
16. [Variables Reference](#16-variables-reference)
17. [Troubleshooting](#17-troubleshooting)
18. [Design Decisions](#18-design-decisions)

---

## 1. Architecture Overview

The build system uses **GNU Make** as the build driver with **Kconfig** (the Linux kernel
configuration system) for build-time configuration. There is no CMake dependency for the
main build (CMake files exist only for Nix builds and cmark's internal build).

### Key Design Principles

- **Single entry point:** The top-level `Makefile` is the only user-facing interface
- **Kconfig for config:** All build options live in `Kconfig` and are set via `menuconfig`/`defconfig`
- **Modular mk/ files:** Build logic is split into focused `.mk` files under `mk/`
- **Per-module Makefiles:** Each library/component has its own `Makefile` invoked as a sub-make
- **Jobserver passthrough:** Parallel builds work across all sub-makes via GNU Make's jobserver
- **No recursive include hell:** Clear include chains with include guards where needed

### Build Flow Summary

```
./configure → .config → make defconfig → syncconfig → make build
                                              │
                                    ┌─────────┴─────────┐
                                    │  scripts/          │
                                    │  syncconfig.sh     │
                                    │                    │
                                    ▼                    ▼
                              auto.conf           autoconf.h
                              (Make vars)         (C #defines)
                                    │
                    ┌───────────────┼───────────────┐
                    ▼               ▼               ▼
              mk/targets.mk   mk/tests.mk    mk/package.mk
                    │
        ┌───────────┼───────────┬────────────┐
        ▼           ▼           ▼            ▼
    configure    subtrees     libs       launcher
    (.in→gen)    (zlib,etc)  (bzip2,     (main app)
                             cmark,etc)
```

---

## 2. Quick Start

### Linux (Native Build)

```bash
# Install dependencies
sudo apt install build-essential qt6-base-dev qt6-5compat-dev \
    libssl-dev flex bison libncurses-dev

# Configure
./configure                    # or: make defconfig
make menuconfig                # optional: tweak settings

# Build
make -j$(nproc)

# Test
make test

# Install
sudo make install PREFIX=/usr/local
```

### Windows (MSVC — from VS Developer Command Prompt)

```cmd
configure.bat
make defconfig
make -j%NUMBER_OF_PROCESSORS%
```

### Windows (MinGW Cross-Compile from Linux)

```bash
./configure --platform windows --cross-compile x86_64-w64-mingw32-
make defconfig && make -j$(nproc)
```

### macOS

```bash
./configure
make defconfig && make -j$(sysctl -n hw.ncpu)
```

---

## 3. Directory Layout

```
ProjT-Launcher/
├── Makefile                 # Top-level entry point (961 lines)
├── Kconfig                  # Configuration menu definitions
├── defconfig                # Default configuration
├── configure                # Unix configure script → generates .config
├── configure.bat            # Windows configure script
├── scripts/
│   └── syncconfig.sh        # Generates auto.conf + autoconf.h from .config
│
├── mk/                      # Build system modules (16 active files)
│   ├── config.mk            # Foundational: paths, auto.conf include, helpers
│   ├── host.mk              # Host tool detection
│   ├── flags.mk             # Compiler flag definitions
│   ├── toolchain.mk         # Toolchain selection from Kconfig values
│   ├── rules.mk             # Common compile/link pattern rules
│   ├── qt.mk                # Qt framework integration
│   ├── qt-tools.mk          # Qt tool paths (MOC, UIC, RCC)
│   ├── module.mk            # Shared build rules for simple modules
│   ├── configure.mk         # Template .in file processing
│   ├── targets.mk           # Build orchestration (6 phases)
│   ├── subtrees.mk          # Subtree library wrappers (zlib, tomlplusplus)
│   ├── tests.mk             # Test build & execution
│   ├── package.mk           # Packaging (DEB, RPM, AppImage, DMG, NSIS)
│   ├── platform.mk          # Platform detection utilities
│   ├── toolchain-gcc.mk     # Standalone GCC build
│   ├── toolchain-llvm.mk    # Standalone LLVM/Clang build
│   └── unused/              # Dead code (preserved for reference)
│       ├── bootstrap.mk     # Self-hosted toolchain bootstrap (unused)
│       ├── modules.mk       # Central module definitions (unused)
│       ├── rules-full.mk    # Extended build rules (unused)
│       ├── toolchain-full.mk # Advanced toolchain detection (unused)
│       ├── buildconfig.mk   # Python-based config gen (unused)
│       ├── qt-gen.mk        # Qt generation rules (unused)
│       ├── vars.mk          # Version/branding vars (unused)
│       └── java.mk          # Java build rules (unused)
│
├── kconfig/                  # Kconfig source (conf, mconf, nconf, qconf, gconf)
├── build/                    # Build output directory (default O=build)
│   ├── .config               # Active configuration
│   ├── include/
│   │   ├── config/auto.conf  # Make-includable config variables
│   │   └── generated/autoconf.h # C-includable config defines
│   ├── obj/                  # Object files per module
│   ├── lib/                  # Static/shared libraries
│   ├── bin/                  # Final executables
│   ├── jars/                 # Java JARs
│   └── tests/                # Test executables
│
├── launcher/                 # Main launcher source (993-line Makefile)
│   ├── Makefile
│   ├── console/
│   ├── icons/
│   ├── java/
│   ├── launch/
│   ├── logs/
│   ├── meta/
│   ├── minecraft/
│   ├── modplatform/
│   ├── net/
│   ├── news/
│   ├── screenshots/
│   ├── settings/
│   ├── tasks/
│   ├── tools/
│   ├── translations/
│   ├── ui/
│   └── updater/
│
├── bzip2/                    # libbz2 (simple module)
├── murmur2/                  # MurmurHash2 (simple module)
├── rainbow/                  # Color library (Qt dep, simple module)
├── qdcss/                    # Qt CSS parser (Qt dep, simple module)
├── systeminfo/               # System info (simple module)
├── libnbtplusplus/           # NBT parser (simple module)
├── gamemode/                 # Linux GameMode (header-only)
├── libpng/                   # PNG library (custom Makefile)
├── libqrencode/              # QR code library (custom Makefile)
├── cmark/                    # CommonMark parser (uses internal CMake)
├── quazip/                   # Qt ZIP library (Qt MOC module)
├── LocalPeer/                # IPC library (Qt MOC module)
├── buildconfig/              # Generated BuildConfig.cpp
├── javacheck/                # Java version checker
├── launcherjava/             # Java launcher wrappers
├── zlib/                     # zlib source (built by subtrees.mk)
├── tomlplusplus/             # TOML parser (header-only)
├── json/                     # nlohmann JSON (header-only)
├── tests/                    # Test sources
└── fuzz/                     # Fuzzing test sources
```

---

## 4. Makefile Include Graph

### Main Makefile (entry point)

```
Makefile
├── include kconfig/Makefile           (Kconfig tool build rules)
├── -include auto.conf                 (CONFIG_* variables)
├── $(MAKE) -f mk/targets.mk          (build, libs, launcher, etc.)
├── $(MAKE) -f mk/tests.mk            (test, tests-build)
├── $(MAKE) -f mk/package.mk          (package-deb, package-rpm, etc.)
├── $(MAKE) -f mk/toolchain-gcc.mk    (toolchain-gcc)
└── $(MAKE) -f mk/toolchain-llvm.mk   (toolchain-llvm)
```

### mk/ Include Dependencies

```
config.mk                   ← foundational, included by 8+ files
├── host.mk                 ← included by config.mk (indirectly via toolchain.mk)
├── toolchain.mk             ← includes config.mk + host.mk
│   └── flags.mk            ← includes toolchain.mk
│       └── qt-tools.mk     ← includes flags.mk
│           └── rules.mk    ← includes qt-tools.mk
│               └── module.mk  ← includes rules.mk (used by ~27 modules)
├── configure.mk             ← includes config.mk
├── qt.mk                   ← includes config.mk
├── subtrees.mk              ← includes config.mk
├── tests.mk                 ← standalone (receives vars via export)
├── package.mk               ← includes config.mk + platform.mk
├── platform.mk              ← includes config.mk
├── toolchain-gcc.mk         ← standalone
└── toolchain-llvm.mk        ← standalone
```

### Module Makefile Pattern

All simple modules follow this pattern:
```makefile
# In bzip2/Makefile, murmur2/Makefile, etc.
lib := bz2                    # Library name → libXXX.a
lib-y := blocksort.c ...      # Source files
includes-y := bzip2           # Include paths
ccflags-y := -DFOO            # Extra C flags (optional)
qt-modules-y := Core          # Qt modules needed (optional)
include $(srctree)/mk/module.mk
```

---

## 5. Build Phases

The build executes in 6 sequential phases (defined in `mk/targets.mk`):

### Phase 1: Configure
- Processes `.in` template files → generates C/C++ source files
- `BuildConfig.cpp.in` → `build/obj/generated/BuildConfig.cpp`
- `Launcher.in` → `build/obj/bin/LauncherScript`
- `pnglibconf.h.prebuilt` → `build/obj/generated/include/pnglibconf.h`
- `nbt_export.h`, `cmark_config.h`, `cmark_export.h`, `cmark_version.h`

### Phase 2: Subtrees
- Builds vendored libraries from source subtrees
- **zlib:** Compiled from `zlib/` → `build/lib/libz.a`
- **tomlplusplus:** Header-only, stamp file only
- **json:** Header-only, stamp file only
- **Qt (if bundled):** Built from `qt/` subtree

### Phase 3: Qt Build
- If `CONFIG_QT_BUNDLED=y`: builds Qt from source
- If system Qt: skipped (uses pkg-config)

### Phase 4: Libraries (3 tiers)
- **Tier 0:** No dependencies → bzip2, murmur2
- **Tier 1:** zlib dependencies → (currently empty, reserved)
- **Tier 2:** General deps → libpng, cmark, libnbtplusplus, libqrencode
- **Tier 3:** Qt dependencies → rainbow, qdcss, LocalPeer, quazip

### Phase 5: Java
- **javacheck:** Java version detection utility
- **launcherjava:** Java launcher wrappers (ProjTLaunch.jar, ProjTLaunchLegacy.jar)

### Phase 6: Launcher
- Builds all launcher submodules (console, icons, java, launch, etc.)
- Links everything into `build/bin/projtlauncher`
- Uses `--whole-archive` to ensure all symbols are included

---

## 6. Configuration System (Kconfig)

### How It Works

1. **`Kconfig`** (root file) defines all configuration options with types, defaults, help text
2. **`./configure`** (or `configure.bat`) creates initial `.config` in the build directory
3. **`make menuconfig`** opens the interactive ncurses-based configuration editor
4. **`scripts/syncconfig.sh`** converts `.config` → `auto.conf` (Make) + `autoconf.h` (C)

### Configuration Files

| File | Format | Purpose |
|------|--------|---------|
| `Kconfig` | Kconfig DSL | Option definitions |
| `defconfig` | key=value | Default values |
| `build/.config` | key=value | Active configuration |
| `build/include/config/auto.conf` | Make include | `CONFIG_*` variables for Make |
| `build/include/generated/autoconf.h` | C header | `#define CONFIG_*` for C/C++ |

### Key Configuration Variables

| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `CONFIG_CC` | string | `"gcc"` | C compiler |
| `CONFIG_CXX` | string | `"g++"` | C++ compiler |
| `CONFIG_BUILD_TYPE` | string | `"Debug"` | Build type (Debug/Release/RelWithDebInfo) |
| `CONFIG_BUILD_TESTS` | bool | `y` | Enable test building |
| `CONFIG_BUILD_FUZZERS` | bool | `n` | Enable fuzzer building |
| `CONFIG_ENABLE_LTO` | bool | `n` | Link-time optimization |
| `CONFIG_QT_VERSION_MAJOR` | string | `"6"` | Qt major version |
| `CONFIG_QT_PREFIX` | string | `""` | Qt installation prefix |
| `CONFIG_USE_BUNDLED_QT` | bool | `n` | Build Qt from source |
| `CONFIG_TARGET_LINUX` | bool | auto | Target Linux |
| `CONFIG_TARGET_WINDOWS` | bool | auto | Target Windows |
| `CONFIG_TARGET_MACOS` | bool | auto | Target macOS |

### syncconfig.sh

The `scripts/syncconfig.sh` script is a lightweight replacement for the kernel's `syncconfig`
mechanism. It:

1. Reads `build/.config`
2. Extracts `CONFIG_*=value` lines
3. Generates `auto.conf` (for Make inclusion)
4. Generates `autoconf.h` (with `#define` for C/C++)
5. Uses `LC_ALL=C` to avoid locale-dependent regex issues

**Critical:** The `LC_ALL=C` export at the top of `syncconfig.sh` is essential. Without it,
Turkish locale (`tr_TR`) breaks `[A-Za-z]` regex patterns, causing CONFIG variables with
uppercase `I` to be silently dropped.

---

## 7. Toolchain Selection

### Selection Priority (highest to lowest)

1. **Command line:** `make CC=clang CXX=clang++`
2. **Kconfig config:** `CONFIG_CC="clang"` in `.config`
3. **Platform default:** Set in main Makefile per platform
4. **ccache/sccache wrapping:** Applied automatically if available

### Platform Defaults

| Platform | CC | CXX | LD |
|----------|----|----|-----|
| Linux | `gcc` | `g++` | `g++` |
| macOS | `clang` | `clang++` | `clang++` |
| Windows (MSVC) | `cl.exe` | `cl.exe` | `link.exe` |
| Windows (MinGW) | `$(CROSS_COMPILE)gcc` | `$(CROSS_COMPILE)g++` | `$(CROSS_COMPILE)g++` |

### Kconfig Override Flow

```
Makefile: CC = gcc              (platform default)
    ↓
-include auto.conf              (loads CONFIG_CC="clang")
    ↓
cfg-unquote strips quotes       (_CC_CFG = clang)
    ↓
CC := clang                     (override applied)
    ↓
CC := ccache clang              (ccache wrapper, if available)
```

### ccache/sccache

Detected automatically. Disable with `NO_CCACHE=1`:
```bash
make NO_CCACHE=1 build
```

---

## 8. mk/ File Reference

### Active Files (16)

#### `mk/config.mk` — Foundational Configuration
- **Lines:** 41
- **Included by:** 8+ other mk/ files
- **Purpose:** Defines `OBJDIR`, `LIBDIR`, `BINDIR`, includes `auto.conf` and `.config`,
  provides `cfg-yes`, `cfg-on`, `cfg-unquote` helper functions
- **Exports:** `CC`, `CXX`, `AR`, `RANLIB`, `OBJDIR`, `LIBDIR`, `BINDIR`

#### `mk/host.mk` — Host Tool Detection
- **Lines:** 10
- **Purpose:** Detects host tools (`HOSTCC`, `INSTALL`, `MKDIR`, etc.)

#### `mk/toolchain.mk` — Toolchain Selection
- **Lines:** 46
- **Includes:** `config.mk`, `host.mk`
- **Purpose:** Reads `CONFIG_CC`, `CONFIG_CXX`, `CONFIG_CROSS_COMPILE`, `CONFIG_SYSROOT`
  from Kconfig and overrides `CC`/`CXX`/`AR`/`STRIP`/etc.
- **Also applies:** `CONFIG_CFLAGS_EXTRA`, `CONFIG_CXXFLAGS_EXTRA`, `CONFIG_LDFLAGS_EXTRA`

#### `mk/flags.mk` — Compiler Flags
- **Lines:** 19
- **Includes:** `toolchain.mk`
- **Purpose:** Sets up `CFLAGS`, `CXXFLAGS`, `LDFLAGS` with platform-appropriate values

#### `mk/qt-tools.mk` — Qt Tool Paths
- **Lines:** 79
- **Includes:** `flags.mk`
- **Purpose:** Locates `MOC`, `UIC`, `RCC`, `LRELEASE` tools via pkg-config or
  `CONFIG_QT_HOST_BINS` prefix

#### `mk/rules.mk` — Common Build Rules
- **Lines:** 63
- **Includes:** `qt-tools.mk`
- **Purpose:** Defines `src-to-obj`, `build-static-lib`, `build-exe` macros and common
  pattern rules for `.c` → `.o`, `.cpp` → `.o` compilation

#### `mk/module.mk` — Module Build Template
- **Lines:** 151
- **Included by:** ~27 module Makefiles
- **Purpose:** Standard build rules for simple modules. Expects `lib`, `lib-y`, `includes-y`
  variables from the including Makefile. Handles MSVC vs GCC differences.

#### `mk/configure.mk` — Template Processing
- **Lines:** 352
- **Includes:** `config.mk`
- **Include guard:** `_MK_CONFIGURE_INCLUDED`
- **Purpose:** Processes `.in` template files with `@VAR@` → value substitutions.
  Generates `BuildConfig.cpp`, `LauncherScript`, `pnglibconf.h`, `nbt_export.h`,
  `cmark_config.h`, `cmark_export.h`, `cmark_version.h`
- **Key macro:** `SED_SUBST` — massive sed expression with all substitution variables

#### `mk/qt.mk` — Qt Framework Integration
- **Lines:** 487
- **Includes:** `config.mk`
- **Include guard:** `_MK_QT_INCLUDED`
- **Purpose:** Detects system Qt or builds bundled Qt. Provides `QT_MODULE_template` macro,
  MOC/UIC/RCC rule templates, Qt CFLAGS/LIBS setup
- **Exports:** `QT_CFLAGS`, `QT_LIBS`, `QT_PREFIX`, `QT_HOST_BINS`, `MOC`, `UIC`, `RCC`

#### `mk/targets.mk` — Build Orchestration
- **Lines:** 521
- **Includes:** `configure.mk`, `qt.mk`
- **Purpose:** Defines the 6-phase build sequence. Contains `build_local` and `clean_local`
  macros for consistent module building. Handles launcher linking with `--whole-archive`.
- **Key targets:** `build`, `configure`, `subtrees`, `qt-build`, `libs`, `launcher-all`,
  `java-modules`, `tests`, `list-modules`

#### `mk/subtrees.mk` — Subtree Library Wrappers
- **Lines:** 318
- **Includes:** `config.mk`
- **Purpose:** Build wrappers for vendored subtree libraries:
  - **zlib:** Full C build → `libz.a` + `libprojtZ.so` (renamed shared lib for tests)
  - **tomlplusplus:** Header-only (stamp file)
  - **json:** Header-only (stamp file)
  - **Qt (bundled):** Conditional Qt build from `qt/` subtree

#### `mk/tests.mk` — Test Infrastructure
- **Lines:** 312
- **Standalone:** Invoked via `$(MAKE) -f mk/tests.mk`
- **Purpose:** Builds and runs 23 test executables. Links against all launcher libraries
  with `--start-group`/`--end-group`. Uses `libprojtZ.so` (renamed shared zlib) to avoid
  symbol conflicts with system Qt's zlib.
- **Key targets:** `test`, `check`, `tests-build`, `tests-run`, `tests-clean`

#### `mk/package.mk` — Packaging
- **Lines:** 508
- **Includes:** `config.mk`, `platform.mk`
- **Purpose:** Creates distribution packages:
  - **DEB:** Debian/Ubuntu `.deb` packages
  - **RPM:** Fedora/RHEL `.rpm` packages
  - **AppImage:** Linux portable AppImage
  - **DMG:** macOS disk image
  - **NSIS:** Windows installer
  - **TAR:** Source/binary tarballs

#### `mk/platform.mk` — Platform Detection
- **Lines:** 279
- **Includes:** `config.mk`
- **Purpose:** Comprehensive OS/architecture detection. Used by `package.mk` and
  toolchain files.

#### `mk/toolchain-gcc.mk` — GCC Build
- **Lines:** 181
- **Standalone:** Invoked via `$(MAKE) -f mk/toolchain-gcc.mk`
- **Purpose:** Downloads and builds GCC from source or subtree for self-hosted builds

#### `mk/toolchain-llvm.mk` — LLVM Build
- **Lines:** 189
- **Standalone:** Invoked via `$(MAKE) -f mk/toolchain-llvm.mk`
- **Purpose:** Downloads and builds LLVM/Clang from source or subtree

### Unused Files (8, in `mk/unused/`)

These files were developed for planned features but are not referenced by any active
build path. Preserved for reference:

| File | Reason Unused |
|------|---------------|
| `bootstrap.mk` | Self-hosted bootstrap — never integrated |
| `modules.mk` | Central module definitions — modules use per-directory Makefiles instead |
| `rules-full.mk` | Extended build rules (MSVC, Obj-C, shared libs) — not needed |
| `toolchain-full.mk` | Advanced toolchain auto-detection — simplified version used instead |
| `buildconfig.mk` | Python-based config generation — configure.mk handles this |
| `qt-gen.mk` | Qt generation rules — qt.mk handles this |
| `vars.mk` | Version/branding variables — main Makefile defines these |
| `java.mk` | Java build rules — targets.mk handles Java directly |

---

## 9. Module System

### Module Types

#### Type 1: Simple Module (uses `mk/module.mk`)

Modules that compile C/C++ sources into a static library:

```makefile
# bzip2/Makefile
lib := bz2
lib-y := blocksort.c huffman.c crctable.c randtable.c \
         compress.c decompress.c bzlib.c
includes-y := bzip2
include $(srctree)/mk/module.mk
```

**Variables accepted by module.mk:**
| Variable | Required | Description |
|----------|----------|-------------|
| `lib` | Yes | Library name (output: `lib$(lib).a`) |
| `lib-y` | Yes | Source files (`.c` and/or `.cpp`) |
| `includes-y` | No | Include directories (relative to `srctree`) |
| `ccflags-y` | No | Extra C flags |
| `cxxflags-y` | No | Extra C++ flags |
| `qt-modules-y` | No | Qt modules needed (e.g., `Core Gui Widgets`) |

**Modules using this pattern:** bzip2, murmur2, rainbow, qdcss, systeminfo,
libnbtplusplus, gamemode

#### Type 2: Qt MOC Module

Modules that need Qt's Meta-Object Compiler:

```makefile
# LocalPeer/Makefile
lib := localpeer
lib-y := LocalPeer.cpp LockedFile.cpp LockedFile_unix.cpp
MOC_HEADERS := include/LocalPeer.h
EXTRA_OBJS := $(MOC_OBJS)
CUSTOM_STATIC_LIB_RULE := 1
includes-y := LocalPeer/include LocalPeer
qt-modules-y := Core Network
include $(srctree)/mk/module.mk
```

**Modules using this pattern:** quazip, LocalPeer

#### Type 3: Custom Makefile

Modules with complex build logic:

- **libpng:** Standalone C build with custom flags
- **libqrencode:** Standalone C build with own CFLAGS
- **buildconfig:** Compiles generated `BuildConfig.cpp`
- **cmark:** Uses its **own internal CMake** build system
- **launcher/:** Complex build with MOC, RCC, multiple submodules

#### Type 4: Java Module

```makefile
# javacheck/Makefile — compiles .java → .jar
```

---

## 10. Qt Integration

### System Qt (Default)

Qt is detected via `pkg-config` or the `CONFIG_QT_PREFIX` path:

```bash
# Auto-detect via pkg-config
./configure

# Specify prefix manually
./configure --qt-prefix /opt/qt6
```

### Bundled Qt

When `CONFIG_USE_BUNDLED_QT=y`, Qt is built from the `qt/` subtree:

```bash
make menuconfig  # Enable "Use Bundled Qt"
make qt-build    # Build Qt from source
make build       # Build the project
```

### Qt Tool Discovery

The `mk/qt-tools.mk` file locates Qt tools in this order:

1. `CONFIG_QT_HOST_BINS` (explicit path from Kconfig)
2. pkg-config `Qt6Core` `host_bins` variable
3. Common system paths (`/usr/lib/qt6/libexec/`, etc.)

### Qt Modules Used

| Qt Module | Used By |
|-----------|---------|
| Core | All Qt-dependent modules |
| Gui | launcher/ui |
| Widgets | launcher/ui |
| Network | launcher/net, LocalPeer |
| Xml | launcher/meta |
| Concurrent | launcher/tasks |
| Core5Compat | Various |

---

## 11. Test Infrastructure

### Overview

- **23 test executables**, each testing a specific component
- Built by `mk/tests.mk`, run sequentially
- Each test links against ALL launcher libraries (to resolve dependencies)
- Uses `libprojtZ.so` (renamed shared zlib) to avoid symbol conflicts with system Qt

### Why libprojtZ.so?

System Qt links against system zlib. Our static `libz.a` has identical symbols, causing
`Z_VERSION_ERROR` at runtime. Solution: build zlib as a shared library with a unique name
(`libprojtZ.so.1`) and link tests against it instead.

### Running Tests

```bash
make test                          # Build + run all tests
make check                         # Same as test
make tests-build                   # Build only (no run)
make TEST_VERBOSE=1 test           # Verbose output
make TEST_FILTER=Version test      # Run only matching tests
```

### Test List

| Test | Component |
|------|-----------|
| `Version_test` | Version parsing |
| `GradleSpecifier_test` | Gradle dependency specifiers |
| `RuntimeVersion_test` | Java runtime version parsing |
| `GZip_test` | GZip compression |
| `ParseUtils_test` | General parsing utilities |
| `JavaVersion_test` | Java version detection |
| `INIFile_test` | INI file parsing |
| `Library_test` | Library management |
| `MojangVersionFormat_test` | Mojang version format |
| `Packwiz_test` | Packwiz modpack format |
| `Index_test` | Index management |
| `MetaComponentParse_test` | Meta component parsing |
| `DataPackParse_test` | Data pack parsing |
| `ResourcePackParse_test` | Resource pack parsing |
| `TexturePackParse_test` | Texture pack parsing |
| `ShaderPackParse_test` | Shader pack parsing |
| `WorldSaveParse_test` | World save parsing |
| `ResourceFolderModel_test` | Resource folder model |
| `Task_test` | Task system |
| `CatPack_test` | Cat pack handling |
| `XmlLogs_test` | XML log parsing |
| `LogEventParser_test` | Log event parsing |
| `ProjTExternalUpdater_test` | External updater (non-macOS only) |

---

## 12. Packaging

Packaging is handled by `mk/package.mk`. All package targets are invoked via:

```bash
make package-deb         # Debian/Ubuntu package
make package-rpm         # RPM package
make package-appimage    # Linux AppImage
make package-dmg         # macOS disk image
make package-nsis        # Windows NSIS installer
make package             # Build all configured packages
```

### Package Output

All packages are created in `build/packages/`.

---

## 13. Cross-Compilation

### Linux → Windows (MinGW)

```bash
./configure --platform windows --cross-compile x86_64-w64-mingw32-
make defconfig && make -j$(nproc)
```

### Linux → macOS (requires osxcross)

```bash
./configure --platform macos --cross-compile x86_64-apple-darwin-
make defconfig && make -j$(nproc)
```

### Linux x86_64 → Linux aarch64

```bash
./configure --arch aarch64 --cross-compile aarch64-linux-gnu-
make defconfig && make -j$(nproc)
```

### Key Variables

| Variable | Example | Description |
|----------|---------|-------------|
| `TARGET_PLATFORM` | `linux`, `windows`, `macos` | Target OS |
| `TARGET_ARCH` | `x86_64`, `aarch64`, `i686` | Target architecture |
| `CROSS_COMPILE` | `x86_64-w64-mingw32-` | Toolchain prefix |
| `WINDOWS_TOOLCHAIN` | `mingw`, `msvc` | Windows toolchain choice |

---

## 14. CI/CD Integration

### CI Targets

```bash
make ci-prepare          # Prepare CI environment
make ci-lint             # Run linting checks
make ci-release          # Build + create release artifacts
```

### GitHub Actions Matrix

The CI runs ~20 build configurations:
- Linux x86_64 (GCC, Clang)
- Linux aarch64 (cross-compile)
- Windows MSVC (x86_64)
- Windows MinGW (x86_64)
- macOS 14, 15, 26 (arm64)

---

## 15. Make Targets Reference

### Configuration

| Target | Description |
|--------|-------------|
| `defconfig` | Load default configuration |
| `menuconfig` | Interactive ncurses configuration |
| `nconfig` | Alternative ncurses configuration |
| `xconfig` | Qt-based graphical configuration |
| `gconfig` | GTK-based graphical configuration |
| `oldconfig` | Update config, prompt for new options |
| `olddefconfig` | Update config, use defaults for new options |
| `savedefconfig` | Save minimal config to `defconfig` |
| `listnewconfig` | List new, unconfigured options |

### Build

| Target | Description |
|--------|-------------|
| `all` (default) | Build everything |
| `build` | Build the project |
| `configure` | Generate headers from `.in` files |
| `subtrees` | Build subtree dependencies |
| `qt-build` | Build Qt (if bundled) |
| `libs` | Build all libraries |
| `launcher-all` | Build launcher and submodules |
| `java-modules` | Build Java modules |

### Install

| Target | Description |
|--------|-------------|
| `install` | Install to `PREFIX` (default: `/usr/local`) |
| `install-bin` | Install binary only |
| `install-data` | Install data files only |
| `uninstall` | Remove installed files |

### Testing

| Target | Description |
|--------|-------------|
| `test` / `check` | Build and run all tests |
| `tests-build` | Build tests without running |
| `fuzz` | Run fuzzers (requires `CONFIG_BUILD_FUZZERS=y`) |

### Packaging

| Target | Description |
|--------|-------------|
| `package` | Build all configured packages |
| `package-deb` | Build `.deb` package |
| `package-rpm` | Build `.rpm` package |
| `package-appimage` | Build AppImage |
| `package-dmg` | Build macOS DMG |
| `package-nsis` | Build Windows NSIS installer |

### Toolchain

| Target | Description |
|--------|-------------|
| `toolchain-gcc` | Build GCC from source |
| `toolchain-llvm` | Build LLVM/Clang from source |
| `toolchain-all` | Build both |
| `toolchain-clean` | Clean toolchain builds |
| `toolchain-distclean` | Remove all toolchain files |
| `toolchain-help` | Show toolchain options |

### Utility

| Target | Description |
|--------|-------------|
| `info` | Show project and build information |
| `version` | Show version string |
| `listconfig` | Show configuration summary |
| `list-modules` | List all available modules |
| `clean` | Remove build artifacts (obj, lib, bin) |
| `distclean` | Remove everything including `.config` |
| `mrproper` | Alias for `distclean` |
| `help` | Show help text |

---

## 16. Variables Reference

### User-Settable Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `V` | `0` | Verbose build (`V=1` for full commands) |
| `O` | `build` | Output directory |
| `PREFIX` | `/usr/local` | Installation prefix |
| `DESTDIR` | (empty) | Staging directory for packages |
| `JOBS` | `$(nproc)` | Parallel job count |
| `NO_CCACHE` | (unset) | Set to `1` to disable ccache |
| `CC` | (auto) | C compiler |
| `CXX` | (auto) | C++ compiler |
| `CROSS_COMPILE` | (empty) | Toolchain prefix |
| `TARGET_PLATFORM` | (auto) | Target OS |
| `TARGET_ARCH` | (auto) | Target architecture |
| `WINDOWS_TOOLCHAIN` | `mingw` | Windows toolchain (`mingw`/`msvc`) |

### Internal Variables (read-only)

| Variable | Description |
|----------|-------------|
| `srctree` | Source tree root (always `$(CURDIR)`) |
| `KBUILD_OUTPUT` | Absolute path to build output |
| `KCONFIG_CONFIG` | Path to `.config` file |
| `KCONFIG_AUTOCONFIG` | Path to `auto.conf` |
| `HOST_PLATFORM` | Host OS (`linux`/`macos`/`windows`) |
| `HOST_ARCH` | Host architecture |
| `OBJDIR` | Object file directory (`build/obj`) |
| `LIBDIR` | Library directory (`build/lib`) |
| `BINDIR` | Binary directory (`build/bin`) |

---

## 17. Troubleshooting

### Build fails with "CONFIG not found" or missing CONFIG variables

**Cause:** Locale-dependent regex in `syncconfig.sh`.  
**Fix:** Ensure `LC_ALL=C` is set in `scripts/syncconfig.sh` (already applied).

```bash
# Verify: should show 70 CONFIG entries
grep -c '^CONFIG_' build/include/config/auto.conf
```

### CC/CXX empty in `make info`

**Cause:** `MAKEFLAGS += -r -R` disables built-in variables, breaking `CC ?=` assignments.  
**Fix:** Main Makefile now uses unconditional `CC =` and applies CONFIG_CC override after
auto.conf include (already fixed).

### Tests fail with "libprojtZ not found"

**Cause:** The shared zlib (`libprojtZ.so`) wasn't being built before test linking.  
**Fix:** `mk/tests.mk` now includes `zlib-shared` as a prerequisite of `tests-build`
(already fixed).

### "Tests not enabled" even though BUILD_TESTS=y in .config

**Cause:** syncconfig.sh failed to generate `auto.conf` correctly (locale bug).  
**Fix:** See "CONFIG not found" above.

### ccache wrapping an empty CC

**Cause:** ccache section ran before CONFIG_CC override.  
**Fix:** ccache/sccache wrapping now happens after auto.conf values are applied.

### Out-of-tree build

```bash
make O=../build-release defconfig
make O=../build-release -j$(nproc)
```

### Windows: "make is not recognized"

Install make via:
- **MSYS2:** `pacman -S make`
- **Chocolatey:** `choco install make`
- **Visual Studio:** Use `nmake` or install GNU Make

### Nix builds

Nix builds use `CMakeLists.txt` (not the Make build system). The CMake files are
maintained separately for Nix compatibility.

---

## 18. Design Decisions

### Why Make instead of CMake?

1. **Simpler:** 961-line Makefile + 16 mk/ files vs. 50+ CMakeLists.txt files
2. **Faster:** No generation step, direct compilation
3. **Kconfig:** Native integration with Linux kernel's proven configuration system
4. **Transparent:** Easy to debug with `make V=1` and `make -p`
5. **Cross-platform:** Works on Linux, macOS, Windows (MSVC, MinGW, MSYS2)

### Why Kconfig?

1. **Battle-tested:** Used by the Linux kernel for 20+ years
2. **Dependency tracking:** Options can depend on other options
3. **Multiple UIs:** `menuconfig` (ncurses), `xconfig` (Qt), `gconfig` (GTK)
4. **Minimal footprint:** Pure C, builds as part of the project

### Why libprojtZ.so for tests?

System Qt links against system zlib. Our static `libz.a` has the same symbols.
When both are linked into the same binary, the zlib version check fails
(`Z_VERSION_ERROR`). Using a renamed shared library (`libprojtZ.so`) with `-rpath`
avoids this conflict.

### Why `-r -R` in MAKEFLAGS?

`-r` disables built-in implicit rules (like `.c.o:`) and `-R` disables built-in
variable definitions (like `CC=cc`). This eliminates ~150 implicit rules and variables
that would otherwise slow down Make's pattern matching. The speed improvement is
significant for large projects.

**Trade-off:** All variables must be explicitly set (no fallback to `CC=cc`).
The Makefile uses unconditional `CC = gcc` (not `CC ?= gcc`) to handle this.

### Why `SHELL := /bin/bash`?

The build system uses bash-specific features:
- `PIPESTATUS` array for checking exit codes in pipelines
- `set -o pipefail` for proper error propagation
- Process substitution `<()` in some scripts

---

## Appendix: File Statistics

| Component | Files | Lines |
|-----------|-------|-------|
| Main Makefile | 1 | 977 |
| Active mk/ files | 16 | ~3,200 |
| Module Makefiles | ~27 | ~2,500 |
| Launcher Makefile | 1 | 993 |
| Launcher sub-Makefiles | 18 | ~3,000 |
| Total build system | ~63 | ~10,700 |
