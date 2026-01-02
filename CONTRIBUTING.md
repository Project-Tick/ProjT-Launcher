# 🤝 Contribution Guidelines

> **ProjT Launcher**  
> Copyright (C) 2026 Project Tick  
> Licensed under multiple licenses (Launcher: GPL-3.0-only, Website: AGPL-3.0-only, Metadata generator: MS-PL). See `LICENSE` and `COPYING`.

## 🧭 Project Areas

- **Launcher app (C++/Qt)**: `launcher/`, `CMakeLists.txt`
- **Website (Eleventy)**: `website/`, `public/`, `package.json`
- **Automation bot (Cloudflare Workers)**: `bot/`
- **Metadata generator (Python)**: `meta/`, `pyproject.toml`
- **Documentation**: `docs/`
- **CI/Tools**: `ci/`, `.github/`, `scripts/`, `tools/`

## 🧩 Vendored Projects

- **bzip2**: See `bzip2/README.md`, `bzip2/COMPILING.md`, `bzip2/NEWS.md`, `bzip2/code-of-conduct.md`.
- **quazip**: See `quazip/README.md`, `quazip/CONTRIBUTING.md`, `quazip/SECURITY.md`, `quazip/QuaZip-1.x-migration.md`.

## 🛠 Engineering Standards

This project follows specific engineering standards to ensure maintainability and quality. Please review them before contributing.

**Key Principles:**

- ✅ **Formatted Code**: Use the formatter for your area (`clang-format` for C++/Qt).
- ✅ **Tested Features**: Write tests for new functionality.
- ✅ **MVVM Architecture**: Keep business logic out of the UI.
- ✅ **Signed Commits**: Sign-off your commits (DCO). The bot enforces this on PRs and will label `status:dco-missing` if any non-bot commit lacks `Signed-off-by:`.

## ✅ DCO Sign-off

Every commit must include a `Signed-off-by:` line. Example:

```bash
git commit -s -m "Fix: explain what changed"
```

---

## 📚 Documentation Index

Please read the specific sections below before writing a single line of code (start with `docs/contributing/`):

### 1. [🚀 Getting Started](docs/contributing/GETTING_STARTED.md)

- **Setup**: Required tools (Qt 6.x, CMake 3.22+).
- **Environment**: How to set up VS Code or Visual Studio.

### 2. [📝 Code Style & Standards](docs/contributing/CODE_STYLE.md)

- **Formatting**: `clang-format` usage.
- **Modern C++**: C++20 features and best practices.
- **Qt Widgets**: Component structure and formatting.

### 3. [📁 Project Structure](docs/contributing/PROJECT_STRUCTURE.md)

- **Organization**: Where to put your files.
- **Naming**: File and class naming conventions.

### 4. [🏗 Architecture (MVVM)](docs/contributing/ARCHITECTURE.md)

- **MVVM**: Model-View-ViewModel pattern explained.
- **Threading**: Keeping the UI responsive.

### 5. [🧪 Testing Standards](docs/contributing/TESTING.md)

- **QtTest**: How to write and run unit tests.
- **Best Practices**: Mocking and async testing.

### 6. [🔄 Workflow & Git](docs/contributing/WORKFLOW.md)

- **Process**: Pull Request lifecycle.
- **Commits**: Conventional Commits and DCO.

### Other Areas

- **Bot**: `bot/README.md`
- **Website**: `.eleventy.js`, `website/`, `package.json`
- **Metadata generator**: `meta/`, `pyproject.toml`

---

## ❓ Quick FAQ

### Why is my PR failing CI?

- Did you run `clang-format`?
- Did you sign off your commits (`-s`)? The bot adds `status:dco-missing` when DCO is missing.
- Did you write tests?

### Can I use a different Qt version?

**No (launcher).** We require exact version matching to prevent "works on my machine" issues.

---

## 📞 Contact

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Questions and discussions
- **Email**: `yongdohyun@projtlauncher.yongdohyun.org.tr`

---

### Last updated: January 2026
