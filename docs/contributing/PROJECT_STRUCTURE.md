# 📁 Project Structure and Organization

## ⚠️ Organization Policy

The project structure is designed to keep the codebase maintainable. Files should be placed in the correct directory to ensure consistency.

## 🧠 The Mental Model (How to think about the code)

Think of the project like a **Restaurant**:

1. **`ui/` & `qml/` (The Waiters)**:
   * They talk to the customer (User).
   * They take orders (Clicks, Input).
   * They show the food (UI).
   * *Rule:* Waiters **never** cook the food. (No business logic in UI).

2. **`viewmodels/` (The Head Chef)**:
   * They take the order from the Waiter.
   * They decide what needs to be done.
   * They tell the Line Cooks what to do.
   * *Rule:* The Chef doesn't serve the table. (No UI code in ViewModels).

3. **`minecraft/`, `net/`, `tasks/` (The Line Cooks)**:
   * They do the hard work: Grilling (Launching Game), Chopping (Unzipping files), Buying ingredients (Downloading).
   * They don't care who ordered the food.

4. **`resources/` (The Pantry)**:
   * Where the ingredients (Icons, Images, Fonts) are stored.

---

## Directory Hierarchy (Mandatory)

```text
ProjT-Launcher/
├── launcher/                    # Main application source
│   ├── ui/                      # C++ classes that manage Windows (The "Frame")
│   ├── viewmodels/              # C++ classes that hold UI state (The "Brain")
│   ├── net/                     # Networking (Downloading files, API calls)
│   ├── minecraft/               # Minecraft Logic (The "Core")
│   │   ├── auth/                # Logging in (Microsoft/Yggdrasil)
│   │   ├── launch/              # Starting the game process
│   │   ├── mod/                 # Installing Mods (Fabric/Forge)
│   │   └── versions/            # Reading version.json files
│   ├── tasks/                   # Long-running jobs (Progress bars)
│   ├── qml/                     # The Visual UI (The "Face")
│   │   ├── components/          # Reusable buttons, text fields
│   │   ├── dialogs/             # Pop-up windows
│   │   ├── settings/            # Settings sub-pages
│   │   ├── instance/            # Instance management pages
│   │   ├── *.qml                # Main Pages (AboutPage.qml, NewsPage.qml)
│   │   └── Theme.js             # Colors and Fonts
│   └── resources/               # Assets (Icons, Images)
├── tests/                       # Unit Tests (Must mirror source structure)
├── libraries/                   # Third-party code (Don't touch this)
├── cmake/                       # Build scripts
└── docs/                        # You are here
```

## File Placement Rules (Where does my file go?)

### 1. C++ Files

| Folder | What goes here? | Example |
| ------ | -------------- | ------- |
| `launcher/viewmodels/` | **Logic for a specific screen.** If you have a "Settings Page", you need a "SettingsViewModel". | `SettingsViewModel.cpp` |
| `launcher/ui/` | **Window management.** Code that talks to the OS window system (System Tray, Taskbar). | `QmlMainWindow.cpp` |
| `launcher/minecraft/` | **Game Logic.** Anything related to Minecraft itself. | `MinecraftInstance.cpp` |
| `launcher/net/` | **Internet stuff.** Downloading files, checking server status. | `NetJob.cpp` |
| `launcher/` | **General Utilities.** String manipulation, File system helpers. | `StringUtils.cpp` |

### 2. QML Files

| Folder | What goes here? | Example |
| ------ | -------------- | ------- |
| `launcher/qml/components/` | **Reusable Widgets.** Buttons, Checkboxes, Cards that are used in multiple places. | `RoundButton.qml` |
| `launcher/qml/` | **Main Pages.** Top-level screens accessible from the navigation. | `NewsPage.qml` |
| `launcher/qml/<feature>/` | **Sub-pages.** Pages specific to a feature (e.g., Settings tabs). | `settings/JavaSettingsPage.qml` |
| `launcher/qml/dialogs/` | **Popups.** Windows that open on top of the main window. | `LoginDialog.qml` |

### 3. Assets

| Folder | What goes here? | Format |
| ------ | -------------- | ------ |
| `launcher/resources/icons/` | App icons and UI symbols. | **SVG** (Preferred) |
| `launcher/resources/images/` | Backgrounds, logos. | **PNG/JPG** |
| `launcher/resources/translations/` | Language files. | **.ts** |

## 🔍 "Where do I put X?" Lookup Table

Use this table if you are unsure where a specific piece of code belongs.

| I want to add... | Where does it go? | Example |
| ---------------- | --------------- | ------- |
| A new **Button** style | `launcher/qml/components/` | `DangerButton.qml` |
| A new **Main Screen** (e.g., Mod Manager) | `launcher/qml/` | `ModManagerPage.qml` |
| A **Sub-screen** (e.g., Mod List) | `launcher/qml/modplatform/` | `ModListPage.qml` |
| Logic for that Screen | `launcher/viewmodels/` | `ModManagerViewModel.cpp` |
| A helper function (e.g., `formatDate`) | `launcher/` (or specific util) | `StringUtils.h` |
| A global constant (e.g., `MAX_RAM`) | `launcher/DefaultVariable.h` | `DefaultVariable.h` |
| A new **Dialog** (e.g., "Are you sure?") | `launcher/qml/dialogs/` | `ConfirmDialog.qml` |
| Code to unzip a file | `launcher/tasks/` | `UnzipTask.cpp` |
| Code to talk to a new API | `launcher/net/` | `CurseForgeAPI.cpp` |
| A new library (e.g., `jsoncpp`) | `libraries/` | `libraries/jsoncpp/` |

## File Naming Conventions (Strict)

| File Type | Naming Rule | Example |
| --------- | ----------- | ------- |
| C++ Class | `PascalCase` | `InstanceViewModel.cpp` |
| QML File | `PascalCase` | `RoundButton.qml` |
| JavaScript | `PascalCase` | `Theme.js` |
| Assets | `kebab-case` | `app-icon.png` |
| CMake | `snake_case` | `CMakeLists.txt` |
| Tests | `PascalCase_test` | `InstanceViewModel_test.cpp` |

## Module Boundaries

* **No Circular Dependencies**: `launcher/minecraft/` should not depend on `launcher/ui/`.
* **Layered Architecture**: `ui` -> `viewmodels` -> `logic` -> `data`.
* **Violation**: Breaking these boundaries will result in immediate PR rejection.

## Third-Party Libraries

* **Location**: `libraries/`
* **Policy**: Do not modify third-party code directly. Use patches in `cmake/patches/` if absolutely necessary.
* **Adding Libraries**: Prefer `vcpkg` or `FetchContent` over vendoring code.

## CMake Structure

* **Root**: `CMakeLists.txt` defines the project and global settings.
* **Subdirectories**: Each module should have its own `CMakeLists.txt` if it's a separate library.
* **Options**: Use `option()` for feature toggles.
