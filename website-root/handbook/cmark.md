# cmark `cmark/`

> **Type**: Markdown Parser Library  
> **License**: BSD-2-Clause (+ MIT for some components)  
> **Fork Origin**: [GitHub](https://github.com/commonmark/cmark)  
> **Status**: Detached Fork (independently maintained)  
> **Specification**: [CommonMark](https://commonmark.org/)
> **Latest Version**: 0.0.5-1

---

## Overview

`cmark` is the C reference implementation of [CommonMark](https://commonmark.org/), a strongly specified, highly compatible version of Markdown. It provides both a shared library (`libcmark`) and a standalone program for parsing and rendering CommonMark documents.

ProjT Launcher maintains a fork of cmark for controlled integration, CI validation, and monorepo compatibility.

---

## Usage in ProjT Launcher

cmark is used for:

- **Mod descriptions** — Rendering mod README files
- **News display** — Formatting launcher news and announcements
- **Instance notes** — User-created markdown notes
- **Wiki integration** — Processing documentation

---

## Features

| Feature | Status |
|---------|--------|
| CommonMark 0.31 compliant | ✅ |
| Streaming/iterating parser | ✅ |
| Safe HTML rendering | ✅ |
| UTF-8 support | ✅ |
| Multiple output formats | ✅ |
| Extensible architecture | ✅ |

### Output Formats

- HTML
- Groff man pages
- CommonMark (normalized)
- XML
- LaTeX

---

## Build Instructions

### Prerequisites

- **CMake 3.12+**
- **C99 compiler** (GCC, Clang, MSVC)
- **Python 3.6+** (for tests)

### Quick Build

```bash
cd cmark
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest -V
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `CMARK_TESTS` | Build test suite | `ON` |
| `CMARK_SHARED` | Build shared library | `ON` |
| `CMARK_STATIC` | Build static library | `OFF` |
| `CMARK_LIB_FUZZER` | Build libFuzzer harness | `OFF` |

---

## API Usage

### Basic Example

```c
#include <cmark.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *markdown = "# Hello World\n\nThis is **CommonMark**!";
    char *html = cmark_markdown_to_html(markdown, strlen(markdown), CMARK_OPT_DEFAULT);
    printf("%s", html);
    free(html);
    return 0;
}
```

### Streaming Parser

```c
#include <cmark.h>

cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);
cmark_parser_feed(parser, chunk1, strlen(chunk1));
cmark_parser_feed(parser, chunk2, strlen(chunk2));
cmark_node *document = cmark_parser_finish(parser);

char *html = cmark_render_html(document, CMARK_OPT_DEFAULT);

cmark_node_free(document);
cmark_parser_free(parser);
free(html);
```

---

## Safety Features

cmark is designed to be safe by default:

- ✅ **No buffer overflows** — Extensively fuzzed
- ✅ **HTML sanitization** — Safe mode available
- ✅ **Memory safety** — No use-after-free bugs
- ✅ **Predictable output** — Spec-compliant behavior

### Safe Rendering Options

```c
// Use CMARK_OPT_SAFE to prevent raw HTML passthrough
char *html = cmark_markdown_to_html(md, len, CMARK_OPT_SAFE);
```

---

## Documentation

| Resource | Location |
|----------|----------|
| API Reference | `cmark/src/cmark.h` |
| Man Page | `cmark/man/man3/cmark.3` |
| [Changelog](../../cmark/changelog.txt) | Upstream change history |
| CommonMark Spec | [spec.commonmark.org](https://spec.commonmark.org/) |

---

## Copyright & Licensing

```
Copyright (c) 2014, John MacFarlane

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice.
2. Redistributions in binary form must reproduce the above copyright notice.

THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY.
```

Full license: `cmark/COPYING`

---

## Related Documentation

- [toml++](./tomlplusplus.md) — TOML parser (similar use case)
- [libnbt++](./libnbtplusplus.md) — NBT format parser
- [Third-party Libraries](./third-party.md) — All dependencies

---

## External Links

- [cmark GitHub](https://github.com/commonmark/cmark)
- [CommonMark Specification](https://commonmark.org/)
- [CommonMark Dingus](https://spec.commonmark.org/dingus/) — Online tester
- [cmark-gfm](https://github.com/github/cmark-gfm) — GitHub's extended fork
