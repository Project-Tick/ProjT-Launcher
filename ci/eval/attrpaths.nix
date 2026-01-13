# =============================================================================
# ProjT Launcher - Build Configuration Paths
# =============================================================================
# Lists all configurable build paths and options for the project.
# Used by CI to validate that all configurations are buildable.
#
# Usage:
#   nix-instantiate --eval --strict --json ci/eval/attrpaths.nix -A names
# =============================================================================

{
  lib ? import (path + "/lib"),
  path ? ./../..,
}:

let
  # Build configurations available in the project
  buildConfigs = {
    # Platform presets from CMakePresets.json
    presets = [
      "linux"
      "windows_mingw"
      "windows_msvc"
      "macos_universal"
    ];

    # Build types
    buildTypes = [
      "Debug"
      "Release"
      "RelWithDebInfo"
      "MinSizeRel"
    ];

    # Compiler options
    compilers = {
      linux = [
        "gcc"
        "clang"
      ];
      macos = [
        "clang"
        "apple-clang"
      ];
      windows = [
        "msvc"
        "mingw-gcc"
        "clang-cl"
      ];
    };

    # Qt versions supported
    qtVersions = [
      "6.6"
      "6.7"
      "6.8"
    ];

    # Feature flags
    features = [
      "LAUNCHER_ENABLE_UPDATER"
      "LAUNCHER_FORCE_BUNDLED_LIBS"
      "LAUNCHER_BUILD_TESTS"
    ];
  };

  # Generate all possible build configuration paths
  generatePaths =
    let
      presetPaths = map (p: [
        "preset"
        p
      ]) buildConfigs.presets;
      buildTypePaths = map (b: [
        "buildType"
        b
      ]) buildConfigs.buildTypes;
      qtPaths = map (q: [
        "qt"
        q
      ]) buildConfigs.qtVersions;
      featurePaths = map (f: [
        "feature"
        f
      ]) buildConfigs.features;
    in
    presetPaths ++ buildTypePaths ++ qtPaths ++ featurePaths;

  # Build component paths
  componentPaths = [
    [
      "launcher"
      "core"
    ]
    [
      "launcher"
      "ui"
    ]
    [
      "launcher"
      "minecraft"
    ]
    [
      "launcher"
      "modplatform"
    ]
    [
      "launcher"
      "java"
    ]
    [
      "launcher"
      "net"
    ]
    [
      "rainbow"
    ]
    [
      "tomlplusplus"
    ]
    [
      "libnbtplusplus"
    ]
    [
      "LocalPeer"
    ]
    [
      "qdcss"
    ]
    [
      "katabasis"
    ]
  ];

  # All paths combined
  paths = generatePaths ++ componentPaths;

  # Convert paths to dotted names
  names = map (p: lib.concatStringsSep "." p) paths;

in
{
  inherit paths names;

  # Export build configs for other tools
  inherit buildConfigs;
}
