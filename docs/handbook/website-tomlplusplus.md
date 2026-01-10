## toml++ Documentation Setup

This directory contains the configuration and source files for building the toml++ API documentation using Poxy (Doxygen + m.css).

### Setup

First, install the required dependencies:

```bash
npm run setup:doxygen
```

This will install:
- **Doxygen** (via Homebrew on macOS)
- **Poxy** (via pipx)

#### Manual Setup

If automatic installation fails, you can install manually:

**macOS:**
```bash
brew install doxygen pipx
pipx install poxy
```

**Linux:**
```bash
sudo apt-get install doxygen python3-pip
python3 -m pip install --user pipx
pipx install poxy
```

### Building Documentation

To build the toml++ documentation:

```bash
npm run build:tomlplusplus
```

This will:
1. Run Poxy with the configuration in `poxy.toml`
2. Generate HTML documentation to `website/tomlplusplus/html/`
3. The HTML will be copied to `_site/tomlplusplus-docs/` during the site build

### Full Site Build

The toml++ documentation is automatically built when you run:

```bash
npm run build
```

### Configuration

The documentation is configured via `poxy.toml`. Key settings:

- **Source paths**: Points to `../../tomlplusplus/include` for the C++ headers
- **Pages**: Markdown files in the `pages/` directory
- **Images**: SVG badges and logos in the `images/` directory
- **Theme**: Dark theme by default
- **Output**: HTML files are generated to `html/` directory

### Troubleshooting

#### Poxy fails with "ModuleNotFoundError: No module named 'jinja2'"

This can happen if system Python is incompatible. Install jinja2 to Homebrew Python:

```bash
/opt/homebrew/bin/python3 -m pip install --break-system-packages jinja2 Pygments
```

#### Doxygen warnings

Minor Doxygen warnings about unresolved links are normal and don't prevent documentation generation.

### Directory Structure

```
website/tomlplusplus/
├── poxy.toml          # Poxy configuration
├── pages/             # Documentation pages (Markdown)
│   └── main_page.md
├── images/            # Badges, logos, and graphics
└── html/              # Generated HTML (after build)
```

### Links

- [Poxy Documentation](https://github.com/marzer/poxy)
- [Repository](https://github.com/Project-Tick/ProjT-Launcher)
- [Doxygen Manual](https://www.doxygen.nl/manual/)
