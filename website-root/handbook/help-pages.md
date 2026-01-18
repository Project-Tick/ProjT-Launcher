# ProjT Launcher Help Pages

> **Player docs**: `docs/handbook/help-pages/` (copied from `website/projtlauncher/wiki/help-pages/`)  
> **Assets**: `website/projtlauncher/img/screenshots/` (`/projtlauncher/img/screenshots/...`)
> **Latest Version**: 0.0.5-1

---

## Overview

User-facing help articles for ProjT Launcher cover launcher settings, platform importers, and common instance workflows. The canonical copies now live in the handbook under `docs/handbook/help-pages/` so they can be built alongside the rest of the documentation. Content originated from the upstream Prism Launcher wiki but has been rebranded and rewired to local screenshot paths.

---

## Content Map

- **Settings**: `apis.md`, `launcher-settings.md`, `java-settings.md`, `java-wizard.md`, `language-settings.md`, `minecraft-settings.md`, `proxy-settings.md`, `notes.md`.
- **Instances & worlds**: `instance-copy.md`, `instance-version.md`, `worlds.md`, `screenshots-management.md`, `zip-import.md`.
- **Platforms/importers**: `vanilla-platform.md`, `mod-platform.md`, `modrinth-platform.md`, `ftb-platform.md`, `technic-platform.md`, `atl-platform.md`, `flame-platform.md`.
- **Mods/loaders**: `loader-mods.md` for selecting Fabric/Quilt/Forge/NeoForge components.
- **Automation & tools**: `custom-commands.md`, `environment-variables.md`, `tools.md`, and API key setup in `apis.md`.

---

## Assets & Linking

- Screenshots live in `website/projtlauncher/img/screenshots/` and are referenced as `/projtlauncher/img/screenshots/<file>.png`.
- Cross-page links target sibling handbook pages or existing getting-started docs (for example `../getting-started/installing-java/`).
- Several articles include `<!-- TODO: ... -->` placeholders for missing screenshots or descriptions (notably APIs, Launcher Settings, Minecraft Settings, Proxy Settings, Tools, Instance Version).

---

## Editing / Porting Notes

- Handbook copies do not use frontmatter; the first heading is injected if missing.
- Add or update screenshots in `website/projtlauncher/img/screenshots/` and keep paths consistent (`/projtlauncher/img/screenshots/<file>.png`).
- If updating the upstream wiki versions in `website/projtlauncher/wiki/help-pages/`, mirror the edits into `docs/handbook/help-pages/` so handbook builds stay in sync.
