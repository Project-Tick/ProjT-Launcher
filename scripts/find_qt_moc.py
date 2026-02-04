#!/usr/bin/env python3
import sys
from pathlib import Path

KEYWORDS = ["Q_OBJECT", "Q_GADGET", "Q_NAMESPACE"]
EXTS = {".h", ".hpp", ".hh", ".hxx", ".cpp", ".cc", ".cxx"}


def scan_file(path: Path) -> bool:
    try:
        data = path.read_text(encoding="utf-8", errors="ignore")
    except Exception:
        return False
    return any(k in data for k in KEYWORDS)


def iter_files(root: Path):
    for p in root.rglob("*"):
        if not p.is_file():
            continue
        if p.suffix.lower() not in EXTS:
            continue
        parts = set(p.parts)
        if "tests" in parts or "test" in parts or "benchmarks" in parts or "examples" in parts or "docs" in parts:
            continue
        yield p


def main(argv):
    if len(argv) < 2:
        print("", end="")
        return 0
    roots = [Path(a) for a in argv[1:]]
    hits = []
    for root in roots:
        if not root.exists():
            continue
        for p in iter_files(root):
            if scan_file(p):
                hits.append(p)
    for p in hits:
        print(str(p))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
