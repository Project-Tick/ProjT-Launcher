# 🚀 Getting Started

## ⚠️ Compliance Warning

**This project requires a consistent development environment.** Deviating from the specified tool versions or configurations may result in build failures.

These steps focus on the launcher. For the bot, website, or metadata generator, see their respective README files.

## Prerequisites

Before you write any code, you need to set up your workspace. If you are new to open source, follow these steps carefully.

### 1. Understand the Rules

- **Code of Conduct**: We want a safe community. Read [CODE_OF_CONDUCT.md](../../CODE_OF_CONDUCT.md).
- **Security**: Found a hack? Don't post it publicly! Read [SECURITY.md](../../SECURITY.md).

### 2. Get the Code (Fork & Clone)

You cannot edit the main project directly. You need your own copy.

1. **Fork**: Click the **Fork** button at the top right of the GitHub page. This creates a copy of `ProjT-Launcher` in your own account (e.g., `YourName/ProjT-Launcher`).
2. **Clone**: Download your fork to your computer. Open your terminal (PowerShell or Terminal) and run:

   ```bash
   # Replace 'YourName' with your GitHub username
   git clone https://github.com/YourName/ProjT-Launcher.git
   cd ProjT-Launcher
   ```

3. **Add Upstream**: Link your local copy to the original project so you can get updates.

   ```bash
   git remote add upstream https://github.com/YongDo-Hyun/ProjT-Launcher.git
   ```

---

## 🛠 Development Environment

## Required Tools (And Why We Need Them)

We use specific tools to build the launcher. You **must** install the exact versions listed.

| Tool | Version | What is it? | Why strict? |
| ------ | --------- | ------------- | ------------- |
| **CMake** | 3.22+ | The "Builder". It creates the instructions for how to compile the code. | Old versions miss features we use. |
| **Qt** | 6.x (Latest) | The "Framework". It provides the windows, buttons, and graphics. | **CRITICAL**. We use Qt 6 features. |
| **Compiler** | C++20 | The "Translator". Converts C++ code into an executable (.exe). | We use modern C++20 features (concepts, ranges). |
| **Ninja** | 1.10+ | The "Worker". It executes the build instructions from CMake very fast. | It's much faster than Make or MSBuild. |
| **Git** | 2.30+ | The "Time Machine". Tracks changes and versions. | Required for version control. |
| **Java JDK** | 8, 17, 21 | The "Engine". Required to actually run Minecraft. | Minecraft requires specific Java versions. |
| **Libraries** | Latest | cmark, zlib, quazip, tomlplusplus, qrcodegencpp | These must be installed on your system. |

## 🖥️ OS-Specific Setup

### Windows

We recommend using **Visual Studio 2022** or **VS Code** with the MSVC compiler.

- Install Visual Studio 2022 Community:
  - Workload: "Desktop development with C++"
- Install Qt 6:
  - Use the Qt Online Installer.
  - Select Custom installation.
  - Select `Qt 6.10.1` -> `MSVC 2022 64-bit`.
  - Select `All additional libraries`.
  - Select `Qt Shader Tools`.
- Install NuGet (Dependency Manager):
  - Add the GitHub Packages source for ProjT dependencies:
    - `nuget sources add -Name project-tick -Source https://nuget.pkg.github.com/Project-Tick/index.json -UserName <github-username> -Password <PAT> -StorePasswordInClearText`
  - Restore binary dependencies into `dependencies/`:
    - `nuget install ./packages.config -OutputDirectory dependencies -ConfigFile nuget.config`

### Linux

- Install Nix: Follow the instructions at <https://nixos.org/download.html>
- Enter the development environment:

```bash
nix develop .#default
```

This will provide all necessary dependencies including Qt 6, CMake, Ninja, and compilers.

### macOS

- Install Nix: Follow the instructions at <https://nixos.org/download.html>
- Enter the development environment:

```bash
nix develop .#default
```

This will provide all necessary dependencies including Qt 6, CMake, Ninja, and compilers.

---

## ⚙️ IDE Configuration

### Visual Studio Code (Highly Recommended)

