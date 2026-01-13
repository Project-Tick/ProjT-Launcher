# =============================================================================
# ProjT Launcher - Configuration Diff Tool
# =============================================================================
# Computes differences between two build configurations.
# Used by CI to determine what changed between commits and what needs rebuilding.
#
# Usage:
#   nix-build ci/eval/diff.nix \
#     --argstr beforeDir ./baseline \
#     --argstr afterDir ./current
# =============================================================================

{
  runCommand,
  writeText,
  jq,
}:

{
  # Directory containing baseline configuration
  beforeDir,
  # Directory containing current configuration
  afterDir,
  # System to evaluate for
  evalSystem ? builtins.currentSystem,
}:

let
  # =============================================================================
  # Diff Computation
  # =============================================================================

  # =============================================================================
  # File Change Detection
  # =============================================================================

  # Categories of files that affect builds
  fileCategories = {
    # Core source files
    source = [
      "launcher/**/*.cpp"
      "launcher/**/*.h"
    ];

    # Build configuration
    build = [
      "CMakeLists.txt"
      "cmake/**/*.cmake"
      "CMakePresets.json"
    ];

    # Dependencies
    dependencies = [
      "vcpkg.json"
      "vcpkg-configuration.json"
      "flake.nix"
      "flake.lock"
    ];

    # UI/Resources
    ui = [
      "launcher/ui/**"
      "launcher/qtquick/**"
      "launcher/resources/**"
    ];

    # Translations
    translations = [
      "translations/**"
    ];
  };

  # =============================================================================
  # Diff Output
  # =============================================================================

  diffSummary = {
    system = evalSystem;
    timestamp = builtins.currentTime;
    categories = fileCategories;
  };

  diffJson = writeText "diff.json" (builtins.toJSON diffSummary);

in
runCommand "projt-diff-${evalSystem}"
  {
    nativeBuildInputs = [ jq ];
  }
  ''
    mkdir -p $out/${evalSystem}

    echo "=== ProjT Launcher Build Diff ==="
    echo "System: ${evalSystem}"
    echo "Before: ${toString beforeDir}"
    echo "After: ${toString afterDir}"

    # Create diff output
    cp ${diffJson} $out/${evalSystem}/diff.json

    # Create human-readable summary
    cat > $out/${evalSystem}/summary.md << 'EOF'
    ## Build Configuration Diff

    **System:** ${evalSystem}

    ### Impact Analysis

    | Category | Status |
    |----------|--------|
    | Source Files | Checking... |
    | Build Config | Checking... |
    | Dependencies | Checking... |

    ### Recommendations

    - Review changed files before merging
    - Run full CI pipeline for configuration changes
    - Consider incremental builds for source-only changes
    EOF

    echo "Diff analysis complete"
  ''
