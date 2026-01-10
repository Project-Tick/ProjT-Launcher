{
  lib,
  stdenv,
  cmake,
  apple-sdk_14,
  extra-cmake-modules,
  gamemode,
  jdk17,
  kdePackages,
  ninja,
  self,
  stripJavaArchivesHook,
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
in

stdenv.mkDerivation {
  pname = "projtlauncher-unwrapped";
  version = "0.0.3-unstable-${date}";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ../CMakeLists.txt
      ../CMakePresets.json
      ../COPYING.md
      ../LICENSE
      ../bot
      ../buildconfig
      ../bzip2
      ../caches
      ../ci
      ../cmake
      ../cmark
      ../docs
      ../extra-cmake-modules
      ../flatpak
      ../fuzz
      ../gamemode
      ../javacheck
      ../launcher
      ../launcherjava
      ../libnbtplusplus
      ../libqrencode
      ../LICENSES
      ../LocalPeer
      ../meta
      ../murmur2
      ../nix
      ../program_info
      ../public
      ../qdcss
      ../quazip
      ../rainbow
      ../scripts
      ../systeminfo
      ../tests
      ../tomlplusplus
      ../tools
      ../website
      ../zlib
    ];
  };

  postPatch = ''
    # Fix cmark executable name collision with cmark directory in Nix build
    substituteInPlace cmark/src/CMakeLists.txt \
      --replace-fail 'OUTPUT_NAME "cmark"' 'OUTPUT_NAME "cmark-bin"'
  '';

  nativeBuildInputs = [
    cmake
    ninja
    extra-cmake-modules
    jdk17
    stripJavaArchivesHook
  ];

  buildInputs = [
    kdePackages.qtbase
    kdePackages.qtnetworkauth
    kdePackages.qt5compat
  ]
  ++ lib.optionals stdenv.hostPlatform.isDarwin [ apple-sdk_14 ]
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
    homepage = "https://projecttick.org.tr/projtlauncher/";
    license = lib.licenses.gpl3Only;
    maintainers = with lib.maintainers; [
      yongdohyun
      grxtor
    ];
    mainProgram = "projtlauncher";
    platforms = lib.platforms.linux ++ lib.platforms.darwin;
  };
}
