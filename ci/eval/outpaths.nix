# =============================================================================
# ProjT Launcher - Build Output Paths
# =============================================================================
# Defines all build output paths for the project across different configurations.
# Used by CI to track what needs to be built and cached.
#
# Usage:
#   nix-env -qaP --no-name --out-path -f ci/eval/outpaths.nix
# =============================================================================

{
  # Systems to generate output paths for
  systems ? builtins.fromJSON (builtins.readFile ../supportedSystems.json),
}:

let
  # Project metadata
  projectName = "projt-launcher";

  # Map system names to output configurations
  systemOutputs = system: {
    name = system;
    outputs = {
      # Main launcher binary
      launcher = {
        type = "executable";
        path = "bin/${projectName}";
        platform = system;
      };

      # Libraries (if built separately)
      libraries = {
        rainbow = "lib/librainbow";
        tomlplusplus = "lib/libtomlplusplus";
        libnbtplusplus = "lib/libnbt++";
        LocalPeer = "lib/libLocalPeer";
        qdcss = "lib/libqdcss";
        katabasis = "lib/libkatabasis";
      };

      # Translations
      translations = {
        type = "data";
        path = "share/${projectName}/translations";
      };

      # Icons and resources
      resources = {
        type = "data";
        path = "share/${projectName}";
      };
    };
  };

  # Build types and their output paths
  buildTypes = {
    Debug = {
      suffix = "-debug";
      optimized = false;
      symbols = true;
    };
    Release = {
      suffix = "";
      optimized = true;
      symbols = false;
    };
    RelWithDebInfo = {
      suffix = "-relwithdebinfo";
      optimized = true;
      symbols = true;
    };
  };

  # Platform-specific packaging outputs
  packageOutputs = {
    "x86_64-linux" = {
      appimage = "ProjT-Launcher-x86_64.AppImage";
      deb = "projt-launcher_VERSION_amd64.deb";
      rpm = "projt-launcher-VERSION.x86_64.rpm";
      flatpak = "org.yongdohyun.ProjTLauncher.flatpak";
    };
    "aarch64-linux" = {
      appimage = "ProjT-Launcher-aarch64.AppImage";
      deb = "projt-launcher_VERSION_arm64.deb";
    };
    "aarch64-darwin" = {
      dmg = "ProjT-Launcher-macOS-AppleSilicon.dmg";
      app = "ProjT Launcher.app";
    };
    "x86_64-windows" = {
      installer = "ProjT-Launcher-Setup.exe";
      portable = "ProjT-Launcher-portable.zip";
      msix = "ProjT-Launcher.msix";
    };
  };

  # Generate output structure for all systems
  allOutputs = builtins.listToAttrs (
    map (system: {
      name = system;
      value = {
        inherit (systemOutputs system) outputs;
        packages = packageOutputs.${system} or { };
        inherit buildTypes;
      };
    }) (if systems == null then [ builtins.currentSystem ] else systems)
  );

in
{
  # All output paths by system
  bySystem = allOutputs;

  # Flat list of all output paths
  allPaths = builtins.concatMap (
    system:
    let
      inherit (allOutputs.${system}) outputs;
    in
    [
      outputs.launcher.path
      outputs.translations.path
      outputs.resources.path
    ]
  ) (builtins.attrNames allOutputs);

  # Metadata
  meta = {
    name = projectName;
    systems = builtins.attrNames allOutputs;
    buildTypes = builtins.attrNames buildTypes;
  };
}
