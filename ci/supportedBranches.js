#!/usr/bin/env node
/**
 * ProjT Launcher Branch Classifier
 * Classifies branches for CI/CD purposes
 */

// Branch type configuration for ProjT Launcher
const typeConfig = {
  develop: ['development', 'primary'],
  master: ['development', 'primary'],
  main: ['development', 'primary'],
  release: ['release', 'primary'],
  feature: ['development', 'feature'],
  bugfix: ['development', 'bugfix'],
  hotfix: ['release', 'hotfix'],
}

// Order ranks branches by priority for base branch selection
const orderConfig = {
  develop: 0,
  master: 1,
  main: 1,
  release: 2,
  feature: 4,
  bugfix: 4,
  hotfix: 5,
}

/**
 * Split branch name into components
 * @param {string} branch - Branch name
 * @returns {object} Parsed branch components
 */
function split(branch) {
  const match = branch.match(
    /(?<prefix>[a-z_-]+?)(-(?<version>\d+\.\d+\.\d+|v?\d+\.\d+))?(-(?<suffix>.*))?$/i
  )
  return match ? match.groups : { prefix: branch }
}

/**
 * Classify a branch for CI purposes
 * @param {string} branch - Branch name
 * @returns {object} Branch classification
 */
function classify(branch) {
  const { prefix, version, suffix } = split(branch)
  const normalizedPrefix = prefix.toLowerCase().replace(/-/g, '_')
  
  return {
    branch,
    order: orderConfig[normalizedPrefix] ?? orderConfig[prefix.split('/')[0]] ?? Infinity,
    isRelease: prefix.startsWith('release') || prefix.startsWith('hotfix'),
    type: typeConfig[normalizedPrefix] ?? typeConfig[prefix.split('/')[0]] ?? ['wip'],
    version: version ?? null,
    suffix: suffix ?? null,
  }
}

/**
 * Check if branch should trigger full CI
 * @param {string} branch - Branch name
 * @returns {boolean}
 */
function shouldRunFullCI(branch) {
  const { type } = classify(branch)
  return type.includes('primary') || type.includes('release')
}

/**
 * Get target branch for backport
 * @param {string} branch - Branch name
 * @returns {string|null}
 */
function getBackportTarget(branch) {
  const { isRelease, version } = classify(branch)
  if (isRelease && version) {
    return `release-${version}`
  }
  return null
}

module.exports = { classify, split, shouldRunFullCI, getBackportTarget }

// CLI tests when run directly
if (require.main === module) {
  console.log('ProjT Launcher Branch Classifier Tests\n')
  
  console.log('split(branch):')
  const testSplits = ['develop', 'release-1.0.0', 'feature/new-ui', 'hotfix-1.0.1']
  testSplits.forEach(b => console.log(`  ${b}:`, split(b)))
  
  console.log('\nclassify(branch):')
  const testClassify = ['develop', 'master', 'release-1.0.0', 'feature/settings', 'bugfix/crash-fix']
  testClassify.forEach(b => console.log(`  ${b}:`, classify(b)))
  
  console.log('\nshouldRunFullCI(branch):')
  testClassify.forEach(b => console.log(`  ${b}: ${shouldRunFullCI(b)}`))
}
