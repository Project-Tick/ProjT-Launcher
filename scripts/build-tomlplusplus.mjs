#!/usr/bin/env node
/**
 * Script to build toml++ documentation using Poxy
 * This generates HTML documentation from the Doxygen-formatted markdown files
 */

import { execSync } from 'child_process';
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'fs';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const projectRoot = join(__dirname, '..');

const colors = {
  reset: '\x1b[0m',
  green: '\x1b[32m',
  yellow: '\x1b[33m',
  red: '\x1b[31m',
  blue: '\x1b[34m',
};

function log(message, color = colors.reset) {
  console.log(`${color}${message}${colors.reset}`);
}

function commandExists(command) {
  try {
    execSync(`which ${command}`, { stdio: 'ignore' });
    return true;
  } catch {
    return false;
  }
}

function syncFeaturesToMainPage() {
  log('\n📋 Syncing features from handbook...', colors.blue);
  
  const handbookPath = join(projectRoot, 'docs', 'handbook', 'tomlplusplus.md');
  const mainPagePath = join(projectRoot, 'website', 'tomlplusplus', 'pages', 'main_page.md');
  
  if (!existsSync(handbookPath) || !existsSync(mainPagePath)) {
    log('⚠️  Files not found, skipping sync', colors.yellow);
    return;
  }
  
  try {
    const handbook = readFileSync(handbookPath, 'utf-8');
    const mainPage = readFileSync(mainPagePath, 'utf-8');
    
    // Extract Library features section from handbook
    const featuresMatch = handbook.match(/## Library features\s*\n\n([\s\S]*?)(?=\n\n##|$)/);
    if (!featuresMatch) {
      log('⚠️  Could not find Library features in handbook', colors.yellow);
      return;
    }
    
    const handbookFeatures = featuresMatch[1].trim();
    
    // Convert to Doxygen list format
    const doxygenFeatures = handbookFeatures
      .split('\n')
      .map(line => line.replace(/^- /, '-   '))
      .join('\n');
    
    // Replace features in main_page.md
    const updatedMainPage = mainPage.replace(
      /@section mainpage-features Features\s*\n\n([\s\S]*?)(?=\n<!-- -+|$)/,
      `@section mainpage-features Features\n\n${doxygenFeatures}\n`
    );
    
    if (updatedMainPage !== mainPage) {
      writeFileSync(mainPagePath, updatedMainPage, 'utf-8');
      log('✅ Updated features in main_page.md from handbook', colors.green);
    } else {
      log('ℹ️  Features are already in sync', colors.blue);
    }
  } catch (error) {
    log(`❌ Sync failed: ${error.message}`, colors.red);
  }
}

async function buildTomlPlusPlusDocs() {
  log('\n🔨 Building toml++ documentation...', colors.blue);
  
  // Sync only the features list from handbook to main_page
  // This keeps main_page.md's detailed API docs while updating basic info
  syncFeaturesToMainPage();
  
  // Check if dependencies exist
  if (!commandExists('doxygen')) {
    log('❌ Doxygen is not installed!', colors.red);
    log('Run: npm run setup:doxygen', colors.yellow);
    process.exit(1);
  }
  
  // Check for poxy in common locations
  const poxyPaths = [
    'poxy',
    `${process.env.HOME}/.local/bin/poxy`,
  ];
  
  let poxyCommand = null;
  for (const path of poxyPaths) {
    if (commandExists(path)) {
      poxyCommand = path;
      break;
    }
  }
  
  if (!poxyCommand) {
    log('❌ Poxy is not installed or not in PATH!', colors.red);
    log('Run: npm run setup:doxygen', colors.yellow);
    log('Or add to PATH: export PATH="$HOME/.local/bin:$PATH"', colors.yellow);
    process.exit(1);
  }
  
  const tomlplusplusDir = join(projectRoot, 'website', 'tomlplusplus');
  
  if (!existsSync(tomlplusplusDir)) {
    log(`❌ toml++ source directory not found: ${tomlplusplusDir}`, colors.red);
    process.exit(1);
  }
  
  try {
    log('📝 Running Poxy to generate documentation...', colors.yellow);
    
    // Change to tomlplusplus directory and run poxy
    process.chdir(tomlplusplusDir);
    
    // Run poxy - it will use the poxy.toml config file
    // Fix PATH to use Homebrew Python instead of Nix Python (if present)
    const fixedPath = `/opt/homebrew/bin:${process.env.HOME}/.local/bin:${process.env.PATH}`;
    execSync(poxyCommand, { 
      stdio: 'inherit',
      env: { ...process.env, PATH: fixedPath }
    });
    
    const outputDir = join(tomlplusplusDir, 'html');
    log('✅ toml++ documentation built successfully!', colors.green);
    log(`📁 Output: ${outputDir}`, colors.blue);
    log('📁 Will be copied to: _site/tomlplusplus/', colors.blue);
    
  } catch (error) {
    log(`❌ Failed to build documentation: ${error.message}`, colors.red);
    log('\n💡 Tips:', colors.yellow);
    log('  - Make sure poxy.toml exists in website/tomlplusplus/', colors.yellow);
    log('  - Check that all source files are present', colors.yellow);
    log('  - Try running manually: cd website/tomlplusplus && poxy', colors.yellow);
    process.exit(1);
  } finally {
    // Return to project root
    process.chdir(projectRoot);
  }
}

buildTomlPlusPlusDocs().catch(error => {
  log(`\n❌ Unexpected error: ${error.message}`, colors.red);
  process.exit(1);
});
