## GitHub Actions Workflows

This directory contains workflow configurations for the ProjT Launcher CI/CD pipeline.

### Overview

The workflows are organized by functionality:

#### Repo Maintenance

- **`ci.yml`**: Very Ci needs is here
- **`scorecard.yml`**: OSSF Scorecard analysis and SARIF upload
- **`clusterfuzzlite.yml`**: ClusterFuzzLite fuzzing runs (libFuzzer targets)
- **`python-fuzz.yml`**: Python Atheris fuzzing runs (meta/)
- **`js-fuzz.yml`**: JavaScript property fuzzing runs (bot/)

### Key Design Principles

#### Push Events

- Most workflows use standard `push` events to develop/release branches
- This allows external contributors to test workflows without prior approval

#### Pull Request Events

- Workflows use `pull_request` event type (not `pull_request_target`)
- Code review and approval by maintainers protects the repository

#### Path Filters

- Workflows use path filters to avoid unnecessary runs
- For example, `build.yml` only runs when C++, CMake, or workflow files change

### Workflow Files

#### Workflow File Structure

Each workflow file contains:

- Trigger conditions (on: events)
- Permissions (least privilege principle)
- Jobs with specific steps
- Environment variables for consistency

### Common Patterns

#### Setup & Checkout

```yaml
- uses: actions/checkout@8e8c483db84b4bee98b60c0593521ed34d9990e8
  with:
    fetch-depth: 0  # Full history for git operations
```

### Conditional Steps

#### Conditional Steps Example

```yaml
- name: Step name
  if: github.ref_type == 'tag'  # Only run on tags
  run: echo "Release build"
```

### Artifacts & Uploads

#### Artifacts & Uploads Example

```yaml
- uses: actions/upload-artifact@b7c566a772e6b6bfb58ed0dc250532a479d7789f
  with:
    name: build-artifacts
    path: build/launcher
```

### Testing Workflows Locally

For most workflows, you can test locally using:

```bash
# Lint check
clang-format -i launcher/**/*.cpp launcher/**/*.h

# Build test
cmake --preset linux && cmake --build --preset linux

# Flake check (if using Nix)
nix flake check
```

### Adding New Workflows

When adding new workflows:

1. Follow the naming convention: lowercase, hyphens for spaces
2. Use path filters to avoid unnecessary runs
3. Apply least privilege permissions
4. Use github.token by default (avoid elevated privileges)
5. Add documentation in this README

### Troubleshooting

#### Troubleshooting Guide

- **Workflow not triggering**: Check path filters and branch configuration
- **Permission denied errors**: Verify permissions: section
- **Tests failing locally but passing in CI**: Check environment differences (dependencies, paths)
- **Slow workflows**: Consider using caching or parallelization
