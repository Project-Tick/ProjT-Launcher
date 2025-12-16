# 🔄 Workflow and Git Standards

## ⚠️ Process Policy

We follow specific workflow standards to ensure a clean history and high code quality.

- **Commits**: Should follow Conventional Commits.
- **Sign-off**: Mandatory (DCO).
- **History**: Linear history preferred (Rebase).

## 1. Branching Strategy (Git Explained)

Think of branches like **Parallel Universes**:

- **`master` (The Public Universe)**:
  - This is what users download. It is always stable.
  - **Never** push directly to master.

- **`develop` (The Construction Site)**:
  - This is where we merge new features. It might have bugs.
  - We build the next release from here.

- **`feat/my-feature` (Your Personal Universe)**:
  - You create this from `develop`.
  - You can break things here. It's safe.
  - When you are done, you ask to merge it back to `develop` (Pull Request).

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
- Connected to ThemeViewModel

Signed-off-by: John Doe <john@example.com>
```

### Example (Bad - Will be Rejected)

- `update code` (What code?)
- `fix bug` (Which bug?)
- `wip` (Work in progress - Squash this before merging!)

## 3. Pull Request Rules

**What is a PR?** It's a request to merge your "Personal Universe" into the "Construction Site".

1. **One Feature per PR**: Do not fix a bug AND add a feature in the same PR.
2. **Clean History (Squash)**:
   - *What is it?* Combining 10 small "wip" commits into 1 clean commit.
   - *Why?* We don't want to see "fix typo", "fix typo again" in the history.
3. **CI Green**: All automated checks must pass.
4. **Review**: At least 1 maintainer must approve your code.

### Pull Request Template (Copy & Paste)

When you open a PR, copy this into the description:

```markdown
## Description
<!-- Briefly describe what you changed and why. -->

## Related Issue
<!-- Fixes #123 or Closes #456 -->

## Type of Change
- [ ] 🐛 Bug fix (non-breaking)
- [ ] ✨ New feature (non-breaking)
- [ ] 💥 Breaking change (affects existing functionality)
- [ ] 📚 Documentation update

## Testing
<!-- How did you test this? Be specific. -->
- [ ] Unit Tests passed
- [ ] Manual testing (Describe steps)

## Screenshots
<!-- If you changed the UI, put a screenshot here. -->

## Checklist
- [ ] I have signed off my commits (`git commit -s`)
- [ ] I have run `clang-format`
- [ ] I have added tests
```

### Issue Template (Copy & Paste)

When you report a bug, use this format:

```markdown
## Bug Description
<!-- What happened? -->

## Steps to Reproduce
1. Go to '...'
2. Click on '...'
3. Scroll down to '...'
4. See error

## Expected Behavior
<!-- What did you expect to happen? -->

## Screenshots
<!-- If applicable, add screenshots to help explain your problem. -->

## Environment
- **OS**: [e.g. Windows 11]
- **Launcher Version**: [e.g. 1.0.0]
- **Java Version**: [e.g. Java 17]
```

## 4. DCO (Developer Certificate of Origin)

You **must** sign off every commit.

```bash
git commit -s -m "feat: my feature"
```

If you forget:

```bash
git commit --amend -s --no-edit
git push --force-with-lease
```

## 5. Release Process

1. **Freeze**: No new features merged to `develop`.
2. **Branch**: Create `release/vX.Y.Z` from `develop`.
3. **Test**: QA performs regression testing.
4. **Fix**: Bug fixes are committed to `release/vX.Y.Z`.
5. **Merge**: Merge `release/vX.Y.Z` into `master` and `develop`.
6. **Tag**: Tag `master` with `vX.Y.Z`.

## 6. Handling Merge Conflicts

**Do not use merge commits.** Use rebase.

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

1. Create `hotfix/vX.Y.Z+1` from `master`.
2. Fix the bug.
3. Merge into `master` and `develop`.
4. Tag `master`.

## 8. PR Description Template

```markdown
## Description
<!-- What does this PR do? -->

## Related Issue
<!-- Fixes #123 or Closes #456 -->

## Type of Change
- [ ] Bug fix (non-breaking)
- [ ] New feature (non-breaking)
- [ ] Breaking change (affects existing functionality)
- [ ] Documentation update

## Testing
<!-- How was it tested? -->

## Screenshots (for UI changes)
<!-- Add if available -->

## Checklist
- [ ] I have reviewed my code
- [ ] I have commented my code (especially hard-to-understand parts)
- [ ] I have updated the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing tests pass
```

## Code Review Process

1. **Automated Checks**: CI/CD pipeline runs.
2. **Bot Checks**: DCO sign-off check.
3. **Maintainer Review**: At least 1 approval required.
4. **Merge**: Squash and merge preferred.

## Backporting

To backport to release branches, add the following labels to the PR:

- `backport release-1.x`
- `backport release-0.x`

---

## ⚖️ License and Copyrights

### License

ProjT Launcher is licensed under **GNU General Public License v3.0 or later**.

### Adding New Files

Every new file must start with this header:

**C++ files:**

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */
```

**QML files:**

```qml
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */
```

### Modifying Existing Code

Preserve original copyright notices when modifying code from other projects:

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
/*
 *  Original work:
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022-2024 Prism Launcher Contributors
 *
 *  Licensed under GPL-3.0-only
 */
```
