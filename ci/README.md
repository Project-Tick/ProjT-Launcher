# CI Support Files

This directory contains files to support CI/CD for the ProjT Launcher project.

## Overview

The CI configuration uses GitHub Actions to automate building, testing, and packaging the ProjT Launcher across multiple platforms (Linux, macOS, and Windows).

## Workflows

### Build Workflow (`.github/workflows/build.yml`)

Automatically builds the project on:

- **Triggers**: Push to `develop` or `release-*` branches, Pull Requests, Manual dispatch
- **Platforms**:
  - Ubuntu 24.04 (x86_64 and ARM64)
  - macOS 14 (ARM64 universal binary)
  - Windows 11/2022 (MinGW and MSVC compilers)

**Build Process**:

1. Setup dependencies using `./.github/actions/setup-dependencies`
2. Configure CMake with the appropriate preset
3. Build using CMake
4. Run tests via CTest
5. Package the binary for distribution
6. Upload artifacts

### Lint Workflow (`.github/workflows/lint.yml`)

Validates code formatting and quality:

- **Tool**: `clang-format` for C++ code
- **Triggers**: Pull Requests, Push to develop/release branches
- **Checks**:
  - C++ code formatting compliance
  - `CMakeLists.txt` syntax validation
  - Code style consistency

### Dependency Check Workflow (`.github/workflows/eval.yml`)

Verifies project dependencies:

- **Validates**: `CMakeLists.txt`, `vcpkg.json`, `conan.txt`
- **Checks**:
  - CMakeLists.txt syntax
  - JSON configuration files
  - Required dependencies (Qt6, CMake, etc.)

## Development Environment

### Using Nix

The `ci/default.nix` provides a development environment with all necessary build dependencies:

```bash
# Enter development environment
nix develop ci/default.nix

# Or with flake.nix
nix develop
```

### Manual Setup

See `.github/actions/setup-dependencies` for platform‑specific dependency installation.

## Local Testing

### Building Locally

```bash
# Configure
cmake --preset linux

# Build
cmake --build --preset linux --config Release
```

### Running Linters

```bash
# Check C++ formatting
clang-format -i launcher/**/*.cpp launcher/**/*.h

# Validate CMakeLists.txt
cmake --lint CMakeLists.txt
```

## Repository Structure

- `.github/workflows/` – GitHub Actions workflow definitions
- `.github/actions/` – Custom GitHub Actions for build, test, and packaging
- `ci/default.nix` – Nix development environment configuration
- `ci/pinned.json` – Pinned dependency versions
- `CMakePresets.json` – CMake build presets for different platforms

## Continuous Integration Best Practices

1. **Always run linters locally before committing**

   ```bash
   clang-format -i <modified-files>
   ```

2. **Test locally with appropriate CMake presets**

   ```bash
   cmake --preset linux && cmake --build --preset linux
   ```

3. **Keep `CMakeLists.txt` well‑maintained** – CI validates syntax.
4. **Update `vcpkg.json` when adding dependencies**
5. **Run the full test suite before pushing**

   ```bash
   ctest --preset linux --output-on-failure
   ```
