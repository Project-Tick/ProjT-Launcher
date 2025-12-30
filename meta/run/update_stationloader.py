import json
import os

from meta.common import upstream_path, ensure_upstream_dir, default_session
from meta.common.stationloader import VERSIONS_FILE, BASE_DIR

UPSTREAM_DIR = upstream_path()

ensure_upstream_dir(BASE_DIR)

sess = default_session()


def main():
    # Placeholder updater: upstream source not implemented yet.
    out_path = os.path.join(UPSTREAM_DIR, VERSIONS_FILE)
    with open(out_path, "w") as f:
        json.dump({}, f, indent=4)

    print(f"Wrote placeholder upstream file: {out_path}")


if __name__ == "__main__":
    main()
