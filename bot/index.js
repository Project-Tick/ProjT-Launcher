export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);

    if (request.method === "GET" && url.pathname === CONFIG.statusPage.path) {
      const prParam = url.searchParams.get("pr");
      const prNumber = prParam ? Number(prParam) : undefined;
      if (prParam && !Number.isFinite(prNumber)) {
        return json({ ok: false, error: "Invalid pr query param" }, 400);
      }
      try {
        const data = await buildStatusPage({ env, prNumber });
        return new Response(renderStatusPage({ data, prNumber }), {
          status: 200,
          headers: { "content-type": "text/html; charset=utf-8", "cache-control": "no-store" },
        });
      } catch (error) {
        console.error("Status page failed:", error?.message ?? error);
        return json({ ok: false, error: "Status page failed", detail: String(error?.message ?? error) }, 500);
      }
    }

    if (request.method === "GET" && (url.pathname === "/" || url.pathname === "/healthz")) {
      return json({ ok: true, service: "projtlauncher-bot", ts: new Date().toISOString() });
    }

    if (url.pathname === "/github/webhook") {
      if (request.method === "GET") {
        return json({ ok: true, message: "GitHub webhook endpoint. Use POST with signature." });
      }
      if (request.method !== "POST") {
        return json({ ok: true, message: "This method is allowed" });
      }
      const rawBody = await request.text();
      const signature = request.headers.get("x-hub-signature-256") ?? "";

      if (!env.GITHUB_OWNER || !env.GITHUB_REPO) {
        return json({ ok: false, error: "Missing GITHUB_OWNER/GITHUB_REPO" }, 500);
      }
      if (!hasGitHubAuth(env)) {
        return json({ ok: false, error: "Missing GitHub auth (token or app credentials)" }, 500);
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

      if (eventName === "workflow_run") {
        const runId = payload?.workflow_run?.id;
        if (typeof runId !== "number") {
          return json({ ok: false, error: "Missing workflow_run.id" }, 400);
        }

        ctx.waitUntil(
          handleWorkflowRun({
            owner: env.GITHUB_OWNER,
            repo: env.GITHUB_REPO,
            payload,
            env,
          })
        );
        return json({ ok: true, accepted: true, event: eventName, runId }, 202);
      }

      if (eventName === "issue_comment") {
        const result = await handleIssueComment({
          payload,
          env,
        });
        return json({ ok: true, event: eventName, result }, 200);
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

  async scheduled(event, env, ctx) {
    const cron = event?.cron ?? "unknown";
    const limit = parsePositiveInt(env.BOT_CRON_MAX_PRS);
    const light = String(env.BOT_CRON_LIGHT ?? "false").toLowerCase() === "true";
    const run = processOpenPullRequests(env, { source: "cron", cron, limit, light });
    ctx.waitUntil(
      run
        .then((result) => console.log(`[cron:${cron}] processed`, result?.stats))
        .catch((error) => console.error(`[cron:${cron}] failed`, error?.message ?? error))
    );
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
    { option: "Bug fix", candidates: ["21.type:bugfix"] },
    { option: "Feature", candidates: ["21.type:feature"] },
    { option: "Documentation", candidates: ["21.type:docs"] },
    { option: "Refactor", candidates: ["21.type:refactor"] },
    { option: "Test", candidates: ["21.type:test"] },
    { option: "Build / CI", candidates: ["21.type:build"] },
    { option: "Chore", candidates: ["21.type:chore", "21.type:other"] },
    { option: "Other", candidates: ["21.type:other"] },
  ],
  templateScopeLabels: [
    {
      option: "Launcher (C++/Qt)",
      label: { name: "31.scope:launcher", color: "0E8A16", description: "Launcher app changes" },
    },
    {
      option: "Website (Eleventy)",
      label: { name: "31.scope:website", color: "0E8A16", description: "Website changes" },
    },
    {
      option: "Bot (Cloudflare Workers)",
      label: { name: "31.scope:bot", color: "0E8A16", description: "Automation bot changes" },
    },
    {
      option: "Metadata Generator (Python)",
      label: { name: "31.scope:metadata", color: "0E8A16", description: "Metadata generator changes" },
    },
    {
      option: "Quazip",
      label: { name: "31.scope:quazip", color: "0E8A16", description: "QuaZip changes" },
    },
    {
      option: "bzip2",
      label: { name: "31.scope:bzip2", color: "0E8A16", description: "bzip2 changes" },
    },
    {
      option: "Docs",
      label: { name: "31.scope:docs", color: "0E8A16", description: "Docs changes" },
    },
    {
      option: "CI",
      label: { name: "31.scope:ci", color: "0E8A16", description: "CI changes" },
    },
    {
      option: "Tools",
      label: { name: "31.scope:tools", color: "0E8A16", description: "Tools changes" },
    },
    {
      option: "Branding",
      label: { name: "31.scope:branding", color: "0E8A16", description: "Branding changes" },
    },
    {
      option: "Launcher Java System",
      label: { name: "31.scope:launcher-java-system", color: "0E8A16", description: "Launcher Java System changes" },
    },
    {
      option: "JavaCheck",
      label: { name: "31.scope:javacheck", color: "0E8A16", description: "JavaCheck changes" },
    },
    {
      option: "libnbtplusplus",
      label: { name: "31.scope:libnbtplusplus", color: "0E8A16", description: "libnbtplusplus changes" },
    },
    {
      option: "Zlib",
      label: { name: "31.scope:zlib", color: "0E8A16", description: "Zlib changes" },
    },
    {
      option: "Packages",
      label: { name: "31.scope:packages", color: "0E8A16", description: "Packages changes" },
    },
    {
      option: "Other (describe):",
      label: { name: "31.scope:other", color: "6A737D", description: "Other changes" },
    },
  ],
  templatePackageLabels: [
    {
      option: "NixOS",
      label: { name: "33.packages:nixos", color: "0E8A16", description: "NixOS packaging changes" },
    },
    {
      option: "Flatpak",
      label: { name: "33.packages:flatpak", color: "0E8A16", description: "Flatpak packaging changes" },
    },
    {
      option: "AUR",
      label: { name: "33.packages:aur", color: "0E8A16", description: "AUR packaging changes" },
    },
    {
      option: "Add a new package manifest",
      label: { name: "33.packages:new-manifest", color: "0E8A16", description: "New package manifest" },
    },
  ],
  maintainersFile: "ci/eval/compare/maintainers.nix",
  maintainerLabel: "32.maintainer:PR",
  alwaysRequestReviewers: [],
  branchLabelPrefix: "14.branch",
  statusLabels: {
    mergeConflict: {
      name: "41.status:merge-conflict",
      color: "D93F0B",
      description: "PR has merge conflicts",
    },
    dcoMissing: {
      name: "41.status:dco-missing",
      color: "B60205",
      description: "Missing DCO Signed-off-by in one or more commits",
    },
  },
  autoMerge: {
    // Controlled via env BOT_AUTO_MERGE_ENABLED (default true)
    enabledEnv: "BOT_AUTO_MERGE_ENABLED",
    mergeMethodEnv: "BOT_AUTO_MERGE_METHOD",
    mergeMethod: "merge",
  },
  autoApprove: {
    enabledEnv: "BOT_AUTO_APPROVE_RUNS",
    workflowRules: [
      {
        scopes: ["Zlib"],
        workflows: ["ci.yml"],
      },
      {
        scopes: ["bzip2", "Quazip"],
        workflows: ["ci.yml"],
      },
      {
        scopes: ["Launcher (C++/Qt)"],
        workflows: ["ci.yml"],
      },
    ],
  },
  ciSummary: {
    marker: "<!-- projt-bot:pr-summary -->",
    workflowFile: "ci.yml",
    jobs: [
      { label: "Prepare", match: ["prepare"] },
      { label: "Check", match: ["check"] },
      { label: "Lint", match: ["lint"] },
      // Aggregate all build-* jobs into a single Build row
      { label: "Build", match: ["build", "cmake-"] },
    ],
  },
  commentCommands: [
    "bot rerun",
    "bot labels",
    "bot merge",
    "/bot rerun",
    "/bot labels",
    "/bot merge",
  ],
  statusPage: {
    path: "/status",
    workflows: [
      { file: "ci.yml", label: "CI" },
    ],
  },
  labeler: {
    enabled: true,
    path: ".github/labeler.yml",
    ttlMs: 5 * 60 * 1000, // cache parsed labeler for 5 minutes
  },
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

function log(level, message, fields = {}) {
  const id = fields.correlationId || fields.reqId || fields.id;
  const payload = {
    level,
    msg: message,
    ts: new Date().toISOString(),
    ...(id ? { correlationId: id } : {}),
    ...fields,
  };
  // Prefer single-line JSON for easier ingestion
  console.log(JSON.stringify(payload));
}

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
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

function badgeForJob({ status, conclusion }) {
  if (status && status !== "completed") return "⏳ in_progress";
  switch (conclusion) {
    case "success":
      return "✅ success";
    case "failure":
      return "❌ failure";
    case "cancelled":
      return "⚠️ cancelled";
    case "skipped":
      return "⏭️ skipped";
    case "timed_out":
      return "⏱️ timed_out";
    case "neutral":
      return "➖ neutral";
    case "action_required":
      return "🛑 action_required";
    default:
      return "❓ unknown";
  }
}

function hasSignedOffBy(message) {
  return /(^|\n)\s*signed-off-by:\s+.+/i.test(String(message));
}

function extractCommands(body, commandList = []) {
  const normalized = String(body || "").toLowerCase();
  const found = new Set();
  for (const cmd of commandList) {
    if (!cmd) continue;
    const needle = String(cmd).toLowerCase();
    if (normalized.includes(needle)) {
      found.add(cmd);
    }
  }
  return found;
}

async function postComment({ env, owner, repo, issueNumber, body }) {
  await githubApi({
    env,
    method: "POST",
    path: `/repos/${owner}/${repo}/issues/${issueNumber}/comments`,
    body: { body },
  });
}

function isBotIdentity({ name, email, login }) {
  const lowerName = String(name ?? "").toLowerCase();
  const lowerEmail = String(email ?? "").toLowerCase();
  const lowerLogin = String(login ?? "").toLowerCase();
  const combined = `${lowerName} ${lowerEmail} ${lowerLogin}`;

  if (!combined.trim()) return false;
  if (combined.includes("[bot]")) return true;
  if (combined.includes("project tick bot")) return true;
  if (combined.includes("projt-launcher-bot")) return true;
  if (lowerEmail.includes("@bot.")) return true;
  if (lowerEmail.includes("bot.yongdohyun.org.tr")) return true;

  return false;
}

function isBotCommit(commit) {
  const author = commit?.commit?.author ?? {};
  const committer = commit?.commit?.committer ?? {};
  const authorUser = commit?.author ?? {};
  const committerUser = commit?.committer ?? {};

  return (
    isBotIdentity({ name: author.name, email: author.email, login: authorUser.login }) ||
    isBotIdentity({ name: committer.name, email: committer.email, login: committerUser.login })
  );
}

function getTemplateSelections(body = "", entries = []) {
  const normalized = body.replace(/\r/g, "");
  const selections = [];
  for (const entry of entries) {
    const pattern = new RegExp(`- \\[[xX]\\]\\s+${escapeRegExp(entry.option)}(?:\\s|$)`);
    if (pattern.test(normalized)) {
      selections.push(entry);
    }
  }
  return selections;
}

function getSelectedScopeOptions(body = "") {
  return new Set(getTemplateSelections(body, CONFIG.templateScopeLabels).map((entry) => entry.option));
}

function getScopeLabelName(scopeOption) {
  const entry = CONFIG.templateScopeLabels.find((item) => item.option === scopeOption);
  return entry?.label?.name ?? null;
}

function isScopeSelected(scopeOption, { selectedOptions, selectedLabels }) {
  if (selectedOptions.has(scopeOption)) return true;
  const labelName = getScopeLabelName(scopeOption);
  return labelName ? selectedLabels.has(labelName) : false;
}

function getWorkflowKey(run) {
  const path = String(run?.path ?? "");
  if (path) return path.split("/").pop();
  return String(run?.name ?? "");
}

function findAutoApproveRule(run) {
  const key = getWorkflowKey(run);
  if (!key) return null;
  return CONFIG.autoApprove?.workflowRules?.find((rule) => rule.workflows.includes(key)) ?? null;
}

function isAutoApproveEnabled(env) {
  const flag = CONFIG.autoApprove?.enabledEnv;
  return String(env?.[flag] ?? "false").toLowerCase() === "true";
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
let botLoginCache = null;
let appTokenCache = null;
const circuitBreaker = {
  failures: 0,
  openUntil: 0,
};

function hasGitHubAuth(env) {
  if (env.GITHUB_TOKEN) return true;
  return Boolean(env.GITHUB_APP_ID && env.GITHUB_APP_INSTALLATION_ID && env.GITHUB_APP_PRIVATE_KEY);
}

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

async function getBotLogin(env) {
  if (botLoginCache) return botLoginCache;

  // If user provided an explicit login (e.g., GitHub App slug), prefer it.
  if (env.BOT_LOGIN) {
    botLoginCache = String(env.BOT_LOGIN);
    return botLoginCache;
  }

  try {
    const { data } = await githubApi({ env, method: "GET", path: "/user" });
    botLoginCache = String(data?.login ?? "");
    return botLoginCache;
  } catch (error) {
    // GitHub App installation tokens cannot call /user; fall back to empty and rely on BOT_LOGIN.
    console.warn("getBotLogin failed; set BOT_LOGIN to your app slug if you want comment matching. Reason:", error?.message ?? error);
    botLoginCache = "";
    return botLoginCache;
  }
}

async function ensureLabelExists({ owner, repo, env, repoLabels, name, color, description }) {
  if (repoLabels.has(name)) return true;

  try {
    await githubApi({
      env,
      method: "POST",
      path: `/repos/${owner}/${repo}/labels`,
      body: { name, color, description },
    });
    repoLabels.add(name);
    return true;
  } catch (error) {
    console.warn(`Failed to create label "${name}":`, error?.message ?? error);
    return false;
  }
}

async function getMaintainers({ owner, repo, ref, env }) {
  const maintainers = new Set();
  const path = CONFIG.maintainersFile;
  if (!path) return maintainers;

  try {
    const refSuffix = ref ? `?ref=${encodeURIComponent(ref)}` : "";
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/contents/${path}${refSuffix}`,
    });
    const content = data?.content ? atob(data.content) : "";
    const githubMatches = [...String(content).matchAll(/github\s*=\s*"([^"]+)"/g)];
    for (const m of githubMatches) {
      if (m[1]) maintainers.add(m[1].toLowerCase());
    }
  } catch (error) {
    console.warn(`Failed to load maintainers from ${path}:`, error?.message ?? error);
  }

  return maintainers;
}

async function ensureReviewers({ owner, repo, pullNumber, pullRequest, maintainers, env }) {
  const desired = new Set(getAlwaysRequestReviewers(env).map((r) => String(r).toLowerCase()));
  // Optionally request the bot itself as a reviewer (set BOT_SELF_REVIEWER=true)
  try {
    const wantSelf = String(env.BOT_SELF_REVIEWER ?? "false").toLowerCase() === "true";
    if (wantSelf) {
      const botLogin = String(await getBotLogin(env) ?? "");
      if (botLogin) desired.add(botLogin.toLowerCase());
    }
  } catch (e) {
    console.warn("Failed to add bot self-reviewer:", e?.message ?? e);
  }
  for (const m of maintainers) desired.add(m);

  const author = String(pullRequest?.user?.login ?? "").toLowerCase();
  desired.delete(author);

  const existingRequested = new Set((pullRequest.requested_reviewers ?? []).map((r) => String(r.login ?? "").toLowerCase()));

  // Skip users who already left a review (avoid re-request after approve)
  const reviews = await listPullRequestReviews({ owner, repo, pullNumber, env });
  const alreadyReviewed = new Set(
    reviews
      .filter((r) => typeof r?.user?.login === "string")
      .map((r) => String(r.user.login).toLowerCase())
  );

  const toAdd = [...desired].filter(
    (r) => r && !existingRequested.has(r) && !alreadyReviewed.has(r)
  );
  if (toAdd.length === 0) return { ok: true, added: [] };

  await githubApi({
    env,
    method: "POST",
    path: `/repos/${owner}/${repo}/pulls/${pullNumber}/requested_reviewers`,
    body: { reviewers: toAdd },
  });

  return { ok: true, added: toAdd };
}

function getAlwaysRequestReviewers(env) {
  const fromEnv = parseListEnv(env.BOT_ALWAYS_REVIEWERS);
  if (fromEnv.length > 0) return fromEnv;
  return Array.isArray(CONFIG.alwaysRequestReviewers) ? CONFIG.alwaysRequestReviewers : [];
}

function parseListEnv(value) {
  if (!value || typeof value !== "string") return [];
  return value
    .split(",")
    .map((v) => v.trim())
    .filter(Boolean);
}

function parsePositiveInt(value) {
  const num = Number.parseInt(String(value ?? ""), 10);
  return Number.isFinite(num) && num > 0 ? num : null;
}

export { safeJsonParse, parsePositiveInt };

async function maybeAutoMerge({ owner, repo, pullNumber, pullRequest, maintainers, currentLabels, ciSummary, env }) {
  const enabledFlag = CONFIG.autoMerge?.enabledEnv;
  const autoMergeEnabled = enabledFlag
    ? String(env[enabledFlag] ?? "true").toLowerCase() === "true"
    : Boolean(CONFIG.autoMerge?.enabled ?? true);
  if (!autoMergeEnabled) return { ok: true, skipped: "disabled" };

  const author = String(pullRequest?.user?.login ?? "").toLowerCase();
  const isMaintainer = maintainers.has(author) || currentLabels.has(CONFIG.maintainerLabel);
  // Allow auto-approve on green when explicitly enabled via env var
  const allowApproveOnGreen = String(env.BOT_APPROVE_ON_GREEN ?? "true").toLowerCase() === "true";
  if (!isMaintainer && !allowApproveOnGreen) return { ok: true, skipped: "auto-merge-disabled" };

  if (pullRequest.state !== "open" || pullRequest.draft) return { ok: true, skipped: "not-open-or-draft" };
  if (pullRequest.mergeable === false) return { ok: true, skipped: "merge-conflict" };

  if (!ciSummary?.ok) return { ok: true, skipped: "no-ci-summary" };

  const jobs = ciSummary?.jobs ?? [];
  const runConclusion = ciSummary?.runConclusion;
  const jobsOk = jobs.length === 0 || jobs.every((j) => (j.conclusion ?? j.status) === "success");
  const runOk = runConclusion ? runConclusion === "success" : jobsOk;
  if (!runOk || !jobsOk) return { ok: true, skipped: "ci-not-green" };

  // Approve (idempotent; GitHub will no-op if already approved by this actor)
  await githubApi({
    env,
    method: "POST",
    path: `/repos/${owner}/${repo}/pulls/${pullNumber}/reviews`,
    body: { event: "APPROVE", body: "Auto-approved by maintainer bot (CI green)." },
  });

  // Merge
  const mergeMethod =
    (CONFIG.autoMerge?.mergeMethodEnv && env[CONFIG.autoMerge.mergeMethodEnv]) ||
    CONFIG.autoMerge?.mergeMethod ||
    "squash";

  await githubApi({
    env,
    method: "PUT",
    path: `/repos/${owner}/${repo}/pulls/${pullNumber}/merge`,
    body: { merge_method: mergeMethod },
  });

  return { ok: true, merged: true, method: mergeMethod };
}

async function processOpenPullRequests(env, options = {}) {
  const owner = env.GITHUB_OWNER;
  const repo = env.GITHUB_REPO;
  if (!owner || !repo) throw new Error("Missing GITHUB_OWNER/GITHUB_REPO");

  const limit = Number.isFinite(options.limit) ? options.limit : null;
  let pullRequests = await listOpenPullRequests({ owner, repo, env });
  if (limit && limit > 0 && pullRequests.length > limit) {
    const tag = options.source ? `[${options.source}]` : "[run]";
    console.warn(`${tag} Limiting PR scan to ${limit}/${pullRequests.length}`);
    pullRequests = pullRequests.slice(0, limit);
  }
  const stats = { total: pullRequests.length, processed: 0, errors: 0 };
  const results = [];

  for (const pr of pullRequests) {
    const pullNumber = pr.number;
    try {
      const result = await handlePullRequest({ owner, repo, pullNumber, env, options });
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

async function handlePullRequest({ owner, repo, pullNumber, env, options = {} }) {
  if (!owner || !repo) throw new Error("Missing owner/repo");
  if (!hasGitHubAuth(env)) throw new Error("Missing GitHub auth (GITHUB_TOKEN or GitHub App creds)");

  const dryRun = String(env.BOT_DRY_RUN ?? "false").toLowerCase() === "true";
  const light = Boolean(options.light);

  const { data: pullRequest } = await githubApi({
    env,
    method: "GET",
    path: `/repos/${owner}/${repo}/pulls/${pullNumber}`,
  });
  const isBackport =
    String(pullRequest?.head?.ref ?? "").startsWith("backport/") ||
    (pullRequest?.labels ?? []).some((l) => {
      const name = String(l?.name ?? "");
      return name === "automated-backport" || name.startsWith("backport/");
    });

  if (pullRequest.state !== "open") {
    return { ok: true, changed: false, added: [], skipped: "closed", dryRun };
  }

  const files = await listPullRequestFiles({ owner, repo, pullNumber, env });
  const repoLabels = await getRepositoryLabels({ owner, repo, env });
  const labelerRules = await loadLabelerRules({ owner, repo, env, ref: pullRequest?.base?.sha ?? null });

  const labelsToAdd = new Set();
  const labelsToRemove = new Set();
  const currentLabels = new Set((pullRequest.labels ?? []).map((l) => l.name));
  const baseRef = pullRequest?.base?.sha ?? pullRequest?.base?.ref ?? null;
  const maintainers = light ? new Set() : await getMaintainers({ owner, repo, ref: baseRef, env });
  const author = String(pullRequest?.user?.login ?? "").toLowerCase();

  for (const file of files) {
    const filename = file.filename;

    const area = getAreaFromPath(filename);
    if (area && CONFIG.labels[area]) labelsToAdd.add(CONFIG.labels[area]);

    const platform = getPlatformFromPath(filename);
    if (platform && CONFIG.platformLabels[platform]) labelsToAdd.add(CONFIG.platformLabels[platform]);

    // Labeler-style glob rules
    const matchedLabeler = matchLabelerRules(labelerRules, filename);
    for (const lbl of matchedLabeler) labelsToAdd.add(lbl);
  }

  labelsToAdd.add(getSizeLabel(pullRequest.additions, pullRequest.deletions));

  const branchType = classifyBranch(pullRequest?.head?.ref ?? "");
  const types = Array.isArray(branchType.type) ? branchType.type : branchType.type ? [branchType.type] : [];
  for (const t of types) {
    const name = `${CONFIG.branchLabelPrefix}:${t}`;
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name,
      color: "6F42C1",
      description: `Branch type: ${t}`,
    });
    if (ready) labelsToAdd.add(name);
  }

  if (maintainers.has(author)) {
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name: CONFIG.maintainerLabel,
      color: "0E8A16",
      description: "Maintainer-authored pull request",
    });
    if (ready) labelsToAdd.add(CONFIG.maintainerLabel);
  }

  const templateSelections = getTemplateSelections(pullRequest.body ?? "", CONFIG.templateTypeLabels);
  for (const selection of templateSelections) {
    const resolved = resolveTemplateLabel(selection, repoLabels);
    if (resolved) labelsToAdd.add(resolved);
  }

  const scopeSelections = getTemplateSelections(pullRequest.body ?? "", CONFIG.templateScopeLabels);
  for (const selection of scopeSelections) {
    const label = selection.label;
    if (!label?.name) continue;
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name: label.name,
      color: label.color,
      description: label.description,
    });
    if (ready) labelsToAdd.add(label.name);
  }

  const packageSelections = getTemplateSelections(pullRequest.body ?? "", CONFIG.templatePackageLabels);
  for (const selection of packageSelections) {
    const label = selection.label;
    if (!label?.name) continue;
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name: label.name,
      color: label.color,
      description: label.description,
    });
    if (ready) labelsToAdd.add(label.name);
  }

  if (pullRequest.mergeable === false) {
    const statusLabel = CONFIG.statusLabels.mergeConflict;
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name: statusLabel.name,
      color: statusLabel.color,
      description: statusLabel.description,
    });
    if (ready) labelsToAdd.add(statusLabel.name);
  } else {
    labelsToRemove.add(CONFIG.statusLabels.mergeConflict.name);
  }

  if (!light) {
    const dcoResult = await checkDcoForPullRequest({ owner, repo, pullNumber, env });
    if (!dcoResult.ok) {
      const dcoLabel = CONFIG.statusLabels.dcoMissing.name;
      const ready = await ensureLabelExists({
        owner,
        repo,
        env,
        repoLabels,
        name: dcoLabel,
        color: CONFIG.statusLabels.dcoMissing.color,
        description: CONFIG.statusLabels.dcoMissing.description,
      });
      if (ready) labelsToAdd.add(dcoLabel);
    } else {
      labelsToRemove.add(CONFIG.statusLabels.dcoMissing.name);
    }
  }

  const newLabels = [...labelsToAdd].filter((l) => !currentLabels.has(l));
  const labelsToDelete = [...labelsToRemove].filter((l) => currentLabels.has(l));

  let ciSummary = null;
  if (!light) {
    try {
      ciSummary = await updateCiSummaryComment({
        owner,
        repo,
        pullNumber,
        pullRequest,
        env,
        dryRun,
      });
    } catch (error) {
      console.warn("CI summary update failed:", error?.message ?? error);
      ciSummary = { ok: false, error: String(error?.message ?? error) };
    }
  } else {
    ciSummary = { ok: false, skipped: "cron-light" };
  }

  try {
    if (!light && !isBackport) {
      await ensureReviewers({
        owner,
        repo,
        pullNumber,
        pullRequest,
        maintainers,
        env,
      });
    }
  } catch (error) {
    console.warn("Reviewer assignment failed:", error?.message ?? error);
  }

  let autoMergeResult = null;
  try {
    autoMergeResult =
      light || isBackport
        ? { ok: true, merged: false }
        : await maybeAutoMerge({
            owner,
            repo,
            pullNumber,
            pullRequest,
            maintainers,
            currentLabels,
            ciSummary,
            env,
          });
  } catch (error) {
    console.warn("Auto-merge failed:", error?.message ?? error);
    autoMergeResult = { ok: false, error: String(error?.message ?? error) };
  }

  if (newLabels.length === 0 && labelsToDelete.length === 0) {
    return { ok: true, changed: false, added: [], removed: [], dryRun, ciSummary, autoMerge: autoMergeResult };
  }

  if (!dryRun) {
    if (newLabels.length > 0) {
      await githubApi({
        env,
        method: "POST",
        path: `/repos/${owner}/${repo}/issues/${pullNumber}/labels`,
        body: { labels: newLabels },
      });
    }

    for (const label of labelsToDelete) {
      await githubApi({
        env,
        method: "DELETE",
        path: `/repos/${owner}/${repo}/issues/${pullNumber}/labels/${encodeURIComponent(label)}`,
      });
    }
  } else {
    console.log(`DRY_RUN: would add labels to PR #${pullNumber}:`, newLabels);
    if (labelsToDelete.length > 0) {
      console.log(`DRY_RUN: would remove labels from PR #${pullNumber}:`, labelsToDelete);
    }
  }

  return { ok: true, changed: true, added: newLabels, removed: labelsToDelete, dryRun, ciSummary, autoMerge: autoMergeResult };
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

async function listIssueComments({ owner, repo, issueNumber, env }) {
  const perPage = 100;
  const comments = [];
  for (let page = 1; page <= 20; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/issues/${issueNumber}/comments?per_page=${perPage}&page=${page}`,
    });
    comments.push(...data);
    if (!Array.isArray(data) || data.length < perPage) break;
  }
  return comments;
}

async function listPullRequestCommits({ owner, repo, pullNumber, env }) {
  const perPage = 100;
  const commits = [];
  for (let page = 1; page <= 50; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/pulls/${pullNumber}/commits?per_page=${perPage}&page=${page}`,
    });
    commits.push(...data);
    if (!Array.isArray(data) || data.length < perPage) break;
  }
  return commits;
}

async function listPullRequestReviews({ owner, repo, pullNumber, env }) {
  const perPage = 100;
  const reviews = [];
  for (let page = 1; page <= 10; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/pulls/${pullNumber}/reviews?per_page=${perPage}&page=${page}`,
    });
    reviews.push(...data);
    if (!Array.isArray(data) || data.length < perPage) break;
  }
  return reviews;
}

async function checkDcoForPullRequest({ owner, repo, pullNumber, env }) {
  const commits = await listPullRequestCommits({ owner, repo, pullNumber, env });
  const missing = [];

  for (const commit of commits) {
    if (isBotCommit(commit)) continue;
    const message = commit?.commit?.message ?? "";
    if (!hasSignedOffBy(message)) {
      missing.push(commit.sha);
    }
  }

  return { ok: missing.length === 0, missing };
}

async function findWorkflowRunForPR({ owner, repo, pullNumber, headSha, env }) {
  const workflow = CONFIG.ciSummary.workflowFile;
  const events = ["pull_request", "pull_request_target"];
  let runs = [];
  for (const event of events) {
    try {
      const { data } = await githubApi({
        env,
        method: "GET",
        path: `/repos/${owner}/${repo}/actions/workflows/${workflow}/runs?per_page=30&event=${event}`,
      });
      runs = runs.concat(Array.isArray(data?.workflow_runs) ? data.workflow_runs : []);
    } catch (err) {
      log("warn", "findWorkflowRunForPR: list runs failed", { workflow, event, error: String(err?.message ?? err) });
    }
  }

  return (
    runs.find((run) => {
      const prs = run.pull_requests ?? [];
      const matchesPr = prs.some((pr) => pr.number === pullNumber);
      if (!matchesPr) return false;
      if (headSha && run.head_sha && run.head_sha !== headSha) return false;
      return true;
    }) ?? null
  );
}

async function listJobsForRun({ owner, repo, runId, env }) {
  const perPage = 100;
  const jobs = [];
  for (let page = 1; page <= 10; page++) {
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/actions/runs/${runId}/jobs?per_page=${perPage}&page=${page}`,
    });
    jobs.push(...(data?.jobs ?? []));
    if (!Array.isArray(data?.jobs) || data.jobs.length < perPage) break;
  }
  return jobs;
}

function pickJobStatus(matched) {
  // Prefer failure > action_required > cancelled > timed_out > in_progress > success > skipped > neutral > unknown
  const order = [
    "failure",
    "action_required",
    "cancelled",
    "timed_out",
    "in_progress",
    "success",
    "skipped",
    "neutral",
  ];
  if (!matched.length) return { status: null, conclusion: null };

  // Normalize statuses/conclusions
  const normalized = matched.map((job) => {
    const status = job.status === "completed" ? job.conclusion : job.status;
    return { status: status ?? job.status, conclusion: job.conclusion };
  });

  for (const key of order) {
    const hit = normalized.find(
      (j) => j.status === key || j.conclusion === key
    );
    if (hit) {
      return { status: hit.status, conclusion: hit.conclusion ?? hit.status };
    }
  }
  return { status: matched[0].status, conclusion: matched[0].conclusion };
}

function buildCiSummaryBody({ run, jobs }) {
  const rows = CONFIG.ciSummary.jobs.map(({ label, match }) => {
    const matchers = Array.isArray(match) ? match : [match];
    const matched = jobs.filter((job) => {
      const name = String(job.name ?? "").toLowerCase();
      return matchers.some((m) => {
        const needle = String(m ?? "").toLowerCase();
        return name === needle || name.includes(needle);
      });
    });
    const { status, conclusion } = pickJobStatus(matched);
    const badge = matched.length
      ? badgeForJob({ status, conclusion })
      : "❓ unknown";
    return `| ${label} | ${badge} |`;
  });

  return [
    CONFIG.ciSummary.marker,
    "## PR CI Summary",
    "",
    `Run: ${run.html_url}`,
    "",
    "| Job | Result |",
    "|-----|--------|",
    ...rows,
  ].join("\n");
}

async function buildStatusPage({ env, prNumber }) {
  const owner = env.GITHUB_OWNER;
  const repo = env.GITHUB_REPO;
  const workflows = CONFIG.statusPage.workflows;
  const results = [];

  for (const wf of workflows) {
    const wfFile = wf.file;
    try {
      const run = await getLatestWorkflowRun({ owner, repo, workflowFile: wfFile, prNumber, env });
      results.push({
        file: wfFile,
        label: wf.label ?? wfFile,
        run,
      });
    } catch (error) {
      results.push({
        file: wfFile,
        label: wf.label ?? wfFile,
        error: String(error?.message ?? error),
      });
    }
  }

  return {
    ok: true,
    workflows: results,
    updatedAt: new Date().toISOString(),
  };
}

async function getLatestWorkflowRun({ owner, repo, workflowFile, prNumber, env }) {
  const { data } = await githubApi({
    env,
    method: "GET",
    path: `/repos/${owner}/${repo}/actions/workflows/${workflowFile}/runs?per_page=30`,
  });
  const runs = Array.isArray(data?.workflow_runs) ? data.workflow_runs : [];
  const run = prNumber
    ? runs.find((r) => Array.isArray(r.pull_requests) && r.pull_requests.some((pr) => pr.number === prNumber))
    : runs[0];
  if (!run) return null;

  const jobs = await listJobsForRun({ owner, repo, runId: run.id, env });
  return {
    id: run.id,
    name: run.name,
    status: run.status,
    conclusion: run.conclusion,
    url: run.html_url,
    event: run.event,
    created_at: run.created_at,
    updated_at: run.updated_at,
    jobs: jobs.map((job) => ({
      id: job.id,
      name: job.name,
      status: job.status,
      conclusion: job.conclusion,
      url: job.html_url,
    })),
    prNumber: prNumber ?? null,
  };
}

function renderStatusPage({ data, prNumber }) {
  const heading = prNumber ? `ProjT Bot Status – PR #${prNumber}` : "ProjT Bot Status";
  const subhead = prNumber
    ? `Latest runs for PR #${prNumber}`
    : "Latest runs on default branches";

  const rows = data.workflows
    .map((wf) => {
      if (wf.error) {
        return `<tr><td>${escapeHtml(wf.label)}</td><td>❓</td><td class="error">${escapeHtml(wf.error)}</td></tr>`;
      }
      if (!wf.run) {
        return `<tr><td>${escapeHtml(wf.label)}</td><td>❓</td><td>No runs</td></tr>`;
      }
      const badge = badgeForJob({ status: wf.run.status, conclusion: wf.run.conclusion });
      return `<tr><td>${escapeHtml(wf.label)}</td><td>${escapeHtml(badge)}</td><td><a href="${wf.run.url}">${wf.run.event ?? ""}</a></td></tr>`;
    })
    .join("");

  return `<!doctype html>
<html>
<head>
  <meta charset="utf-8" />
  <title>${escapeHtml(heading)}</title>
  <style>
    body { font-family: sans-serif; background: #0d1117; color: #e6edf3; padding: 24px; }
    h1 { margin-top: 0; }
    table { border-collapse: collapse; width: 100%; max-width: 720px; }
    th, td { padding: 8px 12px; border-bottom: 1px solid #30363d; }
    a { color: #58a6ff; text-decoration: none; }
    .error { color: #ff7b72; }
    .meta { color: #8b949e; font-size: 0.9em; }
  </style>
</head>
<body>
  <h1>${escapeHtml(heading)}</h1>
  <div class="meta">${escapeHtml(subhead)} · Updated: ${escapeHtml(data.updatedAt)}</div>
  <table>
    <thead><tr><th>Workflow</th><th>Status</th><th>Run</th></tr></thead>
    <tbody>${rows}</tbody>
  </table>
</body>
</html>`;
}

function escapeHtml(str) {
  return String(str ?? "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

let labelerCache = null;

async function loadLabelerRules({ owner, repo, env, ref }) {
  if (!CONFIG.labeler.enabled) return [];
  const now = Date.now();
  if (labelerCache && now - labelerCache.ts < CONFIG.labeler.ttlMs) return labelerCache.rules;

  const path = CONFIG.labeler.path;
  try {
    const refSuffix = ref ? `?ref=${encodeURIComponent(ref)}` : "";
    const { data } = await githubApi({
      env,
      method: "GET",
      path: `/repos/${owner}/${repo}/contents/${path}${refSuffix}`,
    });
    const content = data?.content ? atob(data.content) : "";
    const rules = parseLabelerYaml(content);
    labelerCache = { ts: now, rules };
    return rules;
  } catch (error) {
    console.warn("Failed to load labeler config:", error?.message ?? error);
    return [];
  }
}

function parseLabelerYaml(text) {
  const rules = [];
  let currentLabel = null;
  let collectingGlobs = false;
  for (const rawLine of String(text).split(/\r?\n/)) {
    const line = rawLine.replace(/\t/g, "  ");
    const labelMatch = line.match(/^([A-Za-z0-9._:-]+):\s*$/);
    if (labelMatch) {
      currentLabel = labelMatch[1];
      collectingGlobs = false;
      continue;
    }
    if (!currentLabel) continue;
    if (line.includes("any-glob-to-any-file")) {
      collectingGlobs = true;
      continue;
    }
    if (collectingGlobs) {
      const globMatch = line.match(/^\s*-\s+(.+?)\s*$/);
      if (globMatch && globMatch[1]) {
        const pattern = globMatch[1].trim().replace(/^"|"$/g, "");
        rules.push({ label: currentLabel, pattern });
      }
    }
  }
  return rules;
}

function matchLabelerRules(rules, filePath) {
  const matched = new Set();
  for (const { label, pattern } of rules) {
    if (globMatch(pattern, filePath)) matched.add(label);
  }
  return [...matched];
}

function globMatch(pattern, text) {
  const regex = globToRegExp(pattern);
  return regex.test(text);
}

function globToRegExp(pattern) {
  // Very small glob-to-regex converter supporting *, **, ?, [].
  let re = "";
  let i = 0;
  while (i < pattern.length) {
    const ch = pattern[i];
    if (ch === "*") {
      if (pattern[i + 1] === "*") {
        re += ".*";
        i += 2;
      } else {
        re += "[^/]*";
        i += 1;
      }
    } else if (ch === "?") {
      re += ".";
      i += 1;
    } else if ("\\.[]{}()+-^$|".includes(ch)) {
      re += "\\" + ch;
      i += 1;
    } else {
      re += ch;
      i += 1;
    }
  }
  return new RegExp("^" + re + "$");
}

async function updateCiSummaryComment({ owner, repo, pullNumber, pullRequest, env, dryRun }) {
  const headSha = pullRequest?.head?.sha ?? "";
  const run = await findWorkflowRunForPR({ owner, repo, pullNumber, headSha, env });
  if (!run) return { ok: false, skipped: true, reason: "no-workflow-run" };

  const jobs = await listJobsForRun({ owner, repo, runId: run.id, env });
  const body = buildCiSummaryBody({ run, jobs });
  const summaryMeta = {
    runId: run.id,
    runConclusion: run.conclusion ?? null,
    jobs: jobs.map((job) => ({
      id: job.id,
      name: job.name,
      status: job.status,
      conclusion: job.conclusion,
    })),
  };

  const comments = await listIssueComments({ owner, repo, issueNumber: pullNumber, env });
  const marker = CONFIG.ciSummary.marker;
  const existing = comments.find((comment) => String(comment.body ?? "").includes(marker));

  if (existing) {
    if (String(existing.body ?? "") === body) {
      return { ok: true, updated: false, commentId: existing.id, ...summaryMeta };
    }
    if (!dryRun) {
      await githubApi({
        env,
        method: "PATCH",
        path: `/repos/${owner}/${repo}/issues/comments/${existing.id}`,
        body: { body },
      });
    }
    return { ok: true, updated: true, commentId: existing.id, ...summaryMeta };
  }

  if (!dryRun) {
    const { data } = await githubApi({
      env,
      method: "POST",
      path: `/repos/${owner}/${repo}/issues/${pullNumber}/comments`,
      body: { body },
    });
    return { ok: true, created: true, commentId: data?.id ?? null, ...summaryMeta };
  }

  return { ok: true, created: false, dryRun: true, ...summaryMeta };
}

async function performMergeCommand({ owner, repo, issueNumber, env, commentAuthor }) {
  if (!hasGitHubAuth(env)) throw new Error("Missing GitHub auth (GITHUB_TOKEN or GitHub App creds)");

  const { data: pr } = await githubApi({ env, method: "GET", path: `/repos/${owner}/${repo}/pulls/${issueNumber}` });
  if (!pr) return { ok: false, error: "pull-not-found" };
  if (pr.state !== "open") return { ok: false, skipped: true, reason: "not-open" };
  if (pr.draft) return { ok: false, skipped: true, reason: "draft" };

  // Only allow merge command from users listed in the maintainers file
  try {
    const ref = pr?.base?.sha ?? pr?.base?.ref ?? null;
    const maintainers = await getMaintainers({ owner, repo, ref, env });
    const who = String(commentAuthor ?? "").toLowerCase();
    if (!who || !maintainers.has(who)) {
      return { ok: false, skipped: true, reason: "not-maintainer", allowedBy: "maintainers.nix" };
    }
  } catch (e) {
    console.warn("Failed to verify maintainers for merge command:", e?.message ?? e);
    return { ok: false, error: `maintainers-check-failed: ${String(e?.message ?? e)}` };
  }

  // Check CI runs
  let ciSummary = null;
  try {
    ciSummary = await updateCiSummaryComment({ owner, repo, pullNumber: issueNumber, pullRequest: pr, env, dryRun: false });
  } catch (e) {
    ciSummary = { ok: false, error: String(e?.message ?? e) };
  }

  const allowSkipCi = String(env.BOT_ALLOW_MERGE_COMMAND_SKIP_CI ?? "false").toLowerCase() === "true";

  const jobs = ciSummary?.jobs ?? [];
  const runConclusion = ciSummary?.runConclusion;
  const jobsOk = jobs.length === 0 || jobs.every((j) => (j.conclusion ?? j.status) === "success");
  const runOk = runConclusion ? runConclusion === "success" : jobsOk;
  if (!runOk && !jobsOk && !allowSkipCi) {
    return { ok: false, skipped: true, reason: "ci-not-green" };
  }

  // Approve
  try {
    await githubApi({ env, method: "POST", path: `/repos/${owner}/${repo}/pulls/${issueNumber}/reviews`, body: { event: "APPROVE", body: "Approved by bot on user request." } });
  } catch (e) {
    return { ok: false, error: `approve-failed: ${String(e?.message ?? e)}` };
  }

  // Merge
  try {
    const { data } = await githubApi({ env, method: "PUT", path: `/repos/${owner}/${repo}/pulls/${issueNumber}/merge`, body: { merge_method: CONFIG.autoMerge.mergeMethod || "squash" } });
    return { ok: true, merged: Boolean(data?.merged ?? true), data };
  } catch (e) {
    return { ok: false, error: `merge-failed: ${String(e?.message ?? e)}` };
  }
}

async function handleIssueComment({ payload, env }) {
  const correlationId = crypto.randomUUID();
  const issue = payload?.issue;
  const commentBody = String(payload?.comment?.body ?? "");
  const association = String(payload?.comment?.author_association ?? "");
  const issueNumber = issue?.number;
  const isPR = Boolean(issue?.pull_request);

  if (!isPR || typeof issueNumber !== "number") {
    log("info", "Ignoring comment (not a PR)", { correlationId, issueNumber });
    return { ok: true, ignored: true, reason: "not-a-pr" };
  }

  const allowed = parseAllowedAssociations(env);
  if (!allowed.has(association.toUpperCase())) {
    log("info", "Ignoring comment (association denied)", { correlationId, issueNumber, association });
    return { ok: true, ignored: true, reason: "association-denied", association };
  }

  const owner = env.GITHUB_OWNER;
  const repo = env.GITHUB_REPO;
  const shouldComment = String(env.BOT_COMMENT_ON_COMMAND ?? "false").toLowerCase() === "true";

  const commands = extractCommands(commentBody, CONFIG.commentCommands);
  if (commands.size === 0) {
    log("info", "Ignoring comment (no command)", { correlationId, issueNumber });
    return { ok: true, ignored: true, reason: "no-command" };
  }

  const dispatchResult = [];
  for (const cmd of commands) {
    const logFields = { correlationId, issueNumber, command: cmd };
    if (cmd === "bot merge" || cmd === "/bot merge") {
      try {
        const commentAuthor = String(payload?.comment?.user?.login ?? "").toLowerCase();
        const mergeResult = await performMergeCommand({ owner, repo, issueNumber, env, commentAuthor });
        dispatchResult.push({ command: cmd, result: mergeResult });
        if (shouldComment) {
          const text = mergeResult?.ok
            ? `Merge attempted: ${mergeResult.merged ? "merged" : "not merged"}.`
            : `Merge failed: ${mergeResult?.error ?? "unknown"}`;
          await postComment({ env, owner, repo, issueNumber, body: text });
        }
      } catch (err) {
        log("warn", "Merge command failed", { ...logFields, error: String(err?.message ?? err) });
        if (shouldComment) {
          await postComment({
            env,
            owner,
            repo,
            issueNumber,
            body: `Merge command failed: ${String(err?.message ?? err)}`,
          });
        }
      }
      continue;
    }

    if (cmd === "/retest" || cmd === "bot rerun" || cmd === "/bot rerun") {
      const result = await handlePullRequest({ owner, repo, pullNumber: issueNumber, env });
      dispatchResult.push({ command: cmd, ok: result?.ok !== false, result });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body: formatResultComment({ result, pr: issueNumber }),
        });
      }
      continue;
    }

    if (cmd === "/rebuild") {
      log("info", "Rebuild requested", logFields);
      dispatchResult.push({ command: cmd, ok: true, info: "rebuild-requested" });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body: "Rebuild requested. Please trigger CI via rerun or push.",
        });
      }
      continue;
    }

    if (cmd === "/backport") {
      log("info", "Backport requested", logFields);
      dispatchResult.push({ command: cmd, ok: true, info: "backport-requested" });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body: "Backport requested. Please apply the backport label to target branch.",
        });
      }
      continue;
    }

    if (cmd === "/help" || cmd === "bot help") {
      dispatchResult.push({ command: cmd, ok: true, info: "Help dispatched" });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body:
            "Available commands: `/help`, `/status`, `/version`, `/retest`, `/rebuild`, `/backport`, `/bot merge`, `/bot labels`, `/bot rerun`.",
        });
      }
      continue;
    }

    if (cmd === "/status") {
      dispatchResult.push({ command: cmd, ok: true, info: "Status requested" });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body: "Status: request received. CI results will be updated shortly.",
        });
      }
      continue;
    }

    if (cmd === "/version") {
      dispatchResult.push({ command: cmd, ok: true, info: "Version requested" });
      if (shouldComment) {
        await postComment({
          env,
          owner,
          repo,
          issueNumber,
          body: "Bot version: projtlauncher-bot (worker).",
        });
      }
      continue;
    }

    // Default: rerun/labels/other commands trigger PR processing
    const result = await handlePullRequest({ owner, repo, pullNumber: issueNumber, env });
    dispatchResult.push({ command: cmd, ok: result?.ok !== false, result });
    if (shouldComment) {
      const summary = formatResultComment({ result, pr: issueNumber });
      await postComment({
        env,
        owner,
        repo,
        issueNumber,
        body: summary,
      });
    }
  }

  return { ok: true, handled: true, results: dispatchResult };
}

