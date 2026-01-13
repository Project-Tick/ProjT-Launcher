#!/usr/bin/env node
/**
 * Script to check and install Doxygen and Poxy
 * Supports macOS (Homebrew), Linux (apt), and provides instructions for others
 */

import { execSync } from 'child_process';
import { platform } from 'os';

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

function installDoxygen() {
  const os = platform();
  
  log('\n📦 Doxygen is not installed. Installing...', colors.blue);
  
  try {
    if (os === 'darwin') {
      // macOS - use Homebrew
      if (!commandExists('brew')) {
        log('❌ Homebrew is not installed. Please install it first:', colors.red);
        log('/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"');
        return false;
      }
      log('Installing via Homebrew...', colors.yellow);
      execSync('brew install doxygen', { stdio: 'inherit' });
    } else if (os === 'linux') {
      // Linux - try apt
      log('Installing via apt...', colors.yellow);
      execSync('sudo apt-get update && sudo apt-get install -y doxygen', { stdio: 'inherit' });
    } else {
      log('❌ Automatic installation not supported for your OS.', colors.red);
      log('Please install Doxygen manually from: https://www.doxygen.nl/download.html');
      return false;
    }
    
    log('✅ Doxygen installed successfully!', colors.green);
    return true;
  } catch (error) {
    log(`❌ Failed to install Doxygen: ${error.message}`, colors.red);
    return false;
  }
}

function installPoxy() {
  log('\n📦 Installing Poxy via pipx...', colors.blue);
  
  // First check if pipx is installed
  if (!commandExists('pipx')) {
    log('⚠️  pipx is not installed. Installing pipx first...', colors.yellow);
    
    const os = platform();
    try {
      if (os === 'darwin' && commandExists('brew')) {
        execSync('brew install pipx', { stdio: 'inherit' });
        execSync('pipx ensurepath', { stdio: 'inherit' });
      } else if (commandExists('pip3')) {
        execSync('pip3 install --user pipx', { stdio: 'inherit' });
        execSync('python3 -m pipx ensurepath', { stdio: 'inherit' });
      } else {
        log('❌ Cannot install pipx automatically. Please install it manually:', colors.red);
        log('  macOS: brew install pipx', colors.yellow);
        log('  Linux: python3 -m pip install --user pipx', colors.yellow);
        return false;
      }
    } catch (error) {
      log(`❌ Failed to install pipx: ${error.message}`, colors.red);
      return false;
    }
  }
  
  try {
    execSync('pipx install poxy', { stdio: 'inherit' });
    log('✅ Poxy installed successfully!', colors.green);
    log('⚠️  You may need to restart your terminal or run: source ~/.zshrc', colors.yellow);
    return true;
  } catch (error) {
    log(`❌ Failed to install Poxy: ${error.message}`, colors.red);
    log('You can try manually: pipx install poxy', colors.yellow);
    return false;
  }
}

async function main() {
  log('\n🔧 Checking dependencies for toml++ documentation build...', colors.blue);
  
  let allGood = true;
  
  // Check Doxygen
  if (commandExists('doxygen')) {
    try {
      const version = execSync('doxygen --version', { encoding: 'utf-8' }).trim();
      log(`✅ Doxygen is installed (version ${version})`, colors.green);
    } catch {
      log('✅ Doxygen is installed', colors.green);
    }
  } else {
    allGood = installDoxygen() && allGood;
  }
  
  // Check Poxy
  if (commandExists('poxy')) {
    try {
      const version = execSync('poxy --version', { encoding: 'utf-8' }).trim();
      log(`✅ Poxy is installed (${version})`, colors.green);
    } catch {
      log('✅ Poxy is installed', colors.green);
    }
  } else {
    allGood = installPoxy() && allGood;
  }
  
  if (allGood) {
    log('\n🎉 All dependencies are ready!', colors.green);
    log('You can now run: npm run build:tomlplusplus', colors.blue);
  } else {
    log('\n⚠️  Some dependencies could not be installed automatically.', colors.yellow);
    log('Please install them manually and try again.', colors.yellow);
    process.exit(1);
  }
}

main().catch(error => {
  log(`\n❌ Error: ${error.message}`, colors.red);
  process.exit(1);
});
