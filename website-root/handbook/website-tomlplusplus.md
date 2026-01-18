# toml++ Documentation Setup `website/tomlplusplus/`

> **Location**: `website/tomlplusplus/`  
> **Tools**: Poxy, Doxygen, m.css  
> **Output**: API documentation HTML
> **Latest Version**: 0.0.5-1

---

## Overview

This directory contains configuration for building the toml++ API documentation using Poxy (a Doxygen + m.css wrapper).

---

## Quick Start

### Install Dependencies

```bash
npm run setup:doxygen
```

This installs:
- **Doxygen** (via Homebrew on macOS, apt on Linux)
- **Poxy** (via pipx)

### Build Documentation

```bash
npm run build:tomlplusplus
```

### Full Site Build

```bash
npm run build  # Includes toml++ docs
```

---

## Manual Setup

### macOS

```bash
brew install doxygen pipx
pipx install poxy
```

### Linux

```bash
sudo apt-get install doxygen python3-pip
python3 -m pip install --user pipx
pipx install poxy
```

---

## Configuration

Documentation is configured via `poxy.toml`:

| Setting | Value |
|---------|-------|
| Source paths | `../../tomlplusplus/include` |
| Pages | `pages/` directory |
| Images | `images/` directory |
| Theme | Dark (default) |
| Output | `html/` directory |

---

## Directory Structure

```
website/tomlplusplus/
├── poxy.toml          # Poxy configuration
├── pages/             # Documentation pages (Markdown)
│   └── main_page.md
├── images/            # Badges, logos, graphics
└── html/              # Generated output (after build)
```

---

## Build Process

1. **Poxy** reads `poxy.toml`
2. **Doxygen** parses C++ headers
3. **m.css** generates styled HTML
4. Output copied to `_site/tomlplusplus/` during site build

---

## Troubleshooting

### ModuleNotFoundError: jinja2

Install to Homebrew Python:

```bash
/opt/homebrew/bin/python3 -m pip install --break-system-packages jinja2 Pygments
```

### Doxygen Warnings

Minor warnings about unresolved links are normal and don't prevent generation.

---

## Related Documentation

- [toml++](./tomlplusplus.md) — Library documentation
- [Workflows](./workflows.md) — CI builds documentation

---

## External Links

- [Poxy Documentation](https://github.com/marzer/poxy)
- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [m.css](https://mcss.mosra.cz/)
