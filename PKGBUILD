# Maintainer: Yong Do Hyun <froster12@naver.com>

pkgname=projtlauncher
pkgver=0.0.1
pkgrel=1
pkgdesc='Minecraft launcher with ability to manage multiple instances'
arch=(x86_64 aarch64)
url='https://projtlauncher.yongdohyun.org.tr'
license=('GPL-3.0-or-later AND GPL-3.0-only AND LGPL-3.0-or-later AND LGPL-2.0-or-later AND Apache-2.0 AND MIT AND LicenseRef-Batch AND OFL-1.1')
depends=(
  cmark
  gcc-libs
  glibc
  hicolor-icon-theme
  java-runtime=21
  libgl
  qrencode
  qt6-5compat
  qt6-base
  qt6-imageformats
  qt6-networkauth
  qt6-svg
  quazip-qt6
  tomlplusplus
  zlib
)
provides=('projtlauncher')
conflicts=('projtlauncher')
makedepends=(
  cmake
  extra-cmake-modules
  gamemode
  ghc-filesystem
  git
  jdk17-openjdk
  scdoc
)
optdepends=(
  'glfw: to use system GLFW libraries'
  'openal: to use system OpenAL libraries'
  'visualvm: Profiling support'
  'xorg-xrandr: for older minecraft versions'
  'java-runtime=8: for older minecraft versions'
  'flite: minecraft voice narration'
)
source=(
  'https://github.com/Project-Tick/ProjT-Launcher/archive/refs/tags/v0.0.1.tar.gz'
)
sha256sums=('SKIP')

prepare() {
  cd "ProjT-Launcher"

  git submodule init
  git config submodule.libraries/cmark.active false
  git config submodule.libraries/extra-cmake-modules.active false
  git config submodule.libraries/filesystem.active false
  git config submodule.libraries/libnbtplusplus.url "${srcdir}/libnbtplusplus"
  git config submodule.libraries/quazip.active false
  git config submodule.libraries/tomlplusplus.active false
  git config submodule.libraries/zlib.active false
  git -c protocol.file.allow=always submodule update
}

build() {
  export PATH="/usr/lib/jvm/java-21-openjdk/bin:$PATH"

  cmake -S ProjT-Launcher -B build \
    -DCMAKE_BUILD_TYPE='None' \
    -DCMAKE_INSTALL_PREFIX='/usr' \
    -DLauncher_BUILD_PLATFORM="archlinux" \
    -DLauncher_APP_BINARY_NAME="projtlauncher" \
    -DLauncher_QT_VERSION_MAJOR="6" \
    -Wno-dev
  cmake --build build
}

check() {
  ctest --test-dir build --output-on-failure
}

package() {
  cd "ProjT-Launcher"
  DESTDIR="$pkgdir" cmake --install build

  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/projtlauncher/LICENSE"

  # Eğer Apache lisansı ayrı bir dosyaysa:
  if [[ -f APACHE-LICENSE ]]; then
      install -Dm644 APACHE-LICENSE "$pkgdir/usr/share/licenses/projtlauncher/APACHE-LICENSE"
  fi
}
