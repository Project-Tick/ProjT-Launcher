/**
 * ProjT Launcher - PR Preparation Script
 * Validates PR structure and prepares merge information
 */

const { classify } = require('../supportedBranches.js')
const { postReview } = require('./reviews.js')

const SIGNOFF_MARKER = '<!-- bot:missing-signed-off-by -->'

function stripNoise(body = '') {
  return String(body)
    .replace(/\r/g, '')
    .replace(/<!--.*?-->/gms, '')
    .replace(/(^`{3,})[^`].*?\1/gms, '')
}

function hasSignedOffBy(body = '') {
  const cleaned = stripNoise(body)
  return /^signed-off-by:\s+.+<[^<>]+>\s*$/im.test(cleaned)
}

async function dismissSignoffReviews({ github, context, pull_number }) {
  const reviews = await github.paginate(github.rest.pulls.listReviews, {
    ...context.repo,
    pull_number,
  })

  const signoffReviews = reviews.filter(
    (r) =>
      r.user?.login === 'github-actions[bot]' &&
      r.state === 'CHANGES_REQUESTED' &&
      typeof r.body === 'string' &&
      r.body.includes(SIGNOFF_MARKER),
  )

  for (const review of signoffReviews) {
    await github.rest.pulls.dismissReview({
      ...context.repo,
      pull_number,
      review_id: review.id,
      message: 'Signed-off-by found, thank you!',
    })
  }
}

/**
 * Main PR preparation function
 * Validates that the PR targets the correct branch and can be merged
 */
