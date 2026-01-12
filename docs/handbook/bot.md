---
permalink: /handbook/bot-details/
---

# Bot `bot/`

> **Type**: Cloudflare Worker  
> **Platform**: Cloudflare Workers  
> **Purpose**: PR Automation & Labeling

---

## Overview

The ProjT Launcher bot is a Cloudflare Worker that automates pull request labeling based on changed files. It listens for GitHub webhook events and applies appropriate labels automatically.

---

## Features

| Feature | Description |
|---------|-------------|
| **Auto-labeling** | Labels PRs based on file changes |
| **Path mapping** | Configurable path-to-label rules |
| **Webhook integration** | GitHub webhook receiver |
| **Zero cold start** | Cloudflare Workers edge deployment |

---

## Architecture

```
┌─────────────┐     ┌──────────────────┐     ┌─────────────┐
│   GitHub    │────▶│  Cloudflare      │────▶│   GitHub    │
│  Webhook    │     │  Worker (bot/)   │     │   API       │
└─────────────┘     └──────────────────┘     └─────────────┘
                           │
                    ┌──────▼──────┐
                    │ wrangler.json│
                    │ (config)     │
                    └─────────────┘
```

---

## File Structure

```
bot/
├── index.js        # Main worker logic
├── server.js       # Local development server
├── package.json    # Dependencies
└── wrangler.json   # Cloudflare configuration
```

---

## Label Rules

The bot applies labels based on changed file paths:

| Path Pattern | Label |
|--------------|-------|
| `launcher/**` | `launcher` |
| `buildconfig/**` | `build` |
| `cmake/**` | `build` |
| `.github/workflows/**` | `ci` |
| `docs/**` | `documentation` |
| `zlib/**` | `library: zlib` |
| `bzip2/**` | `library: bzip2` |
| `quazip/**` | `library: quazip` |
| `cmark/**` | `library: cmark` |
| `tomlplusplus/**` | `library: toml++` |
| `libqrencode/**` | `library: qrencode` |
| `website/**` | `website` |

---

## Configuration

### wrangler.json

```json
{
  "name": "projtlauncher-bot",
  "main": "index.js",
  "compatibility_date": "2024-01-01",
  "vars": {
    "GITHUB_APP_ID": "your-app-id"
  }
}
```

### Environment Variables

| Variable | Description |
|----------|-------------|
| `GITHUB_APP_ID` | GitHub App ID |
| `GITHUB_APP_PRIVATE_KEY` | GitHub App private key |
| `GITHUB_WEBHOOK_SECRET` | Webhook signature secret |

---

## Development

### Prerequisites

- Node.js 18+
- Wrangler CLI (`npm install -g wrangler`)
- Cloudflare account

### Local Development

```bash
cd bot
npm install

# Start local dev server
npm run dev
# or
wrangler dev
```

### Testing Webhooks Locally

Use [smee.io](https://smee.io/) or ngrok to forward webhooks:

```bash
# Install smee client
npm install -g smee-client

# Forward webhooks
smee -u https://smee.io/your-channel -t http://localhost:8787
```

---

## Deployment

### Deploy to Cloudflare

```bash
cd bot
wrangler publish
```

### GitHub Webhook Setup

1. Go to repository Settings → Webhooks
2. Add webhook:
   - **Payload URL**: `https://your-worker.workers.dev/webhook`
   - **Content type**: `application/json`
   - **Secret**: Your `GITHUB_WEBHOOK_SECRET`
   - **Events**: Pull requests

---

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Health check |
| `/webhook` | POST | GitHub webhook receiver |

---

## Security

- ✅ **Signature verification** — HMAC-SHA256 webhook signatures
- ✅ **App authentication** — GitHub App JWT tokens
- ✅ **Secret management** — Cloudflare Workers secrets

---

## Troubleshooting

### Bot not labeling PRs

1. Check Cloudflare Workers logs
2. Verify webhook delivery in GitHub settings
3. Ensure secrets are configured correctly

### Labels not created

Labels must exist in the repository before the bot can apply them.

---

## Related Documentation

- [CI Workflows](./workflows.md) — GitHub Actions integration
- [GitHub Scripts](./ptcigh.md) — Additional automation

---

## External Links

- [Cloudflare Workers Docs](https://developers.cloudflare.com/workers/)
- [Wrangler CLI](https://developers.cloudflare.com/workers/wrangler/)
- [GitHub Apps](https://docs.github.com/en/apps)
- [GitHub Webhooks](https://docs.github.com/en/webhooks)
