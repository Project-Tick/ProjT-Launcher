# ProjT-Launcher Bot (Server Version)

This bot was originally designed for Cloudflare Workers but has been adapted to run on a standard Node.js server.

## Setup

1.  **Install Dependencies**:
    ```bash
    pnpm install
    ```
    (or `npm install` if you prefer)

2.  **Configure Environment**:
    Copy `.env.example` to `.env` and fill in the required values.
    ```bash
    cp .env.example .env
    ```

3.  **Run the Server**:
    ```bash
    npm start
    ```
    The server will start on the port specified in `.env` (default: 3000).

## Deployment

You can deploy this to any server that supports Node.js (e.g., VPS, Heroku, Railway, etc.).

### Using PM2 (Recommended)

To keep the bot running in the background:
```bash
npm install -g pm2
pm2 start server.js --name projtlauncher-bot
```

### GitHub Webhook

Point your GitHub repository webhook to `http://your-server-ip:3000/github/webhook`.
Make sure the `Content type` is set to `application/json` (though the server handles any type as text and parses it).
Ensure the `Secret` matches `GITHUB_WEBHOOK_SECRET` in your `.env`.
