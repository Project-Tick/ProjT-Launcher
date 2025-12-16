export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);

    if (request.method === "GET" && (url.pathname === "/" || url.pathname === "/healthz")) {
      return json({ ok: true, service: "projtlauncher-bot", ts: new Date().toISOString() });
    }

    if (url.pathname === "/github/webhook" && request.method === "POST") {
      const rawBody = await request.text();
      const signature = request.headers.get("x-hub-signature-256") ?? "";

      if (!env.GITHUB_OWNER || !env.GITHUB_REPO) {
        return json({ ok: false, error: "Missing GITHUB_OWNER/GITHUB_REPO" }, 500);
      }
      if (!env.GITHUB_TOKEN) {
        return json({ ok: false, error: "Missing GITHUB_TOKEN" }, 500);
      }
      if (!env.GITHUB_WEBHOOK_SECRET) {
        return json({ ok: false, error: "Missing GITHUB_WEBHOOK_SECRET" }, 500);
      }

      const verified = await verifyGitHubSignature({
        secret: env.GITHUB_WEBHOOK_SECRET,
        signatureHeader: signature,
        rawBody,
      });

      if (!verified) {
        return json({ ok: false, error: "Invalid signature" }, 401);
      }

      const eventName = request.headers.get("x-github-event") ?? "unknown";
      const payload = safeJsonParse(rawBody);
      if (!payload) {
        return json({ ok: false, error: "Invalid JSON payload" }, 400);
      }

      const expectedRepo = `${env.GITHUB_OWNER}/${env.GITHUB_REPO}`.toLowerCase();
      const gotRepo = String(payload?.repository?.full_name ?? "").toLowerCase();
      if (gotRepo && gotRepo !== expectedRepo) {
        return json({ ok: true, ignored: true, reason: "repo-mismatch", gotRepo }, 200);
      }

      if (eventName === "pull_request") {
        const prNumber = payload?.pull_request?.number;
        if (typeof prNumber !== "number") {
          return json({ ok: false, error: "Missing pull_request.number" }, 400);
        }

        ctx.waitUntil(
          handlePullRequest({
            owner: env.GITHUB_OWNER,
            repo: env.GITHUB_REPO,
            pullNumber: prNumber,
            env,
          })
        );
        return json({ ok: true, accepted: true, event: eventName, pr: prNumber }, 202);
      }

      return json({ ok: true, ignored: true, event: eventName }, 200);
    }

    if (url.pathname === "/run") {
      const auth = request.headers.get("authorization") ?? "";
      if (!env.ADMIN_TOKEN || auth !== `Bearer ${env.ADMIN_TOKEN}`) {
        return json({ ok: false, error: "Unauthorized" }, 401);
      }

      if (request.method === "POST") {
        const body = safeJsonParse(await request.text()) ?? {};
        const prNumber = typeof body.pr === "number" ? body.pr : null;
        if (!prNumber) {
          return json({ ok: false, error: "Body must include { pr: number }" }, 400);
        }
        const result = await handlePullRequest({
          owner: env.GITHUB_OWNER,
          repo: env.GITHUB_REPO,
          pullNumber: prNumber,
          env,
        });
        return json({ ok: true, result }, 200);
      }

      if (request.method === "POST" || request.method === "GET") {
        const pr = url.searchParams.get("pr");
        if (pr) {
          const prNumber = Number(pr);
          if (!Number.isFinite(prNumber)) {
            return json({ ok: false, error: "Invalid pr query param" }, 400);
          }
          const result = await handlePullRequest({
            owner: env.GITHUB_OWNER,
            repo: env.GITHUB_REPO,
            pullNumber: prNumber,
            env,
          });
          return json({ ok: true, result }, 200);
        }

        const result = await processOpenPullRequests(env);
        return json({ ok: true, result }, 200);
      }

      return json({ ok: false, error: "Unsupported method" }, 405);
    }

    return json({ ok: false, error: "Not found" }, 404);
  },

  async scheduled(_event, env, ctx) {
    ctx.waitUntil(processOpenPullRequests(env));
  },
};

