#!/usr/bin/env python3
import argparse
import re
import shlex
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

EMAIL_RE = re.compile(r"[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}")
DIFF_GIT_RE = re.compile(r"^diff --git\s+(\S+)\s+(\S+)$")
PLUS_RE = re.compile(r"^\+\+\+\s+b/(.+)$")


@dataclass
class CodeownersRule:
    pattern: str
    owners: list[str]
    matcher_kind: str
    matcher: re.Pattern[str]


@dataclass
class MaintainerRecord:
    aliases: set[str]
    emails: set[str]
    paths: list[str]


ALIAS_KEYS = {"github", "user", "username", "nick", "alias", "handle", "name"}


def normalize_path(path: str) -> str:
    p = path.strip()
    if p.startswith("\"") and p.endswith("\"") and len(p) >= 2:
        p = p[1:-1]
    if p.startswith("./"):
        p = p[2:]
    return p


def normalize_alias(token: str) -> str:
    out = token.strip().lower()
    if out.startswith("@"):
        out = out[1:]
    if "/" in out:
        out = out.split("/", 1)[1]
    out = re.sub(r"[^a-z0-9._-]+", "", out)
    return out


def glob_to_regex(glob: str) -> str:
    parts: list[str] = []
    i = 0
    while i < len(glob):
        ch = glob[i]
        if ch == "*":
            if i + 1 < len(glob) and glob[i + 1] == "*":
                while i + 1 < len(glob) and glob[i + 1] == "*":
                    i += 1
                parts.append(".*")
            else:
                parts.append("[^/]*")
        elif ch == "?":
            parts.append("[^/]")
        else:
            parts.append(re.escape(ch))
        i += 1
    return "".join(parts)


def parse_codeowners_line(line: str) -> tuple[str, list[str]] | None:
    payload = line.split("#", 1)[0].strip()
    if not payload:
        return None

    try:
        tokens = shlex.split(payload)
    except ValueError:
        tokens = payload.split()

    if len(tokens) < 2:
        return None

    return tokens[0], tokens[1:]


def compile_rule(pattern: str, owners: list[str]) -> CodeownersRule:
    pat = pattern
    if pat.endswith("/"):
        pat += "**"

    if pat.startswith("/"):
        pat = pat[1:]

    if "/" in pat:
        regex = re.compile(r"^" + glob_to_regex(pat) + r"$")
        return CodeownersRule(pattern=pattern, owners=owners, matcher_kind="path", matcher=regex)

    regex = re.compile(r"^" + glob_to_regex(pat) + r"$")
    return CodeownersRule(pattern=pattern, owners=owners, matcher_kind="component", matcher=regex)


def parse_codeowners(path: Path) -> list[CodeownersRule]:
    rules: list[CodeownersRule] = []
    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        parsed = parse_codeowners_line(raw_line)
        if not parsed:
            continue
        pattern, owners = parsed
        rules.append(compile_rule(pattern, owners))
    return rules


def match_rule(path: str, rule: CodeownersRule) -> bool:
    if rule.matcher_kind == "path":
        return bool(rule.matcher.fullmatch(path))

    parts = [p for p in path.split("/") if p]
    return any(rule.matcher.fullmatch(part) for part in parts)


def owners_for_file(path: str, rules: list[CodeownersRule]) -> list[str]:
    matched: list[str] = []
    for rule in rules:
        if match_rule(path, rule):
            matched = rule.owners
    return matched


def changed_files_from_patch_text(text: str) -> list[str]:
    files: dict[str, None] = {}

    for line in text.splitlines():
        m = DIFF_GIT_RE.match(line)
        if m:
            left = m.group(1)
            right = m.group(2)
            if left.startswith("a/"):
                left = left[2:]
            if right.startswith("b/"):
                right = right[2:]
            picked = right if right != "/dev/null" else left
            if picked and picked != "/dev/null":
                files[normalize_path(picked)] = None
            continue

        m = PLUS_RE.match(line)
        if m:
            picked = normalize_path(m.group(1))
            if picked and picked != "/dev/null":
                files[picked] = None

    return list(files.keys())


def changed_files_from_patch_file(patch_path: str) -> list[str]:
    if patch_path == "-":
        return changed_files_from_patch_text(sys.stdin.read())

    text = Path(patch_path).read_text(encoding="utf-8", errors="ignore")
    return changed_files_from_patch_text(text)