async function handleWorkflowRun({ owner, repo, payload, env }) {
  if (!isAutoApproveEnabled(env)) return { ok: true, skipped: "disabled" };

  const run = payload?.workflow_run;
  if (!run) return { ok: false, error: "missing-workflow-run" };
  if (run.event !== "pull_request") return { ok: true, skipped: "not-pr-event" };
  if (run.status !== "waiting") return { ok: true, skipped: "not-waiting" };

  const rule = findAutoApproveRule(run);
  if (!rule) return { ok: true, skipped: "no-rule" };

  const prNumber = Number(run.pull_requests?.[0]?.number);
  if (!Number.isFinite(prNumber)) return { ok: true, skipped: "no-pr-number" };

  const { data: pr } = await githubApi({
    env,
    method: "GET",
    path: `/repos/${owner}/${repo}/pulls/${prNumber}`,
  });

  const body = String(pr?.body ?? "");
  const selectedOptions = getSelectedScopeOptions(body);
  const selectedLabels = new Set((pr.labels ?? []).map((label) => label.name));
  const scopes = Array.isArray(rule.scopes) ? rule.scopes : [];
  const allowed =
    scopes.length === 0 || scopes.some((scope) => isScopeSelected(scope, { selectedOptions, selectedLabels }));

  if (!allowed) return { ok: true, skipped: "scope-not-selected" };

  await githubApi({
    env,
    method: "POST",
    path: `/repos/${owner}/${repo}/actions/runs/${run.id}/approve`,
  });

  return { ok: true, approved: true };
}

