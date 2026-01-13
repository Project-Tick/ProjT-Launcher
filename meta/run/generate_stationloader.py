import os
from meta.common import ensure_component_dir, launcher_path, upstream_path
from meta.common.stationloader import STATIONLOADER_COMPONENT, VERSIONS_FILE
from meta.model import MetaPackage


LAUNCHER_DIR = launcher_path()
UPSTREAM_DIR = upstream_path()

ensure_component_dir(STATIONLOADER_COMPONENT)


def main():
    package = MetaPackage(
        uid=STATIONLOADER_COMPONENT,
        name="Station Loader",
        description="Station Loader metadata (auto-generated stub)",
    )

    package.write(os.path.join(LAUNCHER_DIR, STATIONLOADER_COMPONENT, "package.json"))


if __name__ == "__main__":
    main()
