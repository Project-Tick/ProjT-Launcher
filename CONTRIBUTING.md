# Contributing to ProjT Launcher

```text
Upstream Maintainer: YongDo-Hyun <yongdohyun@projtlauncher.yongdohyun.org.tr>
License: GPL-3.0-only (Launcher), see COPYING.md
```

## Contributor License Agreement (CLA)

By submitting a contribution to this repository, you agree that your
contribution is made under the terms of the **Project Tick Contributor
License Agreement (CLA)**.

The CLA ensures that:

- you have the legal right to submit the contribution,
- the contribution does not knowingly infringe third-party rights,
- Project Tick may distribute the contribution under the applicable
  Project Tick license(s) governing the component,
- long-term governance and license consistency across the Project Tick
  ecosystem can be maintained.

The CLA applies to all intentional contributions, including but not
limited to source code, documentation, tests, data, media assets, and
configuration files.

The full text of the current CLA is available at:

- https://projecttick.org/licenses/PT-CLA-2.0.txt

If you do not agree to the CLA, please do not submit contributions.

---

## Quick Start

```sh
git clone https://github.com/Project-Tick/ProjT-Launcher.git
cd ProjT-Launcher
cmake --preset your_os
cmake --build --preset your_os
ctest --preset your_os
```

---

## Requirements

| Component | Requirement |
| --------- | ----------- |
| CMake | 3.22+ |
| Qt | 6.10.0 |
| Compiler | C++20 support |
| Python | 3.9+ (for metadata tools) |
| Node.js | 18+ (for website) |

**Why exact Qt version?** ABI stability and CI determinism require all builds to use the same Qt version. Mixing versions causes subtle runtime failures.

---

## Project Areas

```yaml
launcher/           Launcher application (C++/Qt)
website/            Website (Eleventy)
bot/                Automation bot (Cloudflare Workers)
meta/               Metadata generator (Python)
docs/               Documentation
ci/, .github/       CI/CD and automation
scripts/, tools/    Build and development tools
```

### Detached Fork Libraries

These are independently maintained forks, not synced with upstream:

```yaml
zlib/               Compression library
bzip2/              Compression library
quazip/             ZIP archive handling
cmark/              Markdown parsing
tomlplusplus/       TOML parsing
libqrencode/        QR code generation
libnbtplusplus/     NBT format support
extra-cmake-modules/    CMake utilities
```

### Vendored Libraries

```yaml
gamemode/           GameMode integration
LocalPeer/          Single instance support
murmur2/            Hash functions
qdcss/              Dark CSS support
rainbow/            Terminal colors
systeminfo/         System information
```

---

## Code Style

This is a summary. See [CODE_STYLE.md](docs/contributing/CODE_STYLE.md) for full rules.

### C++ (clang-format)

```sh
clang-format -i path/to/file.cpp
```

Key rules:

- Tabs for indentation (width: 4)
- Column limit: 120
- Allman brace style
- C++20 standard

### Commit Messages

```text
component: short description

Optional explanation of what and why.
```

Examples:

```text
launcher: fix crash on startup with invalid config
zlib: update to version 1.3.1
ci: add macOS arm64 build support
docs: update build instructions
```

---

## DCO Sign-off

Every commit must include a sign-off line:

```sh
git commit -s -m "component: description"
```

This adds:

```text
Signed-off-by: Your Name <your.email@example.com>
```

The bot enforces DCO compliance and labels PRs missing sign-off.

---

## Pull Request Process

### Before Submitting

- Run clang-format on changed files
- Ensure code compiles without warnings
- Add tests for new functionality
- Sign off all commits
- Update documentation if needed

### PR Requirements

- Clear description of what and why
- Reference related issues
- Pass all CI checks
- One logical change per PR
- **Do not mix**: refactors, features, and third-party updates must be in separate PRs
- Third-party library updates require standalone PRs with documented rationale

### Review Process

1. Automated CI runs tests and linting
2. Maintainer reviews code
3. Address feedback
4. Merge when approved

---

## Testing

### Running Tests

```sh
ctest --preset default
```

### Running Specific Tests

```sh
ctest -R test_name --preset default
```

### Writing Tests

- Use QtTest framework
- Test public interfaces
- Mock external dependencies
- Cover edge cases

---

## Architecture

The launcher follows MVVM (Model-View-ViewModel) pattern:

```yaml
Model       Data and business logic
ViewModel   Presentation logic, state management
View        Qt Widgets UI components
```

MVVM is enforced conceptually. UI classes (`launcher/ui/`) must not contain business logic—only presentation and user interaction. See [ARCHITECTURE.md](docs/contributing/ARCHITECTURE.md) for full design guidance.

---

## Documentation

See `docs/` for detailed documentation:

- [docs/contributing/](docs/contributing/) - Contribution guides
- [docs/handbook/](docs/handbook/) - Developer handbook
- [docs/](docs/) - General documentation

---

## Reporting Issues

Include in bug reports:

- Operating system and version
- ProjT Launcher version
- Steps to reproduce
- Expected vs actual behavior
- Logs from `~/.local/share/ProjT/logs/`

---

## FAQ

Q: Why is my PR failing CI?

- Run clang-format
- Sign off commits (`git commit -s`)
- Add tests for new code

Q: Can I use a different Qt version?

- No. Exact version matching is required.

Q: How do I update a fork library?

- Document changes thoroughly
- Test all dependent code
- Submit separate PR for library update

---

## Contact

- Issues: [GitHub Issues](https://github.com/Project-Tick/ProjT-Launcher/issues)
- Discussions: [GitHub Discussions](https://github.com/Project-Tick/ProjT-Launcher/discussions)
- Email: [yongdohyun@projtlauncher.yongdohyun.org.tr](mailto:yongdohyun@projtlauncher.yongdohyun.org.tr)

---

## License

By contributing, you agree to license your work under the project's licenses.
See [LICENSE](LICENSE) and [COPYING.md](COPYING.md).

## Code of Conduct

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