async function githubApi({ env, method, path, body }) {
  const { token } = await getGitHubAuth(env);
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

  const maxAttempts = Number(env.BOT_GITHUB_RETRIES || 3);
  const timeoutMs = Number(env.BOT_GITHUB_TIMEOUT_MS || 15000);
  const baseDelay = Number(env.BOT_GITHUB_BACKOFF_MS || 500);
  const cbFails = Number(env.BOT_GITHUB_CB_FAILS || 5);
  const cbCooldown = Number(env.BOT_GITHUB_CB_COOLDOWN_MS || 60_000);

  const now = Date.now();
  if (now < circuitBreaker.openUntil) {
    const waitMs = circuitBreaker.openUntil - now;
    const err = new Error(`GitHub API circuit open, retry after ${waitMs}ms`);
    err.circuitOpen = true;
    throw err;
  }

  let lastError;
  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), timeoutMs);
    try {
      const res = await fetch(url, { ...init, signal: controller.signal });
      clearTimeout(timer);
      const contentType = res.headers.get("content-type") ?? "";
      const data = contentType.includes("application/json") ? await res.json() : await res.text();
      if (!res.ok) {
        const message = typeof data === "string" ? data : JSON.stringify(data);
        const error = new Error(`GitHub API ${method} ${path} failed: ${res.status} ${message}`);
        error.status = res.status;
        error.response = data;
        throw error;
      }
      // success -> reset breaker
      circuitBreaker.failures = 0;
      circuitBreaker.openUntil = 0;
      return { res, data };
    } catch (error) {
      clearTimeout(timer);
      lastError = error;
      const status = error?.status;
      const retryable =
        !status || status >= 500 || status === 429 || error?.name === "AbortError";
      log("warn", "GitHub API attempt failed", {
        attempt,
        maxAttempts,
        method,
        path,
        status: status ?? "unknown",
        retryable,
        error: String(error?.message ?? error),
      });
      if (!retryable || attempt === maxAttempts) {
        circuitBreaker.failures += 1;
        if (circuitBreaker.failures >= cbFails) {
          circuitBreaker.openUntil = Date.now() + cbCooldown;
          log("error", "GitHub API circuit opened", {
            failures: circuitBreaker.failures,
            cooldownMs: cbCooldown,
            method,
            path,
          });
        }
        throw error;
      }
      const delay = baseDelay * Math.pow(2, attempt - 1);
      await sleep(delay);
    }
  }
  throw lastError ?? new Error(`GitHub API ${method} ${path} failed after retries`);
}

