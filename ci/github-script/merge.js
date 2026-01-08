/**
 * ProjT Launcher - Merge Handler
 * Handles PR merge operations with validation and queue management
 */

const { classify } = require('../supportedBranches.js')

// Component definitions for ProjT Launcher
const COMPONENTS = {
  core: ['launcher/', 'systeminfo/', 'katabasis/', 'libnbtplusplus/', 'launcherjava/'],
  ui: ['launcher/ui/', 'launcher/resources/', 'launcher/ui/'],
  minecraft: ['launcher/minecraft/', 'tomlplusplus/', 'qdcss/'],
  modplatform: ['launcher/modplatform/'],
  build: ['CMakeLists.txt', 'cmake/', 'vcpkg.json', 'CMakePresets.json'],
  docs: ['docs/', 'README.md', 'CONTRIBUTING.md'],
  ci: ['.github/', 'ci/'],
}

/**
 * Get component owners for changed files
 * @param {Array} files - Changed files
 * @returns {Set} Component owners
 */
function getComponentOwners(files) {
  const owners = new Set()
  
  for (const { filename } of files) {
    for (const [component, paths] of Object.entries(COMPONENTS)) {
      if (paths.some(path => filename.startsWith(path) || filename === path)) {
        owners.add(component)
      }
    }
  }
  
  return owners
}

/**
 * Run merge checklist for ProjT Launcher PRs
 */
function runChecklist({
  committers,
  events,
  files,
  pull_request,
  log,
  maintainers,
  user,
  userIsMaintainer,
}) {
  // Check what components are touched
  const components = getComponentOwners(files)
  
  // Get eligible reviewers from maintainers
  const eligible = maintainers && maintainers.length > 0
    ? new Set(maintainers)
    : new Set()
  
  // Get current approvals
  const approvals = new Set(
    events
      .filter(
        ({ event, state, commit_id }) =>
          event === 'reviewed' &&
          state === 'approved' &&
          // Only approvals for the current head SHA count
          commit_id === pull_request.head.sha,
      )
      .map(({ user }) => user?.id)
      .filter(Boolean),
  )

  const checklist = {
    'PR targets a development branch (develop, master)':
      classify(pull_request.base.ref).type.includes('development'),
    
    'PR has passing CI checks':
      pull_request.mergeable_state !== 'blocked',
    
    'PR is at least one of:': {
      'Approved by a maintainer': committers.intersection(approvals).size > 0,
      'Opened by a maintainer': committers.has(pull_request.user.id),
      'Part of a backport': 
        pull_request.head.ref.startsWith('backport-') ||
        pull_request.labels?.some(l => l.name === 'backport'),
    },
    
    'PR has no merge conflicts':
      pull_request.mergeable === true,
  }

  if (user) {
    checklist[`${user.login} is a project maintainer`] = userIsMaintainer
    if (components.size > 0) {
      checklist[`${user.login} owns touched components (${Array.from(components).join(', ')})`] =
        eligible.has(user.id)
    }
  } else {
    checklist['PR has eligible reviewers'] = eligible.size > 0
  }

  const result = Object.values(checklist).every((v) =>
    typeof v === 'boolean' ? v : Object.values(v).some(Boolean),
  )

  log('checklist', JSON.stringify(checklist))
  log('components', JSON.stringify(Array.from(components)))
  log('eligible', JSON.stringify(Array.from(eligible)))
  log('result', result)

  return {
    checklist,
    eligible,
    components,
    result,
  }
}

/**
 * Check for merge command in comment
 * Format: @projt-launcher-bot merge
 */
function hasMergeCommand(body) {
  return (body ?? '')
    .replace(/<!--.*?-->/gms, '')
    .replace(/(^`{3,})[^`].*?\1/gms, '')
    .match(/^@projt-launcher-bot\s+merge\s*$/im)
}

/**
 * Handle merge comment reaction
 */
async function handleMergeComment({ github, body, node_id, reaction }) {
  if (!hasMergeCommand(body)) return

  await github.graphql(
    `mutation($node_id: ID!, $reaction: ReactionContent!) {
      addReaction(input: {
        content: $reaction,
        subjectId: $node_id
      })
      { clientMutationId }
    }`,
    { node_id, reaction },
  )
}

