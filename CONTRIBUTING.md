# Contributing to ProjT Launcher

```text
Upstream Maintainer: YongDo-Hyun <yongdohyun@projtlauncher.yongdohyun.org.tr>
License: GPL-3.0-only (Launcher), see COPYING.md
```

## Restrictions on Generative AI Usage (AI Policy)
> [!NOTE]
> The following is adapted from [matplotlib's contributing guide](https://matplotlib.org/devdocs/devel/contribute.html#generative-ai) and the [Linux Kernel policy guide](https://www.kernel.org/doc./html/next/process/coding-assistants.html)

We expect authentic engagement in our community.

- Do not post output from Large Language Models or similar generative AI as comments on GitHub or our discord server, as such comments tend to be formulaic and low-quality content.
- If you use generative AI tools as an aid in developing code or documentation changes, ensure that you fully understand the proposed changes and can explain why they are the correct approach.

Make sure you have added value based on your personal competency to your contributions. 
Just taking some input, feeding it to an AI and posting the result is not of value to the project. 
To preserve precious core developer capacity, we reserve the right to rigorously reject seemingly AI generated low-value contributions.

### Signed-off-by and Developer Certificate of Origin 

AI agents MUST NOT add Signed-off-by tags. Only humans can legally certify the Developer Certificate of Origin (DCO). The human submitter is responsible for:

  - Reviewing all AI-generated code
  - Ensuring compliance with licensing requirements
  - Adding their own Signed-off-by tag to certify the DCO
  - Taking full responsibility for the contribution
  
See [Signing your work](#signing-your-work) for more information.


### Attribution

When AI tools contribute to development, proper attribution helps track the evolving role of AI in the development process. Contributions should include an Assisted-by tag in the commit message with the following format:

```
Assisted-by: AGENT_NAME:MODEL_VERSION [TOOL1] [TOOL2]
```

Where:

  - `AGENT_NAME` is the name of the AI tool or framework
  - `MODEL_VERSION` is the specific model version used
  - `[TOOL1] [TOOL2]` are optional specialized analysis tools used (e.g., coccinelle, sparse, smatch, clang-tidy)

Basic development tools (git, gcc, make, editors) should not be listed.

Example:

```
Assisted-by: Claude:claude-3-opus coccinelle sparse
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

- <https://projecttick.org/licenses/PT-CLA-2.0.txt>

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
gamemode/           GameMode
```

### Vendored Libraries

```yaml
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
clang-format -i path/to/file.cpp # Please use LLVM 19
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

Every commit must include a sign-off line and all files to commit:

```sh
git commit -s -a
```

This adds:

```text
Signed-off-by: Your Name <your.email@example.com>
```

The bot enforces DCO compliance and labels MRs missing sign-off.

---

## Merge Request Process

### Before Submitting

- Run clang-format on changed files
- Ensure code compiles without warnings
- Add tests for new functionality
- Sign off all commits
- Update documentation if needed

### MR Requirements

- Clear description of what and why
- Reference related issues
- Pass all CI checks
- One logical change per MR
- **Do not mix**: refactors, features, and third-party updates must be in separate MRs
- Third-party library updates require standalone MRs with documented rationale

### Review Process

1. Automated CI runs tests and linting
2. Maintainer reviews code
3. Address feedback
4. Merge when approved

---

## Testing

### Running Tests

```sh
ctest --preset [ linux or macos or windows_msvc or windows_mingw ]
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

Q: Why is my MR failing CI?

- Run clang-format
- Sign off commits (`git commit -s`)
- Add tests for new code

Q: Can I use a different Qt version?

- No. Exact version matching is required.

Q: How do I update a fork library?

- Document changes thoroughly
- Test all dependent code
- Submit separate MR for library update

---

## Contact

- Issues: [GitLab Issues](https://github.com/Project-Tick/core/ProjT-Launcher/-/issues)
- Email: [yongdohyun@projtlauncher.yongdohyun.org.tr](mailto:yongdohyun@projtlauncher.yongdohyun.org.tr)

---

## License

By contributing, you agree to license your work under the project's licenses.
See [COPYING](COPYING) and LICENSES/ folder.

## Code of Conduct

See [CODE_OF_CONDUCT](CODE_OF_CONDUCT).