async function getGitHubAuth(env) {
  if (env.GITHUB_TOKEN) {
    return { token: env.GITHUB_TOKEN, source: "token" };
  }

  const appId = env.GITHUB_APP_ID;
  const installationId = env.GITHUB_APP_INSTALLATION_ID;
  const privateKey = env.GITHUB_APP_PRIVATE_KEY;
  if (!appId || !installationId || !privateKey) {
    throw new Error("Missing GITHUB_TOKEN or GitHub App credentials");
  }

  const now = Date.now();
  if (appTokenCache && now < appTokenCache.expiresAt - 60 * 1000) {
    return { token: appTokenCache.token, source: "app" };
  }

  const jwt = await createGitHubAppJwt({ appId, privateKey });
  const res = await fetch(
    `https://api.github.com/app/installations/${installationId}/access_tokens`,
    {
      method: "POST",
      headers: {
        accept: "application/vnd.github+json",
        authorization: `Bearer ${jwt}`,
        "user-agent": "projtlauncher-bot-worker",
      },
    }
  );
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`GitHub App token exchange failed: ${res.status} ${text}`);
  }
  const data = await res.json();
  const expiresAt = Date.parse(data?.expires_at ?? "") || now + 30 * 60 * 1000;

  appTokenCache = { token: data.token, expiresAt };
  return { token: data.token, source: "app" };
}

