# Getting Started

Guide to setting up your development environment for ProjT Launcher.

---

## Prerequisites

### Required Tools

| Tool | Version | Purpose |
|------|---------|---------|
| CMake | 3.22+ | Build system |
| Qt | 6.8.0+ | GUI framework |
| Compiler | C++20 | GCC 11+, Clang 14+, MSVC 2022 |
| Ninja | 1.10+ | Build tool |
| Git | 2.30+ | Version control |

### Optional Tools

| Tool | Purpose |
|------|---------|
| clang-format | Code formatting |
| clang-tidy | Static analysis |
| Valgrind | Memory checking (Linux) |

---

## Clone Repository

```sh
# Fork on GitHub first, then:
git clone https://github.com/YourName/ProjT-Launcher.git
cd ProjT-Launcher

# Add upstream remote
git remote add upstream https://github.com/Project-Tick/ProjT-Launcher.git
```

---

## Platform Setup

### Windows

**Visual Studio 2022**:

1. Install VS 2022 with "Desktop development with C++"
2. Install Qt 6.8.0+ via Qt Online Installer
   - Select MSVC 2022 64-bit
   - Select Qt Shader Tools
3. Install dependencies via NuGet (required for CI parity):

```powershell
nuget install ./packages.config -OutputDirectory dependencies
```

**Note**: NuGet dependencies are mandatory for Windows builds to ensure CI reproducibility.

### Linux

**Nix (Recommended)**:

```sh
nix develop .#default
```

**Manual**:

```sh
# Debian/Ubuntu
sudo apt install cmake ninja-build qt6-base-dev qt6-tools-dev

# Fedora
sudo dnf install cmake ninja-build qt6-qtbase-devel qt6-qttools-devel

# Arch
sudo pacman -S cmake ninja qt6-base qt6-tools
```

### macOS

**Nix (Recommended)**:

```sh
nix develop .#default
```

**Homebrew**:

```sh
brew install cmake ninja qt@6
```

---

## Build

### Configure

```sh
# List available presets
cmake --list-presets

# Windows
cmake --preset windows_msvc

# Linux
cmake --preset linux

# macOS
cmake --preset macos
```

### Build

```sh
# Debug (development)
cmake --build --preset <preset> --config Debug

# Release (production)
cmake --build --preset <preset> --config Release
```

### Run

```sh
# Windows (MSVC preset)
./build/windows_msvc/Debug/ProjT-Launcher.exe

# Linux
./build/linux/Debug/ProjT-Launcher

# macOS
./build/macos/Debug/ProjT-Launcher.app/Contents/MacOS/ProjT-Launcher
```

### Test

```sh
ctest --preset <preset> --output-on-failure
```

---

## IDE Setup

### VS Code (Recommended)

Extensions:
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- clangd (LLVM)

Settings (`.vscode/settings.json`):

```json
{
  "cmake.configureOnOpen": true,
  "cmake.generator": "Ninja"
}
```

### Qt Creator

1. Open `CMakeLists.txt` as project
2. Select Qt 6.8.0+ kit
3. Import `.clang-format` in Tools > Options > C++ > Code Style

### Visual Studio 2022

1. File > Open > CMake
2. Select `CMakeLists.txt`
3. Configure Qt path in CMake settings

---

## Troubleshooting

### Qt not found

```sh
cmake --preset <preset> -DCMAKE_PREFIX_PATH="/path/to/Qt/6.8.0/gcc_64"
```

### Wrong Qt minor version

CI requires exact Qt version match. Check `cmake/versions.cmake` for the required version. Mismatched Qt versions cause ABI incompatibility.

### MSVC vs MinGW mismatch

Qt kit must match your compiler. Use MSVC kit with Visual Studio, MinGW kit with MinGW toolchain. Do not mix.

### clang-format version mismatch

Use the version specified in CI. Different versions produce different output. Check `.gitlab-ci.yml` and `.gitlab/ci/` for the expected version.

### Ninja not found

Install Ninja or use a different generator:

```sh
cmake -G "Unix Makefiles" ..
```

### C++20 not supported

Update your compiler:
- GCC: 11+
- Clang: 14+
- MSVC: 2022

---

## Next Steps

These documents must be read before submitting code:

- [Code Style](./CODE_STYLE.md) - Formatting rules
- [Project Structure](./PROJECT_STRUCTURE.md) - Where files go
- [Architecture](./ARCHITECTURE.md) - How components interact
- [Workflow](./WORKFLOW.md) - Git workflow and PR process