def changed_files_from_patch_files(patch_paths: list[str]) -> list[str]:
    if not patch_paths:
        return []

    if len(patch_paths) > 1 and "-" in patch_paths:
        raise RuntimeError("'-' stdin patch cannot be combined with other patch files")

    files: dict[str, None] = {}
    for patch_path in patch_paths:
        for path in changed_files_from_patch_file(patch_path):
            files[path] = None
    return list(files.keys())


def changed_files_from_git_range(git_range: str) -> list[str]:
    result = subprocess.run(
        ["git", "diff", "--name-only", git_range],
        check=False,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "git diff failed")

    files = [normalize_path(line) for line in result.stdout.splitlines() if line.strip()]
    return files


def aliases_from_maintainer_file(content: str, stem: str) -> set[str]:
    aliases: set[str] = set()
    base = normalize_alias(stem)
    if base:
        aliases.add(base)

    for line in content.splitlines():
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        key_norm = key.strip().lower()
        if key_norm not in ALIAS_KEYS:
            continue
        for token in re.split(r"[\s,]+", value.strip()):
            norm = normalize_alias(token)
            if norm:
                aliases.add(norm)
        for mention in re.findall(r"@[A-Za-z0-9._-]+", value):
            norm = normalize_alias(mention)
            if norm:
                aliases.add(norm)

    return aliases


def parse_maintainers_records(path: Path) -> list[MaintainerRecord]:
    records: list[MaintainerRecord] = []
    current_aliases: set[str] | None = None
    current_emails: set[str] | None = None
    current_paths: list[str] | None = None

    def flush_current() -> None:
        nonlocal current_aliases, current_emails, current_paths
        if current_aliases is None or current_emails is None or current_paths is None:
            return
        if not current_emails:
            current_aliases = None
            current_emails = None
            current_paths = None
            return
        if not current_paths:
            current_paths = ["**"]
        records.append(
            MaintainerRecord(aliases=set(current_aliases), emails=set(current_emails), paths=list(current_paths))
        )
        current_aliases = None
        current_emails = None
        current_paths = None

    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line:
            continue

        if line.startswith("[") and line.endswith("]"):
            flush_current()
            current_aliases = set()
            current_emails = set()
            current_paths = []
            section_name = line[1:-1].strip()
            section_alias = normalize_alias(section_name)
            if section_alias:
                current_aliases.add(section_alias)
            continue

        if line.startswith("#"):
            continue

        if current_aliases is None or current_emails is None or current_paths is None:
            continue

        payload = line.split("#", 1)[0].strip()
        if ":" not in payload:
            continue

        key, value = payload.split(":", 1)
        key_norm = key.strip().lower()
        value = value.strip()
        if not value:
            continue

        for email in EMAIL_RE.findall(value):
            current_emails.add(email.lower())

        if key_norm in {"path", "paths"}:
            for token in re.split(r"[\s,]+", value):
                candidate = token.strip()
                if candidate:
                    current_paths.append(candidate)

        if key_norm in ALIAS_KEYS:
            for token in re.split(r"[\s,]+", value):
                alias = normalize_alias(token)
                if alias:
                    current_aliases.add(alias)
            for mention in re.findall(r"@[A-Za-z0-9._-]+", value):
                alias = normalize_alias(mention)
                if alias:
                    current_aliases.add(alias)

    flush_current()
    return records


def rules_from_maintainers_file(path: Path) -> list[CodeownersRule]:
    if not path.exists() or not path.is_file():
        return []

    rules: list[CodeownersRule] = []
    for record in parse_maintainers_records(path):
        owners = [f"@{alias}" for alias in sorted(record.aliases)]
        if not owners:
            owners = sorted(record.emails)
        for pattern in record.paths:
            rules.append(compile_rule(pattern, owners))

    return rules


def build_alias_email_index(maintainers_source: Path) -> dict[str, set[str]]:
    index: dict[str, set[str]] = {}

    if not maintainers_source.exists():
        return index

    if maintainers_source.is_file():
        for record in parse_maintainers_records(maintainers_source):
            for alias in record.aliases:
                index.setdefault(alias, set()).update(record.emails)
        return index

    if not maintainers_source.is_dir():
        return index

    for entry in sorted(maintainers_source.rglob("*.txt")):
        text = entry.read_text(encoding="utf-8", errors="ignore")
        emails = {email.lower() for email in EMAIL_RE.findall(text)}
        if emails:
            aliases = aliases_from_maintainer_file(text, entry.stem)
            for alias in aliases:
                index.setdefault(alias, set()).update(emails)

    return index


