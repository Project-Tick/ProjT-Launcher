# GitHub Actions Workflows

This directory contains workflow configurations for the ProjT Launcher CI/CD pipeline.

## Overview

The workflows are organized by functionality:

### Pull Request Automation

- **`pull-request-target.yml`**: Prepares PR metadata used by other workflows (safe for forks)
- **`check.yml`**: Reusable workflow for PR validation (commits/CODEOWNERS/basic quality checks)
- **`bot.yml`**: PR automation (labeling, assigning reviewers, and maintainer-only bot commands like `backport`)
- **`comment.yml`**: Handles bot commands in PR comments
- **`scope-guard.yml`**: Validates PR scope checkbox selection against changed files
- **`review.yml`** / **`reviewed.yml`**: Review event handling and status updates
- **`edited.yml`**: Re-runs labeler when PR base branch changes

### Builds & Releases

- **`build.yml`**: Main build workflow (Linux/macOS/Windows) including packaging steps (AppImage/Flatpak/Nix/Windows installers)
- **`release.yml`**: Creates GitHub releases from build artifacts
- **`publish.yml`**: Publishes installers/assets after release

### Branch / Merge Maintenance

- **`merge-group.yml`**, **`merge-blocking-pr.yml`**: Merge-group and merge safety helpers
- **`periodic-merge.yml`**, **`periodic-merge-24h.yml`**: Periodically merges branches and opens issues on failure

### Repo Maintenance

- **`teams.yml`**: Syncs `.github/teams.json`
- **`backport.yml`**: Creates backport PRs when a merged PR is labeled `backport/release-*`
- **`stale.yml`**: Marks inactive issues/PRs (never closes them)
- **`update-flake.yml`**: Weekly `flake.lock` updates
- **`scorecard.yml`**: OSSF Scorecard analysis and SARIF upload

## Bot Commands

Maintainers can trigger certain actions via PR comments:

- Backport to latest release branch: `@projt-launcher-bot backport latest`
- Backport to all release branches: `@projt-launcher-bot backport all`
- Backport to a specific release branch: `@projt-launcher-bot backport release-1.2.3`
- Re-run an existing backport branch: add `--force`
- Only push the branch (no PR): add `--no-pr`

## Key Design Principles

### Push Events

- Most workflows use standard `push` events to develop/release branches
- This allows external contributors to test workflows without prior approval

### Pull Request Events

- Workflows use `pull_request` event type (not `pull_request_target`)
- Code review and approval by maintainers protects the repository

### Path Filters

- Workflows use path filters to avoid unnecessary runs
- For example, `build.yml` only runs when C++, CMake, or workflow files change

## Workflow Files

## Workflow File Structure

Each workflow file contains:

- Trigger conditions (on: events)
- Permissions (least privilege principle)
- Jobs with specific steps
- Environment variables for consistency

## Common Patterns

### Setup & Checkout

```yaml
- uses: actions/checkout@v4.2.2
  with:
    fetch-depth: 0  # Full history for git operations
```

### Conditional Steps

### Conditional Steps Example

```yaml
- name: Step name
  if: github.ref_type == 'tag'  # Only run on tags
  run: echo "Release build"
```

### Artifacts & Uploads

### Artifacts & Uploads Example

```yaml
- uses: actions/upload-artifact@v5.0.0
  with:
    name: build-artifacts
    path: build/launcher
```

## Testing Workflows Locally

For most workflows, you can test locally using:

```bash
# Lint check
clang-format -i launcher/**/*.cpp launcher/**/*.h

# Build test
cmake --preset linux && cmake --build --preset linux

# Flake check (if using Nix)
nix flake check
```

## Adding New Workflows

When adding new workflows:

1. Follow the naming convention: lowercase, hyphens for spaces
2. Use path filters to avoid unnecessary runs
3. Apply least privilege permissions
4. Use github.token by default (avoid elevated privileges)
5. Add documentation in this README

## Troubleshooting

## Troubleshooting Guide

- **Workflow not triggering**: Check path filters and branch configuration
- **Permission denied errors**: Verify permissions: section
- **Tests failing locally but passing in CI**: Check environment differences (dependencies, paths)
- **Slow workflows**: Consider using caching or parallelization
