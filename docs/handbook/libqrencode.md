# libqrencode `libqrencode/`

> **Type**: QR Code Generation Library  
> **License**: LGPL-2.1-or-later  
> **Fork Origin**: [GitHub](https://github.com/fukuchi/libqrencode)  
> **Status**: Detached Fork (independently maintained)  
> **Standards**: ISO/IEC 18004:2015
> **Latest Version**: 0.0.5-1

---

## Overview

libqrencode is a fast and compact library for encoding data into a QR Code symbol. It supports the full range of QR Code functionality as defined by the ISO standard.

ProjT Launcher maintains a fork of libqrencode for controlled integration and monorepo CI compatibility.

---

## Usage in ProjT Launcher

libqrencode is used for:

- **Account linking** — Microsoft account QR authentication
- **Share URLs** — Quick sharing of instance/mod links
- **Export/Import** — Instance configuration sharing

---

## Features

| Feature | Status |
|---------|--------|
| QR Code Model 1 & 2 | ✅ |
| Micro QR Code | ✅ |
| All error correction levels | ✅ |
| Structured append | ✅ |
| 8-bit byte mode | ✅ |
| Kanji mode | ✅ |
| ECI mode | ✅ |

### Error Correction Levels

| Level | Recovery | Use Case |
|-------|----------|----------|
| L | ~7% | High density |
| M | ~15% | Standard (default) |
| Q | ~25% | Industrial |
| H | ~30% | Critical data |

---

## Build Instructions

### Prerequisites

- **CMake 3.10+** or **Autotools**
- **C99 compiler** (GCC, Clang, MSVC)
- **libpng** (optional, for PNG output)

### CMake Build

```bash
cd libqrencode
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest -V
```

### Autotools Build

```bash
cd libqrencode
./autogen.sh
./configure
make
make check
```

### CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `WITH_TOOLS` | Build qrencode CLI tool | `ON` |
| `WITH_PNG` | Enable PNG support | `ON` |
| `WITH_TESTS` | Build test suite | `ON` |
| `BUILD_SHARED_LIBS` | Build shared library | `ON` |

---

## API Usage

### Basic Example

```c
#include <qrencode.h>
#include <stdio.h>

int main() {
    QRcode *qr = QRcode_encodeString("https://projecttick.org",
                                      0,                    // Version (0 = auto)
                                      QR_ECLEVEL_M,         // Error correction
                                      QR_MODE_8,            // Encoding mode
                                      1);                   // Case sensitive

    if (qr == NULL) {
        fprintf(stderr, "Failed to encode\n");
        return 1;
    }

    // Access the QR code data
    printf("Version: %d\n", qr->version);
    printf("Width: %d\n", qr->width);

    // qr->data contains the QR code matrix
    // Each byte: bit 0 = module (1=black, 0=white)

    QRcode_free(qr);
    return 0;
}
```

### Structured Append (Large Data)

```c
// Split data across multiple QR codes
QRcode_List *list = QRcode_encodeStringStructured(
    large_data, 0, QR_ECLEVEL_M, QR_MODE_8, 1);

for (QRcode_List *entry = list; entry != NULL; entry = entry->next) {
    // Process each QR code in sequence
    process_qrcode(entry->code);
}

QRcode_List_free(list);
```

---

## Command Line Tool

The `qrencode` CLI tool creates QR code images:

```bash
# Generate PNG
qrencode -o output.png "Hello World"

# Generate SVG
qrencode -t SVG -o output.svg "https://projecttick.org"

# Generate ASCII art
qrencode -t ASCII "Hello World"

# Specify error correction level
qrencode -l H -o secure.png "Sensitive Data"
```

### Output Formats

- PNG (requires libpng)
- EPS
- SVG
- ANSI terminal
- ASCII art
- UTF-8 terminal

---

## Documentation

| Resource | Location |
|----------|----------|
| API Reference | `libqrencode/qrencode.h` |
| Man Page | `libqrencode/qrencode.1.in` |
| [Original README](../../libqrencode/README.md) | Upstream library documentation |

---

## Testing

```bash
cd libqrencode/build
ctest -V

# Or with autotools
cd libqrencode
make check
```

See [ci-libqrencode.yml](../../.github/workflows/ci-libqrencode.yml) for CI configuration.

---

## Copyright & Licensing

```
Copyright (C) 2006-2017 Kentaro Fukuchi <kentaro@fukuchi.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
```

Full license: `libqrencode/COPYING`

---

## Related Documentation

- [Third-party Libraries](./third-party.md) — All dependencies

---

## External Links

- [libqrencode GitHub](https://github.com/fukuchi/libqrencode)
- [ISO/IEC 18004](https://www.iso.org/standard/62021.html) — QR Code standard
- [QR Code Wikipedia](https://en.wikipedia.org/wiki/QR_code)
- [ZXing](https://github.com/zxing/zxing) — QR Code reading library
