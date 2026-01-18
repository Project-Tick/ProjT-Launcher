# JavaCheck `javacheck/`

> **Location**: `javacheck/`  
> **Language**: Java  
> **Purpose**: Java runtime property detection
> **Latest Version**: 0.0.5-1

---

## Overview

JavaCheck is a minimal Java program that prints system properties. It's used by ProjT Launcher to detect and validate Java installations.

---

## How It Works

```
┌─────────────────┐      ┌─────────────┐     ┌──────────────┐
│  ProjT Launcher │────▶│  JavaCheck  │────▶│   stdout     │
│     (C++)       │      │   (Java)    │     │  properties  │
└─────────────────┘      └─────────────┘     └──────────────┘
```

### Execution

```bash
java -jar javacheck.jar java.version java.home os.arch
```

### Output

```
java.version=17.0.9
java.home=/usr/lib/jvm/java-17-openjdk
os.arch=amd64
```

---

## Properties Checked

| Property | Description |
|----------|-------------|
| `java.version` | Java version string |
| `java.home` | JRE installation path |
| `os.arch` | System architecture |
| `java.vendor` | JVM vendor |
| `java.vm.name` | JVM implementation |

---

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | All properties found |
| 1 | One or more properties null |

---

## Usage in Launcher

ProjT Launcher uses JavaCheck to:

1. **Validate Java installations** — Ensure Java is working
2. **Detect version** — Choose correct Java for Minecraft version
3. **Get architecture** — Match 32/64-bit requirements
4. **Auto-detect Java** — Find installed JREs/JDKs

---

## Building

```bash
cd javacheck
javac JavaCheck.java
jar cfe javacheck.jar JavaCheck JavaCheck.class
```

---

## Related Documentation

- [LauncherJava](./launcherjava.md) — Launch component
- [Program Info](./program_info.md) — Application config