/**
 * Handle merge request for a PR
 */
async function handleMerge({
  github,
  context,
  core,
  log,
  dry,
  pull_request,
  events,
  maintainers,
  getTeamMembers,
  getUser,
}) {
  const pull_number = pull_request.number

  // Get list of maintainers (project committers)
  const committers = new Set(
    (await getTeamMembers('projt-maintainers')).map(({ id }) => id),
  )

  // Get changed files
  const files = (
    await github.rest.pulls.listFiles({
      ...context.repo,
      pull_number,
      per_page: 100,
    })
  ).data

  // Early exit for large PRs
  if (files.length >= 100) {
    core.warning('PR touches 100+ files, manual merge required')
    return false
  }

  // Only look through comments after the latest push
  const lastPush = events.findLastIndex(
    ({ event, sha, commit_id }) =>
      ['committed', 'head_ref_force_pushed'].includes(event) &&
      (sha ?? commit_id) === pull_request.head.sha,
  )

  const comments = events.slice(lastPush + 1).filter(
    ({ event, body, user, node_id }) =>
      ['commented', 'reviewed'].includes(event) &&
      hasMergeCommand(body) &&
      user &&
      (dry ||
        !events.some(
          ({ event, body }) =>
            ['commented'].includes(event) &&
            body.match(new RegExp(`^<!-- comment: ${node_id} -->$`, 'm')),
        )),
  )

  /**
   * Perform the merge
   */
  async function merge() {
    if (dry) {
      core.info(`Would merge #${pull_number}... (dry run)`)
      return 'Merge completed (dry run)'
    }

    // Use merge queue if available, otherwise regular merge
    try {
      const resp = await github.graphql(
        `mutation($node_id: ID!, $sha: GitObjectID) {
          enqueuePullRequest(input: {
            expectedHeadOid: $sha,
            pullRequestId: $node_id
          })
          {
            clientMutationId,
            mergeQueueEntry { mergeQueue { url } }
          }
        }`,
        { node_id: pull_request.node_id, sha: pull_request.head.sha },
      )
      return [
        `:heavy_check_mark: [Queued](${resp.enqueuePullRequest.mergeQueueEntry.mergeQueue.url}) for merge`,
      ]
    } catch (e) {
      log('Queue merge failed, trying direct merge', e.response?.errors?.[0]?.message)
    }

    // Fallback to direct merge
    try {
      await github.rest.pulls.merge({
        ...context.repo,
        pull_number,
        merge_method: 'squash',
        sha: pull_request.head.sha,
      })
      return [':heavy_check_mark: Merged successfully']
    } catch (e) {
      return [`:x: Merge failed: ${e.message}`]
    }
  }

  // Process merge commands
  for (const comment of comments) {
    const user = await getUser(comment.user.id)
    
    const { checklist, result } = runChecklist({
      committers,
      events,
      files,
      pull_request,
      log,
      maintainers: maintainers || [],
      user,
      userIsMaintainer: committers.has(user.id),
    })

    const response = []

    if (result) {
      response.push(...(await merge()))
    } else {
      response.push(':x: Cannot merge - checklist not satisfied:')
      response.push('')
      response.push('```')
      response.push(JSON.stringify(checklist, null, 2))
      response.push('```')
    }

    if (!dry) {
      await github.rest.issues.createComment({
        ...context.repo,
        issue_number: pull_number,
        body: [
          `<!-- comment: ${comment.node_id} -->`,
          '',
          ...response,
        ].join('\n'),
      })

      await handleMergeComment({
        github,
        body: comment.body,
        node_id: comment.node_id,
        reaction: result ? 'ROCKET' : 'CONFUSED',
      })
    } else {
      core.info(`Response: ${response.join('\n')}`)
    }
  }

  return comments.length > 0
}

module.exports = {
  runChecklist,
  hasMergeCommand,
  handleMergeComment,
  handleMerge,
  getComponentOwners,
}
