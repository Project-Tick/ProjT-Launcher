# GitHub Actions Workflows

> **Location**: `.github/workflows/`  
> **Platform**: GitHub Actions  
> **Type**: CI/CD Pipeline
> **Latest Version**: 0.0.5-1

---

## Overview

ProjT Launcher uses a modular GitHub Actions workflow architecture. The CI system is split into specialized sub-workflows that are orchestrated by a main coordinator workflow.

---

## Workflow Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      ci-new.yml                             │
│                  (Main Orchestrator)                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ ci-prepare  │  │  ci-lint    │  │     ci-release      │  │
│  │   .yml      │  │    .yml     │  │        .yml         │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              Library Workflows                      │    │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌───────────┐  │    │
│  │  │ci-zlib  │ │ci-bzip2 │ │ci-quazip│ │ ci-cmark  │  │    │
│  │  │  .yml   │ │  .yml   │ │  .yml   │ │   .yml    │  │    │
│  │  └─────────┘ └─────────┘ └─────────┘ └───────────┘  │    │
│  │  ┌──────────────┐ ┌────────────────┐                │    │
│  │  │ci-tomlplusplus│ │ ci-libqrencode │               │    │
│  │  │     .yml      │ │      .yml      │               │    │
│  │  └──────────────┘ └────────────────┘                │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ ci-website  │  │ci-scheduled │  │      (unlock)       │  │
│  │   .yml      │  │    .yml     │  │      Merge Gate     │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Workflow Files

### Main Orchestrator

| File | Description |
|------|-------------|
| `ci-new.yml` | Main coordinator that calls all sub-workflows with conditional logic |

### Core Workflows

| File | Purpose | Triggers |
|------|---------|----------|
| `ci-prepare.yml` | Initial setup, dependency caching | Called by main |
| `ci-lint.yml` | Code quality checks (clang-format, linters) | Push, PR, Manual |
| `ci-release.yml` | Build releases, create artifacts | Release, Manual |
| `ci-website.yml` | Build and deploy website | Push, PR |
| `ci-scheduled.yml` | Scheduled maintenance tasks | Cron |

### Library Workflows

Each bundled library has its own independent CI:

| File | Library | Tests |
|------|---------|-------|
| `ci-zlib.yml` | zlib | Compression tests |
| `ci-bzip2.yml` | bzip2 | Compression + valgrind |
| `ci-quazip.yml` | QuaZip | Qt ZIP operations |
| `ci-cmark.yml` | cmark | Markdown parsing |
| `ci-tomlplusplus.yml` | toml++ | TOML parsing |
| `ci-libqrencode.yml` | libqrencode | QR generation |

---

## Workflow Design Principles

### 1. Modular Architecture

Each workflow is self-contained and can be run independently:

```yaml
# Sub-workflow pattern
on:
  workflow_call:
    inputs:
      ref:
        required: false
        type: string
  push:
    paths:
      - 'library-name/**'
  pull_request:
    paths:
      - 'library-name/**'
```

### 2. Centralized Control

The main orchestrator (`ci-new.yml`) controls when each sub-workflow runs:

```yaml
jobs:
  call-zlib:
    if: needs.prepare.outputs.run-zlib == 'true'
    uses: ./.github/workflows/ci-zlib.yml
```

### 3. Path Filtering

Workflows only run when relevant files change:

```yaml
paths:
  - 'zlib/**'
  - '.github/workflows/ci-zlib.yml'
```

### 4. Merge Gate

The `unlock` job aggregates all results for merge protection:

{% raw %}
```yaml
unlock:
  needs: [lint, build, test-zlib, test-bzip2, ...]
  if: always()
  runs-on: ubuntu-latest
  steps:
    - name: Check all jobs
      run: |
        if [[ "${{ contains(needs.*.result, 'failure') }}" == "true" ]]; then
          exit 1
        fi
```
{% endraw %}

---

## Triggers

| Trigger | Workflows | Description |
|---------|-----------|-------------|
| `push` | All | Commits to main branches |
| `pull_request` | All | Pull request changes |
| `release` | Release | GitHub releases |
| `schedule` | Scheduled | Cron jobs |
| `workflow_dispatch` | All | Manual triggers |
| `workflow_call` | Sub-workflows | Called by orchestrator |

---

## Environment Variables

| Variable | Description |
|----------|-------------|
| `GITHUB_TOKEN` | Automatic GitHub token |
| `MSA_CLIENT_ID` | Microsoft Auth client ID |
| `CACHIX_AUTH_TOKEN` | Nix cache auth |

---

## Adding New Workflows

1. **Create the workflow file** in `.github/workflows/`
2. **Add workflow_call trigger** for orchestrator integration
3. **Add push/PR triggers** for independent execution
4. **Update ci-new.yml** to include the new workflow
5. **Document** in this handbook

### Template

{% raw %}
```yaml
name: CI Library Name

on:
  workflow_call:
    inputs:
      ref:
        required: false
        type: string
  push:
    paths:
      - 'library-name/**'
      - '.github/workflows/ci-libraryname.yml'
  pull_request:
    paths:
      - 'library-name/**'
      - '.github/workflows/ci-libraryname.yml'

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ inputs.ref || github.ref }}
      # Build steps...
```
{% endraw %}

---

## Related Documentation

- [CI Support Files](./ci_support.md) — Configuration files
- [CI Evaluation](./ptcieval.md) — Nix-based validation
- [GitHub Scripts](./ptcigh.md) — Automation helpers
- [Bot](./bot.md) — PR automation

---

## External Links

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions)
- [Reusable Workflows](https://docs.github.com/en/actions/using-workflows/reusing-workflows)