async function createGitHubAppJwt({ appId, privateKey }) {
  const header = { alg: "RS256", typ: "JWT" };
  const now = Math.floor(Date.now() / 1000);
  const payload = {
    iat: now - 60,
    exp: now + 9 * 60, // GitHub requires max 10 minutes
    iss: String(appId),
  };

  const enc = (obj) => base64UrlEncode(new TextEncoder().encode(JSON.stringify(obj)));
  const unsigned = `${enc(header)}.${enc(payload)}`;

  const keyData = pemToArrayBuffer(normalizePrivateKey(privateKey));
  const key = await crypto.subtle.importKey(
    "pkcs8",
    keyData,
    { name: "RSASSA-PKCS1-v1_5", hash: "SHA-256" },
    false,
    ["sign"]
  );
  const signature = await crypto.subtle.sign(
    { name: "RSASSA-PKCS1-v1_5" },
    key,
    new TextEncoder().encode(unsigned)
  );

  return `${unsigned}.${base64UrlEncode(new Uint8Array(signature))}`;
}

function normalizePrivateKey(pem) {
  const normalized = String(pem).replace(/\\n/g, "\n").trim();
  if (normalized.includes("BEGIN")) return normalized;
  // Allow raw base64 content without headers
  return `-----BEGIN PRIVATE KEY-----\n${normalized}\n-----END PRIVATE KEY-----`;
}