const CONFIG = {
  labels: {
    core: "11.area:core",
    ui: "11.area:ui",
    minecraft: "11.area:minecraft",
    modplatform: "11.area:modplatform",
    build: "11.area:build",
    ci: "11.area:ci",
    documentation: "11.area:docs",
    translations: "11.area:translations",
  },
  sizeLabels: {
    xs: { max: 10, label: "12.size:xs" },
    s: { max: 50, label: "12.size:s" },
    m: { max: 200, label: "12.size:m" },
    l: { max: 500, label: "12.size:l" },
    xl: { max: Infinity, label: "12.size:xl" },
  },
  platformLabels: {
    linux: "13.platform:linux",
    macos: "13.platform:macos",
    windows: "13.platform:windows",
  },
  templateTypeLabels: [
    { option: "Bug fix", candidates: ["21.type:bugfix", "bug", "Bug"] },
    { option: "Feature", candidates: ["21.type:feature", "enhancement", "Feature"] },
    { option: "Documentation", candidates: ["21.type:docs", "documentation", "docs"] },
    { option: "Refactor", candidates: ["21.type:refactor", "refactor"] },
    { option: "Test", candidates: ["21.type:test", "tests", "test"] },
    { option: "Build / CI", candidates: ["21.type:build", "ci", "build"] },
    { option: "Other", candidates: ["21.type:other", "other"] },
  ],
};

function json(data, status = 200) {
  return new Response(JSON.stringify(data), {
    status,
    headers: {
      "content-type": "application/json; charset=utf-8",
      "cache-control": "no-store",
    },
  });
}

function safeJsonParse(text) {
  try {
    return JSON.parse(text);
  } catch {
    return null;
  }
}

