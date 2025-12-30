---
title: Download ProjT Launcher for Windows
eleventyNavigation:
  key: <i class="fa fa-windows" aria-hidden="true"></i> Windows
  order: 4
---

<div class="download-content">
  <div class="row">
    <div class="column">
      <div>
        <h1>Windows Download</h1>
        <p>Windows 10/11 64bit</p>
        <a class="button size-large" href="https://github.com/Project-Tick/ProjT-Launcher/releases/download/{{version.current}}/ProjTLauncher-Windows-MSVC-Setup-{{version.current}}.exe">Installer (.exe)</a>
        <a class="button size-large" href="https://github.com/Project-Tick/ProjT-Launcher/releases/download/{{version.current}}/ProjTLauncher-Windows-MinGW-w64-Portable-{{version.current}}.zip">Portable (.zip)</a>
        <p>Windows 10/11 ARM64</p>
        <a class="button size-large" href="https://github.com/Project-Tick/ProjT-Launcher/releases/download/{{version.current}}/ProjTLauncher-Windows-MSVC-arm64-Setup-{{version.current}}.exe">Installer (ARM64) (.exe)</a>
        <a class="button size-large" href="https://github.com/Project-Tick/ProjT-Launcher/releases/download/{{version.current}}/ProjTLauncher-Windows-MSVC-arm64-Portable-{{version.current}}.zip">Portable (ARM64) (.zip)</a>
      </div>
    </div>
  </div>
</div>

<div class="infobox top">

# Advanced Windows install options

## MinGW-w64

These builds are built with MinGW and do not require the Visual C++ Redistributable to be installed on your system. They require Windows 10/11 64bit.

Note: These builds are less tested than the MSVC builds.

- [Installer (64bit) (.exe)](https://github.com/Project-Tick/ProjT-Launcher/releases/download/{{version.current}}/ProjTLauncher-Windows-MinGW-w64-Setup-{{version.current}}.exe)

<!-- ## Winget Package

```powershell
winget install --exact yongdohyun.ProjTLauncher
```

</div>

<div class="infobox top">

# Community Packages

## Scoop Package

```powershell
scoop bucket add games
scoop install projtlauncher 
```

Unstable [development builds](/wiki/development/development-builds) are also available in the `versions` bucket.

```powershell
scoop bucket add versions
scoop install projtlauncher-git
```

## Chocolatey Package

```powershell
choco install projtlauncher
```

## [PortableApps.com](https://portableapps.com) Installer

A portable installer for ProjT Launcher can be found here: <https://RuiNtD.github.io/ProjTLauncherPortable/> -->

</div>
