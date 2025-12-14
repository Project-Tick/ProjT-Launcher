// =============================================================================
// ProjT Launcher - GitHub Actions Bot
// =============================================================================
// Main bot logic for PR automation in ProjT Launcher CI.
// Handles labeling, review requests, and PR validation.
//
// Usage: Called from GitHub Actions workflows
// =============================================================================

module.exports = async ({ github, context, core, dry }) => {
  const { readFile } = require('node:fs/promises');
  const withRateLimit = require('./withRateLimit.js');
  const { classify } = require('../supportedBranches.js');

  // =============================================================================
  // Configuration
  // =============================================================================
  
  const config = {
    // Labels for different types of changes
    labels: {
      core: '11.area:core',
      ui: '11.area:ui',
      minecraft: '11.area:minecraft',
      modplatform: '11.area:modplatform',
      build: '11.area:build',
      ci: '11.area:ci',
      documentation: '11.area:docs',
      translations: '11.area:translations',
    },
    
    // Size labels based on lines changed
    sizeLabels: {
      xs: { max: 10, label: '12.size:xs' },
      s: { max: 50, label: '12.size:s' },
      m: { max: 200, label: '12.size:m' },
      l: { max: 500, label: '12.size:l' },
      xl: { max: Infinity, label: '12.size:xl' },
    },
    
    // Platform labels
    platformLabels: {
      linux: '13.platform:linux',
      macos: '13.platform:macos',
      windows: '13.platform:windows',
    },
  };

  // =============================================================================
  // Helper Functions
  // =============================================================================

  /**
   * Get the area category from a file path
   */
  function getAreaFromPath(filePath) {
    if (filePath.startsWith('launcher/ui/') || filePath.startsWith('launcher/qtquick/')) {
      return 'ui';
    }
    if (filePath.startsWith('launcher/minecraft/')) {
      return 'minecraft';
    }
    if (filePath.startsWith('launcher/modplatform/')) {
      return 'modplatform';
    }
    if (filePath.startsWith('launcher/')) {
      return 'core';
    }
    if (filePath.startsWith('libraries/')) {
      return 'core';
    }
    if (filePath.startsWith('cmake/') || filePath.endsWith('CMakeLists.txt')) {
      return 'build';
    }
    if (filePath.startsWith('.github/') || filePath.startsWith('ci/')) {
      return 'ci';
    }
    if (filePath.startsWith('docs/') || filePath.endsWith('.md')) {
      return 'documentation';
    }
    if (filePath.startsWith('translations/')) {
      return 'translations';
    }
    return null;
  }

  /**
   * Get platform from file path
   */
  function getPlatformFromPath(filePath) {
    const lowerPath = filePath.toLowerCase();
    if (lowerPath.includes('linux') || lowerPath.includes('unix')) {
      return 'linux';
    }
    if (lowerPath.includes('darwin') || lowerPath.includes('macos') || lowerPath.includes('apple')) {
      return 'macos';
    }
    if (lowerPath.includes('windows') || lowerPath.includes('win32') || lowerPath.includes('msvc')) {
      return 'windows';
    }
    return null;
  }

  /**
   * Get size label based on lines changed
   */
  function getSizeLabel(additions, deletions) {
    const total = additions + deletions;
    for (const [size, { max, label }] of Object.entries(config.sizeLabels)) {
      if (total <= max) {
        return label;
      }
    }
    return config.sizeLabels.xl.label;
  }

  /**
   * Log helper with PR context
   */
  function log(prNumber, key, value) {
    core.info(`PR #${prNumber} - ${key}: ${value}`);
  }

  // =============================================================================
  // Main PR Handler
  // =============================================================================

  async function handlePullRequest({ item, stats }) {
    const pullNumber = item.number;
    log(pullNumber, 'Processing', 'started');

    try {
      // Get PR details
      const { data: pullRequest } = await github.rest.pulls.get({
        ...context.repo,
        pull_number: pullNumber,
      });

      log(pullNumber, 'Author', pullRequest.user?.login || 'unknown');
      log(pullNumber, 'Base', pullRequest.base.ref);
      log(pullNumber, 'Head', pullRequest.head.ref);

      // Get changed files
      const files = await github.paginate(github.rest.pulls.listFiles, {
        ...context.repo,
        pull_number: pullNumber,
      });

      log(pullNumber, 'Files changed', files.length);

      // Determine labels to add
      const labelsToAdd = new Set();
      const areas = new Set();
      const platforms = new Set();

      for (const file of files) {
        const area = getAreaFromPath(file.filename);
        if (area) {
          areas.add(area);
          if (config.labels[area]) {
            labelsToAdd.add(config.labels[area]);
          }
        }

        const platform = getPlatformFromPath(file.filename);
        if (platform) {
          platforms.add(platform);
          if (config.platformLabels[platform]) {
            labelsToAdd.add(config.platformLabels[platform]);
          }
        }
      }

      // Add size label
      const sizeLabel = getSizeLabel(pullRequest.additions, pullRequest.deletions);
      labelsToAdd.add(sizeLabel);

      // Add branch type label
      const branchType = classify(pullRequest.head.ref);
      if (Array.isArray(branchType.type)) {
        for (const t of branchType.type) {
          labelsToAdd.add(`branch:${t}`);
        }
      } else if (branchType.type) {
        labelsToAdd.add(`branch:${branchType.type}`);
      }

      // Check for merge conflicts
      if (pullRequest.mergeable === false) {
        labelsToAdd.add('status:merge-conflict');
      }

      // Get current labels
      const currentLabels = pullRequest.labels.map(l => l.name);
      
      // Determine which labels to actually add (not already present)
      const newLabels = [...labelsToAdd].filter(l => !currentLabels.includes(l));

      if (newLabels.length > 0) {
        log(pullNumber, 'Adding labels', newLabels.join(', '));
        
        if (!dry) {
          await github.rest.issues.addLabels({
            ...context.repo,
            issue_number: pullNumber,
            labels: newLabels,
          });
        }
      }

      // Update stats
      stats.processed++;
      stats.areas = [...new Set([...stats.areas || [], ...areas])];
      stats.platforms = [...new Set([...stats.platforms || [], ...platforms])];

      log(pullNumber, 'Processing', 'complete');
      
      return {
        success: true,
        labels: [...labelsToAdd],
        areas: [...areas],
        platforms: [...platforms],
      };

    } catch (error) {
      core.error(`PR #${pullNumber} - Error: ${error.message}`);
      stats.errors++;
      return {
        success: false,
        error: error.message,
      };
    }
  }

  // =============================================================================
  // Batch Processing
  // =============================================================================

  async function processPullRequests(pullRequests) {
    const stats = {
      total: pullRequests.length,
      processed: 0,
      errors: 0,
      areas: [],
      platforms: [],
    };

    core.info(`Processing ${stats.total} pull request(s)...`);

    const results = [];
    for (const item of pullRequests) {
      const result = await handlePullRequest({ item, stats });
      results.push({ number: item.number, ...result });
    }

    core.info(`\n=== Summary ===`);
    core.info(`Total: ${stats.total}`);
    core.info(`Processed: ${stats.processed}`);
    core.info(`Errors: ${stats.errors}`);
    core.info(`Areas affected: ${stats.areas.join(', ') || 'none'}`);
    core.info(`Platforms affected: ${stats.platforms.join(', ') || 'none'}`);

    return { stats, results };
  }

  // =============================================================================
  // Entry Point
  // =============================================================================

  // Determine what to process based on context
  if (context.eventName === 'pull_request' || context.eventName === 'pull_request_target') {
    // Single PR from event
    const pullRequest = context.payload.pull_request;
    return handlePullRequest({
      item: { number: pullRequest.number },
      stats: { processed: 0, errors: 0 },
    });
  } else if (context.eventName === 'schedule' || context.eventName === 'workflow_dispatch') {
    // Batch processing - get open PRs
    const { data: pullRequests } = await github.rest.pulls.list({
      ...context.repo,
      state: 'open',
      per_page: 100,
    });
    return processPullRequests(pullRequests);
  } else {
    core.warning(`Unsupported event: ${context.eventName}`);
    return { success: false, error: 'Unsupported event' };
  }
};
