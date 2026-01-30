#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root_dir"

action_file=".github/actions/setup-dependencies/action.yml"
workflow_files=(
  ".github/workflows/ci-launcher.yml"
)

doc_files=(
  "CONTRIBUTING.md"
  "docs/contributing/GETTING_STARTED.md"
)

update_workflows="${UPDATE_QT_WORKFLOWS:-1}"
update_default="${UPDATE_QT_DEFAULT:-0}"
update_docs="${UPDATE_QT_DOCS:-0}"

current_version="$(awk '
  BEGIN { in_qt=0; update=0 }
  /^\s*qt:\s*$/ { in_qt=1; next }
  in_qt && /^\s*platform:\s*$/ { in_qt=0; update=0 }
  in_qt && /^\s*-\s*name:/ { update=0 }
  in_qt && /^\s*qt-update:\s*1/ { update=1 }
  in_qt && update && /^\s*qt-version:/ { print $2; exit }
' ".github/workflows/ci-launcher.yml" | tr -d '\r')"
if [[ -z "$current_version" ]]; then
  current_version="$(awk '/qt-version:/{found=1} found && /default:/{print $2; exit}' "$action_file" | tr -d '\r')"
fi
if [[ -z "$current_version" ]]; then
  echo "Failed to locate current Qt version in ci-launcher.yml or $action_file" >&2
  exit 1
fi

track="${current_version%.*}"
base_url="https://download.qt.io/official_releases/qt/${track}/"

html="$(curl -fsSL "$base_url")"
latest_version="$(echo "$html" | grep -oE "${track}\\.[0-9]+/" | tr -d '/' | sort -V | tail -1)"
if [[ -z "$latest_version" ]]; then
  echo "Failed to resolve latest Qt version from $base_url" >&2
  exit 1
fi

changed=false
if [[ "$latest_version" != "$current_version" ]]; then
  if [[ "$update_default" == "1" ]]; then
    tmp_file="$(mktemp)"
    cp "$action_file" "$tmp_file"
    perl -0777 -i -pe "s/(qt-version:[\\s\\S]*?default: )\\Q$current_version\\E/\\1$latest_version/" "$action_file"
    if ! cmp -s "$tmp_file" "$action_file"; then
      changed=true
    fi
    rm -f "$tmp_file"
  fi

  if [[ "$update_workflows" == "1" ]]; then
    for file in "${workflow_files[@]}"; do
      if [[ -f "$file" ]]; then
        tmp_file="$(mktemp)"
        cp "$file" "$tmp_file"
        perl -i -pe '
          if (/^\s*qt:\s*$/) { $in_qt = 1; $update = 0; next; }
          if ($in_qt && /^\s*platform:\s*$/) { $in_qt = 0; $update = 0; }
          if ($in_qt && /^\s*-\s*name:/) { $update = 0; }
          if ($in_qt && /^\s*qt-update:\s*1\b/) { $update = 1; }
          if ($in_qt && $update && /^\s*qt-version:\s*\S+/) { s/(qt-version:\s*)\S+/$1'"$latest_version"'/; }
        ' "$file"
        if ! cmp -s "$tmp_file" "$file"; then
          changed=true
        fi
        rm -f "$tmp_file"
      fi
    done
  fi

  if [[ "$update_docs" == "1" ]]; then
    for file in "${doc_files[@]}"; do
      if [[ -f "$file" ]]; then
        tmp_file="$(mktemp)"
        cp "$file" "$tmp_file"
        perl -i -pe "s/\\b\\Q$current_version\\E\\b/$latest_version/g" "$file"
        if ! cmp -s "$tmp_file" "$file"; then
          changed=true
        fi
        rm -f "$tmp_file"
      fi
    done
  fi
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "qt_version_current=$current_version"
    echo "qt_version_latest=$latest_version"
    echo "qt_version_changed=$changed"
  } >> "$GITHUB_OUTPUT"
fi

echo "Qt version: ${current_version} -> ${latest_version} (changed=${changed})"