function escapeRegExp(value) {
  return String(value).replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

function getTemplateSelections(body = "") {
  const normalized = body.replace(/\r/g, "");
  const selections = [];
  for (const entry of CONFIG.templateTypeLabels) {
    const pattern = new RegExp(`- \\[[xX]\\]\\s+${escapeRegExp(entry.option)}(?:\\s|$)`);
    if (pattern.test(normalized)) {
      selections.push(entry);
    }
  }
  return selections;
}

function resolveTemplateLabel(selection, repoLabels) {
  for (const candidate of selection.candidates) {
    if (repoLabels.has(candidate)) return candidate;
  }
  return null;
}

function getAreaFromPath(filePath) {
  if (filePath.startsWith("launcher/ui/") || filePath.startsWith("launcher/qtquick/")) return "ui";
  if (filePath.startsWith("launcher/minecraft/")) return "minecraft";
  if (filePath.startsWith("launcher/modplatform/")) return "modplatform";
  if (filePath.startsWith("launcher/")) return "core";
  if (filePath.startsWith("libraries/")) return "core";
  if (filePath.startsWith("cmake/") || filePath.endsWith("CMakeLists.txt")) return "build";
  if (filePath.startsWith(".github/") || filePath.startsWith("ci/")) return "ci";
  if (filePath.startsWith("docs/") || filePath.endsWith(".md")) return "documentation";
  if (filePath.startsWith("translations/")) return "translations";
  return null;
}

function getPlatformFromPath(filePath) {
  const lowerPath = filePath.toLowerCase();
  if (lowerPath.includes("linux") || lowerPath.includes("unix")) return "linux";
  if (lowerPath.includes("darwin") || lowerPath.includes("macos") || lowerPath.includes("apple")) return "macos";
  if (lowerPath.includes("windows") || lowerPath.includes("win32") || lowerPath.includes("msvc")) return "windows";
  return null;
}

function getSizeLabel(additions, deletions) {
  const total = (Number(additions) || 0) + (Number(deletions) || 0);
  for (const { max, label } of Object.values(CONFIG.sizeLabels)) {
    if (total <= max) return label;
  }
  return CONFIG.sizeLabels.xl.label;
}

function splitBranch(branch) {
  const match = String(branch).match(
    /(?<prefix>[a-z_-]+?)(-(?<version>\d+\.\d+\.\d+|v?\d+\.\d+))?(-(?<suffix>.*))?$/i
  );
  return match?.groups ?? { prefix: String(branch) };
}

function classifyBranch(branch) {
  const typeConfig = {
    develop: ["development", "primary"],
    master: ["development", "primary"],
    main: ["development", "primary"],
    release: ["release", "primary"],
    qml_migration: ["development", "feature"],
    feature: ["development", "feature"],
    bugfix: ["development", "bugfix"],
    hotfix: ["release", "hotfix"],
  };

  const { prefix, version, suffix } = splitBranch(branch);
  const normalizedPrefix = prefix.toLowerCase().replace(/-/g, "_");
  const type = typeConfig[normalizedPrefix] ?? typeConfig[prefix.split("/")[0]] ?? ["wip"];

  return {
    branch,
    type,
    version: version ?? null,
    suffix: suffix ?? null,
  };
}

let repoLabelsCache = null;

async function getRepositoryLabels({ owner, repo, env }) {
  const now = Date.now();
  if (repoLabelsCache && now - repoLabelsCache.ts < 10 * 60 * 1000) return repoLabelsCache.labels;

  const labels = new Set();
  const perPage = 100;
  for (let page = 1; page <= 20; page++) {
    const { data, res } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/labels?per_page=${perPage}&page=${page}`,
    });
    for (const label of data) labels.add(label.name);
    if (!Array.isArray(data) || data.length < perPage) break;
    if (!res.headers.get("link")) continue;
  }

  repoLabelsCache = { ts: now, labels };
  return labels;
}

async function processOpenPullRequests(env) {
  const owner = env.GITHUB_OWNER;
  const repo = env.GITHUB_REPO;
  if (!owner || !repo) throw new Error("Missing GITHUB_OWNER/GITHUB_REPO");

  const pullRequests = await listOpenPullRequests({ owner, repo, env });
  const stats = { total: pullRequests.length, processed: 0, errors: 0 };
  const results = [];

  for (const pr of pullRequests) {
    const pullNumber = pr.number;
    try {
      const result = await handlePullRequest({ owner, repo, pullNumber, env });
      results.push({ number: pullNumber, ...result });
      stats.processed++;
    } catch (error) {
      console.error(`PR #${pullNumber}:`, error);
      results.push({ number: pullNumber, ok: false, error: String(error?.message ?? error) });
      stats.errors++;
    }
  }

  return { stats, results };
}

async function handlePullRequest({ owner, repo, pullNumber, env }) {
  if (!owner || !repo) throw new Error("Missing owner/repo");
  if (!env.GITHUB_TOKEN) throw new Error("Missing GITHUB_TOKEN");

  const dryRun = String(env.BOT_DRY_RUN ?? "false").toLowerCase() === "true";

  const { data: pullRequest } = await githubApi({
    env,
    method: "GET",
    path: `/repos/${owner}/${repo}/pulls/${pullNumber}`,
  });

  const files = await listPullRequestFiles({ owner, repo, pullNumber, env });
  const repoLabels = await getRepositoryLabels({ owner, repo, env });

  const labelsToAdd = new Set();

  for (const file of files) {
    const filename = file.filename;

    const area = getAreaFromPath(filename);
    if (area && CONFIG.labels[area]) labelsToAdd.add(CONFIG.labels[area]);

    const platform = getPlatformFromPath(filename);
    if (platform && CONFIG.platformLabels[platform]) labelsToAdd.add(CONFIG.platformLabels[platform]);
  }

  labelsToAdd.add(getSizeLabel(pullRequest.additions, pullRequest.deletions));

  const branchType = classifyBranch(pullRequest?.head?.ref ?? "");
  const types = Array.isArray(branchType.type) ? branchType.type : branchType.type ? [branchType.type] : [];
  for (const t of types) labelsToAdd.add(`branch:${t}`);

  const templateSelections = getTemplateSelections(pullRequest.body ?? "");
  for (const selection of templateSelections) {
    const resolved = resolveTemplateLabel(selection, repoLabels);
    if (resolved) labelsToAdd.add(resolved);
  }

  if (pullRequest.mergeable === false) labelsToAdd.add("status:merge-conflict");

  const currentLabels = new Set((pullRequest.labels ?? []).map((l) => l.name));
  const newLabels = [...labelsToAdd].filter((l) => !currentLabels.has(l));

  if (newLabels.length === 0) {
    return { ok: true, changed: false, added: [], dryRun };
  }

  if (!dryRun) {
    await githubApi({
      env,
      method: "POST",
      path: `/repos/${owner}/${repo}/issues/${pullNumber}/labels`,
      body: { labels: newLabels },
    });
  } else {
    console.log(`DRY_RUN: would add labels to PR #${pullNumber}:`, newLabels);
  }

  return { ok: true, changed: true, added: newLabels, dryRun };
}

