/**
 * ProjT Launcher - Reviewer Assignment
 * Automatically assigns reviewers based on changed files and CODEOWNERS
 */

const fs = require('node:fs')
const path = require('node:path')

function extractMaintainersBlock(source) {
  const token = 'maintainers ='
  const start = source.indexOf(token)
  if (start === -1) {
    return ''
  }

  const braceStart = source.indexOf('{', start)
  if (braceStart === -1) {
    return ''
  }

  let depth = 0
  for (let i = braceStart; i < source.length; i += 1) {
    const char = source[i]
    if (char === '{') {
      depth += 1
    } else if (char === '}') {
      depth -= 1
      if (depth === 0) {
        return source.slice(braceStart, i + 1)
      }
    }
  }
  return ''
}

function parseAreas(areaBlock) {
  const matches = areaBlock.match(/"([^"]+)"/g) || []
  return matches.map(entry => entry.replace(/"/g, ''))
}

function loadMaintainersFromNix() {
  const maintainersPath = path.join(__dirname, '..', 'eval', 'compare', 'maintainers.nix')
  try {
    const source = fs.readFileSync(maintainersPath, 'utf8')
    const block = extractMaintainersBlock(source)
    if (!block) {
      return []
    }

    const entryRegex = /(\w+)\s*=\s*{([\s\S]*?)\n\s*};/g
    const maintainers = []
    let match
    while ((match = entryRegex.exec(block)) !== null) {
      const [, , body] = match
      const githubMatch = body.match(/github\s*=\s*"([^"]+)"/)
      if (!githubMatch) continue
      const areasMatch = body.match(/areas\s*=\s*\[([\s\S]*?)\]/)
      const areas = areasMatch ? parseAreas(areasMatch[1]) : []
      maintainers.push({
        handle: githubMatch[1],
        areas,
      })
    }
    return maintainers
  } catch (error) {
    console.warn(`Could not read maintainers from maintainers.nix: ${error.message}`)
    return []
  }
}

const FALLBACK_MAINTAINERS = [
  {
    handle: 'YongDo-Hyun',
    areas: ['all'],
  },
  {
    handle: 'grxtor',
    areas: ['all'],
  },
]

const MAINTAINERS = (() => {
  const parsed = loadMaintainersFromNix()
  return parsed.length > 0 ? parsed : FALLBACK_MAINTAINERS
})()

