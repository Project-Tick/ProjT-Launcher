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
    scdoc \
    appstream \
    lcov \
    libxcb-cursor-dev \
    libdbus-1-dev \
    libinih-dev \
    libsystemd-dev \
    libtiff6 \
    pkg-config \
  && if apt-cache show openjdk-21-jdk >/dev/null 2>&1; then \
       apt-get install -y --no-install-recommends openjdk-21-jdk; \
     else \
       apt-get install -y --no-install-recommends openjdk-17-jdk; \
     fi \
  && rm -rf /var/lib/apt/lists/*
