# GitHub CI Scripts `ci/github-script/`

> **Location**: `ci/github-script/`  
> **Platform**: GitHub Actions (`actions/github-script`)  
> **Purpose**: PR automation and validation
> **Latest Version**: 0.0.5-1

---

## Overview

This directory contains JavaScript code for GitHub Actions automation using [`actions/github-script`](https://github.com/actions/github-script). These scripts automate various workflow tasks.

---

## Features

| Feature | Description |
|---------|-------------|
| PR validation | Check PR requirements |
| Auto-labeling | Apply labels based on changes |
| Commit checking | Validate commit messages |
| Reviewer assignment | Auto-assign reviewers |
| Branch management | Merge queue handling |

---

## Scripts

| Script | Purpose |
|--------|---------|
| `bot.js` | Main bot logic for PR automation |
| `commits.js` | Commit message validation |
| `merge.js` | Merge queue handling |
| `prepare.js` | PR preparation checks |
| `reviewers.js` | Automatic reviewer assignment |
| `reviews.js` | Review status checking |
| `withRateLimit.js` | GitHub API rate limiting |

---

## Local Development

### Prerequisites

- Node.js 18+
- `gh` CLI (authenticated)

### Setup

```bash
cd ci/github-script
npm install
```

### Running Scripts

```bash
# Check commits in a PR
./run commits Project-Tick ProjT-Launcher 123

# Check labels
./run labels Project-Tick ProjT-Launcher
```

---

## Environment Variables

| Variable | Description |
|----------|-------------|
| `GITHUB_TOKEN` | GitHub API token |
| `GITHUB_REPOSITORY` | Repository (`owner/repo`) |

---

## Integration

These scripts are called from workflows in `.github/workflows/`:

```yaml
- uses: actions/github-script@v7
  with:
    script: |
      const script = require('./ci/github-script/commits.js');
      await script({ github, context, core });
```

---

## Related Documentation

- [Workflows](./workflows.md) — CI overview
- [Bot](./bot.md) — Cloudflare Worker bot
- [CI Support](./ci_support.md) — Support files
