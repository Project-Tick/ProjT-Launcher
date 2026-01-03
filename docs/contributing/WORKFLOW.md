# 🔄 Workflow and Git Standards

## ⚠️ Process Policy

We follow specific workflow standards to ensure traceability, reviewability,
and a clear project history.

- **Commits**: Must follow Conventional Commits.
- **Sign-off**: Mandatory (DCO). The bot labels `status:dco-missing` when any non-bot commit lacks `Signed-off-by:`.
- **History**: Rebase must never be used on branches that are already part of an open PR or have been merged into `develop`.
- **Merge strategy**: Merge commits only (no squash, no rebase merge).

## 1. Branching Strategy (Git Explained)

Think of branches like **Parallel Universes**:

- **develop**
  - Integration branch for upcoming releases.
  - Protected branch.
  - Never push directly.

- **release-X.Y.Z**
  - Created from `develop`.
  - May contain multiple commits.

## 2. Commit Messages (Strict)

**Why?** Good commit messages let us generate changelogs automatically.

We use **Conventional Commits**.

```text
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
Signed-off-by: Name <email>
```

### Allowed Types

- `feat`: New feature (User sees this)
- `fix`: Bug fix (User sees this)
- `docs`: Documentation only (User doesn't care)
- `style`: Formatting (white-space, formatting, missing semi-colons, etc)
- `refactor`: A code change that neither fixes a bug nor adds a feature
- `perf`: A code change that improves performance
- `test`: Adding missing tests or correcting existing tests
- `chore`: Changes to the build process or auxiliary tools

### Example (Good)

```text
feat(ui): add dark mode toggle

- Added toggle button to settings
- Connected toggle to settings handler

Signed-off-by: John Doe <john@example.com>
```

### Example (Bad - Will be Rejected)

- `update code` (What code?)
- `fix bug` (Which bug?)
- `wip` (Work in progress commits must be cleaned up before opening a PR)

## 3. Pull Request Rules

**What is a PR?** It's a request to merge your "Personal Universe" into the "Construction Site".

1. **One feature per PR**
   - No mixing unrelated changes.

2. **Merge Commit Required**
   - Pull requests must be merged using **Create a merge commit**.
   - **Squash merge is forbidden.**
   - **Rebase merge is forbidden.**
   - Individual commits must remain visible in history.

3. **CI Green**
   - All checks must pass before merging.

4. **Review**
   - At least one maintainer approval is required.

### Pull Request Template

GitHub will prefill `.github/pull_request_template.md`. Fill out the scope, testing notes, and the DCO checklist.

### Issue Template

Use the issue template in `.github/ISSUE_TEMPLATE.md` when reporting bugs.

## 4. DCO (Developer Certificate of Origin)

You **must** sign off every commit.

```bash
git commit -s -m "feat: my feature"
```

If the bot applies `status:dco-missing`, at least one non-bot commit is missing a sign-off.

If you forget:

```bash
git commit --amend -s --no-edit
git push --force-with-lease
```

## 5. Release Process

1. **Freeze**: No new features merged to `develop`.
2. **Branch**: Create `release-X.Y.Z` from `develop`.
3. **Test**: QA performs regression testing.
4. **Fix**: Bug fixes are committed to `release-X.Y.Z`.
5. **Merge**: Merge `release-X.Y.Z` into  `develop`.
6. **Tag**: Tag `develop` with `X.Y.Z`.

Rebase is allowed **only** for the following purposes:

- Resolving merge conflicts
- Cleaning up a local feature branch before opening a PR

Rewriting commit history after a PR is opened is discouraged.

## 6. Handling Merge Conflicts

```bash
git fetch origin
git rebase origin/develop
# Fix conflicts
git add .
git rebase --continue
git push --force-with-lease
```

## 7. Hotfix Process

For critical bugs in production:

1. Create `hotfix/X.Y.Z+1` from `develop`.
2. Fix the bug.
3. Merge into `develop`.
4. Tag `develop`.

## Code Review Process

1. CI/CD pipeline runs
2. DCO label must be clear
3. At least one maintainer approval
4. Merge using **Create a merge commit**

---

## ⚖️ License and Copyrights

### License

ProjT Launcher is licensed under **GPL-3.0-only**.

### Adding New Files

Every new file must start with this header:

**C++ files:**

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
```

**Qt Designer (`.ui`) files:**

Keep the license header in the paired C++ class; `.ui` XML files should remain generated by Qt Designer.

### Modifying Existing Code

Preserve original copyright notices when modifying code from other projects:

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
/*
 *  Original work:
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022-2024 Prism Launcher Contributors
 *
 *  Licensed under GPL-3.0-only
 */
```
