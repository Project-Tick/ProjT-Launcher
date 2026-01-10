## Zlib `zlib/`

Zlib is a general-purpose, lossless data-compression library used for
ZIP and GZip functionality.

This repository contains a maintained fork of the upstream zlib project.
The fork exists to allow controlled integration, CI validation, and
long-term maintenance within the ProjT Launcher monorepo.

- **License**: zlib license (see `zlib/LICENSE`)
- **Upstream project**: <https://zlib.net>
- **Upstream source**: <https://github.com/madler/zlib>
- **Fork base version**: 1.3.1.2

### Fork policy

This fork aims to stay as close to upstream as possible.

- Upstream releases are periodically reviewed and merged.
- Changes are limited to build integration, CI compatibility,
  or clearly documented fixes.
- Functional divergence from upstream is avoided unless required.

Any deviations from upstream behavior are documented explicitly.

### Documentation

- API reference: `zlib/zlib.h`
- FAQ: `zlib/FAQ`
- Change history: `zlib/ChangeLog`

### Build integration

Zlib is built as part of the ProjT Launcher build.
Upstream build systems are preserved for reference, but the primary
build path is integrated into the monorepo tooling.

For upstream-specific build instructions, see <https://zlib.net>.

### Licensing

Zlib is licensed under the zlib license.
The full license text is included unmodified in `zlib/LICENSE`.

Original work:
Copyright © 1995–2025
Jean-loup Gailly, Mark Adler

Modifications:
Copyright © 2026
Project Tick contributors

### Fork policy

This fork aims to stay as close to upstream as possible.

- Upstream releases are periodically reviewed and merged.
- Changes are limited to build integration, CI compatibility,
  or clearly documented fixes.
- Functional divergence from upstream is avoided unless required.

Any deviations from upstream behavior are documented explicitly.

### Documentation

- API reference: `zlib/zlib.h`
- FAQ: `zlib/FAQ`
- Change history: `zlib/ChangeLog`

### Build integration

Zlib is built as part of the ProjT Launcher build.
Upstream build systems are preserved for reference, but the primary
build path is integrated into the monorepo tooling.

For upstream-specific build instructions, see <https://zlib.net>.

### Licensing

Zlib is licensed under the zlib license.
The full license text is included unmodified in `zlib/LICENSE`.

Original work:
Copyright © 1995–2025  
Jean-loup Gailly, Mark Adler

Modifications:
Copyright © 2026  
Project Tick contributors
