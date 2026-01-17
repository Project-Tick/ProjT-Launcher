# CI Support `ci/`

> **Location**: `ci/`  
> **Purpose**: CI configuration and support files
> **Latest Version**: 0.0.5-1

---

## Overview

The `ci/` directory contains configuration files, scripts, and Nix expressions that support the CI/CD pipeline. These files are used by GitHub Actions workflows for code quality, validation, and build processes.

---

## Directory Structure

```
ci/
├── code-quality.nix        # Nix expression for code quality tools
├── code-quality.sh         # Shell script for linting/formatting
├── default.nix             # Default Nix entry point
├── parse.nix               # Nix parsing utilities
├── pinned.json             # Pinned Nix package versions
├── supportedBranches.js    # Branch configuration
├── supportedSystems.json   # Target system matrix
├── supportedVersions.nix   # Version compatibility
├── update-pinned.sh        # Update pinned packages
├── OWNERS                  # Code ownership file
│
├── codeowners-validator/   # CODEOWNERS validation
├── eval/                   # Evaluation helpers
├── github-script/          # GitHub Script automation
└── nixpkgs-vet/            # Nixpkgs validation
```

---

## File Descriptions

### Core Files

| File | Description |
|------|-------------|
| `code-quality.nix` | Nix expression defining code quality tool environment |
| `code-quality.sh` | Main script for running clang-format, linters, etc. |
| `default.nix` | Default Nix derivation entry point |

### Configuration

| File | Description |
|------|-------------|
| `pinned.json` | Pinned nixpkgs revision for reproducibility |
| `supportedSystems.json` | Build matrix (Linux, macOS, Windows) |
| `supportedVersions.nix` | Supported version compatibility matrix |
| `supportedBranches.js` | CI branch filtering rules |

### Subdirectories

| Directory | Purpose |
|-----------|---------|
| `codeowners-validator/` | Validates CODEOWNERS file format |
| `eval/` | Nix evaluation helpers for project validation |
| `github-script/` | JavaScript helpers for GitHub Actions |
| `nixpkgs-vet/` | Nixpkgs best practices validation |

---

## Usage

### Running Code Quality Checks

```bash
# Using Nix
nix-shell ci/default.nix --run "./ci/code-quality.sh"

# Or directly
./ci/code-quality.sh
```

### Updating Pinned Packages

```bash
./ci/update-pinned.sh
```

---

## Integration

These files are used by:

- `ci-lint.yml` — Code quality workflow
- `ci-new.yml` — Main orchestrator
- `flake.nix` — Nix flake development shell

---

## Related Documentation

- [Workflows](./workflows.md) — GitHub Actions overview
- [CI Evaluation](./ptcieval.md) — Project validation
- [GitHub Scripts](./ptcigh.md) — Automation helpers
- [Nix Packaging](./nix.md) — Nix build system
