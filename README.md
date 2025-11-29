<p align="center">
  <picture>
    <source srcset="/program_info/tr.org.yongdohyun.ProjTLauncher.logo-darkmode.svg">
    <img alt="ProjT Launcher" src="/program_info/tr.org.yongdohyun.ProjTLauncher.logo.svg" width="40%">
  </picture>
</p>

<p align="center">
  <img alt="ProjT Launcher Main Menu" src="https://projtlauncher.yongdohyun.org.tr/img/screenshots/launcher.png" width="80%">
</p>
````markdown
<p align="center">
  <strong>ProjT Launcher — QML Migration</strong>
</p>

This README focuses on the project's migration of the user interface to QML (Qt Quick). The goal is to move from the current C++/Widgets mixed UI to a modern, maintainable, and higher-performance QML-based UI.

Short summary:
- Branch: `qml_migration` — QML-related work is collected on this branch.
- Goal: A faster, more flexible, and modern UI built with modular components.
- Status: In development. Some packaging scripts already account for QML asset locations (for example `debian/rules`).

Why QML?
- Performance: GPU-accelerated rendering and smoother UI.
- Faster prototyping: UI changes are quicker to iterate in QML.
- Modern UX: Easier animations and responsive layouts.

Developer requirements
- CMake (3.22+ recommended)
- Qt 6 (Qt Quick / QML modules) — e.g. packages like `qt6-base`, `qt6-declarative`/`qt6-quick`
- C++20 capable compiler
- Git and submodules (if used)

Quick start (example commands for fish shell)
```fish
# switch to the QML branch
git switch qml_migration

# configure (Debug recommended for development)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# build
cmake --build build -j (nproc)

# find a built launcher binary (example)
find build -type f -executable -name '*launcher*' -print -quit

# run the binary you found (example)
./build/path/to/your/launcher
```

Note: binary names and locations depend on your CMake configuration; the CMake target name is set as `Launcher_Name` inside the build files.

Testing & debugging
- Use `QT_LOGGING_RULES` and run the app from a terminal to see QML compile/runtime errors and warnings.
- QML engine errors print to stdout/stderr and are helpful to trace missing modules or binding issues.

Where to look in this repo
- C++ side: the `launcher/` directory contains `Application` and startup/back-end code where the QML integration points (C++ to QML bridges) are expected.
- Packaging: `debian/rules` and similar scripts show how QML files are packaged for distributions.
- There might not yet be a single `qml/` directory; QML assets will be added and their locations may evolve during the migration.

How to contribute
- Build small, isolated QML components and expose minimal C++ APIs for integration.
- Port UI skins/themes by translating visual structure into QML components.
- Run integration tests across platforms (Linux, macOS, Windows) to ensure consistent behavior.

Common issues
- "QML module not found": ensure required Qt QML modules are installed on the system.
- Performance regressions: run Qt Quick performance tools and profile to find heavy bindings or unnecessary repaints.

Next steps / roadmap
- Migrate core UI screens to QML
- Split UI into reusable component modules
- Improve theming and accessibility (a11y)
- Validate packaging and distribution (flatpak / deb / nix)

Support & contact
- Open issues on GitHub for bugs or feature requests.
- If you want a detailed example component or platform-specific instructions, tell me which platform to prioritize and I can add an example QML component and integration guide.

---

<p align="center">Made with love for the ProjT contributors.</p>
