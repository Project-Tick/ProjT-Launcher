<!--
Hey there! Thanks for your contribution.

Please make sure that your commits are signed off first.
If you don't know how that works, check out our contribution guidelines: https://github.com/Project-Tick/ProjT-Launcher/blob/develop/CONTRIBUTING.md#signing-your-work
If you already created your commits, you can run `git rebase --signoff develop` to retroactively sign-off all your commits and `git push --force` to override what you have pushed already.

Note that signing and signing-off are two different things!
-->

## Contribution Checklist

### 1. Before you start

* [ ] Read the repository README and relevant developer notes for architecture and conventions.
* [ ] Search the repo for existing code related to your change to avoid duplication.
* [ ] Create a focused branch with a clear name (e.g. `feature/auth-refresh`, `fix/ui-button-hover`).

### 2. Commits & history

* [ ] Keep each commit atomic and focused on a single logical change.
* [ ] Use imperative, descriptive commit messages (title + optional body).
* [ ] Squash or rebase local fixup commits before merging to keep history clean.
* [ ] Avoid large monolithic commits that mix formatting, logic, and refactors.

### 3. Code quality & style

* [ ] Run the project's linters and fix all reported issues.
* [ ] Run the formatter (prettier/black/clang-format/etc.) and ensure no unrelated formatting diffs.
* [ ] Adhere to established naming conventions, file structure, and code patterns used in the repo.
* [ ] Add inline comments where complex logic is unavoidable; keep them concise and technical.

### 4. Tests

* [ ] Add unit tests for new logic and edge cases.
* [ ] Update or add integration tests if behavior across components changed.
* [ ] Ensure tests run locally and pass (`npm test`, `pytest`, etc.).
* [ ] Keep tests deterministic — avoid network/time-dependent flakiness.

### 5. CI / Build

* [ ] Verify that the change builds successfully in the project's standard build environment.
* [ ] Ensure CI pipelines pass (lint → build → test → other checks).
* [ ] Add or update CI config only when necessary and document why.

### 6. Dependency changes

* [ ] Minimize dependency upgrades; prefer patch/minor updates unless required.
* [ ] When bumping versions, check changelogs for breaking changes and security fixes.
* [ ] Update lockfiles and run a clean install to validate reproducible builds.

### 7. Security & secrets

* [ ] Do not commit secrets, API keys, or credentials. Use environment/config management.
* [ ] Validate inputs and sanitize outputs when relevant to your change.
* [ ] Consider threat model impacts for code that touches auth, crypto, or external data.

### 8. Performance & resource usage

* [ ] Check for obvious performance regressions (hot loops, unnecessary allocations).
* [ ] Add benchmarking or profiling notes when making performance-sensitive changes.

### 9. Accessibility & internationalization

* [ ] Ensure UI changes meet basic accessibility (semantic HTML, ARIA where needed).
* [ ] Avoid hard-coded strings when the project supports localization; use existing i18n patterns.

### 10. Backwards compatibility & migrations

* [ ] Consider API/ABI compatibility; provide migration paths if behavior changes.
* [ ] Add migration scripts or notes for database/schema changes when required.

### 11. Tests of the change after merge

* [ ] Confirm staging deployment (if applicable) and smoke-test critical flows.
* [ ] Monitor CI and error tracking briefly after merge for regressions.

### 12. Housekeeping

* [ ] Remove debug/logging statements added during development.
* [ ] Keep TODOs actionable; don’t leave temporary hacks without tracking.
* [ ] Update relevant configuration files only when necessary and explain why in the commit.