function pemToArrayBuffer(pem) {
  const base64 = pem.replace(/-----[^-]+-----/g, "").replace(/\s+/g, "");
  const binary = atob(base64);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
  return bytes.buffer;
}

function base64UrlEncode(data) {
  const bytes = data instanceof Uint8Array ? data : new Uint8Array(data);
  let binary = "";
  for (const b of bytes) binary += String.fromCharCode(b);
  return btoa(binary).replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/g, "");
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

function containsCommand(body, commands) {
  const normalized = body.toLowerCase();
  return commands.some(cmd => normalized.includes(cmd.toLowerCase()));
}

function parseAllowedAssociations(env) {
  const value = env.BOT_ALLOWED_ASSOCIATIONS;
  if (value && typeof value === "string") {
    return new Set(
      value
        .split(",")
        .map(v => v.trim().toUpperCase())
        .filter(Boolean)
    );
  }
  // Default: owners, members, collaborators
  return new Set(["OWNER", "MEMBER", "COLLABORATOR"]);
}

function formatResultComment({ result, pr }) {
  if (!result?.ok) {
    return `PR #${pr}: Bot failed: ${result?.error ?? "unknown error"}`;
  }
  if (result.dryRun) {
    return `PR #${pr}: DRY_RUN enabled; would add labels: ${result.added?.join(", ") || "none"}.`;
  }
  if (!result.changed) {
    return `PR #${pr}: No new labels needed.`;
  }
  const added = result.added?.length ? `Added labels: ${result.added.join(", ")}.` : "";
  const removed = result.removed?.length ? `Removed labels: ${result.removed.join(", ")}.` : "";
  const details = [added, removed].filter(Boolean).join(" ");
  return `PR #${pr}: ${details || "Label updates applied."}`;
}
