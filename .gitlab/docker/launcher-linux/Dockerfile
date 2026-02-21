FROM debian:bookworm-slim

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
    scdoc \
    appstream \
    openjdk-21-jdk \
    lcov \
    libxcb-cursor-dev \
    libdbus-1-dev \
    libinih-dev \
    libsystemd-dev \
    libtiff6 \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*
