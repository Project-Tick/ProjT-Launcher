/**
 * ProjT Launcher - Backport Handler
 * Handles backport requests via PR comments.
 *
 * Command (single line):
 *   @projt-launcher-bot backport <target...> [--force] [--no-pr]
 *
 * Targets:
 *   - release-* branch name (e.g. release-1.2.3)
 *   - latest (highest versioned release-*)
 *   - all (all release-* branches)
 *
 * If no targets are provided, it falls back to PR labels:
 *   backport/<branch>, backport/latest, backport/all
 */

const { execFile } = require('node:child_process')
const { promisify } = require('node:util')

const execFileAsync = promisify(execFile)

function stripNoise(body = '') {
  return String(body)
    .replace(/\r/g, '')
    .replace(/<!--.*?-->/gms, '')
    .replace(/(^`{3,})[^`].*?\1/gms, '')
}

function tokenize(argString) {
  const tokens = []
  let i = 0
  let current = ''
  let quote = null

  const push = () => {
    if (current.length > 0) tokens.push(current)
    current = ''
  }

  while (i < argString.length) {
    const ch = argString[i]

    if (quote) {
      if (ch === quote) {
        quote = null
      } else if (ch === '\\' && i + 1 < argString.length) {
        i++
        current += argString[i]
      } else {
        current += ch
      }
      i++
      continue
    }

    if (ch === '"' || ch === "'") {
      quote = ch
      i++
      continue
    }

    if (/\s/.test(ch)) {
      push()
      i++
      while (i < argString.length && /\s/.test(argString[i])) i++
      continue
    }

    current += ch
    i++
  }

  push()
  return tokens
}

function parseBackportCommand(body) {
  const cleaned = stripNoise(body)
  const match = cleaned.match(/^@projt-launcher-bot\s+backport\b(.*)$/im)
  if (!match) return null

  const tokens = tokenize(match[1] ?? '')
  const targets = []
  const options = {
    force: false,
    noPr: false,
  }

  for (let idx = 0; idx < tokens.length; idx++) {
    const t = tokens[idx]
    if (!t) continue

    if (t === '--force') {
      options.force = true
      continue
    }

    if (t === '--no-pr') {
      options.noPr = true
      continue
    }

    if (t === '--to') {
      const next = tokens[idx + 1]
      if (next) {
        targets.push(next)
        idx++
      }
      continue
    }

    if (t.startsWith('--to=')) {
      targets.push(t.slice('--to='.length))
      continue
    }

    if (t.startsWith('-')) {
      continue
    }

    targets.push(t)
  }

  return { targets, options }
}

function parseReleaseVersionTuple(branch) {
  const m = String(branch).match(/^release-(v?\d+(?:\.\d+){1,2})(?:$|[-_].*)$/i)
  if (!m) return null
  const parts = m[1].replace(/^v/i, '').split('.').map((p) => Number(p))
  while (parts.length < 3) parts.push(0)
  if (parts.some((n) => Number.isNaN(n))) return null
  return parts
}

function compareVersionTuples(a, b) {
  for (let i = 0; i < Math.max(a.length, b.length); i++) {
    const av = a[i] ?? 0
    const bv = b[i] ?? 0
    if (av !== bv) return av - bv
  }
  return 0
}

async function addReaction({ github, node_id, reaction }) {
  await github.graphql(
    `mutation($node_id: ID!, $reaction: ReactionContent!) {
      addReaction(input: { content: $reaction, subjectId: $node_id }) {
        clientMutationId
      }
    }`,
    { node_id, reaction },
  )
}

async function listReleaseBranches({ github, context }) {
  const branches = await github.paginate(github.rest.repos.listBranches, {
    ...context.repo,
    per_page: 100,
  })
  return branches.map((b) => b.name).filter((n) => /^release-/.test(n))
}

