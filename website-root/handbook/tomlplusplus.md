# toml++ `tomlplusplus/`

> **Type**: TOML Parser Library  
> **License**: MIT  
> **Fork Origin**: [GitHub](https://github.com/marzer/tomlplusplus)  
> **Status**: Detached Fork (independently maintained)  
> **Specification**: [TOML v1.0.0](https://toml.io/en/v1.0.0)
> **Latest Version**: 0.0.5-1

[![CI](https://github.com/Project-Tick/ProjT-Launcher/actions/workflows/ci-new.yml/badge.svg)](https://github.com/Project-Tick/ProjT-Launcher/actions/workflows/ci-new.yml)

---

## Overview

toml++ is a header-only TOML configuration file parser and serializer for C++17 and later. It provides a clean, intuitive API for reading and writing TOML files with full TOML v1.0.0 compliance.

ProjT Launcher maintains a detached fork for independent development and CI integration.

---

## Usage in ProjT Launcher

toml++ is used for:

- **Configuration files** — Reading launcher settings
- **Instance configs** — Parsing instance metadata
- **Mod manifests** — Processing `fabric.mod.json` alternatives
- **Pack metadata** — Reading modpack configuration

---

## Features

| Feature | Status |
|---------|--------|
| TOML v1.0.0 compliant | ✅ |
| Header-only library | ✅ |
| C++17/20/23 support | ✅ |
| Unicode support | ✅ |
| Serialize to TOML | ✅ |
| Parse from string/file | ✅ |
| Error messages with line numbers | ✅ |
| Compile-time optimizations | ✅ |

---

## Quick Start

### Single Header Include

```cpp
#include <toml++/toml.hpp>
```

### Parse TOML

```cpp
// From string
auto config = toml::parse(R"(
    [server]
    host = "localhost"
    port = 8080
)");

// From file
auto config = toml::parse_file("config.toml");

// Access values
std::string host = config["server"]["host"].value_or("127.0.0.1");
int port = config["server"]["port"].value_or(80);
```

### Create TOML

{% raw %}
```cpp
auto tbl = toml::table{{
    {"server", toml::table{{
        {"host", "localhost"},
        {"port", 8080}
    }}}
}};

std::cout << tbl << std::endl;
```
{% endraw %}

---

## Build Integration

### CMake (Recommended)

toml++ is header-only, but provides a CMake target:

```cmake
# Add as subdirectory
add_subdirectory(tomlplusplus)

# Or find installed package
find_package(tomlplusplus REQUIRED)

# Link to your target
target_link_libraries(your_target PRIVATE tomlplusplus::tomlplusplus)
```

### Include Directly

```cmake
target_include_directories(your_target PRIVATE path/to/tomlplusplus/include)
```

### Compiler Requirements

| Compiler | Minimum Version |
|----------|-----------------|
| GCC | 8+ |
| Clang | 8+ |
| MSVC | 19.20+ (VS 2019) |
| ICC | 19.11+ |

---

## Configuration Options

Define before including to customize behavior:

```cpp
// Disable exceptions
#define TOML_EXCEPTIONS 0

// Disable optional features
#define TOML_ENABLE_FORMATTERS 0
#define TOML_ENABLE_PARSER 0  // Write-only mode

// Unicode handling
#define TOML_ASSUME_UTF8_EVERYWHERE 1
```

---

## Error Handling

### With Exceptions (Default)

```cpp
try {
    auto config = toml::parse_file("config.toml");
} catch (const toml::parse_error& err) {
    std::cerr << "Parse error at " << err.source().begin << ":\n"
              << err.description() << std::endl;
}
```

### Without Exceptions

```cpp
#define TOML_EXCEPTIONS 0

toml::parse_result result = toml::parse_file("config.toml");
if (!result) {
    std::cerr << "Parse error: " << result.error() << std::endl;
    return;
}
toml::table& config = result.table();
```

---

## Testing

toml++ includes a comprehensive test suite:

```bash
cd tomlplusplus
mkdir build && cd build
cmake .. -DTOML_BUILD_TESTS=ON
cmake --build .
ctest -V
```

See [ci-tomlplusplus.yml](../../.github/workflows/ci-tomlplusplus.yml) for CI configuration.

---

## Documentation

| Resource | Description |
|----------|-------------|
| [API Docs](https://projecttick.org/tomlplusplus/) | Full API reference |
| [TOML Spec](https://toml.io/en/v1.0.0) | TOML v1.0.0 specification |
| [Original README](../../tomlplusplus/README.md) | Upstream library documentation |
| [Website Build](./website-tomlplusplus.md) | Building API docs |

---

## Copyright & Licensing

```
Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software to deal in the Software without restriction, including the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
```

Full license: `tomlplusplus/LICENSE`

---

## Related Documentation

- [cmark](./cmark.md) — Markdown parser (similar use case)
- [libnbt++](./libnbtplusplus.md) — NBT format parser
- [Website toml++ Docs](./website-tomlplusplus.md) — Building documentation
- [Third-party Libraries](./third-party.md) — All dependencies

---

## External Links

- [toml++ GitHub](https://github.com/marzer/tomlplusplus)
- [TOML Official Site](https://toml.io/)
- [toml++ Online Docs](https://marzer.github.io/tomlplusplus/)
- [TOML Test Suite](https://github.com/BurntSushi/toml-test)
