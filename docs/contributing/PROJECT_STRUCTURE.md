# Project Structure

Directory layout and file placement guide.

---

## Directory Layout

```
ProjT-Launcher/
├── launcher/              # Main application
│   ├── ui/                # Qt Widgets
│   │   ├── pages/         # Main screens
│   │   ├── widgets/       # Reusable components
│   │   ├── dialogs/       # Modal windows
│   │   └── setupwizard/   # First-run wizard
│   ├── minecraft/         # Game logic
│   │   ├── auth/          # Account authentication
│   │   ├── launch/        # Game process
│   │   ├── mod/           # Mod loading
│   │   └── versions/      # Version parsing
│   ├── net/               # Networking
│   ├── tasks/             # Background jobs
│   ├── java/              # Java runtime
│   ├── modplatform/       # Mod platform APIs
│   ├── resources/         # Images, themes
│   ├── icons/             # App icons
│   └── translations/      # Language files
├── tests/                 # Unit tests
├── cmake/                 # Build configuration
└── docs/                  # Documentation
```

---

## File Placement

### C++ Files

| Location | Purpose |
|----------|---------|
| `launcher/ui/` | Qt Widgets UI |
| `launcher/minecraft/` | Game logic |
| `launcher/net/` | HTTP, downloads |
| `launcher/tasks/` | Async operations |
| `launcher/modplatform/` | Modrinth, CurseForge |

**Note**: `minecraft/` is for Minecraft-specific logic only (versions, mods, launch process). Generic launcher functionality belongs in `launcher/` or appropriate submodules.

### UI Files

| Location | Purpose |
|----------|---------|
| `launcher/ui/widgets/` | Reusable widgets |
| `launcher/ui/pages/` | Main screens |
| `launcher/ui/dialogs/` | Popups, modals |
| `launcher/ui/setupwizard/` | First-run flow |

### Assets

| Location | Format |
|----------|--------|
| `launcher/resources/` | PNG, SVG |
| `launcher/icons/` | ICO, PNG, SVG |
| `launcher/translations/` | .ts |

---

## Quick Reference

| I want to add... | Location |
|------------------|----------|
| New screen | `launcher/ui/pages/` |
| Reusable widget | `launcher/ui/widgets/` |
| Modal dialog | `launcher/ui/dialogs/` |
| Network API | `launcher/net/` |
| Background job | `launcher/tasks/` |
| Game logic | `launcher/minecraft/` |
| Unit test | `tests/` |

---

## Naming

| Type | Convention | Example |
|------|------------|---------|
| C++ class | PascalCase | `InstanceList.cpp` |
| UI file | PascalCase | `SettingsPage.ui` |
| Asset | kebab-case | `app-icon.png` |
| Test | PascalCase_test | `FileSystem_test.cpp` |

---

## Rules

- No circular dependencies between modules
- `ui/` → `core` → `data` layering (conceptual layers, not directory names)
- Tests mirror source structure
- Do not create new top-level directories without maintainer approval

---

## Third-Party Libraries

Location: Root directory (e.g., `zlib/`, `quazip/`)

All third-party code is maintained as detached forks. See [third-party.md](../handbook/third-party.md) for the complete list, upstream references, and patch policies.

---

## Test Scope

- `tests/` contains unit tests primarily
- Integration tests go in `tests/` but must be clearly named
- UI tests are discouraged; prefer testing core logic
- See [TESTING.md](./TESTING.md) for test standards

---

## Related

- [Architecture](./ARCHITECTURE.md)
- [Testing](./TESTING.md)
