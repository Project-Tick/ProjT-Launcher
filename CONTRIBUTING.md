# 🤝 Contribution Guidelines

> **ProjT Launcher - Minecraft Launcher**  
> Copyright (C) 2025 Project Tick  
> Licensed under GPL-3.0-only

## 🛠 Engineering Standards

This project follows specific engineering standards to ensure maintainability and quality. Please review them before contributing.

**Key Principles:**

- ✅ **Formatted Code**: Use `clang-format` to keep code consistent.
- ✅ **Tested Features**: Write tests for new functionality.
- ✅ **MVVM Architecture**: Keep business logic out of the UI.
- ✅ **Signed Commits**: Sign-off your commits (DCO).

---

## 📚 Documentation Index

Please read the specific sections below before writing a single line of code:

### 1. [🚀 Getting Started](docs/contributing/GETTING_STARTED.md)

- **Setup**: Required tools (Qt 6.x, CMake 3.22+).
- **Environment**: How to set up VS Code or Visual Studio.

### 2. [📝 Code Style & Standards](docs/contributing/CODE_STYLE.md)

- **Formatting**: `clang-format` usage.
- **Modern C++**: C++20 features and best practices.
- **QML**: Component structure and formatting.

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

---

## ❓ Quick FAQ

### Why is my PR failing CI?

- Did you run `clang-format`?
- Did you sign off your commits (`-s`)?
- Did you write tests?

### Can I use a different Qt version?

**No.** We require exact version matching to prevent "works on my machine" issues.

---

## 📞 Contact

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Questions and discussions
- **Email**: `yongdohyun@projtlauncher.yongdohyun.org.tr`

---

### Last updated: December 2025
