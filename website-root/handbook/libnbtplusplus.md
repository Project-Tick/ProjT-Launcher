# libnbt++ `libnbtplusplus/`

> **Type**: NBT Format Library  
> **License**: LGPL-3.0
> **Fork Origin**: [GitHub](https://github.com/PrismLauncher/libnbtplusplus)  
> **Status**: Detached Fork (independently maintained)  
> **Format**: Minecraft Named Binary Tag
> **Latest Version**: 0.0.5-1

---

## Overview

libnbt++ is a free C++ library for Minecraft's Named Binary Tag (NBT) file format. It can read and write compressed and uncompressed NBT files and provides a code interface for working with NBT data.

### Version History

- **libnbt++ 1** — Original library (deprecated)
- **libnbt++ 2** — Rewritten with cleaner API (deprecated)
- **libnbt++ 3** — Current version used by ProjT Launcher

---

## Usage in ProjT Launcher

libnbt++ is used for:

- **World data** — Reading level.dat and player data
- **Server data** — Parsing server.dat files
- **Mod integration** — Processing NBT-based configurations
- **Backup/export** — Instance data serialization

---

## Features

| Feature | Status |
|---------|--------|
| NBT reading | ✅ |
| NBT writing | ✅ |
| GZip compression | ✅ |
| Zlib compression | ✅ |
| Big-endian support | ✅ |
| Little-endian (Bedrock) | ⚠️ Limited |

### Supported Tag Types

| Type ID | Tag Name | C++ Type |
|---------|----------|----------|
| 0 | TAG_End | — |
| 1 | TAG_Byte | `int8_t` |
| 2 | TAG_Short | `int16_t` |
| 3 | TAG_Int | `int32_t` |
| 4 | TAG_Long | `int64_t` |
| 5 | TAG_Float | `float` |
| 6 | TAG_Double | `double` |
| 7 | TAG_Byte_Array | `std::vector<int8_t>` |
| 8 | TAG_String | `std::string` |
| 9 | TAG_List | `nbt::tag_list` |
| 10 | TAG_Compound | `nbt::tag_compound` |
| 11 | TAG_Int_Array | `std::vector<int32_t>` |
| 12 | TAG_Long_Array | `std::vector<int64_t>` |

---

## Build Instructions

### Prerequisites

- C++11 compatible compiler
- CMake 3.15 or later
- ZLIB (optional, for compression support)

### Build Steps

```bash
cd libnbtplusplus
mkdir build && cd build
cmake ..
cmake --build .
```

### CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `NBT_BUILD_SHARED` | Build shared library | `OFF` |
| `NBT_USE_ZLIB` | Enable zlib support | `ON` |
| `NBT_BUILD_TESTS` | Build tests | `ON` |

---

## API Usage

### Reading NBT

```cpp
#include <nbt_tags.h>
#include <io/stream_reader.h>
#include <fstream>

std::ifstream file("level.dat", std::ios::binary);
nbt::io::stream_reader reader(file, endian::big);

// Read compound tag
auto result = reader.read_compound();
nbt::tag_compound& root = result.first;
std::string name = result.second;

// Access data
auto& level = root["Data"].as<nbt::tag_compound>();
std::string levelName = level["LevelName"].as<nbt::tag_string>();
int32_t gameType = level["GameType"].as<nbt::tag_int>();
```

### Writing NBT

{% raw %}
```cpp
#include <nbt_tags.h>
#include <io/stream_writer.h>
#include <fstream>

nbt::tag_compound data;
data["LevelName"] = nbt::tag_string("My World");
data["GameType"] = nbt::tag_int(0);  // Survival

std::ofstream file("level.dat", std::ios::binary);
nbt::io::stream_writer writer(file, endian::big);
writer.write_tag("", nbt::tag_compound{{"Data", std::move(data)}});
```
{% endraw %}

### Working with Lists

```cpp
// Create a list of strings
nbt::tag_list items(nbt::tag_type::String);
items.push_back(nbt::tag_string("item1"));
items.push_back(nbt::tag_string("item2"));

// Access list
for (const auto& item : root["Items"].as<nbt::tag_list>()) {
    std::cout << item.as<nbt::tag_string>().get() << std::endl;
}
```

---

## Testing

```bash
cd libnbtplusplus/build
ctest -V
```

---

## Documentation

| Resource | Location |
|----------|----------|
| API Headers | `libnbtplusplus/include/` |
| Tests | `libnbtplusplus/test/` |
| License | `libnbtplusplus/COPYING`, `libnbtplusplus/COPYING.LESSER` |
| Upstream Wiki | [GitHub Wiki](https://github.com/PrismLauncher/libnbtplusplus/wiki) |

---

## Copyright & Licensing

Full license: `libnbtplusplus/COPYING`

---

## Related Documentation

- [toml++](./tomlplusplus.md) — TOML configuration parser
- [cmark](./cmark.md) — Markdown parser
- [Third-party Libraries](./third-party.md) — All dependencies

---

## External Links

- [libnbt++ GitHub](https://github.com/PrismLauncher/libnbtplusplus)
- [NBT Format Wiki](https://minecraft.wiki/w/NBT_format)
- [Minecraft Data Files](https://minecraft.wiki/w/Data_file)
