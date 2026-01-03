# Project Tick - ProjT Launcher Pull Request

## Description
<!-- Summarize the changes and why they are needed. -->

## Scope
<!--
IMPORTANT:
- CI jobs ARE filtered based on this section.
- Selecting the wrong scope may skip required checks.
- If unsure, select more scopes, not fewer.
-->
<!--
Rules:
- Docs/Branding-only changes should not modify runtime code.
- CI-only changes may affect job logic and require full checks.
- If code is touched, related runtime scopes MUST be selected.
-->

### Core

- [ ] Launcher (C++/Qt)
- [ ] Launcher Java System

### Libraries

- [ ] Zlib
- [ ] bzip2
- [ ] Quazip
- [ ] libnbtplusplus
- [ ] JavaCheck

### Services

- [ ] Bot (Cloudflare Workers)
- [ ] Metadata Generator (Python)

### Distribution

- [ ] Packages

### Infrastructure

- [ ] CI
- [ ] Tools
- [ ] Branding
- [ ] Website (Eleventy)
- [ ] Docs

### Other
<!--
If "Other" is selected, describe clearly and expect full CI checks.
-->

- [ ] Other (describe):

## Type of Change
<!--
Type of Change is informational.
CI decisions are primarily based on Scope and file changes.
-->

- [ ] Bug fix
- [ ] Feature
- [ ] Documentation
- [ ] Refactor
- [ ] Test
- [ ] Build / CI
- [ ] Chore

## If you selected the "Packages" scope, please select Package Types
<!--
If "Packages" is selected above:
- At least one package type MUST be checked below.
- CI may fail otherwise.
-->

- [ ] NixOS
- [ ] Flatpak
- [ ] AUR
- [ ] Add a new package manifest

## Checklist

- [ ] I have read `CONTRIBUTING.md`.
- [ ] DCO: each commit includes `Signed-off-by:`
- [ ] My code follows the project's style guidelines.
- [ ] I have added tests that prove my fix is effective or explained why not.
- [ ] All new and existing tests pass or I explained why not.
- [ ] I have updated documentation accordingly or confirmed it is not needed.

## Related Issues
<!-- Link to any related issues, e.g., `Fixes #123` -->

## Additional Notes
<!-- Any additional information, screenshots, or context. -->

## Signed-off-by
<!--
NOTE: This section does NOT replace git commit Signed-off-by lines.
-->
<!-- Please use signed-off-by: name <email> -->
