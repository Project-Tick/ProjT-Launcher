# =============================================================================
# ProjT Launcher - CI Evaluation Module
# =============================================================================
# Validates project structure, CMake configuration, and Nix flake.
# This is used by GitHub Actions CI to ensure PRs don't break the build.
#
# Usage:
#   nix-build ci/eval -A validate
#   nix-build ci/eval -A cmake
#   nix-build ci/eval -A vcpkg
#   nix-build ci/eval -A full
# =============================================================================

{
  lib,
  runCommand,
  cmake,
  nix,
  jq,
}:

{
  # Quick validation mode (skip some checks)
  quickTest ? false,
}:

let
  # Project source (filtered to only include relevant files)
  projectSrc =
    with lib.fileset;
    toSource {
      root = ../..;
      fileset = unions (
        map (lib.path.append ../..) [
          "CMakeLists.txt"
          "CMakePresets.json"
          "cmake"
          "launcher"
          "launcherjava"
          "javacheck"
          "LocalPeer"
          "murmur2"
          "qdcss"
          "rainbow"
          "systeminfo"
          "buildconfig"
          "program_info"
          "gamemode"
          "flake.nix"
          "default.nix"
          "shell.nix"
        ]
      );
    };

  # =============================================================================
  # CMake Validation
  # =============================================================================
  validateCMake =
    runCommand "projt-validate-cmake"
      {
        src = projectSrc;
        nativeBuildInputs = [ cmake ];
      }
      ''
        mkdir -p $out
        cd $src

        echo "=== Validating CMakeLists.txt (basic) ==="

        # Check main CMakeLists.txt exists
        if [ ! -f CMakeLists.txt ]; then
          echo "ERROR: CMakeLists.txt not found"
          exit 1
        fi

        # Basic sanity checks (cheap and dependency-free)
        if ! grep -Fqi 'cmake_minimum_required(' CMakeLists.txt; then
          echo "ERROR: Missing cmake_minimum_required(...)"
          exit 1
        fi

        if ! grep -Fqi 'project(' CMakeLists.txt; then
          echo "ERROR: Missing project(...)"
          exit 1
        fi

        echo "CMake validation passed" > $out/cmake.txt
      '';

  # =============================================================================
  # vcpkg Validation
  # =============================================================================
  validateVcpkg =
    runCommand "projt-validate-vcpkg"
      {
        src = projectSrc;
        nativeBuildInputs = [ jq ];
      }
      ''
        mkdir -p $out
        cd $src

        echo "=== Validating vcpkg.json ==="

        # Check vcpkg.json exists and is valid JSON
        if [ -f vcpkg.json ]; then
          jq . vcpkg.json > /dev/null || {
            echo "ERROR: vcpkg.json is not valid JSON"
            exit 1
          }
          
          if jq -e '.name' vcpkg.json > /dev/null; then
            echo "INFO: vcpkg.json name field present: $(jq -r '.name' vcpkg.json)"
          else
            echo "WARNING: vcpkg.json missing 'name' field (optional for manifest mode)"
          fi

          echo "vcpkg.json validation passed"
        else
          echo "WARNING: vcpkg.json not found (may not be using vcpkg)"
        fi

        # Check vcpkg-configuration.json
        if [ -f vcpkg-configuration.json ]; then
          jq . vcpkg-configuration.json > /dev/null || {
            echo "ERROR: vcpkg-configuration.json is not valid JSON"
            exit 1
          }
          echo "vcpkg-configuration.json validation passed"
        fi

        echo "vcpkg validation passed" > $out/vcpkg.txt
      '';

  # =============================================================================
  # CMake Presets Validation
  # =============================================================================
  validatePresets =
    runCommand "projt-validate-presets"
      {
        src = projectSrc;
        nativeBuildInputs = [ jq ];
      }
      ''
        mkdir -p $out
        cd $src

        echo "=== Validating CMakePresets.json ==="

        if [ -f CMakePresets.json ]; then
          jq . CMakePresets.json > /dev/null || {
            echo "ERROR: CMakePresets.json is not valid JSON"
            exit 1
          }
          
          # Check for required presets
          for preset in linux windows_mingw windows_msvc macos_universal; do
            if ! jq -e ".configurePresets[] | select(.name == \"$preset\")" CMakePresets.json > /dev/null 2>&1; then
              echo "WARNING: Preset '$preset' not found in CMakePresets.json"
            else
              echo "Found preset: $preset"
            fi
          done
          
          echo "CMakePresets.json validation passed"
        else
          echo "WARNING: CMakePresets.json not found"
        fi

        echo "presets validation passed" > $out/presets.txt
      '';

  # =============================================================================
  # Nix Flake Validation
  # =============================================================================
  validateNix =
    runCommand "projt-validate-nix"
      {
        src = projectSrc;
        nativeBuildInputs = [ nix ];
      }
      ''
        mkdir -p $out
        cd $src

        echo "=== Validating Nix files ==="

        check_nix_file() {
          local file="$1"
          if [ ! -f "$file" ]; then
            return 0
          fi

          mkdir -p "$TMPDIR/nix/state" "$TMPDIR/nix/log" "$TMPDIR/nix/etc"
          if env -i \
            PATH="$PATH" \
            HOME="$TMPDIR" \
            TMPDIR="$TMPDIR" \
            NIX_REMOTE=local \
            NIX_STATE_DIR="$TMPDIR/nix/state" \
            NIX_LOG_DIR="$TMPDIR/nix/log" \
            NIX_CONF_DIR="$TMPDIR/nix/etc" \
            nix-instantiate --parse "$file" > /dev/null 2>&1; then
            echo "$file syntax OK"
          else
            echo "ERROR: Failed to parse $file"
            exit 1
          fi
        }

        check_nix_file flake.nix
        check_nix_file default.nix
        check_nix_file shell.nix

        echo "nix validation passed" > $out/nix.txt
      '';

  # =============================================================================
  # Project Structure Validation
  # =============================================================================
  validateStructure =
    runCommand "projt-validate-structure"
      {
        src = projectSrc;
      }
      ''
        mkdir -p $out
        cd $src

        echo "=== Validating project structure ==="

        # Check required directories exist
        for dir in launcher libraries cmake buildconfig program_info; do
          if [ -d "$dir" ]; then
            echo "OK: Directory exists: $dir"
          else
            echo "WARNING: Expected directory not found: $dir"
          fi
        done

        # Check launcher source files
        if [ -f launcher/Application.cpp ] && [ -f launcher/Application.h ]; then
          echo "OK: Core launcher files found"
        else
          echo "WARNING: Core launcher files may be missing"
        fi

        echo "structure validation passed" > $out/structure.txt
      '';

  # =============================================================================
  # Full Validation
  # =============================================================================
  fullValidation =
    runCommand "projt-validate-full"
      {
        inherit
          validateCMake
          validateVcpkg
          validatePresets
          validateNix
          validateStructure
          ;
      }
      ''
        mkdir -p $out

        echo "=== ProjT Launcher CI Evaluation ==="
        echo ""

        echo "CMake: $(cat $validateCMake/cmake.txt)"
        echo "vcpkg: $(cat $validateVcpkg/vcpkg.txt)"
        echo "Presets: $(cat $validatePresets/presets.txt)"
        echo "Nix: $(cat $validateNix/nix.txt)"
        echo "Structure: $(cat $validateStructure/structure.txt)"

        echo ""
        echo "=== All validations passed ==="

        # Create summary
        cat > $out/summary.md << 'EOF'
        ## ProjT Launcher CI Evaluation Results

        | Check | Status |
        |-------|--------|
        | CMake | Passed |
        | vcpkg | Passed |
        | Presets | Passed |
        | Nix | Passed |
        | Structure | Passed |

        All validation checks completed successfully.
        EOF

        echo "full validation passed" > $out/result.txt
      '';

in
{
  # Individual validations
  cmake = validateCMake;
  vcpkg = validateVcpkg;
  presets = validatePresets;
  nix = validateNix;
  structure = validateStructure;

  # Quick validation (subset)
  validate = if quickTest then validateCMake else fullValidation;

  # Full validation
  full = fullValidation;

  # Alias for CI
  baseline = fullValidation;
}
