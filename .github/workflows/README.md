# GitHub Actions Workflows

This directory contains workflow configurations for the ProjT Launcher CI/CD pipeline.

## Overview

The workflows are organized by functionality:

### Build & Test Workflows

- **`build.yml`**: Main build workflow that compiles the launcher on Linux, macOS, and Windows
  - Triggers: Push to `develop`/`release-*`, Pull Requests, Manual dispatch
  - Tests code, builds packages, uploads artifacts
  
- **`lint.yml`**: Code quality and formatting checks
  - Uses `clang-format` to validate C++ code formatting
  - Checks CMakeLists.txt and configuration files
  - Runs on: Pull Requests, Push to develop/release branches

- **`eval.yml`**: Dependency and configuration validation
  - Verifies CMakeLists.txt, vcpkg.json, conan.txt syntax
  - Validates required dependencies (Qt6, CMake, etc.)
  - Ensures configuration files are valid JSON

### Release & Maintenance Workflows

- **`nix.yml`**: Nix Flake builds and checks
  - Checks flake.nix syntax and outputs
  - Builds packages via Nix on multiple systems
  - Validates flake checks

- **`backport.yml`**: Automated backport to release branches
  - Creates backport PRs when labeled with `backport/release-*`
  - Requires PR to be merged first
  - Automatically adds labels and comments

### Other Workflows

- **`publish.yml`**: Publishing and release artifacts
- **`release.yml`**: Release management
- **`flatpak.yml`**: Flatpak package building
- **`update-flake.yml`**: Flake.lock updates
- **`codeql.yml`**: Security analysis

## Key Design Principles

### Push Events

- Most workflows use standard `push` events to develop/release branches
- This allows external contributors to test workflows without prior approval

### Pull Request Events

- Workflows use `pull_request` event type (not `pull_request_target`)
- Code review and approval by maintainers protects the repository

### Path Filters

- Workflows use path filters to avoid unnecessary runs
- For example, `build.yml` only runs when C++, CMake, or workflow files change

## Workflow Files

## Workflow File Structure

Each workflow file contains:

- Trigger conditions (on: events)
- Permissions (least privilege principle)
- Jobs with specific steps
- Environment variables for consistency

## Common Patterns

### Setup & Checkout

```yaml
- uses: actions/checkout@v4.2.2
  with:
    fetch-depth: 0  # Full history for git operations
```

### Conditional Steps

### Conditional Steps Example

```yaml
- name: Step name
  if: github.ref_type == 'tag'  # Only run on tags
  run: echo "Release build"
```

### Artifacts & Uploads

### Artifacts & Uploads Example

```yaml
- uses: actions/upload-artifact@v5.0.0
  with:
    name: build-artifacts
    path: build/launcher
```

## Testing Workflows Locally

For most workflows, you can test locally using:

```bash
# Lint check
clang-format -i launcher/**/*.cpp launcher/**/*.h

# Build test
cmake --preset linux && cmake --build --preset linux

# Flake check (if using Nix)
nix flake check
```

## Adding New Workflows

When adding new workflows:

1. Follow the naming convention: lowercase, hyphens for spaces
2. Use path filters to avoid unnecessary runs
3. Apply least privilege permissions
4. Use github.token by default (avoid elevated privileges)
5. Add documentation in this README

## Troubleshooting

## Troubleshooting Guide

- **Workflow not triggering**: Check path filters and branch configuration
- **Permission denied errors**: Verify permissions: section
- **Tests failing locally but passing in CI**: Check environment differences (dependencies, paths)
- **Slow workflows**: Consider using caching or parallelization
