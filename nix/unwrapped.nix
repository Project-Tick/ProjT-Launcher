{
  alsa-lib,
  atk,
  at-spi2-atk,
  cairo,
  fetchzip,
  lib,
  libcap,
  stdenv,
  cmake,
  apple-sdk_14,
  cups,
  dbus,
  expat,
  extra-cmake-modules,
  gamemode,
  glib,
  inih,
  jdk17,
  kdePackages,
  libxcb,
  libxkbcommon,
  ninja,
  nspr,
  nss,
  mesa,
  self,
  stripJavaArchivesHook,
  systemd,
  xorg,
  msaClientID ? null,
  gamemodeSupport ? stdenv.hostPlatform.isLinux,
  cefVersion ? "144.0.11+ge135be2+chromium-144.0.7559.97",
  cefDistribution ? "minimal",
  cefHash ? if stdenv.hostPlatform.system == "x86_64-linux" then "sha256-gdwTnWfOJ7O/9c5uPa2OWvzeD4O0COyzg2qMOy7JknU=" else null,
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

  cefBinaryPlatform =
    if stdenv.hostPlatform.system == "x86_64-linux" then
      "linux64"
    else if stdenv.hostPlatform.system == "aarch64-linux" then
      "linuxarm64"
    else
      null;

  cefRootDir =
    if cefBinaryPlatform != null then
      "cef_binary_${cefVersion}_${cefBinaryPlatform}_${cefDistribution}"
    else
      null;

  cefBinary =
    if stdenv.hostPlatform.isLinux then
      assert lib.assertMsg (cefBinaryPlatform != null) "No packaged Linux CEF binary platform is configured for ${stdenv.hostPlatform.system}.";
      assert lib.assertMsg (cefHash != null) "No Linux CEF hash is configured for ${stdenv.hostPlatform.system}. Pass `cefHash` when calling `projtlauncher-unwrapped`.";
      fetchzip {
        url = "https://cef-builds.spotifycdn.com/${cefRootDir}.tar.bz2";
        hash = cefHash;
        stripRoot = false;
      }
    else
      null;
in

stdenv.mkDerivation {
  pname = "projtlauncher-unwrapped";
  version = "0.0.5-1-unstable-${date}";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ../CMakeLists.txt
      ../CMakePresets.json
      ../COPYING
      ../bootstrap
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
      ../libpng
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
    inih
    kdePackages.qtbase
    kdePackages.qtnetworkauth
  ]
  ++ lib.optionals stdenv.hostPlatform.isDarwin [
    kdePackages.qtwebchannel
    kdePackages.qtwebengine
  ]
  ++ lib.optionals stdenv.hostPlatform.isLinux [
    alsa-lib
    atk
    at-spi2-atk
    cairo
    cups
    dbus
    expat
    glib
    libcap
    libxcb
    libxkbcommon
    mesa
    nspr
    nss
    systemd
    xorg.libX11
    xorg.libXcomposite
    xorg.libXdamage
    xorg.libXext
    xorg.libXfixes
    xorg.libXrandr
  ]
  ++ lib.optionals stdenv.hostPlatform.isDarwin [ apple-sdk_14 ]
  ++ lib.optional gamemodeSupport gamemode;

  cmakeFlags = [
    # downstream branding
    (lib.cmakeBool "BUILD_TESTING" false)
    (lib.cmakeFeature "Launcher_BUILD_PLATFORM" "nixpkgs")
  ]
  ++ lib.optionals stdenv.hostPlatform.isLinux [
    (lib.cmakeBool "Launcher_CEF_AUTO_DOWNLOAD" false)
    (lib.cmakeFeature "Launcher_CEF_ROOT" "${cefBinary}/${cefRootDir}")
    (lib.cmakeFeature "GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR" "lib/systemd/user")
    (lib.cmakeFeature "GAMEMODE_WITH_SYSTEMD_GROUP_DIR" "lib/sysusers.d")
    (lib.cmakeFeature "GAMEMODE_WITH_PAM_LIMITS_DIR" "etc/security/limits.d")
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
  preCheck = ''
    export CMAKE_PREFIX_PATH="$PWD/zlib/test/test_install:$CMAKE_PREFIX_PATH"
  '';

  dontWrapQtApps = true;

  meta = {
    description = "Free, open source launcher for Minecraft";
    longDescription = ''

      Allows you to have multiple, separate instances of Minecraft (each with
      their own mods, texture packs, saves, etc) and helps you manage them and
      their associated options with a simple interface.
    '';
    homepage = "https://projecttick.org/p/projt-launcher/";
    license = lib.licenses.gpl3Only;
    maintainers = with lib.maintainers; [
      yongdohyun
      grxtor
    ];
    mainProgram = "projtlauncher";
    platforms = lib.platforms.linux ++ lib.platforms.darwin;
  };
}