// File patterns to components mapping
const FILE_PATTERNS = [
  { pattern: /^launcher\/ui\//, component: 'ui' },
  { pattern: /^launcher\/minecraft\//, component: 'minecraft' },
  { pattern: /^launcher\/modplatform\//, component: 'modplatform' },
  { pattern: /^launcher\//, component: 'core' },
  { pattern: /^libraries\//, component: 'core' },
  { pattern: /^\.github\//, component: 'ci' },
  { pattern: /^ci\//, component: 'ci' },
  { pattern: /CMakeLists\.txt$/, component: 'build' },
  { pattern: /\.cmake$/, component: 'build' },
  { pattern: /vcpkg/, component: 'build' },
  { pattern: /^docs\//, component: 'docs' },
  { pattern: /\.md$/, component: 'docs' },
  { pattern: /translations\//, component: 'translations' },
]

const COMPONENTS = Array.from(new Set(FILE_PATTERNS.map(({ component }) => component)))

const getMaintainersForComponent = component => {
  const assigned = MAINTAINERS.filter(
    maintainer =>
      maintainer.areas.includes(component) || maintainer.areas.includes('all')
  ).map(maintainer => maintainer.handle)

  return assigned.length > 0 ? assigned : MAINTAINERS.map(maintainer => maintainer.handle)
}

// Component to reviewer mapping for ProjT Launcher
const COMPONENT_REVIEWERS = Object.fromEntries(
  COMPONENTS.map(component => [component, getMaintainersForComponent(component)])
)

/**
 * Get components affected by file changes
 * @param {Array} files - List of changed files
 * @returns {Set} Affected components
 */
function getAffectedComponents(files) {
  const components = new Set()
  
  for (const file of files) {
    const filename = file.filename || file
    for (const { pattern, component } of FILE_PATTERNS) {
      if (pattern.test(filename)) {
        components.add(component)
        break
      }
    }
  }
  
  return components
}

/**
 * Get reviewers for components
 * @param {Set} components - Affected components
 * @returns {Set} Reviewers
 */
function getReviewersForComponents(components) {
  const reviewers = new Set()
  
  for (const component of components) {
    const componentReviewers = COMPONENT_REVIEWERS[component] || []
    for (const reviewer of componentReviewers) {
      reviewers.add(reviewer.toLowerCase())
    }
  }
  
  return reviewers
}

/**
 * Handle reviewer assignment for a PR
 */
async function handleReviewers({
  github,
  context,
  core,
  log,
  dry,
  pull_request,
  reviews,
  maintainers,
  owners,
  getTeamMembers,
  getUser,
}) {
  const pull_number = pull_request.number

  // Get currently requested reviewers
  const requested_reviewers = new Set(
    pull_request.requested_reviewers.map(({ login }) => login.toLowerCase()),
  )
  log?.(
    'reviewers - requested_reviewers',
    Array.from(requested_reviewers).join(', '),
  )

  // Get existing reviewers (already reviewed)
  const existing_reviewers = new Set(
    reviews.map(({ user }) => user?.login.toLowerCase()).filter(Boolean),
  )
  log?.(
    'reviewers - existing_reviewers',
    Array.from(existing_reviewers).join(', '),
  )

  // Guard against too many reviewers from large PRs
  if (maintainers && maintainers.length > 16) {
    core.warning('Too many potential reviewers, skipping automatic assignment.')
    return existing_reviewers.size === 0 && requested_reviewers.size === 0
  }

  // Build list of potential reviewers
  const users = new Set()
  
  // Add maintainers
  if (maintainers) {
    for (const id of maintainers) {
      try {
        const user = await getUser(id)
        users.add(user.login.toLowerCase())
      } catch (e) {
        core.warning(`Could not resolve user ID ${id}`)
      }
    }
  }
  
  // Add owners (from CODEOWNERS)
  if (owners) {
    for (const handle of owners) {
      if (handle && !handle.includes('/')) {
        users.add(handle.toLowerCase())
      }
    }
  }
  
  log?.('reviewers - users', Array.from(users).join(', '))

  // Handle team-based owners
  const teams = new Set()
  if (owners) {
    for (const handle of owners) {
      const parts = handle.split('/')
      if (parts.length === 2 && parts[0] === context.repo.owner) {
        teams.add(parts[1])
      }
    }
  }
  log?.('reviewers - teams', Array.from(teams).join(', '))

  // Get team members
  const team_members = new Set()
  if (teams.size > 0 && getTeamMembers) {
    for (const team of teams) {
      try {
        const members = await getTeamMembers(team)
        for (const member of members) {
          team_members.add(member.login.toLowerCase())
        }
      } catch (e) {
        core.warning(`Could not fetch team ${team}`)
      }
    }
  }
  log?.('reviewers - team_members', Array.from(team_members).join(', '))

  // Combine all potential reviewers
  const all_reviewers = new Set([...users, ...team_members])
  
  // Remove PR author - can't review own PR
  const author = pull_request.user?.login.toLowerCase()
  all_reviewers.delete(author)
  
  log?.('reviewers - all_reviewers', Array.from(all_reviewers).join(', '))

  // Filter to collaborators only
  const reviewers = []
  for (const username of all_reviewers) {
    try {
      await github.rest.repos.checkCollaborator({
        ...context.repo,
        username,
      })
      reviewers.push(username)
    } catch (e) {
      if (e.status !== 404) throw e
      core.warning(
        `User ${username} cannot be requested for review (not a collaborator)`,
      )
    }
  }
  log?.('reviewers - filtered_reviewers', reviewers.join(', '))

  // Limit reviewers
  if (reviewers.length > 10) {
    core.warning(`Too many reviewers (${reviewers.length}), limiting to 10`)
    reviewers.length = 10
  }

  // Determine who needs to be requested
  const new_reviewers = new Set(reviewers)
    .difference(requested_reviewers)
    .difference(existing_reviewers)
  
  log?.(
    'reviewers - new_reviewers',
    Array.from(new_reviewers).join(', '),
  )

  if (new_reviewers.size === 0) {
    log?.('Has reviewer changes', 'false (no new reviewers)')
  } else if (dry) {
    core.info(
      `Would request reviewers for #${pull_number}: ${Array.from(new_reviewers).join(', ')} (dry run)`,
    )
  } else {
    await github.rest.pulls.requestReviewers({
      ...context.repo,
      pull_number,
      reviewers: Array.from(new_reviewers),
    })
    core.info(
      `Requested reviewers for #${pull_number}: ${Array.from(new_reviewers).join(', ')}`,
    )
  }

  // Return whether "needs-reviewers" label should be set
  return (
    new_reviewers.size === 0 &&
    existing_reviewers.size === 0 &&
    requested_reviewers.size === 0
  )
}

module.exports = {
  handleReviewers,
  getAffectedComponents,
  getReviewersForComponents,
  COMPONENT_REVIEWERS,
  FILE_PATTERNS,
}
