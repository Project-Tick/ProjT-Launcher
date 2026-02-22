#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root_dir"

action_file=".github/actions/setup-dependencies/action.yml"
workflow_files=(
  ".gitlab-ci.yml"
  ".gitlab/ci/github-workflow-parity.yml"
  ".github/workflows/ci-launcher.yml"
)

doc_files=(
  "CONTRIBUTING.md"
  "docs/contributing/GETTING_STARTED.md"
)

current_version="$(awk '/qt-version:/{found=1} found && /default:/{print $2; exit}' "$action_file" | tr -d '\r')"
if [[ -z "$current_version" ]]; then
  echo "Failed to locate current Qt version in $action_file" >&2
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
  perl -0777 -i -pe "s/(qt-version:[\s\S]*?default: )\Q$current_version\E/\1$latest_version/" "$action_file"

  for file in "${workflow_files[@]}"; do
    if [[ -f "$file" ]]; then
      perl -i -pe "s/(qt-version:\\s*)\\Q$current_version\\E/\\1$latest_version/g; s/(QT_VERSION:\\s*\\\")\\Q$current_version\\E(\\\")/\\1$latest_version\\2/g; s/(qt_version:\\s*\\\")\\Q$current_version\\E(\\\")/\\1$latest_version\\2/g" "$file"
    fi
  done

  for file in "${doc_files[@]}"; do
    if [[ -f "$file" ]]; then
      perl -i -pe "s/\\b\\Q$current_version\\E\\b/$latest_version/g" "$file"
    fi
  done
  changed=true
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "qt_version_current=$current_version"
    echo "qt_version_latest=$latest_version"
    echo "qt_version_changed=$changed"
  } >> "$GITHUB_OUTPUT"
fi

echo "Qt version: ${current_version} -> ${latest_version} (changed=${changed})"