async function listOpenPullRequests({ owner, repo, env }) {
  const perPage = 100;
  const pulls = [];
  for (let page = 1; page <= 20; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/pulls?state=open&per_page=${perPage}&page=${page}`,
    });
    pulls.push(...data);
    if (!Array.isArray(data) || data.length < perPage) break;
  }
  return pulls;
}

async function listPullRequestFiles({ owner, repo, pullNumber, env }) {
  const perPage = 100;
  const files = [];
  for (let page = 1; page <= 50; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/pulls/${pullNumber}/files?per_page=${perPage}&page=${page}`,
    });
    files.push(...data);
    if (!Array.isArray(data) || data.length < perPage) break;
  }
  return files;
}

async function githubApi({ env, method, path, body }) {
  const token = env.GITHUB_TOKEN;
  const url = `https://api.github.com${path}`;
  const init = {
    method,
    headers: {
      accept: "application/vnd.github+json",
      "user-agent": "projtlauncher-bot-worker",
      "x-github-api-version": "2022-11-28",
      authorization: `Bearer ${token}`,
    },
  };

  if (body !== undefined) {
    init.headers["content-type"] = "application/json; charset=utf-8";
    init.body = JSON.stringify(body);
  }

  const res = await fetch(url, init);
  const contentType = res.headers.get("content-type") ?? "";
  const data = contentType.includes("application/json") ? await res.json() : await res.text();
  if (!res.ok) {
    const message = typeof data === "string" ? data : JSON.stringify(data);
    throw new Error(`GitHub API ${method} ${path} failed: ${res.status} ${message}`);
  }

  return { res, data };
}

async function verifyGitHubSignature({ secret, signatureHeader, rawBody }) {
  const [algo, signatureHex] = String(signatureHeader).split("=", 2);
  if (algo !== "sha256" || !signatureHex) return false;

  const computed = await hmacSha256Hex(secret, rawBody);
  return timingSafeEqualHex(signatureHex.toLowerCase(), computed);
}

function timingSafeEqualHex(a, b) {
  if (a.length !== b.length) return false;
  let result = 0;
  for (let i = 0; i < a.length; i++) result |= a.charCodeAt(i) ^ b.charCodeAt(i);
  return result === 0;
}

async function hmacSha256Hex(secret, message) {
  const encoder = new TextEncoder();
  const key = await crypto.subtle.importKey(
    "raw",
    encoder.encode(secret),
    { name: "HMAC", hash: "SHA-256" },
    false,
    ["sign"]
  );
  const sig = await crypto.subtle.sign({ name: "HMAC" }, key, encoder.encode(message));
  return [...new Uint8Array(sig)].map((b) => b.toString(16).padStart(2, "0")).join("");
}
