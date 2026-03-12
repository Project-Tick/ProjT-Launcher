#!/usr/bin/env python3
import os
import re
import sys
from pathlib import Path

TRUE_SET = {"1", "on", "yes", "true", "y"}


def is_true(val: str) -> bool:
    return val.strip().lower() in TRUE_SET


def main(argv):
    if len(argv) != 3:
        print("usage: gen_buildconfig.py <in> <out>", file=sys.stderr)
        return 2
    inp = Path(argv[1])
    out = Path(argv[2])
    text = inp.read_text(encoding="utf-8", errors="ignore")

    # Handle #cmakedefine01
    def repl_define(match):
        name = match.group(1)
        val = os.environ.get(name, "0")
        return f"#define {name} {1 if is_true(val) else 0}"

    text = re.sub(r"^#cmakedefine01\s+(\w+)\s*$", repl_define, text, flags=re.M)

    # Replace @VAR@ tokens
    def repl_var(match):
        key = match.group(1)
        return os.environ.get(key, "")

    text = re.sub(r"@([A-Za-z0-9_]+)@", repl_var, text)

    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(text, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
