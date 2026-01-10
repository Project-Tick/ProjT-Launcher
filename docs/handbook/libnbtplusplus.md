## libnbt++ 3 `libnbtplusplus/`

This repository includes **libnbt++ 3**, the currently maintained version
used by ProjT Launcher.

Earlier versions (libnbt++ and libnbt++2) are documented here for historical
context only and are not used directly.

See forked [github repo](https://github.com/PrismLauncher/libnbtplusplus).

libnbt++ is a free C++ library for Minecraft's file format Named Binary Tag
(NBT). It can read and write compressed and uncompressed NBT files and
provides a code interface for working with NBT data.

libnbt++ 2 is a remake of the old libnbt++ library with the goal of making it
more easily usable and fixing some problems. The old libnbt++ especially
suffered from a very convoluted syntax and boilerplate code needed to work
with NBT data.

libnbt++ 3 is a remake of the old libnbt++ 2 library with the goal of fixing some
problems. The old libnbt++2 especially suffered from a very convoluted syntax and
boilerplate code needed to work with NBT data.

### Building

This project uses CMake for building. Ensure you have CMake installed.

#### Prerequisites

- C++11 compatible compiler
- CMake 3.15 or later
- ZLIB (optional, for compressed NBT support)

#### Build Steps

1. Clone the repository:

   ```bash
   git clone https://github.com/Project-Tick/ProjT-Launcher.git
   cd ProjT-Launcher/libnbtplusplus
   ```

2. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:

   ```bash
   cmake ..
   ```

   Options:
   - `NBT_BUILD_SHARED=OFF` (default): Build static library
   - `NBT_USE_ZLIB=ON` (default): Enable zlib support
   - `NBT_BUILD_TESTS=ON` (default): Build tests

4. Build:

   ```bash
   cmake --build .
   ```

5. Install (optional):

   ```bash
   cmake --install .
   ```

### Usage

Include the headers and link against the library.

#### Example

```cpp
#include <nbt_tags.h>
#include <fstream>
#include <iostream>

int main() {
    // Read an NBT file
    std::ifstream file("example.nbt", std::ios::binary);
    nbt::tag_compound root = nbt::io::read_compound(file).first;

    // Access data
    std::cout << root["some_key"].as<nbt::tag_string>() << std::endl;

    return 0;
}
```

### License

This project is licensed under the GNU General Public License v3.0. See the [COPYING](libnbtplusplus/COPYING) file for details.