async function resolveTargets({ github, context, core, pull_request, requestedTargets }) {
  const releaseBranches = await listReleaseBranches({ github, context })
  const releaseSet = new Set(releaseBranches)

  const normalized = (requestedTargets ?? [])
    .map((t) => String(t).trim())
    .filter(Boolean)

  const wantsAll = normalized.includes('all')
  const wantsLatest = normalized.includes('latest')

  const explicit = normalized.filter((t) => t !== 'all' && t !== 'latest')

  const resolved = new Set()

  if (wantsAll) {
    for (const b of releaseBranches) resolved.add(b)
  }

  if (wantsLatest) {
    const candidates = releaseBranches
      .map((b) => ({ b, v: parseReleaseVersionTuple(b) }))
      .filter((x) => x.v)
      .sort((x, y) => compareVersionTuples(x.v, y.v))

    if (candidates.length > 0) {
      resolved.add(candidates[candidates.length - 1].b)
    } else {
      core.warning('No versioned release-* branches found for target "latest"')
    }
  }

  for (const t of explicit) {
    if (releaseSet.has(t)) {
      resolved.add(t)
    } else {
      core.warning(`Ignoring unknown target branch: ${t}`)
    }
  }

  // Fallback to PR labels if comment had no targets.
  if (resolved.size === 0) {
    const labels = (pull_request.labels ?? []).map((l) => l.name)
    const labelTargets = []
    for (const label of labels) {
      if (!label.startsWith('backport/')) continue
      labelTargets.push(label.slice('backport/'.length))
    }
    if (labelTargets.length > 0) {
      return resolveTargets({
        github,
        context,
        core,
        pull_request,
        requestedTargets: labelTargets,
      })
    }
  }

  return [...resolved]
}

async function git(args, opts = {}) {
  const { cwd, core, allowFailure } = opts
  try {
    const { stdout, stderr } = await execFileAsync('git', args, { cwd })
    if (stderr && core) core.info(stderr.trim())
    return stdout.trim()
  } catch (e) {
    if (allowFailure) return null
    throw e
  }
}

async function remoteBranchExists({ cwd, branch }) {
  try {
    await execFileAsync('git', ['ls-remote', '--exit-code', '--heads', 'origin', branch], { cwd })
    return true
  } catch {
    return false
  }
}

async function getCommitParentCount({ cwd, sha }) {
  const raw = await git(['cat-file', '-p', sha], { cwd })
  return raw.split('\n').filter((l) => l.startsWith('parent ')).length
}

async function createOrReuseBackportPR({
  github,
  context,
  core,
  targetBranch,
  backportBranch,
  originalPR,
  originalTitle,
  cherryPickedSha,
  requestedVia = 'bot comment',
}) {
  const head = `${context.repo.owner}:${backportBranch}`

  const { data: prs } = await github.rest.pulls.list({
    ...context.repo,
    state: 'all',
    head,
    base: targetBranch,
    per_page: 10,
  })

  if (prs.length > 0) {
    return { number: prs[0].number, url: prs[0].html_url, state: prs[0].state, reused: true }
  }

  const { data: created } = await github.rest.pulls.create({
    ...context.repo,
    title: `[Backport ${targetBranch}] ${originalTitle}`,
    body: [
      `Automated backport of #${originalPR} to \`${targetBranch}\`.`,
      ``,
      `- Original PR: #${originalPR}`,
      `- Cherry-picked: \`${cherryPickedSha}\``,
      `- Requested via ${requestedVia}`,
    ].join('\n'),
    head: backportBranch,
    base: targetBranch,
    maintainer_can_modify: true,
  })

  try {
    await github.rest.issues.addLabels({
      ...context.repo,
      issue_number: created.number,
      labels: ['automated-backport'],
    })
  } catch (e) {
    core.warning(`Failed to add label "automated-backport" to #${created.number}: ${e.message}`)
  }

  return { number: created.number, url: created.html_url, state: created.state, reused: false }
}

async function performBackport({
  github,
  context,
  core,
  cwd,
  pull_request,
  targetBranch,
  backportBranch,
  mergeSha,
  options,
  requestedVia,
}) {
  const baseBranch = pull_request.base.ref

  if (!options.force) {
    const exists = await remoteBranchExists({ cwd, branch: backportBranch })
    if (exists) {
      return {
        targetBranch,
        backportBranch,
        status: 'skipped',
        message: `Branch \`${backportBranch}\` already exists (use \`--force\` to rewrite)`,
      }
    }
  }

  await git(['config', 'user.name', 'github-actions[bot]'], { cwd })
  await git(['config', 'user.email', 'github-actions[bot]@users.noreply.github.com'], { cwd })

  await git(['fetch', 'origin', targetBranch, baseBranch], { cwd })
  await git(['checkout', '-B', backportBranch, `origin/${targetBranch}`], { cwd })

  const parentCount = await getCommitParentCount({ cwd, sha: mergeSha })
  const cherryPickArgs = parentCount > 1 ? ['cherry-pick', '-m', '1', mergeSha] : ['cherry-pick', mergeSha]

  try {
    await git(cherryPickArgs, { cwd })
  } catch (e) {
    await git(['cherry-pick', '--abort'], { cwd, allowFailure: true })
    return {
      targetBranch,
      backportBranch,
      status: 'conflict',
      message: `Cherry-pick failed with conflicts for \`${targetBranch}\``,
    }
  }

  await git(['push', '--force-with-lease', 'origin', backportBranch], { cwd })

  if (options.noPr) {
    return {
      targetBranch,
      backportBranch,
      status: 'pushed',
      message: `Pushed \`${backportBranch}\` (PR creation disabled via --no-pr)`,
    }
  }

  const pr = await createOrReuseBackportPR({
    github,
    context,
    core,
    targetBranch,
    backportBranch,
    originalPR: pull_request.number,
    originalTitle: pull_request.title,
    cherryPickedSha: mergeSha,
    requestedVia,
  })

  return {
    targetBranch,
    backportBranch,
    status: 'pr',
    pr,
    message: pr.reused
      ? `Reused backport PR #${pr.number} (${pr.url})`
      : `Created backport PR #${pr.number} (${pr.url})`,
  }
}

