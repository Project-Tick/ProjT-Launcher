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
    { option: "Chore", candidates: ["21.type:chore", "21.type:other", "chore"] },
    { option: "Other", candidates: ["21.type:other", "other"] },
  ],
  dco: {
    label: "status:dco-missing",
    color: "B60205",
    description: "Missing DCO Signed-off-by in one or more commits",
  },
  ciSummary: {
    marker: "<!-- projt-bot:pr-summary -->",
    workflowFile: "pull-request-target.yml",
    jobs: ["Prepare", "Check", "Lint", "Build"],
  },
  commentCommands: [
    "bot rerun",
    "bot labels",
    "/bot rerun",
    "/bot labels",
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
let botLoginCache = null;

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
  const { data } = await githubApi({ env, method: "GET", path: "/user" });
  botLoginCache = String(data?.login ?? "");
  return botLoginCache;
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

  if (pullRequest.state !== "open") {
    return { ok: true, changed: false, added: [], skipped: "closed", dryRun };
  }

  const files = await listPullRequestFiles({ owner, repo, pullNumber, env });
  const repoLabels = await getRepositoryLabels({ owner, repo, env });

  const labelsToAdd = new Set();
  const labelsToRemove = new Set();
  const currentLabels = new Set((pullRequest.labels ?? []).map((l) => l.name));

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

  const dcoResult = await checkDcoForPullRequest({ owner, repo, pullNumber, env });
  if (!dcoResult.ok) {
    const dcoLabel = CONFIG.dco.label;
    const ready = await ensureLabelExists({
      owner,
      repo,
      env,
      repoLabels,
      name: dcoLabel,
      color: CONFIG.dco.color,
      description: CONFIG.dco.description,
    });
    if (ready) labelsToAdd.add(dcoLabel);
  } else {
    labelsToRemove.add(CONFIG.dco.label);
  }

  const newLabels = [...labelsToAdd].filter((l) => !currentLabels.has(l));
  const labelsToDelete = [...labelsToRemove].filter((l) => currentLabels.has(l));

  let ciSummary = null;
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

  if (newLabels.length === 0 && labelsToDelete.length === 0) {
    return { ok: true, changed: false, added: [], removed: [], dryRun, ciSummary };
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

  return { ok: true, changed: true, added: newLabels, removed: labelsToDelete, dryRun, ciSummary };
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
  const { data } = await githubApi({
    env,
    method: "GET",
    path: `/repos/${owner}/${repo}/actions/workflows/${workflow}/runs?per_page=30&event=pull_request_target`,
  });
  const runs = Array.isArray(data?.workflow_runs) ? data.workflow_runs : [];
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

function buildCiSummaryBody({ run, jobs }) {
  const jobMap = new Map(jobs.map((job) => [job.name, job]));
  const rows = CONFIG.ciSummary.jobs.map((name) => {
    const job = jobMap.get(name);
    const badge = job
      ? badgeForJob({ status: job.status, conclusion: job.conclusion })
      : "❓ unknown";
    return `| ${name} | ${badge} |`;
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

async function updateCiSummaryComment({ owner, repo, pullNumber, pullRequest, env, dryRun }) {
  const headSha = pullRequest?.head?.sha ?? "";
  const run = await findWorkflowRunForPR({ owner, repo, pullNumber, headSha, env });
  if (!run) return { ok: false, skipped: true, reason: "no-workflow-run" };

  const jobs = await listJobsForRun({ owner, repo, runId: run.id, env });
  const body = buildCiSummaryBody({ run, jobs });

  const comments = await listIssueComments({ owner, repo, issueNumber: pullNumber, env });
  const marker = CONFIG.ciSummary.marker;
  const existing = comments.find((comment) => String(comment.body ?? "").includes(marker));
  const botLogin = await getBotLogin(env);

  if (existing && existing.user?.login === botLogin) {
    if (String(existing.body ?? "") === body) {
      return { ok: true, updated: false, commentId: existing.id };
    }
    if (!dryRun) {
      await githubApi({
        env,
        method: "PATCH",
        path: `/repos/${owner}/${repo}/issues/comments/${existing.id}`,
        body: { body },
      });
    }
    return { ok: true, updated: true, commentId: existing.id };
  }

  if (existing && existing.user?.login === "github-actions[bot]" && !dryRun) {
    try {
      await githubApi({
        env,
        method: "DELETE",
        path: `/repos/${owner}/${repo}/issues/comments/${existing.id}`,
      });
    } catch (error) {
      console.warn("Failed to remove previous summary comment:", error?.message ?? error);
    }
  }

  if (!dryRun) {
    const { data } = await githubApi({
      env,
      method: "POST",
      path: `/repos/${owner}/${repo}/issues/${pullNumber}/comments`,
      body: { body },
    });
    return { ok: true, created: true, commentId: data?.id ?? null };
  }

  return { ok: true, created: false, dryRun: true };
}

async function handleIssueComment({ payload, env }) {
  const issue = payload?.issue;
  const commentBody = String(payload?.comment?.body ?? "");
  const association = String(payload?.comment?.author_association ?? "");
  const issueNumber = issue?.number;
  const isPR = Boolean(issue?.pull_request);

  if (!isPR || typeof issueNumber !== "number") {
    return { ok: true, ignored: true, reason: "not-a-pr" };
  }

  const allowed = parseAllowedAssociations(env);
  if (!allowed.has(association.toUpperCase())) {
    return { ok: true, ignored: true, reason: "association-denied", association };
  }

  if (!containsCommand(commentBody, CONFIG.commentCommands)) {
    return { ok: true, ignored: true, reason: "no-command" };
  }

  const owner = env.GITHUB_OWNER;
  const repo = env.GITHUB_REPO;
  const result = await handlePullRequest({ owner, repo, pullNumber: issueNumber, env });

  const shouldComment = String(env.BOT_COMMENT_ON_COMMAND ?? "false").toLowerCase() === "true";
  if (shouldComment) {
    const summary = formatResultComment({ result, pr: issueNumber });
    await githubApi({
      env,
      method: "POST",
      path: `/repos/${owner}/${repo}/issues/${issueNumber}/comments`,
      body: { body: summary },
    });
  }

  return { ok: true, handled: true, result };
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
