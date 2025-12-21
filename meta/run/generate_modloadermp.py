import os
from meta.common import ensure_component_dir, launcher_path, upstream_path
from meta.common.modloadermp import MODLOADERMP_COMPONENT, VERSIONS_FILE
from meta.model import MetaPackage


LAUNCHER_DIR = launcher_path()
UPSTREAM_DIR = upstream_path()

ensure_component_dir(MODLOADERMP_COMPONENT)


def main():
    package = MetaPackage(
        uid=MODLOADERMP_COMPONENT,
        name="ModLoaderMP",
        description="ModLoaderMP metadata (auto-generated stub)",
    )

    package.write(os.path.join(LAUNCHER_DIR, MODLOADERMP_COMPONENT, "package.json"))


if __name__ == "__main__":
    main()