module.exports = async ({ github, context, core, dry }) => {
  const payload = context.payload || {}
  const pull_number =
    payload?.pull_request?.number ??
    (Array.isArray(payload?.merge_group?.pull_requests) &&
      payload.merge_group.pull_requests[0]?.number)

  if (typeof pull_number !== 'number') {
    core.info('No pull request found on this event; skipping prepare step.')
    return { ok: true, skipped: true, reason: 'no-pull-request' }
  }

  // Wait for GitHub to compute merge status
  for (const retryInterval of [5, 10, 20, 40]) {
    core.info('Checking whether the pull request can be merged...')
    const prInfo = (
      await github.rest.pulls.get({
        ...context.repo,
        pull_number,
      })
    ).data

    if (prInfo.state !== 'open') {
      throw new Error('PR is not open anymore.')
    }

    if (prInfo.mergeable == null) {
      core.info(
        `GitHub is still computing merge status, waiting ${retryInterval} seconds...`,
      )
      await new Promise((resolve) => setTimeout(resolve, retryInterval * 1000))
      continue
    }

    const { base, head, user } = prInfo

    const authorLogin = user?.login ?? ''
    const isBotAuthor =
      (user?.type ?? '').toLowerCase() === 'bot' || /\[bot\]$/i.test(authorLogin)

    // Enforce PR template sign-off (Signed-off-by: Name <email>)
    if (isBotAuthor) {
      core.info(`Skipping Signed-off-by requirement for bot author: ${authorLogin}`)
      if (!dry) {
        await dismissSignoffReviews({ github, context, pull_number })
      }
    } else if (!hasSignedOffBy(prInfo.body)) {
      const body = [
        SIGNOFF_MARKER,
        '',
        '## Missing Signed-off-by',
        '',
        'This repository requires a DCO-style sign-off line in the PR description.',
        '',
        'Add a line like this to the PR description (under “Signed-off-by”):',
        '',
        '```',
        'Signed-off-by: Your Name <you@example.com>',
        '```',
        '',
        'After updating the PR description, this check will re-run automatically.',
      ].join('\n')

      await postReview({ github, context, core, dry, body })
      throw new Error('Missing Signed-off-by in PR description')
    } else if (!dry) {
      await dismissSignoffReviews({ github, context, pull_number })
    }

    // Classify base branch
    const baseClassification = classify(base.ref)
    core.setOutput('base', baseClassification)
    core.info(`Base branch classification: ${JSON.stringify(baseClassification)}`)

    // Classify head branch
    const headClassification =
      base.repo.full_name === head.repo.full_name
        ? classify(head.ref)
        : { type: ['wip'] } // PRs from forks are WIP
    core.setOutput('head', headClassification)
    core.info(`Head branch classification: ${JSON.stringify(headClassification)}`)

    // Validate base branch targeting
    if (!baseClassification.type.includes('development') && 
        !baseClassification.type.includes('release')) {
      const body = [
        '## Invalid Target Branch',
        '',
        `This PR targets \`${base.ref}\`, which is not a valid target branch.`,
        '',
        '### Valid target branches for ProjT Launcher:',
        '',
        '| Branch | Purpose |',
        '|--------|---------|',
        '| `develop` | Main development branch |',
        '| `master` / `main` | Stable branch |',
        '| `release-X.Y.Z` | Release branches |',
        '',
        'Please [change the base branch](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/changing-the-base-branch-of-a-pull-request) to the appropriate target.',
      ].join('\n')

      await postReview({ github, context, core, dry, body })
      throw new Error('PR targets invalid branch.')
    }

    // Check for release branch targeting from wrong branch
    if (baseClassification.isRelease) {
      // For release branches, typically only hotfixes and backports should target them
      const isBackport = head.ref.startsWith('backport-')
      const isHotfix = head.ref.startsWith('hotfix-') || head.ref.startsWith('hotfix/')
      
      if (!isBackport && !isHotfix && headClassification.type.includes('wip')) {
        const body = [
          '## Release Branch Warning',
          '',
          `This PR targets the release branch \`${base.ref}\`.`,
          '',
          'Release branches should only receive:',
          '- **Backports** from the development branch',
          '- **Hotfixes** for critical bugs',
          '',
          'If this is a regular feature/fix, please target `develop` instead.',
          '',
          'If this is intentionally a hotfix, consider naming your branch `hotfix/description`.',
        ].join('\n')

        await postReview({ github, context, core, dry, body })
        // This is a warning, not an error
        core.warning('PR targets release branch from non-hotfix/backport branch')
      }
    }

    // Validate feature branches target develop
    if (headClassification.isFeature && 
        !['develop'].includes(base.ref)) {
      const body = [
        '## Feature Branch Target',
        '',
        `Feature branches should typically target \`develop\`, not \`${base.ref}\`.`,
        '',
        'Please verify this is the correct target branch.',
      ].join('\n')

      core.warning(body)
      // Don't block, just warn
    }

    // Process merge state
    let mergedSha, targetSha

    if (prInfo.mergeable) {
      core.info('✓ PR can be merged.')

      mergedSha = prInfo.merge_commit_sha
      targetSha = (
        await github.rest.repos.getCommit({
          ...context.repo,
          ref: prInfo.merge_commit_sha,
        })
      ).data.parents[0].sha
    } else {
      core.warning('⚠ PR has merge conflicts.')

      mergedSha = head.sha
      targetSha = (
        await github.rest.repos.compareCommitsWithBasehead({
          ...context.repo,
          basehead: `${base.sha}...${head.sha}`,
        })
      ).data.merge_base_commit.sha
    }

    // Set outputs for downstream jobs
    core.setOutput('mergedSha', mergedSha)
    core.setOutput('targetSha', targetSha)
    core.setOutput('mergeable', prInfo.mergeable)
    core.setOutput('headSha', head.sha)
    core.setOutput('baseSha', base.sha)

    // Get changed files for analysis
    const files = await github.paginate(github.rest.pulls.listFiles, {
      ...context.repo,
      pull_number,
      per_page: 100,
    })

    // Categorize changes
    const categories = {
      source: files.filter(f => 
        f.filename.startsWith('launcher/')
      ).length,
      ui: files.filter(f => 
        f.filename.includes('/ui/')
      ).length,
      build: files.filter(f => 
        f.filename.includes('CMake') || 
        f.filename.includes('vcpkg') ||
        f.filename.endsWith('.cmake')
      ).length,
      ci: files.filter(f => 
        f.filename.startsWith('.github/') || 
        f.filename.startsWith('ci/')
      ).length,
      docs: files.filter(f => 
        f.filename.startsWith('docs/') || 
        f.filename.endsWith('.md')
      ).length,
      translations: files.filter(f => 
        f.filename.includes('translations/')
      ).length,
    }

    core.info(`Changes summary:`)
    core.info(`  Source files: ${categories.source}`)
    core.info(`  UI files: ${categories.ui}`)
    core.info(`  Build files: ${categories.build}`)
    core.info(`  CI files: ${categories.ci}`)
    core.info(`  Documentation: ${categories.docs}`)
    core.info(`  Translations: ${categories.translations}`)

    core.setOutput('categories', JSON.stringify(categories))
    core.setOutput('totalFiles', files.length)

    // Write step summary
    if (process.env.GITHUB_STEP_SUMMARY) {
      const fs = require('node:fs')
      const summary = [
        '## PR Preparation Summary',
        '',
        `| Property | Value |`,
        `|----------|-------|`,
        `| PR Number | #${pull_number} |`,
        `| Base Branch | \`${base.ref}\` |`,
        `| Head Branch | \`${head.ref}\` |`,
        `| Mergeable | ${prInfo.mergeable ? '✅ Yes' : '❌ No'} |`,
        `| Total Files | ${files.length} |`,
        '',
        '### Change Categories',
        '',
        `| Category | Files |`,
        `|----------|-------|`,
        ...Object.entries(categories).map(([cat, count]) => 
          `| ${cat.charAt(0).toUpperCase() + cat.slice(1)} | ${count} |`
        ),
      ].join('\n')

      fs.appendFileSync(process.env.GITHUB_STEP_SUMMARY, summary)
    }

    return {
      mergeable: prInfo.mergeable,
      mergedSha,
      targetSha,
      headSha: head.sha,
      baseSha: base.sha,
      base: baseClassification,
      head: headClassification,
      files: files.length,
      categories,
    }
  }

  throw new Error('Timeout waiting for merge status computation')
}
