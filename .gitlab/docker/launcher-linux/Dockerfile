FROM debian:trixie

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
    lcov \
    libxcb-cursor-dev \
    libdbus-1-dev \
    libinih-dev \
    libsystemd-dev \
    libssl-dev \
    libtiff6 \
    pkg-config \
    openjdk-21-jdk \
    nodejs \
    npm \
    perl \
  && rm -rf /var/lib/apt/lists/*
