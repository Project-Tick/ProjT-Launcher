# Nix Packaging `nix/`

> **Location**: `nix/`, `flake.nix`, `shell.nix`, `default.nix`  
> **Platform**: NixOS, Nix package manager  
> **Purpose**: Reproducible builds and packaging
> **Latest Version**: 0.0.5-1

---

## Overview

ProjT Launcher provides first-class Nix support for reproducible builds, development environments, and packaging. Both Flakes and traditional Nix expressions are supported.

---

## Quick Start

### Run Without Installing

```bash
nix run github:Project-Tick/ProjT-Launcher
```

### Install via Flakes

```bash
nix profile install github:Project-Tick/ProjT-Launcher
```

### Development Shell

```bash
# With Flakes
nix develop github:Project-Tick/ProjT-Launcher

# Traditional
nix-shell
```

---

## Binary Cache

We use Cachix for pre-built binaries. Add to avoid rebuilds:

<!-- ### NixOS Configuration

```nix
{
  nix.settings = {
    trusted-substituters = [ "https://cache.projecttick.org" ];
    trusted-public-keys = [
      "cache.projecttick.org-1:HrpR1buYLhqx0ooS1rMgyHChoYf+faZm82hsIY6JS+s="
    ];
  };
}
``` -->

### Flakes (Temporary)

```bash
nix run github:Project-Tick/ProjT-Launcher --accept-flake-config
```

---

## Installation Methods

### Flakes (Recommended)

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    projtlauncher.url = "github:Project-Tick/ProjT-Launcher";
  };

  outputs = { nixpkgs, projtlauncher, ... }: {
    nixosConfigurations.myhost = nixpkgs.lib.nixosSystem {
      modules = [
        ({ pkgs, ... }: {
          environment.systemPackages = [
            projtlauncher.packages.${pkgs.system}.projtlauncher
          ];
        })
      ];
    };
  };
}
```

### Using Overlay

```nix
{
  nixpkgs.overlays = [ projtlauncher.overlays.default ];
  environment.systemPackages = [ pkgs.projtlauncher ];
}
```

### Traditional Nix (No Flakes)

```nix
{ pkgs, ... }:
{
  environment.systemPackages = [
    (import (builtins.fetchTarball 
      "https://github.com/Project-Tick/ProjT-Launcher/archive/develop.tar.gz"
    )).packages.${pkgs.system}.projtlauncher
  ];
}
```

---

## Package Variants

| Package | Description |
|---------|-------------|
| `projtlauncher` | Fully wrapped with runtime dependencies |
| `projtlauncher-unwrapped` | Minimal build for customization |

### Customization Options

The wrapped package accepts these overrides:

| Option | Default | Description |
|--------|---------|-------------|
| `additionalLibs` | `[]` | Extra `LD_LIBRARY_PATH` entries |
| `additionalPrograms` | `[]` | Extra `PATH` entries |
| `controllerSupport` | `isLinux` | Game controller support |
| `gamemodeSupport` | `isLinux` | Feral GameMode integration |
| `jdks` | `[jdk21 jdk17 jdk8]` | Available Java runtimes |
| `msaClientID` | `null` | Microsoft Auth client ID |
| `textToSpeechSupport` | `isLinux` | TTS support |

### Example Override

```nix
projtlauncher.override {
  jdks = [ pkgs.jdk21 pkgs.jdk17 ];
  gamemodeSupport = false;
}
```

---

## Development

### Enter Development Shell

```bash
nix develop
# or
nix-shell
```

### Build Locally

```bash
nix build .#projtlauncher
./result/bin/projtlauncher
```

---

## File Structure

```
├── flake.nix          # Flake definition
├── flake.lock         # Locked dependencies
├── default.nix        # Flake-compat entry
├── shell.nix          # Development shell
└── nix/
    ├── default.nix    # Package derivation
    └── ...
```

---

## Troubleshooting

### Binary Cache Not Working

Ensure the cache is in `trusted-substituters` (requires root):

```bash
sudo nix-channel --update
```

### Build Failures

Try with fresh nixpkgs:

```bash
nix build --override-input nixpkgs github:NixOS/nixpkgs/nixos-unstable
```

---

## Related Documentation

- [CI Support](./ci_support.md) — CI Nix integration
- [CI Evaluation](./ptcieval.md) — Nix-based validation

---

## External Links

- [Nix Manual](https://nixos.org/manual/nix/stable/)
- [NixOS Wiki: ProjT Launcher](https://wiki.nixos.org/wiki/ProjT_Launcher)
- [Cachix](https://cachix.org/)