- Extensions:
  - **C/C++** (Microsoft)
  - **CMake Tools** (Microsoft)
  - **clangd** (LLVM) - *Disable C/C++ IntelliSense if using this.*
  - **Qt All Extensions Pack** (Optional, for Qt UI files and tooling)

- Settings (`.vscode/settings.json`):

```json
{
    "cmake.configureOnOpen": true,
    "cmake.generator": "Ninja",
    "files.associations": {
        "*.h": "cpp",
        "*.cpp": "cpp",
        "*.ui": "xml"
    }
}
```

- Launch Configuration (`.vscode/launch.json`):
  Use this to debug the application directly from VS Code.

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Launcher",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [
                {
                    "name": "QT_LOGGING_RULES",
                    "value": "*.debug=true"
                }
            ],
            "console": "integratedTerminal"
        }
    ]
}
```

## 🏗️ Building the Project

### Using VS Code (Easiest)

- Open the folder in VS Code.
- Select your Kit (e.g., `Visual Studio Community 2022 Release - amd64`).
- Click **Build** in the status bar (or press `F7`).
- Click **Run** (play button) to start the launcher.

### Using Command Line

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure (Replace path to Qt if not in PATH)
cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.x/msvc2019_64" -GNinja ..

# 3. Build
ninja

# 4. Run
./Launcher.exe
```

- Debug Configuration (`.vscode/launch.json`):
  Create this file to enable F5 debugging.

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Launcher",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [
                {
                    "name": "PATH",
                    "value": "${env:PATH};C:/Qt/6.10.1/msvc2019_64/bin"
                }
            ],
            "console": "integratedTerminal"
        }
    ]
}
```

### Qt Creator

- Open Project: Open `CMakeLists.txt` as a project.
- Kit Selection: Select the Kit matching your Qt 6.10.1 installation.
- Code Style: Go to `Tools > Options > C++ > Code Style` and import `.clang-format`.

---

## 🏗️ Build Instructions (Step-by-Step)

Building means converting the source code (text files) into an executable program (like `.exe`)

### Step 1: Configure

**What it does:** Checks your system for the required tools (Qt, Compiler) and creates a "build plan".
**Command:**

```bash
# First, see what presets are available for your OS
cmake --list-presets

# For Windows (Visual Studio):
cmake --preset windows_msvc

# For Linux:
cmake --preset linux
```

*If this fails, CMake can't find your tools. Check the Troubleshooting section.*

### Step 2: Build

**What it does:** Compiles the code. This takes time.
**Command:**

```bash
# Build in Debug mode (Best for coding, has extra checks)
cmake --build --preset windows_msvc --config Debug

# Build in Release mode (Best for playing, runs fast)
cmake --build --preset windows_msvc --config Release
```

### Step 3: Run

**What it does:** Starts the launcher!
**Command:**

```bash
# The executable will be in the build folder
./build/windows_msvc/Debug/ProjT-Launcher.exe
```

### Step 4: Test

**What it does:** Runs automated checks to make sure you didn't break anything.
**Command:**

```bash
ctest --preset windows_msvc --build-config Debug --output-on-failure
```

## Supported Presets

| Preset | Description | OS |
| ------ | ----------- | -- |
| `windows_msvc` | MSVC Compiler (Standard) | Windows |
| `windows_mingw` | MinGW GCC (Alternative) | Windows |
| `linux` | GCC/Clang | Linux |
| `macos` | Clang (Apple) | macOS |
| `ci-windows` | CI Configuration | Windows |

---

## ❓ Troubleshooting

### "Qt Config file not found"

- **Cause**: CMake cannot find your Qt installation.
- **Fix**: Set `CMAKE_PREFIX_PATH` to your Qt bin folder.

```bash
cmake --preset windows_msvc -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/msvc2019_64"
```

### "Ninja not found"

- **Cause**: Ninja is not in your PATH.
- **Fix**: Install Ninja and add it to PATH, or use `Visual Studio` generator (slower).

### "C++20 not supported"

- **Cause**: Your compiler is too old.
- **Fix**: Update VS 2022, GCC to 11+, or Clang to 14+.
