{
  lib,
  stdenv,
  cmake,
  cmark,
  apple-sdk_11,
  extra-cmake-modules,
  gamemode,
  jdk17,
  kdePackages,
  libnbtplusplus,
  qrcodegenerator,
  ninja,
  self,
  stripJavaArchivesHook,
  tomlplusplus,
  zlib,
  msaClientID ? null,
  gamemodeSupport ? stdenv.hostPlatform.isLinux,
}:
assert lib.assertMsg (
  gamemodeSupport -> stdenv.hostPlatform.isLinux
) "gamemodeSupport is only available on Linux.";

let
  date =
    let
      # YYYYMMDD
      date' = lib.substring 0 8 self.lastModifiedDate;
      year = lib.substring 0 4 date';
      month = lib.substring 4 2 date';
      date = lib.substring 6 2 date';
    in
    if (self ? "lastModifiedDate") then
      lib.concatStringsSep "-" [
        year
        month
        date
      ]
    else
      "unknown";
  javacheck = pkgs.stdenv.mkDerivation {
    pname = "javacheck";
    version = "1.0";
    src = builtins.fetchGit {
      url = "https://github.com/Project-Tick/javacheck.git";
      rev = "13c75395fcbe4b282e429c53065d820a14b701b2";
    };
    buildInputs = [ pkgs.jdk ];
    installPhase = ''
      mkdir -p javacheck
      cp -r * javacheck/
    '';
  };
  launchersm = pkgs.stdenv.mkDerivation {
    pname = "launchersm";
    version = "1.0";
    src = builtins.fetchGit {
      url = "https://github.com/Project-Tick/PTLLauncherSM.git";
      rev = "ae13360ed8e5a490ff6d8198286fbac677b28b05";
    };
    buildInputs = [ pkgs.jdk ];
    installPhase = ''
      mkdir -p launchersm
      cp -r * launchersm/
    '';
  };
in

stdenv.mkDerivation {
  pname = "projtlauncher-unwrapped";
  version = "0.0.2-unstable-${date}";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ../CMakeLists.txt
      ../COPYING.md

      ../buildconfig
      ../cmake
      ../launcher
      ../libraries
      ../program_info
      ../tests
    ];
  };

  postUnpack = ''
    rm -rf source/libraries/libnbtplusplus
    ln -s ${libnbtplusplus} source/libraries/libnbtplusplus

    rm -rf source/libraries/qrcodegenerator
    ln -s ${qrcodegenerator} source/libraries/qrcodegenerator

    rm -rf source/libraries/javacheck
    ln -s javacheck/ source/libraries/javacheck

    rm -rf source/libraries/launcher
    ln -s launchersm/ source/libraries/launcher
  '';

  nativeBuildInputs = [
    cmake
    ninja
    extra-cmake-modules
    jdk17
    stripJavaArchivesHook
  ];

  buildInputs = [
    cmark
    kdePackages.qtbase
    kdePackages.qtnetworkauth
    kdePackages.quazip
    tomlplusplus
    zlib
  ]
  ++ lib.optionals stdenv.hostPlatform.isDarwin [ apple-sdk_11 ]
  ++ lib.optional gamemodeSupport gamemode;

  hardeningEnable = lib.optionals stdenv.hostPlatform.isLinux [ "pie" ];

  cmakeFlags = [
    # downstream branding
    (lib.cmakeFeature "Launcher_BUILD_PLATFORM" "nixpkgs")
  ]
  ++ lib.optionals (msaClientID != null) [
    (lib.cmakeFeature "Launcher_MSA_CLIENT_ID" (toString msaClientID))
  ]
  ++ lib.optionals stdenv.hostPlatform.isDarwin [
    # we wrap our binary manually
    (lib.cmakeFeature "INSTALL_BUNDLE" "nodeps")
    # disable built-in updater
    (lib.cmakeFeature "MACOSX_SPARKLE_UPDATE_FEED_URL" "''")
    (lib.cmakeFeature "CMAKE_INSTALL_PREFIX" "${placeholder "out"}/Applications/")
  ];

  doCheck = true;

  dontWrapQtApps = true;

  meta = {
    description = "Free, open source launcher for Minecraft";
    longDescription = ''
      Allows you to have multiple, separate instances of Minecraft (each with
      their own mods, texture packs, saves, etc) and helps you manage them and
      their associated options with a simple interface.
    '';
    homepage = "https://projtlauncher.yongdohyun.org.tr/";
    license = lib.licenses.gpl3Only;
    maintainers = with lib.maintainers; [
      Scrumplex
      getchoo
    ];
    mainProgram = "projtlauncher";
    platforms = lib.platforms.linux ++ lib.platforms.darwin;
  };
}
