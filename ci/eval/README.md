# ProjT Launcher CI Evaluation

This directory contains Nix helpers for validating repository configuration locally (and can be wired into CI if desired).

## Purpose

This evaluation module performs:

- CMakeLists.txt syntax validation
- vcpkg.json dependency verification
- Nix flake structure checking
- Build configuration validation across platforms

## Local Usage

### Quick validation

```bash
# Validate project structure
nix-build ci -A eval.validate

# Check specific component
nix-build ci -A eval.cmake
nix-build ci -A eval.vcpkg
nix-build ci -A eval.nix
```

### Full evaluation

```bash
# Run complete evaluation
nix-build ci -A eval.full
```

## Supported Systems

Evaluation is performed for the following platforms:

- `x86_64-linux` - Linux (64-bit)
- `x86_64-darwin` - macOS Intel
- `aarch64-darwin` - macOS Apple Silicon
- `x86_64-windows` - Windows (via cross-compilation)

## Configuration

The following arguments can be used:

- `--arg quickTest true`: Enable quick validation mode
- `--arg systems '["x86_64-linux"]'`: Limit to specific systems

## Project Structure Validation

The evaluation checks:

### CMake Configuration

- `CMakeLists.txt` - Main build configuration
- `cmake/*.cmake` - CMake modules
- `CMakePresets.json` - Build presets

### Dependencies

- `vcpkg.json` - vcpkg dependencies
- `vcpkg-configuration.json` - vcpkg settings

### Nix Build

- `flake.nix` - Nix flake definition
- `default.nix` - Default Nix expression
- `shell.nix` - Development shell

## CI Integration

Evaluation now runs automatically in `.github/workflows/eval.yml`. The workflow installs Nix on `ubuntu-latest`, evaluates the module with `nix-build --expr 'let pkgs = import <nixpkgs> {}; eval = (import ./ci/eval { inherit (pkgs) lib runCommand cmake nix jq; }) {}; in eval.full'`, and publishes the generated summary to the workflow run. Trigger it manually with **Run workflow** or let it execute on every pull request. You can mirror the same steps locally with:

```bash
NIX_PATH=nixpkgs=channel:nixos-unstable \
nix-build --expr 'let pkgs = import <nixpkgs> {}; eval = (import ./ci/eval { inherit (pkgs) lib runCommand cmake nix jq; }) {}; in eval.full'
cat result/summary.md
```
