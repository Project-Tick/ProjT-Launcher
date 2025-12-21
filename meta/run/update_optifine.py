import json
import os
import re
from urllib.parse import urljoin, urlparse, parse_qs
import concurrent.futures
import threading

try:
    from meta.common import upstream_path, ensure_upstream_dir, default_session
    from meta.common.optifine import VERSIONS_FILE, BASE_DIR
    HAVE_META = True
except Exception:
    # meta.common or its dependencies (requests) may not be available in this environment.
    HAVE_META = False
    def upstream_path():
        return "upstream"

    def ensure_upstream_dir(path):
        path = os.path.join(upstream_path(), path)
        if not os.path.exists(path):
            os.makedirs(path, exist_ok=True)

    def default_session():
        raise RuntimeError("HTTP session unavailable: install 'requests' and 'cachecontrol'")

    VERSIONS_FILE = "versions.json"
    BASE_DIR = "optifine"

UPSTREAM_DIR = upstream_path()

ensure_upstream_dir(BASE_DIR)

sess = None
if HAVE_META:
    sess = default_session()


def _resolve_href(href: str):
    """Return (filename, resolved_href).

    Handles cases where href is a redirect wrapper (e.g., adfoc.us with an inner
    'url=' parameter) or where the 'f' query parameter is present.
    """
    parsed = urlparse(href)
    q = parse_qs(parsed.query)

    # Direct f parameter
    f = q.get("f")
    if f:
        return f[0], href

    # Some wrappers embed an inner url parameter that contains the real target
    inner = q.get("url")
    if inner:
        # inner may be a list; pick first
        inner_url = inner[0]
        inner_parsed = urlparse(inner_url)
        inner_q = parse_qs(inner_parsed.query)
        inner_f = inner_q.get("f")
        if inner_f:
            return inner_f[0], inner_url

    # fallback: last path component
    return os.path.basename(parsed.path), href


def _clean_key(filename: str) -> str:
    # Remove OptiFine prefix, any trailing ad-wrapper segments, and the .jar suffix
    key = re.sub(r"^OptiFine[_-]", "", filename, flags=re.IGNORECASE)
    key = re.sub(r"\.jar$", "", key, flags=re.IGNORECASE)
    # Strip trailing ad/adload/adloadx wrapper fragments that appear in some links
    key = re.sub(r"[_-]ad[a-z0-9_-]*$", "", key, flags=re.IGNORECASE)
    return key


def _strip_ad_wrapper(filename: str) -> str:
    """Remove trailing ad/adload/adloadx wrapper fragments from a filename.

    Example: OptiFine_1.20.1_HD_U_H7_adloadx.jar -> OptiFine_1.20.1_HD_U_H7.jar
    """
    if not filename:
        return filename
    root, ext = os.path.splitext(filename)
    # remove trailing segments that start with _ad or -ad
    root = re.sub(r"[_-]ad[a-z0-9_-]*$", "", root, flags=re.IGNORECASE)
    return root + ext


def _guess_platforms(filename: str, label: str = None, changelog: str = None):
    """Heuristically guess platform compatibility tags for an OptiFine build.

    Returns a list like ['mojang', 'neoforge', 'fabric'] based on keywords.
    """
    text = " ".join(filter(None, [filename or "", label or "", changelog or ""]))
    tl = text.lower()
    platforms = []
    # OptiFine always targets vanilla (Mojang) builds
    platforms.append("mojang")
    # Forge / NeoForge variants
    if "neoforge" in tl or "neo-forge" in tl or "forge" in tl:
        platforms.append("neoforge")
    # Fabric
    if "fabric" in tl:
        platforms.append("fabric")
    # Quilt
    if "quilt" in tl:
        platforms.append("quilt")
    # LiteLoader / older loaders
    if "liteloader" in tl:
        platforms.append("liteloader")

    # Deduplicate while preserving order
    seen = set()
    out = []
    for p in platforms:
        if p not in seen:
            seen.add(p)
            out.append(p)
    return out


