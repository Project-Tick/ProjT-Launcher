set(Launcher_PackageName "projtlauncher" CACHE STRING "Package name for distro packages")
set(Launcher_PackageVendor "Project Tick" CACHE STRING "Package vendor for distro packages")
set(Launcher_PackageContact "Project Tick" CACHE STRING "Package contact/maintainer")
set(Launcher_PackageDescription "${Launcher_DisplayName} - Minecraft Launcher" CACHE STRING "Package description summary")

set(Launcher_ARCH_MAINTAINER "Mehmet Samet Duman <yongdohyun@mail.projecttick.org>" CACHE STRING "PKGBUILD maintainer")
set(Launcher_ARCH_PKGREL "2" CACHE STRING "PKGBUILD pkgrel")
set(Launcher_ARCH_PKGDESC "A Minecraft launcher and automation stack for long-term project health. " CACHE STRING "PKGBUILD description")
set(Launcher_ARCH_URL "https://projecttick.org" CACHE STRING "PKGBUILD URL")
set(Launcher_ARCH_ARCHS "x86_64 aarch64" CACHE STRING "PKGBUILD arch list")
set(Launcher_ARCH_LICENSES "GPL-3.0-only AND LGPL-3.0-or-later AND LGPL-2.0-or-later AND Apache-2.0 AND MIT AND LicenseRef-Batch AND OFL-1.1 AND Zlib AND bzip2"
    CACHE STRING "PKGBUILD license string")
set(Launcher_ARCH_DEPENDS "gcc-libs glibc hicolor-icon-theme java-runtime=17 java-runtime=21 libgl qt6-5compat qt6-base qt6-imageformats qt6-networkauth qt6-svg"
    CACHE STRING "Space-separated Arch Linux dependencies for PKGBUILD")
set(Launcher_ARCH_MAKEDEPENDS "cmake ghc-filesystem git jdk21-openjdk scdoc"
    CACHE STRING "Space-separated Arch Linux makedepends for PKGBUILD")
set(Launcher_ARCH_OPTDEPENDS "'glfw: to use system GLFW libraries' 'openal: to use system OpenAL libraries' 'visualvm: Profiling support' 'xorg-xrandr: for older minecraft versions' 'java-runtime=8: for older minecraft versions' 'flite: minecraft voice narration'"
    CACHE STRING "Quoted Arch Linux optdepends entries for PKGBUILD")
set(Launcher_ARCH_SHA256SUMS "'SKIP' '2ee3ba8d96e9882150783b6444651ea4a65d779532ecac8646f2ecd3a48c2770' '009e25d32aab6dbae193aac4b82fa1a26cb07f288225b2906da425a0f219bc4c' '32646946afc31ef5a4ce2cbb5a5a68a9f552c540a78ef23344c51c3efca58fa6'"
    CACHE STRING "Quoted sha256sums list for PKGBUILD")

set(CPACK_PACKAGE_NAME "${Launcher_PackageName}")
set(CPACK_PACKAGE_VENDOR "${Launcher_PackageVendor}")
set(CPACK_PACKAGE_CONTACT "${Launcher_PackageContact}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${Launcher_PackageDescription}")

set(CPACK_PACKAGE_VERSION "${Launcher_VERSION_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${Launcher_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Launcher_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${Launcher_VERSION_PATCH}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
# Only override the install prefix during packaging (not regular installs).
# Using CPACK_SET_DESTDIR=OFF makes CPack apply CPACK_PACKAGING_INSTALL_PREFIX
# instead of CMAKE_INSTALL_PREFIX.
set(CPACK_SET_DESTDIR OFF)
set(CPACK_COMPONENTS_ALL Runtime Development)

# Debian packaging
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${Launcher_PackageContact}")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
if(DEFINED Launcher_DEB_DEPENDS AND NOT Launcher_DEB_DEPENDS STREQUAL "")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${Launcher_DEB_DEPENDS}")
endif()

# Fedora / RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-only")
set(CPACK_RPM_PACKAGE_GROUP "Games")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_MAIN_COMPONENT Runtime)
set(CPACK_RPM_DEVELOPMENT_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-devel")
set(CPACK_RPM_DEVELOPMENT_PACKAGE_SUMMARY "${CPACK_PACKAGE_NAME} development files")
set(CPACK_RPM_DEVELOPMENT_PACKAGE_REQUIRES "${CPACK_PACKAGE_NAME} = ${CPACK_PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_SOURCES ON)
if(DEFINED Launcher_RPM_REQUIRES AND NOT Launcher_RPM_REQUIRES STREQUAL "")
    set(CPACK_RPM_PACKAGE_REQUIRES "${Launcher_RPM_REQUIRES}")
endif()

# Generate PKGBUILD template for Arch
set(_arch_pkgbuild_in "${CMAKE_CURRENT_SOURCE_DIR}/packaging/arch/PKGBUILD.in")
if(EXISTS "${_arch_pkgbuild_in}")
    set(_arch_pkgbuild_out "${CMAKE_CURRENT_BINARY_DIR}/packaging/arch/PKGBUILD")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/packaging/arch")
    configure_file("${_arch_pkgbuild_in}" "${_arch_pkgbuild_out}" @ONLY)
endif()

include(CPack)
