import os
from meta.common import ensure_component_dir, launcher_path, upstream_path
from meta.common.risugami import RISUGAMI_COMPONENT, VERSIONS_FILE
from meta.model import MetaPackage


LAUNCHER_DIR = launcher_path()
UPSTREAM_DIR = upstream_path()

ensure_component_dir(RISUGAMI_COMPONENT)


def main():
    # If an upstream versions file exists, we could parse it later.
    # For now create a minimal package.json so the meta tooling recognizes the component.
    package = MetaPackage(
        uid=RISUGAMI_COMPONENT,
        name="Risugami ModLoader",
        description="Risugami ModLoader metadata (auto-generated stub)",
    )

    package.write(os.path.join(LAUNCHER_DIR, RISUGAMI_COMPONENT, "package.json"))


if __name__ == "__main__":
    main()
