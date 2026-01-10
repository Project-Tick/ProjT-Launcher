## ProjT Launcher - GitHub CI Scripts

This folder contains [`actions/github-script`](https://github.com/actions/github-script)-based JavaScript code for CI automation.

### Overview

These scripts automate various GitHub workflow tasks:

- PR validation and labeling
- Commit message checking
- Automated reviews
- Branch management

### Local Development

#### Prerequisites

- Node.js 18+
- `gh` CLI authenticated

#### Setup

```bash
cd ci/github-script
npm install
```

#### Running Scripts

##### Check Commits

Validates commit messages in a PR:

```bash
./run commits YongDo-Hyun ProjT-Launcher 123
```

##### Check Labels

Validates PR labels:

```bash
./run labels YongDo-Hyun ProjT-Launcher
```

### Scripts

|Script|Description|
|`bot.js`|Main bot logic for PR automation|
|`commits.js`|Commit message validation|
|`merge.js`|Merge queue handling|
|`prepare.js`|PR preparation checks|
|`reviewers.js`|Automatic reviewer assignment|
|`reviews.js`|Review status checking|
|`withRateLimit.js`|GitHub API rate limiting|

### Configuration

Scripts use environment variables:

- `GITHUB_TOKEN`: GitHub API token
- `GITHUB_REPOSITORY`: Repository in `owner/repo` format

### Integration

These scripts are called from GitHub Actions workflows in `.github/workflows/`.
