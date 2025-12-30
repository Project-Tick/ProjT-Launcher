import os
import json
from datetime import datetime
from typing import List

from meta.common import ensure_component_dir, launcher_path, upstream_path
from meta.common.optifine import OPTIFINE_COMPONENT, VERSIONS_FILE, OPTIFINE_UPSTREAM_DIR
from meta.model import MetaPackage, MetaVersion, Library, MojangLibraryDownloads, MojangArtifact


LAUNCHER_DIR = launcher_path()
UPSTREAM_DIR = upstream_path()

ensure_component_dir(OPTIFINE_COMPONENT)


def _parse_date(d: str):
    # dates on the site are like DD.MM.YYYY
    try:
        return datetime.strptime(d, "%d.%m.%Y")
    except Exception:
        return None


def main():
    # Prefer per-version files in the upstream component directory, fallback to combined versions.json
    # upstream files live under `upstream/optifine`, launcher metadata should go under `launcher/net.optifine`
    comp_dir = os.path.join(UPSTREAM_DIR, OPTIFINE_UPSTREAM_DIR)
    entries = {}
    if os.path.isdir(comp_dir):
        files = [f for f in os.listdir(comp_dir) if f.endswith(".json")]
        # If there are many per-version files (excluding the combined file), read them
        per_files = [f for f in files if f != VERSIONS_FILE]
        if per_files:
            for fn in per_files:
                path = os.path.join(comp_dir, fn)
                try:
                    with open(path, "r", encoding="utf-8") as f:
                        data = json.load(f)
                    key = os.path.splitext(fn)[0]
                    entries[key] = data
                except Exception:
                    print(f"Warning: failed to read upstream per-version file: {path}")
    # fallback to combined index
    if not entries:
        src = os.path.join(UPSTREAM_DIR, VERSIONS_FILE)
        if not os.path.exists(src):
            print(f"Missing upstream file: {src}")
            return
        with open(src, "r", encoding="utf-8") as f:
            entries = json.load(f)

    versions: List[str] = []
    parsed_versions = []

    for key, data in entries.items():
        # key already normalized by the updater
        v = MetaVersion(name="OptiFine", uid=OPTIFINE_COMPONENT, version=key)
        v.type = "release"
        v.order = 10

        filename = data.get("filename")
        download_page = data.get("download_page")
        resolved = data.get("resolved_url") or download_page
        label = data.get("label")
        changelog = data.get("changelog")
        date = data.get("date")
        size = data.get("size")
        sha256 = data.get("sha256")

        # attach jar mod as a simple artifact entry; prefer resolved_url and include sha256/size
        lib = Library()
        artifact_kwargs = {}
        if resolved:
            artifact_kwargs["url"] = resolved
        else:
            artifact_kwargs["url"] = download_page
        if size is not None:
            artifact_kwargs["size"] = size
        if sha256 is not None:
            artifact_kwargs["sha256"] = sha256

        artifact = MojangArtifact(**artifact_kwargs)
        lib.downloads = MojangLibraryDownloads(artifact=artifact)

        v.jar_mods = [lib]
        if label:
            v.name = label

        if date:
            dt = _parse_date(date)
            if dt:
                v.release_time = dt

        v.write(os.path.join(LAUNCHER_DIR, OPTIFINE_COMPONENT, f"{v.version}.json"))
        parsed_versions.append((v.version, v.release_time))

    # choose recommended: latest non-preview by release_time if available
    parsed_versions.sort(key=lambda x: (x[1] or datetime.min), reverse=True)
    recommended = [p[0] for p in parsed_versions[:3]]

    package = MetaPackage(uid=OPTIFINE_COMPONENT, name="OptiFine")
    package.recommended = recommended
    package.description = "OptiFine installer and downloads"
    package.project_url = "https://optifine.net"
    package.write(os.path.join(LAUNCHER_DIR, OPTIFINE_COMPONENT, "package.json"))


if __name__ == "__main__":
    main()
