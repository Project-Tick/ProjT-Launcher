# ProjT Launcher - GitHub Script Development Shell
# Provides Node.js environment for CI automation scripts
{
  system ? builtins.currentSystem,
  pkgs ? import <nixpkgs> { inherit system; },
}:

pkgs.mkShell {
  name = "projt-launcher-github-script";

  packages = with pkgs; [
    # Node.js for running scripts
    nodejs_20

    # GitHub CLI for authentication
    gh

    # Optional: development tools
    nodePackages.npm
  ];

  shellHook = ''
    echo "ProjT Launcher GitHub Script Development Environment"
    echo ""
    echo "Available commands:"
    echo "  npm install    - Install dependencies"
    echo "  ./run --help   - Show available CLI commands"
    echo "  gh auth login  - Authenticate with GitHub"
    echo ""

    # Install npm dependencies if package-lock.json exists
    if [ -f package-lock.json ] && [ ! -d node_modules ]; then
      echo "Installing npm dependencies..."
      npm ci
    fi
  '';

  # Environment variables
  PROJT_CI_ENV = "development";
}
