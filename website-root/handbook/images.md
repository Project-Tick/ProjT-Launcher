# CI Docker Images

> **Purpose**: Minimal Qt build images for CI testing
> **Latest Version**: 0.0.5-1

---

## Overview

These are minimal Docker images containing Qt built from source with specific options for CI testing purposes.

---

## Why Custom Images?

The standard `jurplel/install-qt-action` downloads pre-built Qt binaries, but:

- ❌ Does not support all build options
- ❌ Cannot test `-qt-zlib` builds
- ❌ Limited configuration options

Our custom images provide:

- ✅ Qt built with `-qt-zlib` option
- ✅ Testing `QUAZIP_USE_QT_ZLIB=ON` builds
- ✅ Full control over Qt configuration

---

## Available Images

| Image | Qt Version | Platform |
|-------|------------|----------|
| `qt-minimal-linux` | Qt 6.x | Ubuntu |
| `qt-minimal-static` | Qt 6.x (static) | Ubuntu |

---

## Usage in CI

```yaml
jobs:
  test:
    runs-on: ubuntu-latest
    container: ghcr.io/project-tick/qt-minimal-linux:latest
    steps:
      - uses: actions/checkout@v4
      - run: cmake -B build -DQUAZIP_USE_QT_ZLIB=ON
      - run: cmake --build build
```

---

## Building Images

```bash
cd ci/images
docker build -t qt-minimal-linux -f Dockerfile.linux .
```

---

## Related Documentation

- [Workflows](./workflows.md) — CI overview
- [QuaZip](./quazip.md) — Uses these images for testing
