/**
 * ProjT Launcher - Commit Validation for Pull Requests
 * Validates commit messages, structure, and conventions
 */

const { classify } = require('../supportedBranches.js')
const withRateLimit = require('./withRateLimit.js')
const { dismissReviews, postReview } = require('./reviews.js')

// Commit message conventions for ProjT Launcher
const COMMIT_TYPES = [
  'feat',      // New feature
  'fix',       // Bug fix
  'docs',      // Documentation
  'style',     // Formatting, no code change
  'refactor',  // Code restructuring
  'perf',      // Performance improvement
  'test',      // Adding tests
  'build',     // Build system changes
  'ci',        // CI configuration
  'chore',     // Maintenance tasks
  'revert',    // Revert changes
  'deps',      // Dependency updates
  'deps(deps)', // Dependency updates (explicit)
]

// Component scopes for ProjT Launcher
const VALID_SCOPES = [
  'core',
  'ui',
  'qml',
  'minecraft',
  'modplatform',
  'modrinth',
  'curseforge',
  'ftb',
  'technic',
  'atlauncher',
  'auth',
  'java',
  'news',
  'settings',
  'skins',
  'translations',
  'build',
  'nix',
  'vcpkg',
  'deps',
]

/**
 * Validate commit message format
 * Expected format: type(scope): description
 * @param {string} message - Commit message
 * @returns {object} Validation result
 */
function validateCommitMessage(message) {
  const firstLine = message.split('\n')[0]
  
  // Check for conventional commit format
  const conventionalMatch = firstLine.match(
    /^(?<type>\w+)(?:\((?<scope>[\w-]+)\))?!?:\s*(?<description>.+)$/
  )
  
  if (!conventionalMatch) {
    return {
      valid: false,
      severity: 'warning',
      message: `Commit message doesn't follow conventional format: "${firstLine.substring(0, 50)}..."`,
    }
  }
  
  const { type, scope, description } = conventionalMatch.groups
  
  // Validate type
  if (!COMMIT_TYPES.includes(type.toLowerCase())) {
    return {
      valid: false,
      severity: 'warning',
      message: `Unknown commit type "${type}". Valid types: ${COMMIT_TYPES.join(', ')}`,
    }
  }
  
  // Validate scope if present
  if (scope && !VALID_SCOPES.includes(scope.toLowerCase())) {
    return {
      valid: false,
      severity: 'info',
      message: `Unknown scope "${scope}". Consider using: ${VALID_SCOPES.slice(0, 5).join(', ')}...`,
    }
  }
  
  // Check description length
  if (description.length < 10) {
    return {
      valid: false,
      severity: 'warning',
      message: 'Commit description too short (min 10 chars)',
    }
  }
  
  if (firstLine.length > 72) {
    return {
      valid: false,
      severity: 'info',
      message: 'First line exceeds 72 characters',
    }
  }
  
  return { valid: true }
}

/**
 * Check commit for specific patterns
 * @param {object} commit - Commit object
 * @returns {object} Check result
 */
function checkCommitPatterns(commit) {
  const message = commit.message
  const issues = []
  
  // Check for WIP markers
  if (message.match(/\bWIP\b/i)) {
    issues.push({
      severity: 'warning',
      message: 'Commit contains WIP marker',
    })
  }
  
  // Check for fixup/squash commits
  if (message.match(/^(fixup|squash)!/i)) {
    issues.push({
      severity: 'info',
      message: 'Commit is a fixup/squash commit - remember to rebase before merge',
    })
  }
  
  // Check for merge commits
  if (message.startsWith('Merge ')) {
    issues.push({
      severity: 'info',
      message: 'Merge commit detected - consider rebasing instead',
    })
  }
  
  // Check for large descriptions without body
  if (message.split('\n').length === 1 && message.length > 100) {
    issues.push({
      severity: 'info',
      message: 'Long commit message without body - consider adding details in commit body',
    })
  }
  
  return issues
}

/**
 * Validate all commits in a PR
 */
module.exports = async ({ github, context, core, dry }) => {
  await withRateLimit({ github, core }, async (stats) => {
    stats.prs = 1

    const pull_number = context.payload.pull_request.number
    const base = context.payload.pull_request.base.ref
    const baseClassification = classify(base)

    // Get all commits in the PR
    const commits = await github.paginate(github.rest.pulls.listCommits, {
      ...context.repo,
      pull_number,
    })

    core.info(`Validating ${commits.length} commits for PR #${pull_number}`)

    const results = []

    for (const { sha, commit } of commits) {
      const commitResults = {
        sha: sha.substring(0, 7),
        fullSha: sha,
        author: commit.author.name,
        message: commit.message.split('\n')[0],
        issues: [],
      }

      // Validate commit message format
      const formatValidation = validateCommitMessage(commit.message)
      if (!formatValidation.valid) {
        commitResults.issues.push({
          severity: formatValidation.severity,
          message: formatValidation.message,
        })
      }

      // Check for commit patterns
      const patternIssues = checkCommitPatterns(commit)
      commitResults.issues.push(...patternIssues)

      results.push(commitResults)
    }

    // Log results
    let hasErrors = false
    let hasWarnings = false

    for (const result of results) {
      core.startGroup(`Commit ${result.sha}`)
      core.info(`Author: ${result.author}`)
      core.info(`Message: ${result.message}`)
      
      if (result.issues.length === 0) {
        core.info('✓ No issues found')
      } else {
        for (const issue of result.issues) {
          switch (issue.severity) {
            case 'error':
              core.error(issue.message)
              hasErrors = true
              break
            case 'warning':
              core.warning(issue.message)
              hasWarnings = true
              break
            default:
              core.info(`ℹ ${issue.message}`)
          }
        }
      }
      core.endGroup()
    }

    // If all commits are valid, dismiss any previous reviews
    if (!hasErrors && !hasWarnings) {
      await dismissReviews({ github, context, dry })
      core.info('✓ All commits passed validation')
      return
    }

    // Generate summary for issues
    const issueCommits = results.filter(r => r.issues.length > 0)
    
    if (issueCommits.length > 0) {
      const body = [
        '## Commit Validation Issues',
        '',
        'The following commits have issues that should be addressed:',
        '',
        ...issueCommits.flatMap(commit => [
          `### \`${commit.sha}\`: ${commit.message}`,
          '',
          ...commit.issues.map(issue => `- **${issue.severity}**: ${issue.message}`),
          '',
        ]),
        '---',
        '',
        '### Commit Message Guidelines',
        '',
        'ProjT Launcher uses [Conventional Commits](https://www.conventionalcommits.org/):',
        '',
        '```',
        'type(scope): description',
        '',
        '[optional body]',
        '',
        '[optional footer]',
        '```',
        '',
        `**Types**: ${COMMIT_TYPES.join(', ')}`,
        '',
        `**Scopes**: ${VALID_SCOPES.slice(0, 8).join(', ')}, ...`,
      ].join('\n')

      // Post review only for errors/warnings, not info
      if (hasErrors || hasWarnings) {
        await postReview({ github, context, core, dry, body })
      }

      // Write step summary
      const fs = require('node:fs')
      if (process.env.GITHUB_STEP_SUMMARY) {
        fs.appendFileSync(process.env.GITHUB_STEP_SUMMARY, body)
      }
    }

    if (hasErrors) {
      throw new Error('Commit validation failed with errors')
    }
  })
}
