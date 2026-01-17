# CI Evaluation `ci/eval/`

> **Location**: `ci/eval/`  
> **Platform**: Nix  
> **Purpose**: Project configuration validation
> **Latest Version**: 0.0.5-1

---

## Overview

The CI evaluation module provides Nix-based validation of the project structure and configuration. It catches errors in build files before they reach CI.

---

## What It Validates

| Component | Checks |
|-----------|--------|
| **CMake** | Syntax, target definitions |
| **vcpkg** | Dependency declarations |
| **Nix** | Flake structure, expressions |
| **Build** | Cross-platform configuration |

---

## Quick Usage

### Validate Everything

```bash
nix-build ci -A eval.full
```

### Validate Specific Component

```bash
nix-build ci -A eval.cmake
nix-build ci -A eval.vcpkg
nix-build ci -A eval.nix
```

### Quick Test Mode

```bash
nix-build ci -A eval.validate --arg quickTest true
```

---

## Supported Systems

| System | Platform |
|--------|----------|
| `x86_64-linux` | Linux 64-bit |
| `x86_64-darwin` | macOS Intel |
| `aarch64-darwin` | macOS Apple Silicon |
| `x86_64-windows` | Windows (cross) |

### Limit to Specific System

```bash
nix-build ci -A eval.full --arg systems '["x86_64-linux"]'
```

---

## Configuration Validation

### CMake Files

- `CMakeLists.txt` — Main configuration
- `cmake/*.cmake` — CMake modules
- `CMakePresets.json` — Build presets

### Dependencies

- `vcpkg.json` — vcpkg dependencies
- `vcpkg-configuration.json` — vcpkg settings

### Nix Build

- `flake.nix` — Flake definition
- `default.nix` — Default expression
- `shell.nix` — Development shell

---

## CI Integration

Evaluation runs in `.github/workflows/eval.yml`:

```yaml
- name: Evaluate
  run: |
    nix-build --expr 'let
      pkgs = import <nixpkgs> {};
      eval = (import ./ci/eval { inherit (pkgs) lib runCommand cmake nix jq; }) {};
    in eval.full'
```

---

## Local Replication

```bash
NIX_PATH=nixpkgs=channel:nixos-unstable \
nix-build --expr 'let
  pkgs = import <nixpkgs> {};
  eval = (import ./ci/eval { inherit (pkgs) lib runCommand cmake nix jq; }) {};
in eval.full'

cat result/summary.md
```

---

## Related Documentation

- [Workflows](./workflows.md) — CI overview
- [CI Support](./ci_support.md) — Support files
- [Nix Packaging](./nix.md) — Nix build system
