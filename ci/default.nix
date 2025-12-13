# ProjT Launcher CI Configuration
# This Nix expression provides a development environment and build dependencies
# for the ProjT Launcher project

{
  system ? builtins.currentSystem,
}:

let
  nixpkgs = import <nixpkgs> { inherit system; };
  pkgs = nixpkgs;
in

rec {
  # Development environment with all build dependencies
  devEnv = pkgs.mkShell {
    buildInputs = with pkgs; [
      # Build tools
      cmake
      ninja
      pkg-config

      # Compilers
      gcc
      clang

      # Qt6 dependencies
      qt6.full
      qt6.base
      qt6.declarative
      qt6.multimedia
      qt6.tools

      # Other dependencies
      zlib
      libxkbcommon

      # Code quality tools
      clang-tools
      cmake-format

      # Testing
      gtest
    ];

    shellHook = ''
      echo "ProjT Launcher development environment loaded"
      echo "Available: cmake, ninja, qt6, gcc, clang"
    '';
  };

  # Build configuration for CI
  buildConfig = {
    buildType = "Release";
    enableTesting = true;
    enableLTO = true;
  };

  # Test environment
  testEnv = pkgs.mkShell {
    buildInputs = with pkgs; [
      cmake
      ninja
      qt6.full
      gtest
    ];
  };
}
