# LauncherJava `launcherjava/`

> **Location**: `launcherjava/`  
> **Language**: Java  
> **Purpose**: Minecraft launching component
> **Latest Version**: 0.0.5-1

---

## Overview

LauncherJava is the Java-based component responsible for actually launching Minecraft. It acts as an intermediary between the C++ launcher and the Minecraft JVM process.

---

## How It Works

```
┌─────────────┐      ┌──────────────┐     ┌─────────────┐
│  ProjT      │────▶│ LauncherJava │────▶│  Minecraft  │
│  Launcher   │      │   (Java)     │     │    JVM      │
│  (C++)      │      │              │     │             │
└─────────────┘      └──────────────┘     └─────────────┘
      │                    │
      │   stdin            │   exec
      │   (script)         │   (launch)
      ▼                    ▼
```

### Launch Sequence

1. C++ launcher sends launch script via stdin
2. LauncherJava parses the script
3. Waits for `launcher` command
4. Executes Minecraft with configured parameters

---

## Launch Script Format

The launcher communicates via a text-based protocol:

```text
mod legacyjavafixer-1.0
mainClass net.minecraft.launchwrapper.Launch
param --username
param PlayerName
param --version
param ProjT Launcher
param --gameDir
param /path/to/minecraft
param --assetsDir
param /path/to/assets
param --assetIndex
param 1.20.4
param --uuid
param xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
param --accessToken
param [TOKEN]
param --userType
param msa
windowTitle ProjT Launcher: Instance Name
windowParams 854x480
userName PlayerName
sessionId token:[TOKEN]:[UUID]
launcher standard
```

### Commands

| Command | Description |
|---------|-------------|
| `mod <name>` | Load a mod (tweaker) |
| `mainClass <class>` | Set main class |
| `param <value>` | Add launch parameter |
| `windowTitle <title>` | Set window title |
| `windowParams <WxH>` | Set window size |
| `userName <name>` | Player username |
| `sessionId <id>` | Session identifier |
| `launcher <type>` | Execute launch |
| `abort` | Cancel launch |

---

## Launcher Types

| Type | Use Case |
|------|----------|
| `standard` | All Minecraft versions (recommended) |
| `legacy` | Minecraft < 1.6 (deprecated) |

### Differences

- **Standard**: Universal launcher, works with all versions
- **Legacy**: Supports custom window icon/title but only for old versions

---

## Building

LauncherJava is built as part of the main CMake build:

```bash
# Built automatically with main project
cmake --build build

# Or build separately
cd launcherjava
./gradlew build
```

---

## Debugging

### Enable Debug Output

```text
debug on
mainClass ...
```

### Attach Profiler

Since LauncherJava waits for the `launcher` command, you can:

1. Start LauncherJava
2. Attach your profiler (VisualVM, JProfiler, etc.)
3. Send the `launcher` command

---

## License

Available under **GPL-3.0** (with classpath exception), sublicensed from its original **Apache-2.0** codebase.

---

## Related Documentation

- [Program Info](./program_info.md) — Application identity
- [JavaCheck](./javacheck.md) — Java detection
