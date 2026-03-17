{
  addDriverRunpath,
  alsa-lib,
  atk,
  at-spi2-atk,
  cairo,
  cups,
  dbus,
  expat,
  fontconfig,
  flite,
  freetype,
  gamemode,
  gdk-pixbuf,
  glfw3-minecraft,
  jdk17,
  jdk21,
  jdk8,
  kdePackages,
  glib,
  gtk3,
  harfbuzz,
  lib,
  libGL,
  libX11,
  libXcomposite,
  libXcursor,
  libXdamage,
  libXext,
  libXfixes,
  libXrandr,
  libXxf86vm,
  libxcb,
  libxkbcommon,
  libjack2,
  libpulseaudio,
  libusb1,
  mesa-demos,
  mesa,
  nspr,
  nss,
  openal,
  pango,
  pciutils,
  pipewire,
  projtlauncher-unwrapped,
  stdenv,
  symlinkJoin,
  udev,
  vulkan-loader,
  xorg,
  xrandr,

  additionalLibs ? [ ],
  additionalPrograms ? [ ],
  controllerSupport ? stdenv.hostPlatform.isLinux,
  gamemodeSupport ? stdenv.hostPlatform.isLinux,
  jdks ? [
    jdk21 # need for newest Minecraft versions
    jdk17 # need for old and new Minecraft versions
    jdk8 # need for legacy Minecraft versions
  ],
  msaClientID ? null,
  textToSpeechSupport ? stdenv.hostPlatform.isLinux,
}:

assert lib.assertMsg (
  controllerSupport -> stdenv.hostPlatform.isLinux
) "controllerSupport only has an effect on Linux.";

assert lib.assertMsg (
  textToSpeechSupport -> stdenv.hostPlatform.isLinux
) "textToSpeechSupport only has an effect on Linux.";

let
  projtlauncher' = projtlauncher-unwrapped.override { inherit msaClientID gamemodeSupport; };
in

symlinkJoin {
  pname = "projtlauncher";
  inherit (projtlauncher') version;

  paths = [ projtlauncher' ];

  nativeBuildInputs = [ kdePackages.wrapQtAppsHook ];

  buildInputs = [
    kdePackages.qtbase
    kdePackages.qtsvg
  ]
  ++ lib.optional (
    lib.versionAtLeast kdePackages.qtbase.version "6" && stdenv.hostPlatform.isLinux
  ) kdePackages.qtwayland;

  postBuild = ''
    wrapQtAppsHook
  '';

  qtWrapperArgs =
    let
      runtimeLibs = [
        (lib.getLib stdenv.cc.cc)
        ## native versions
        glfw3-minecraft
        openal

        ## openal
        alsa-lib
        libjack2
        libpulseaudio
        pipewire

        ## CEF
        atk
        at-spi2-atk
        cairo
        cups
        dbus
        expat
        fontconfig
        freetype
        gdk-pixbuf
        glib
        gtk3
        harfbuzz
        libxcb
        libxkbcommon
        mesa
        nspr
        nss
        pango

        ## glfw
        libGL
        libX11
        libXcomposite
        libXcursor
        libXdamage
        libXext
        libXfixes
        libXrandr
        libXxf86vm
        xorg.xcbutilcursor

        udev # oshi

        vulkan-loader # VulkanMod's lwjgl
      ]
      ++ lib.optional textToSpeechSupport flite
      ++ lib.optional gamemodeSupport gamemode.lib
      ++ lib.optional controllerSupport libusb1
      ++ additionalLibs;

      runtimePrograms = [
        mesa-demos
        pciutils # need lspci
        xrandr # needed for LWJGL [2.9.2, 3) https://github.com/LWJGL/lwjgl/issues/128
      ]
      ++ additionalPrograms;

    in
    [ "--prefix PROJTLAUNCHER_JAVA_PATHS : ${lib.makeSearchPath "bin/java" jdks}" ]
    ++ lib.optionals stdenv.hostPlatform.isLinux [
      "--set LD_LIBRARY_PATH ${projtlauncher'}/lib/projtlauncher:${projtlauncher'}/lib:${addDriverRunpath.driverLink}/lib:${lib.makeLibraryPath runtimeLibs}"
      "--prefix PATH : ${lib.makeBinPath runtimePrograms}"
    ];

  meta = {
    inherit (projtlauncher'.meta)
      description
      longDescription
      homepage
      changelog
      license
      maintainers
      mainProgram
      platforms
      ;
  };
}