async function handleBackportComment({ github, context, core }) {
  const payload = context.payload
  const commentBody = payload.comment?.body ?? ''
  const command = parseBackportCommand(commentBody)
  if (!command) return false

  if (!payload.issue?.pull_request) {
    core.info('Backport command ignored: not a pull request')
    return false
  }

  const association = payload.comment?.author_association
  const allowed = new Set(['OWNER', 'MEMBER', 'COLLABORATOR'])
  if (!allowed.has(String(association))) {
    core.info(`Backport command ignored: insufficient permissions (${association})`)
    return false
  }

  const prNumber = payload.issue.number
  const { data: pull_request } = await github.rest.pulls.get({
    ...context.repo,
    pull_number: prNumber,
  })

  if (!pull_request.merged) {
    await github.rest.issues.createComment({
      ...context.repo,
      issue_number: prNumber,
      body: 'Backport request ignored: PR is not merged.',
    })
    return true
  }

  const nodeId = payload.comment?.node_id
  if (nodeId) {
    try {
      await addReaction({ github, node_id: nodeId, reaction: 'EYES' })
    } catch {
      // ignore
    }
  }

  const targets = await resolveTargets({
    github,
    context,
    core,
    pull_request,
    requestedTargets: command.targets,
  })

  if (targets.length === 0) {
    await github.rest.issues.createComment({
      ...context.repo,
      issue_number: prNumber,
      body: [
        'Backport failed: no valid targets resolved.',
        '',
        'Use one of:',
        '- `@projt-launcher-bot backport latest`',
        '- `@projt-launcher-bot backport all`',
        '- `@projt-launcher-bot backport release-1.2.3`',
      ].join('\n'),
    })
    if (nodeId) {
      try {
        await addReaction({ github, node_id: nodeId, reaction: 'CONFUSED' })
      } catch {
        // ignore
      }
    }
    return true
  }

  const cwd = process.env.GITHUB_WORKSPACE || process.cwd()
  const mergeSha = pull_request.merge_commit_sha
  if (!mergeSha) {
    await github.rest.issues.createComment({
      ...context.repo,
      issue_number: prNumber,
      body: 'Backport failed: merge commit SHA is missing for this PR.',
    })
    if (nodeId) {
      try {
        await addReaction({ github, node_id: nodeId, reaction: 'CONFUSED' })
      } catch {
        // ignore
      }
    }
    return true
  }

  const results = []
  for (const targetBranch of targets) {
    const backportBranch = `backport/${targetBranch}/pr-${pull_request.number}`
    const res = await performBackport({
      github,
      context,
      core,
      cwd,
      pull_request,
      targetBranch,
      backportBranch,
      mergeSha,
      options: command.options,
      requestedVia: 'bot comment',
    })
    results.push(res)
  }

  const lines = []
  lines.push('## Backport results')
  lines.push('')
  lines.push(`Original PR: #${pull_request.number}`)
  lines.push(`Cherry-picked: \`${mergeSha}\``)
  lines.push('')
  for (const r of results) {
    if (r.status === 'pr') {
      lines.push(`- OK \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'pushed') {
      lines.push(`- OK \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'skipped') {
      lines.push(`- SKIP \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'conflict') {
      lines.push(`- FAIL \`${r.targetBranch}\`: ${r.message}`)
    } else {
      lines.push(`- WARN \`${r.targetBranch}\`: ${r.message ?? 'unknown status'}`)
    }
  }

  await github.rest.issues.createComment({
    ...context.repo,
    issue_number: prNumber,
    body: lines.join('\n'),
  })

  const anyConflict = results.some((r) => r.status === 'conflict')
  if (nodeId) {
    try {
      await addReaction({
        github,
        node_id: nodeId,
        reaction: anyConflict ? 'CONFUSED' : 'ROCKET',
      })
    } catch {
      // ignore
    }
  }

  return true
}

function getBackportLabelTargets(labels = []) {
  return labels
    .filter((l) => typeof l === 'string' && l.startsWith('backport/'))
    .map((l) => l.slice('backport/'.length))
}

function optionsFromLabels(labelTargets = []) {
  return {
    force: labelTargets.includes('force'),
    noPr: labelTargets.includes('no-pr'),
    skip: labelTargets.includes('skip'),
  }
}

async function upsertBackportSummaryComment({ github, context, pull_number, body }) {
  const marker = '<!-- projt-bot:backport-summary -->'
  const fullBody = [marker, body].join('\n')

  const comments = await github.paginate(github.rest.issues.listComments, {
    ...context.repo,
    issue_number: pull_number,
    per_page: 100,
  })

  const existing = comments.find(
    (c) => c.user?.login === 'github-actions[bot]' && typeof c.body === 'string' && c.body.includes(marker),
  )

  if (existing) {
    await github.rest.issues.updateComment({
      ...context.repo,
      comment_id: existing.id,
      body: fullBody,
    })
  } else {
    await github.rest.issues.createComment({
      ...context.repo,
      issue_number: pull_number,
      body: fullBody,
    })
  }
}

async function handleBackportOnClose({ github, context, core }) {
  const payload = context.payload
  const pr = payload.pull_request
  if (!pr) return false

  // Only act when a PR is merged and has backport/* labels.
  if (!pr.merged) return false

  const labelNames = (pr.labels ?? []).map((l) => l.name)
  const labelTargets = getBackportLabelTargets(labelNames)
  if (labelTargets.length === 0) return false

  const opts = optionsFromLabels(labelTargets)
  if (opts.skip) {
    core.info('Backport skipped via backport/skip label')
    return true
  }

  const requestedTargets = labelTargets.filter((t) => !['force', 'no-pr', 'skip'].includes(t))

  const targets = await resolveTargets({
    github,
    context,
    core,
    pull_request: pr,
    requestedTargets,
  })

  if (targets.length === 0) {
    await upsertBackportSummaryComment({
      github,
      context,
      pull_number: pr.number,
      body: [
        '## Backport results',
        '',
        'No valid targets resolved from backport labels.',
        '',
        `Labels: ${labelNames.filter((n) => n.startsWith('backport/')).join(', ')}`,
      ].join('\n'),
    })
    return true
  }

  const mergeSha = pr.merge_commit_sha
  if (!mergeSha) {
    await upsertBackportSummaryComment({
      github,
      context,
      pull_number: pr.number,
      body: 'Backport failed: merge commit SHA is missing for this PR.',
    })
    return true
  }

  const cwd = process.env.GITHUB_WORKSPACE || process.cwd()
  const results = []
  for (const targetBranch of targets) {
    const backportBranch = `backport/${targetBranch}/pr-${pr.number}`
    const res = await performBackport({
      github,
      context,
      core,
      cwd,
      pull_request: pr,
      targetBranch,
      backportBranch,
      mergeSha,
      options: { force: opts.force, noPr: opts.noPr },
      requestedVia: 'labels',
    })
    results.push(res)
  }

  const lines = []
  lines.push('## Backport results')
  lines.push('')
  lines.push(`Original PR: #${pr.number}`)
  lines.push(`Cherry-picked: \`${mergeSha}\``)
  lines.push('')
  for (const r of results) {
    if (r.status === 'pr') {
      lines.push(`- OK \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'pushed') {
      lines.push(`- OK \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'skipped') {
      lines.push(`- SKIP \`${r.targetBranch}\`: ${r.message}`)
    } else if (r.status === 'conflict') {
      lines.push(`- FAIL \`${r.targetBranch}\`: ${r.message}`)
    } else {
      lines.push(`- WARN \`${r.targetBranch}\`: ${r.message ?? 'unknown status'}`)
    }
  }

  await upsertBackportSummaryComment({
    github,
    context,
    pull_number: pr.number,
    body: lines.join('\n'),
  })

  return true
}

module.exports = {
  parseBackportCommand,
  handleBackportComment,
  handleBackportOnClose,
}
