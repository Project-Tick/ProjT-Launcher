FROM gcr.io/oss-fuzz-base/base-builder

# Dependencies required to build the fuzzing targets
RUN apt-get update && apt-get install -y --no-install-recommends \
    qtbase6-dev \
    qttools6-dev \
    qttools6-dev-tools \
    ninja-build \
 && rm -rf /var/lib/apt/lists/*

# Copy the source into the image
WORKDIR /src/projt-launcher
COPY . /src/projt-launcher

# Ensure the build script is available at the root for the builder entrypoint
COPY .clusterfuzzlite/build.sh /src/build.sh