def _score_entry(entry: dict) -> int:
    url = (entry.get("download_page") or "").lower()
    s = 0
    if "optifine.net/adloadx" in url or "optifine.net/adload" in url or "optifine.net/download" in url:
        s += 10
    if url.endswith(".jar") or entry.get("filename", "").lower().endswith(".jar"):
        s += 5
    if "preview" in (entry.get("filename") or "").lower():
        s -= 2
    return s


def main():
    url = "https://optifine.net/downloads"
    print(f"Fetching OptiFine downloads page: {url}")
    # configurable timeouts (seconds)
    default_timeout = float(os.environ.get("OPTIFINE_TIMEOUT", "10"))

    try:
        r = sess.get(url, timeout=default_timeout)
        r.raise_for_status()
        html = r.text
    except Exception as e:
        print(f"Error fetching downloads page: {e}")
        html = ""

    versions = {}

    # Try parsing with BeautifulSoup if available; be permissive about href forms
    try:
        from bs4 import BeautifulSoup

        soup = BeautifulSoup(html, "html.parser")
        anchors = soup.find_all("a", href=True)
        inspected = 0
        matched = 0
        for a in anchors:
            inspected += 1
            href = a["href"]
            href_l = href.lower()

            # Accept several formats: any URL containing '?f=' (adload/adloadx/download), or direct .jar links
            if "?f=" not in href_l and not href_l.endswith(".jar"):
                continue

            matched += 1
            filename, resolved = _resolve_href(href)
            # strip ad/adload/adloadx wrapper parts from filename
            filename = _strip_ad_wrapper(filename)

            # Try to get version text from the same table row or nearby text
            ver_text = None
            changelog = None
            date = None
            tr = a.find_parent("tr")
            if tr:
                tds = tr.find_all("td")
                if tds:
                    ver_text = tds[0].get_text(strip=True)
                # find changelog link in the row
                ch = tr.find("a", href=lambda h: h and "changelog" in h)
                if ch:
                    changelog = ch.get("href")
                # find date cell
                date_td = tr.find("td", class_=lambda c: c and "colDate" in c)
                if date_td:
                    date = date_td.get_text(strip=True)

            if not ver_text:
                # fallback: anchor text or nearby text nodes
                if a.string and a.string.strip():
                    ver_text = a.string.strip()
                else:
                    prev = a.find_previous(string=True)
                    if prev:
                        ver_text = prev.strip()

            key = _clean_key(filename)
            data = {
                "filename": filename,
                "download_page": urljoin(url, resolved),
                "label": ver_text or filename,
                "changelog": changelog,
                "date": date,
            }

            existing = versions.get(key)
            if existing is None or _score_entry(data) > _score_entry(existing):
                versions[key] = data
                platforms = _guess_platforms(data.get("filename"), data.get("label"), data.get("changelog"))
                print(f"Added {key}: platforms: {', '.join(platforms)}")

        print(f"Inspected {inspected} anchors, matched {matched} potential downloads")
    except Exception:
        # Fallback: regex parse (case-insensitive)
        print("BeautifulSoup not available or parsing failed, falling back to regex parse")
        for match in re.finditer(r'href="([^"]*\?f=[^"\s]+)"', html, flags=re.IGNORECASE):
            href = match.group(1)
            filename, resolved = _resolve_href(href)
            filename = _strip_ad_wrapper(filename)
            key = _clean_key(filename)
            data = {
                "filename": filename,
                "download_page": urljoin(url, resolved),
                "label": filename,
            }
            existing = versions.get(key)
            if existing is None or _score_entry(data) > _score_entry(existing):
                versions[key] = data
                platforms = _guess_platforms(data.get("filename"), data.get("label"), data.get("changelog"))
                print(f"Added {key}: platforms: {', '.join(platforms)}")

    # Determine base output directory. Some upstream implementations return a
    # path that already includes BASE_DIR, avoid duplicating it.
    if UPSTREAM_DIR.endswith(BASE_DIR):
        base_out_dir = UPSTREAM_DIR
    else:
        base_out_dir = os.path.join(UPSTREAM_DIR, BASE_DIR)

    # Ensure output directory exists (defensive: collapse duplicate trailing BASE_DIR segments)
    parts = base_out_dir.split(os.sep)
    while len(parts) >= 2 and parts[-1] == BASE_DIR and parts[-2] == BASE_DIR:
        parts.pop(-1)
    base_out_dir = os.sep.join(parts)
    os.makedirs(base_out_dir, exist_ok=True)

    out_path = os.path.join(base_out_dir, VERSIONS_FILE)
    # Attempt to resolve final download URLs and optionally compute hashes
    # Default to computing SHA256 for each resolved file unless explicitly disabled
    compute_hash = os.environ.get("OPTIFINE_COMPUTE_HASH", "1").lower() in ("1", "true", "yes")
    resolved_count = 0
    hashed_count = 0

    if HAVE_META and sess is not None:
        try:
            # Use a ThreadPoolExecutor to parallelize network I/O for resolving URLs
            concurrency = int(os.environ.get("OPTIFINE_CONCURRENCY", "8"))
            if concurrency < 1:
                concurrency = 1

            total = len(versions)
            counter = {"idx": 0}
            counter_lock = threading.Lock()

            def _process_item(item):
                key, data = item
                with counter_lock:
                    counter["idx"] += 1
                    idx = counter["idx"]

                dp = data.get("download_page")
                if not dp:
                    return key, data, False, False

                print(f"[{idx}/{total}] Resolving {key} ({data.get('filename')}) -> {dp}")

                # Each worker creates its own session to avoid any session thread-safety issues
                sess_local = None
                if HAVE_META:
                    try:
                        sess_local = default_session()
                    except Exception:
                        sess_local = None

                # Fallback to global sess if default_session unavailable
                if sess_local is None:
                    sess_local = sess

                final_url = None
                try:
                    # Try HEAD first
                    try:
                        resp = sess_local.head(dp, allow_redirects=True, timeout=default_timeout)
                    except Exception as e_head:
                        # Try GET as fallback for hosts that block HEAD
                        try:
                            resp = sess_local.get(dp, allow_redirects=True, timeout=default_timeout)
                        except Exception:
                            resp = None

                    if resp is not None:
                        final_url = getattr(resp, "url", None)

                    # Try to extract downloadx link from page HTML (short GET if needed)
                    page_text = None
                    if resp is not None and hasattr(resp, "text") and resp.text:
                        page_text = resp.text
                    else:
                        try:
                            rtmp = sess_local.get(dp, allow_redirects=True, timeout=5)
                            page_text = getattr(rtmp, "text", None)
                            final_url = getattr(rtmp, "url", final_url)
                        except Exception:
                            page_text = None

                    if page_text:
                        m = re.search(r"(downloadx\?f=[^\"'\s>]+)", page_text, flags=re.IGNORECASE)
                        if m:
                            candidate = m.group(1)
                            base_for_join = final_url or dp
                            final_url = urljoin(base_for_join, candidate)
                            print(f"  Extracted downloadx link for {key}: {final_url}")

                    # If still not a .jar/f param, do a full GET and inspect final URL
                    if not final_url or (".jar" not in final_url and "?f=" not in final_url):
                        try:
                            resp2 = sess_local.get(dp, allow_redirects=True, timeout=30)
                            final_url = getattr(resp2, "url", final_url)
                        except Exception:
                            pass

                    hashed = False
                    if final_url:
                        data["resolved_url"] = final_url
                        print(f"  Resolved {key} -> {final_url}")

                        if compute_hash:
                            try:
                                import hashlib

                                print(f"  Hashing {key} from {final_url} ...")
                                h = hashlib.sha256()
                                size = 0
                                hash_timeout = float(os.environ.get("OPTIFINE_HASH_TIMEOUT", "120"))
                                r2 = sess_local.get(final_url, stream=True, timeout=hash_timeout)
                                r2.raise_for_status()
                                for chunk in r2.iter_content(8192):
                                    if not chunk:
                                        continue
                                    h.update(chunk)
                                    size += len(chunk)
                                data["sha256"] = h.hexdigest()
                                data["size"] = size
                                hashed = True
                                print(f"  Hashed {key}: sha256={data['sha256']} size={data['size']}")
                            except Exception as e_hash:
                                print(f"  Warning: failed to hash {final_url}: {e_hash}")

                    return key, data, bool(final_url), hashed
                except Exception as e:
                    print(f"  Error processing {key}: {e}")
                    return key, data, False, False

            items = list(versions.items())
            if concurrency == 1:
                # run serially
                results = map(_process_item, items)
            else:
                with concurrent.futures.ThreadPoolExecutor(max_workers=concurrency) as ex:
                    results = ex.map(_process_item, items)

            # Collect results and write per-version files as each item completes
            for key, data, resolved_flag, hashed_flag in results:
                versions[key] = data
                # Ensure per-version dir exists
                try:
                    os.makedirs(base_out_dir, exist_ok=True)
                    per_path = os.path.join(base_out_dir, f"{key}.json")
                    with open(per_path, "w") as pf:
                        json.dump(data, pf, indent=4)
                    print(f"Wrote per-version file: {per_path}")
                except Exception as e:
                    print(f"Warning: failed to write per-version file for {key}: {e}")

                if resolved_flag:
                    resolved_count += 1
                if hashed_flag:
                    hashed_count += 1
        except KeyboardInterrupt:
            print("Interrupted by user (KeyboardInterrupt). Writing partial results...")

    # Write combined index (ensure parent exists)
    os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)
    with open(out_path, "w") as f:
        json.dump(versions, f, indent=4)

    # Also write per-version JSON files under the upstream component directory
    try:
        for key, data in versions.items():
            per_path = os.path.join(base_out_dir, f"{key}.json")
            with open(per_path, "w") as pf:
                json.dump(data, pf, indent=4)
            print(f"Wrote per-version file: {per_path}")
    except Exception as e:
        print(f"Warning: failed to write per-version files: {e}")

    print(f"Wrote {len(versions)} OptiFine entries to {out_path}")
    if HAVE_META and sess is not None:
        print(f"Resolved {resolved_count} final URLs")
        if compute_hash:
            print(f"Computed {hashed_count} SHA256 hashes (OPTIFINE_COMPUTE_HASH=1)")
            # If some entries are missing sha256 (e.g., were written before hashing completed),
            # compute them now in parallel and update files.
            missing = [ (k,v) for k,v in versions.items() if v.get("resolved_url") and not v.get("sha256") ]
            if missing:
                print(f"Computing missing SHA256 for {len(missing)} entries...")
                def _compute_and_write(item):
                    k, v = item
                    url_final = v.get("resolved_url")
                    try:
                        import hashlib
                        hash_timeout = float(os.environ.get("OPTIFINE_HASH_TIMEOUT", "120"))
                        h = hashlib.sha256()
                        size = 0
                        r = sess.get(url_final, stream=True, timeout=hash_timeout)
                        r.raise_for_status()
                        for chunk in r.iter_content(8192):
                            if not chunk:
                                continue
                            h.update(chunk)
                            size += len(chunk)
                        v["sha256"] = h.hexdigest()
                        v["size"] = size
                        per_path = os.path.join(base_out_dir, f"{k}.json")
                        with open(per_path, "w") as pf:
                            json.dump(v, pf, indent=4)
                        print(f"  Hashed {k}: {v['sha256']} size={v['size']}")
                        return True
                    except Exception as e:
                        print(f"  Warning: failed to compute hash for {k}: {e}")
                        return False

                concurrency = int(os.environ.get("OPTIFINE_CONCURRENCY", "8"))
                if concurrency < 1:
                    concurrency = 1
                completed = 0
                with concurrent.futures.ThreadPoolExecutor(max_workers=concurrency) as ex:
                    for ok in ex.map(_compute_and_write, missing):
                        if ok:
                            completed += 1
                print(f"Completed extra hashing: {completed}/{len(missing)}")


if __name__ == "__main__":
    main()
