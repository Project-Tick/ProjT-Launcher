FROM gcr.io/oss-fuzz-base/base-builder

# Minimal dependencies for fuzzer build
# Qt6 will be installed via aqt in build.sh
RUN apt-get update && apt-get install -y --no-install-recommends \
    ninja-build \
    python3-pip \
 && rm -rf /var/lib/apt/lists/*

# Copy the source into the image
WORKDIR /src/projt-launcher
COPY . /src/projt-launcher

# Ensure the build script is available at the root for the builder entrypoint
COPY .clusterfuzzlite/build.sh /src/build.sh
