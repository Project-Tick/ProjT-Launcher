# Workflow

Git workflow and contribution process.

---

## Branches

| Branch | Purpose |
|--------|---------|
| `develop` | Integration branch (protected) |
| `release-X.Y.Z` | Release preparation |
| `hotfix/X.Y.Z` | Critical bug fixes |

Never push directly to `develop`.

---

## Commit Messages

We use Conventional Commits:

```
<type>(<scope>): <description>

Signed-off-by: Name <email>
```

### Types

| Type | Use |
|------|-----|
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation |
| `style` | Formatting |
| `refactor` | Code change (no behavior change) |
| `perf` | Performance |
| `test` | Tests |
| `chore` | Build/tooling |

### Example

```
feat(ui): add dark mode toggle

Signed-off-by: John Doe <john@example.com>
```

**Forbidden**: Do not mix scopes in a single commit. A commit touching both `ui/` and `minecraft/` must be split.

---

## DCO (Sign-off)

Every commit must be signed:

```bash
git commit -s -m "feat: my feature"
```

Forgot sign-off?

```bash
git commit --amend -s --no-edit
git push --force-with-lease
```

---

## Pull Requests

1. **One feature per PR**
2. **Merge commit only** (no squash, no rebase)
3. **CI must pass**
4. **One maintainer approval required**

**Why merge commits?** History preservation and review traceability. Each commit remains linked to its PR and review discussion.

---

## Merge Conflicts

```bash
git fetch origin
git rebase origin/develop
# Fix conflicts
git add .
git rebase --continue
git push --force-with-lease
```

**Rebase policy**: Rebase is allowed only on local branches before PR submission. Never rebase protected branches or branches with open PRs.

---

## Release Process

1. Freeze `develop`
2. Create `release-X.Y.Z`
3. QA testing
4. Bug fixes to release branch
5. Merge to `develop`
6. Tag release

---

## License Headers

### Project Tick–Owned Files

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */
```

### Upstream Forked Files

Do not modify existing license headers in forked code. When adding substantial changes, add a comment block below the original header:

```cpp
// Modifications by Project Tick, 2026
```

### Modified Files

Preserve original copyright notices and add Project Tick copyright.

---

## Related

- [Getting Started](./GETTING_STARTED.md)
- [Code Style](./CODE_STYLE.md)
