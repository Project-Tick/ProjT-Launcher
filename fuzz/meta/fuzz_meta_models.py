import json
import os
import sys

import atheris

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if REPO_ROOT not in sys.path:
    sys.path.insert(0, REPO_ROOT)

from meta.model import MetaPackage, MetaVersion  # noqa: E402


def test_one_input(data: bytes) -> None:
    try:
        text = data.decode("utf-8", errors="ignore")
    except Exception:
        return

    if not text:
        return

    try:
        json.loads(text)
    except Exception:
        return

    try:
        MetaPackage.parse_raw(text)
    except Exception:
        pass

    try:
        MetaVersion.parse_raw(text)
    except Exception:
        pass


def main() -> None:
    atheris.Setup(sys.argv, test_one_input)
    atheris.Fuzz()


if __name__ == "__main__":
    main()