def resolve_owner_emails(owner: str, alias_index: dict[str, set[str]]) -> set[str]:
    direct = {email.lower() for email in EMAIL_RE.findall(owner)}
    if direct:
        return direct

    emails: set[str] = set()
    norm = normalize_alias(owner)
    if norm in alias_index:
        emails.update(alias_index[norm])

    if owner.startswith("@") and "/" in owner:
        tail_norm = normalize_alias(owner.split("/", 1)[1])
        if tail_norm in alias_index:
            emails.update(alias_index[tail_norm])

    return emails


def choose_codeowners(explicit: str | None) -> Path | None:
    if explicit:
        return Path(explicit)

    for candidate in (Path(".github/CODEOWNERS"), Path("CODEOWNERS")):
        if candidate.exists():
            return candidate

    return None


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Find maintainer emails impacted by a patch using CODEOWNERS-like matching."
    )
    source_group = parser.add_mutually_exclusive_group(required=True)
    source_group.add_argument(
        "--patch",
        nargs="+",
        default=None,
        help="One or more patch file paths. Use '-' to read patch from stdin.",
    )
    source_group.add_argument("--git-range", default=None, help="Git diff range, e.g. HEAD~1..HEAD")
    parser.add_argument("--codeowners", default=None, help="Path to CODEOWNERS file")
    parser.add_argument(
        "--maintainers-dir",
        default="MAINTAINERS",
        help="Maintainer source path (directory with *.txt files or a MAINTAINERS file)",
    )
    parser.add_argument("--show-details", action="store_true", help="Print per-file owner and email mapping")
    parser.add_argument(
        "--strict-unresolved",
        action="store_true",
        help="Exit with code 2 if at least one owner cannot be resolved to an email",
    )
    args = parser.parse_args(argv)

    maintainers_source = Path(args.maintainers_dir)
    codeowners_path = choose_codeowners(args.codeowners)
    if codeowners_path is not None:
        if not codeowners_path.exists():
            print(f"error: CODEOWNERS file not found: {codeowners_path}", file=sys.stderr)
            return 1
        rules = parse_codeowners(codeowners_path)
        if not rules:
            print(f"error: no usable rules found in {codeowners_path}", file=sys.stderr)
            return 1
    else:
        rules = rules_from_maintainers_file(maintainers_source)
        if not rules:
            print(
                f"error: no ownership rules found. Provide --codeowners or a valid MAINTAINERS file: {maintainers_source}",
                file=sys.stderr,
            )
            return 1

    try:
        if args.git_range:
            changed_files = changed_files_from_git_range(args.git_range)
        else:
            changed_files = changed_files_from_patch_files(args.patch)
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    if not changed_files:
        return 0

    alias_index = build_alias_email_index(maintainers_source)

    all_emails: set[str] = set()
    unresolved: set[str] = set()
    unowned_files: list[str] = []
    per_file: list[tuple[str, list[str], list[str]]] = []

    for changed in changed_files:
        owners = owners_for_file(changed, rules)
        file_emails: set[str] = set()
        if not owners:
            unowned_files.append(changed)

        for owner in owners:
            resolved = resolve_owner_emails(owner, alias_index)
            if resolved:
                file_emails.update(resolved)
            elif not EMAIL_RE.search(owner):
                unresolved.add(owner)

        sorted_file_emails = sorted(file_emails)
        sorted_owners = sorted(set(owners))
        per_file.append((changed, sorted_owners, sorted_file_emails))
        all_emails.update(file_emails)

    if args.show_details:
        for changed, owners, emails in per_file:
            owners_text = ", ".join(owners) if owners else "-"
            emails_text = ", ".join(emails) if emails else "-"
            print(f"{changed}\towners={owners_text}\temails={emails_text}")

    for email in sorted(all_emails):
        print(email)

    if unowned_files:
        preview = ", ".join(unowned_files[:5])
        if len(unowned_files) > 5:
            preview += ", ..."
        print(
            f"warning: no ownership rule match for {len(unowned_files)} changed file(s): {preview}",
            file=sys.stderr,
        )

    if not all_emails:
        print(
            "warning: no maintainer emails resolved. Check ownership rules and MAINTAINERS aliases.",
            file=sys.stderr,
        )

    if unresolved:
        print(
            "warning: unresolved owners: " + ", ".join(sorted(unresolved)),
            file=sys.stderr,
        )

    if args.strict_unresolved and (unresolved or unowned_files or not all_emails):
        return 2

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
