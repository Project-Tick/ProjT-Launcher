# ProjT Launcher - Supported Dependency Versions
# Returns a list of supported Qt and compiler versions for testing

{
  pkgs ? import <nixpkgs> { },
}:
let
  inherit (pkgs) lib;

  # Supported Qt versions
  qtVersions = [
    "qt6Packages" # Qt 6.x (primary)
  ];

  # Supported compiler versions
  compilerVersions = {
    gcc = [
      "gcc13"
      "gcc14"
    ];
    clang = [
      "clang_17"
      "clang_18"
    ];
  };

  # Supported CMake versions
  cmakeVersions = [
    "cmake" # Latest stable
  ];

  # Build matrix combinations
  buildMatrix = lib.flatten (
    map (
      qt:
      map (compiler: {
        inherit qt;
        inherit compiler;
        cmake = "cmake";
      }) (compilerVersions.gcc ++ compilerVersions.clang)
    ) qtVersions
  );

in
{
  inherit
    qtVersions
    compilerVersions
    cmakeVersions
    buildMatrix
    ;

  # Minimum required versions
  minimum = {
    cmake = "3.22";
    qt = "6.5";
    gcc = "12";
    clang = "15";
  };

  # Recommended versions
  recommended = {
    cmake = "3.28";
    qt = "6.7";
    gcc = "14";
    clang = "18";
  };
}
