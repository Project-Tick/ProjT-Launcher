# Bzip2 Test Files

> **Purpose**: Test file collection for bzip2 validation
> **Latest Version**: 0.0.5-1

---

## Overview

A collection of `.bz2` files used to test bzip2 functionality. Files come from various projects and cover edge cases.

---

## Directory Contents

Each test directory contains:

| File Type | Description |
|-----------|-------------|
| `README` | Source and license information |
| `*.bz2` | Compressed files (should work) |
| `*.md5` | Hash of original file |
| `*.bz2.bad` | Corrupt files (should fail) |

---

## File Types

### Good Files (`.bz2`)

Compressed files that should decompress correctly. Each has an `.md5` file for verification.

### Bad Files (`.bz2.bad`)

Deliberately corrupt files to test error handling. These should **not** decompress successfully.

---

## Verification

### Generate MD5

```bash
md5sum < file > file.md5
```

### Verify MD5

```bash
md5sum --check file.md5 < file
```

---

## Adding Test Files

### Good Files

1. Create the `.bz2` file
2. Generate `.md5`: `md5sum < original > original.md5`
3. Add to appropriate directory
4. Update README with source

### Bad Files

1. Create corrupt `.bz2` file
2. Name with `.bz2.bad` extension
3. Document the corruption type

---

## ⚠️ Security Notice

> **Vulnerability Reporting**
>
> If you find a file that causes crashes, buffer overflows, or security issues:
>
> 1. **Do NOT share publicly**
> 2. Report confidentially to maintainers
> 3. Allow 60-90 days for fix before disclosure

---

## Credits

Test files contributed by:
- Apache Commons Compress
- DotNetZip Library
- Go Lang project
- lbzip2 project
- pyflate project

Thanks to Mark Wielaard for assembling the collection.

---

## Related Documentation

- [Bzip2 Overview](./bzip2.md) — Main documentation
- [Bzip2 Tests](./bzip2-tests.md) — Test suite
- [Compiling](./bzip2-compiling.md) — Build instructions
