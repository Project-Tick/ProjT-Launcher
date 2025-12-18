# Bot (Cloudflare Workers)

This directory contains a Cloudflare Worker that can auto-label GitHub pull requests (similar to the CI bot).

## Endpoints

- `GET /` or `GET /healthz`: health check
- `POST /github/webhook`: GitHub webhook receiver (requires signature)
- `POST /run` or `GET /run`: manual run (requires `Authorization: Bearer <ADMIN_TOKEN>`)
- `issue_comment` webhook: `bot rerun` / `bot labels` (or `/bot rerun`) triggers re-label for that PR (author association must be owner/member/collaborator by default). Set `BOT_COMMENT_ON_COMMAND=true` to let the bot reply with a short summary.

## Required secrets (Cloudflare)

Set these as Worker secrets:

- `GITHUB_TOKEN`: token with permission to add/create labels (repo access as needed)
- `GITHUB_WEBHOOK_SECRET`: the webhook secret configured in GitHub
- `ADMIN_TOKEN`: (optional but recommended) protects `/run`
- `BOT_COMMENT_ON_COMMAND`: (optional) `true` to comment after handling `issue_comment` commands
- `BOT_ALLOWED_ASSOCIATIONS`: (optional) comma-separated GitHub author_association values allowed to run commands (default: `OWNER,MEMBER,COLLABORATOR`)

Example (local):

```bash
cd bot
wrangler secret put GITHUB_TOKEN
wrangler secret put GITHUB_WEBHOOK_SECRET
wrangler secret put ADMIN_TOKEN
```

## Local smoke test

```bash
cd bot
wrangler dev
curl -sS http://localhost:8787/healthz
curl -sS -H "Authorization: Bearer $ADMIN_TOKEN" "http://localhost:8787/run?pr=123"
```

## Config vars

Defined in `bot/wrangler.json`:

- `GITHUB_OWNER` (default: `Project-Tick`)
- `GITHUB_REPO` (default: `ProjT-Launcher`)
- `BOT_DRY_RUN` (`true`/`false`)

## DCO check

The bot validates that each non-bot commit in a PR includes `Signed-off-by:`. If any are missing, it applies the `status:dco-missing` label (created automatically if needed). Bot commits (`[bot]`, `Project Tick Bot`, or `*@bot.*`) are exempt.

## CI summary comment

The bot posts or updates a `PR CI Summary` comment based on the latest `pull-request-target.yml` run for the PR. It requires `actions:read` in addition to issues/labels write access.

## GitHub webhook setup

Create a GitHub webhook pointing to:

- Payload URL: `https://<your-worker-domain>/github/webhook`
- Content type: `application/json`
- Secret: same as `GITHUB_WEBHOOK_SECRET`
- Events: `Pull requests` (at minimum)

## GitHub Actions deploy

Workflow: `.github/workflows/deploy-bot-worker.yml`

Repository secrets expected:

- `CLOUDFLARE_API_TOKEN`
- `CLOUDFLARE_ACCOUNT_ID`
- `PROJT_BOT_GITHUB_TOKEN` (mapped to Worker `GITHUB_TOKEN`)
- `PROJT_BOT_WEBHOOK_SECRET` (mapped to Worker `GITHUB_WEBHOOK_SECRET`)
- `PROJT_BOT_ADMIN_TOKEN` (mapped to Worker `ADMIN_TOKEN`)
