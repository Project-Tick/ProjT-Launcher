## CI Support Files

This directory contains support files and scripts used by CI/CD for the ProjT Launcher repository.

### What uses this directory

- GitHub Actions workflows in `.github/workflows/` (CI automation, PR tooling)
- Nix helpers for local development and validation (`ci/default.nix`, `ci/eval/`, `ci/parse.nix`)
- PR automation scripts (`ci/github-script/`)

### Useful entry points

- `ci/code-quality.sh`: Local PR-style checks between HEAD and a base branch
- `ci/default.nix`: Nix dev environment with build dependencies
- `ci/pinned.json`: Reference versions for tooling/CI runners

### Local usage

```bash
# Compare your current branch against develop
./ci/code-quality.sh develop

# Enter Nix dev shell (if you use Nix)
nix develop -f ci/default.nix
```
