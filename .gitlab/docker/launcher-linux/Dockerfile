FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    git \
    tar \
    xz-utils \
    gzip \
    bzip2 \
    build-essential \
    ccache \
    cmake \
    dpkg-dev \
    ninja-build \
    clang \
    wget \
    scdoc \
    appstream \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-5compat-dev \
    qt6-networkauth-dev \
    qt6-webengine-dev \
    qt6-wayland-dev \
    lcov \
    libxcb-cursor-dev \
    libdbus-1-dev \
    libinih-dev \
    libopengl0 \
    libsystemd-dev \
    libssl-dev \
    libtiff6 \
    libvulkan-dev \
    vulkan-tools \
    pkg-config \
    openjdk-21-jdk \
    nodejs \
    npm \
    perl \
  && rm -rf /var/lib/apt/lists/*

RUN ARCH_DIR=$(dpkg-architecture -q DEB_HOST_MULTIARCH) \
  && if [ -e "/usr/lib/${ARCH_DIR}/libtiff.so.6" ] && [ ! -e "/usr/lib/${ARCH_DIR}/libtiff.so.5" ]; then \
    ln -sf "/usr/lib/${ARCH_DIR}/libtiff.so.6" "/usr/lib/${ARCH_DIR}/libtiff.so.5"; \
  fi
