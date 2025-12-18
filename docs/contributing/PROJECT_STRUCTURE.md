# 📁 Project Structure and Organization

## ⚠️ Organization Policy

The project structure is designed to keep the codebase maintainable. Files should be placed in the correct directory to ensure consistency.

## 🧠 The Mental Model (How to think about the code)

Think of the project like a **Restaurant**:

1. **`ui/` (The Waiters)**:
   * They talk to the customer (User).
   * They take orders (Clicks, Input).
   * They show the food (UI).
   * *Rule:* Waiters **never** cook the food. (No business logic in UI).

2. **`launcher/`, `minecraft/`, `net/`, `tasks/` (The Head Chef + Line Cooks)**:
   * They take the order from the UI.
   * They decide what needs to be done and do the work.
   * They don't care who ordered the food.

3. **`resources/`, `icons/`, `translations/` (The Pantry)**:
   * Where the ingredients (Icons, Images, Fonts) are stored.

---

## Directory Hierarchy (Mandatory)

```text
ProjT-Launcher/
├── launcher/                    # Main application source
│   ├── ui/                      # Qt Widgets UI (.ui + C++)
│   │   ├── pages/               # Main pages (tabs, settings)
│   │   ├── widgets/             # Reusable widgets
│   │   ├── dialogs/             # Modal dialogs
│   │   └── setupwizard/         # First-run/setup wizard
│   ├── net/                     # Networking (Downloading files, API calls)
│   ├── minecraft/               # Minecraft Logic (The "Core")
│   │   ├── auth/                # Logging in (Microsoft/Yggdrasil)
│   │   ├── launch/              # Starting the game process
│   │   ├── mod/                 # Installing Mods (Fabric/Forge)
│   │   └── versions/            # Reading version.json files
│   ├── tasks/                   # Long-running jobs (Progress bars)
│   ├── java/                    # Java runtime discovery and metadata
│   ├── modplatform/             # Mod platform integrations
│   ├── resources/               # Assets (Icons, Images)
│   ├── icons/                   # App icons
│   └── translations/            # Language files (.ts)
├── tests/                       # Unit Tests (Must mirror source structure)
├── libraries/                   # Third-party code (Don't touch this)
├── cmake/                       # Build scripts
└── docs/                        # You are here
```

## File Placement Rules (Where does my file go?)

### 1. C++ Files

| Folder | What goes here? | Example |
| ------ | -------------- | ------- |
| `launcher/ui/` | **Qt Widgets UI.** Pages, dialogs, and widget logic. | `MainWindow.cpp` |
| `launcher/minecraft/` | **Game Logic.** Anything related to Minecraft itself. | `MinecraftInstance.cpp` |
| `launcher/net/` | **Internet stuff.** Downloading files, checking server status. | `NetJob.cpp` |
| `launcher/modplatform/` | **Mod platform integrations.** | `ModrinthPackInstallTask.cpp` |
| `launcher/` | **General Utilities.** String manipulation, File system helpers. | `StringUtils.cpp` |

### 2. UI Files (Qt Widgets)

| Folder | What goes here? | Example |
| ------ | -------------- | ------- |
| `launcher/ui/widgets/` | **Reusable Widgets.** Buttons, panels, controls used in multiple places. | `ProgressWidget.ui` |
| `launcher/ui/pages/` | **Main Pages.** Top-level screens accessible from navigation. | `LauncherPage.ui` |
| `launcher/ui/pages/<feature>/` | **Sub-pages.** Pages specific to a feature (e.g., settings tabs). | `settings/JavaSettingsPage.ui` |
| `launcher/ui/dialogs/` | **Dialogs.** Modal windows and popups. | `LoginDialog.ui` |
| `launcher/ui/setupwizard/` | **First-run flow.** Wizard pages. | `LoginWizardPage.ui` |

### 3. Assets

| Folder | What goes here? | Format |
| ------ | -------------- | ------ |
| `launcher/resources/` | Images, themes, shared assets. | **PNG/JPG/SVG** |
| `launcher/icons/` | App icons. | **SVG/PNG/ICO** |
| `launcher/translations/` | Language files. | **.ts** |

## 🔍 "Where do I put X?" Lookup Table

Use this table if you are unsure where a specific piece of code belongs.

| I want to add... | Where does it go? | Example |
| ---------------- | --------------- | ------- |
| A new **Button** style | `launcher/ui/widgets/` | `DangerButton.ui` |
| A new **Main Screen** (e.g., Mod Manager) | `launcher/ui/pages/` | `ModManagerPage.ui` |
| A **Sub-screen** (e.g., Mod List) | `launcher/ui/pages/modplatform/` | `ModListPage.ui` |
| Logic for that Screen | `launcher/ui/` (paired C++ class) | `ModManagerPage.cpp` |
| A helper function (e.g., `formatDate`) | `launcher/` (or specific util) | `StringUtils.h` |
| A global constant (e.g., `MAX_RAM`) | `launcher/DefaultVariable.h` | `DefaultVariable.h` |
| A new **Dialog** (e.g., "Are you sure?") | `launcher/ui/dialogs/` | `ConfirmDialog.ui` |
| Code to unzip a file | `launcher/tasks/` | `UnzipTask.cpp` |
| Code to talk to a new API | `launcher/net/` | `CurseForgeAPI.cpp` |
| A new library (e.g., `jsoncpp`) | `libraries/` | `libraries/jsoncpp/` |

## File Naming Conventions (Strict)

| File Type | Naming Rule | Example |
| --------- | ----------- | ------- |
| C++ Class | `PascalCase` | `InstanceList.cpp` |
| UI Form (`.ui`) | `PascalCase` | `RoundButton.ui` |
| Qt Widget Class | `PascalCase` | `RoundButton.cpp` |
| Assets | `kebab-case` | `app-icon.png` |
| CMake | `snake_case` | `CMakeLists.txt` |
| Tests | `PascalCase_test` | `FileSystem_test.cpp` |

## Module Boundaries

* **No Circular Dependencies**: `launcher/minecraft/` should not depend on `launcher/ui/`.
* **Layered Architecture**: `ui` -> `core` -> `data`.
* **Violation**: Breaking these boundaries will result in immediate PR rejection.

## Third-Party Libraries

* **Location**: `libraries/`
* **Policy**: Do not modify third-party code directly. Use patches in `cmake/patches/` if absolutely necessary.
* **Adding Libraries**: Prefer `vcpkg` or `FetchContent` over vendoring code.

## CMake Structure

* **Root**: `CMakeLists.txt` defines the project and global settings.
* **Subdirectories**: Each module should have its own `CMakeLists.txt` if it's a separate library.
* **Options**: Use `option()` for feature toggles.
