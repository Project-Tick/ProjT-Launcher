#!/usr/bin/env bash
# =============================================================================
# ProjT Launcher - Code Quality Validation Script
# =============================================================================
# Validates code quality between branches/commits for CI purposes
#
# Usage:
#   ./ci/code-quality.sh <base_branch> [repository]
#
# Example:
#   ./ci/code-quality.sh develop
#   ./ci/code-quality.sh master https://github.com/Project-Tick/ProjT-Launcher.git
# =============================================================================

set -o pipefail -o errexit -o nounset

trace() { echo >&2 "$@"; }

tmp=$(mktemp -d)
cleanup() {
  set +o errexit
  trace -n "Cleaning up.. "
  [[ -e "$tmp/base" ]] && git worktree remove --force "$tmp/base"
  [[ -e "$tmp/merged" ]] && git worktree remove --force "$tmp/merged"
  rm -rf "$tmp"
  trace "Done"
}
trap cleanup exit

# Default repository
repo=https://github.com/Project-Tick/ProjT-Launcher.git

# Parse arguments
if (( $# != 0 )); then
  baseBranch=$1
  shift
else
  trace "Usage: $0 BASE_BRANCH [REPOSITORY]"
  trace "BASE_BRANCH: The base branch to compare against (e.g., develop, master)"
  trace "REPOSITORY: Repository URL (defaults to $repo)"
  exit 1
fi

if (( $# != 0 )); then
  repo=$1
  shift
fi

# Check for uncommitted changes
if [[ -n "$(git status --porcelain)" ]]; then
  trace -e "\e[33mWarning: Dirty tree, uncommitted changes won't be checked\e[0m"
fi

headSha=$(git rev-parse HEAD)
trace -e "Using HEAD commit \e[34m$headSha\e[0m"

# Create worktree for HEAD
trace -n "Creating Git worktree for HEAD in $tmp/merged.. "
git worktree add --detach -q "$tmp/merged" HEAD
trace "Done"

# Fetch and create worktree for base branch
trace -n "Fetching base branch $baseBranch from $repo.. "
git fetch -q "$repo" "refs/heads/$baseBranch"
baseSha=$(git rev-parse FETCH_HEAD)
trace -e "Done (\e[34m$baseSha\e[0m)"

trace -n "Creating Git worktree for base in $tmp/base.. "
git worktree add --detach -q "$tmp/base" "$baseSha"
trace "Done"

# Run code quality checks
trace ""
trace "=== Running Code Quality Checks ==="
trace ""

# Check for clang-format
if command -v clang-format &> /dev/null; then
  trace "Checking C++ code formatting..."

  format_errors=0
  while IFS= read -r -d '' file; do
    if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
      trace "  Format issue: $file"
      ((format_errors++)) || true
    fi
  done < <(find "$tmp/merged" -type f \( -name "*.cpp" -o -name "*.h" \) -print0 2>/dev/null)

  if (( format_errors > 0 )); then
    trace -e "\e[33mFound $format_errors files with formatting issues\e[0m"
  else
    trace -e "\e[32mAll C++ files are properly formatted\e[0m"
  fi
else
  trace "clang-format not found, skipping format check"
fi

# Check for changed files
trace ""
trace "Changed files in this PR:"
git diff --name-only "$baseSha" "$headSha" | while read -r file; do
  trace "  $file"
done

# Count changes by category
trace ""
trace "Change summary:"
source_changes=$(git diff --name-only "$baseSha" "$headSha" | grep -E "^(launcher|libraries)/" | wc -l)
build_changes=$(git diff --name-only "$baseSha" "$headSha" | grep -E "(CMake|\\.cmake|vcpkg)" | wc -l)
ci_changes=$(git diff --name-only "$baseSha" "$headSha" | grep -E "^(\\.github|ci)/" | wc -l)
doc_changes=$(git diff --name-only "$baseSha" "$headSha" | grep -E "\\.md$" | wc -l)

trace "  Source files: $source_changes"
trace "  Build files:  $build_changes"
trace "  CI files:     $ci_changes"
trace "  Documentation: $doc_changes"

# Check for common issues in changed files
trace ""
trace "Checking for common issues..."

issues=0

# Check for debug statements
if git diff "$baseSha" "$headSha" -- '*.cpp' '*.h' | grep -E "^\\+.*qDebug|^\\+.*std::cout" | grep -v "// DEBUG" > /dev/null 2>&1; then
  trace -e "\e[33mWarning: New debug statements found\e[0m"
  ((issues++)) || true
fi

# Check for large files
large_files=$(git diff --name-only "$baseSha" "$headSha" | while read -r file; do
  if [[ -f "$tmp/merged/$file" ]]; then
    size=$(wc -c < "$tmp/merged/$file" 2>/dev/null || echo 0)
    if (( size > 1000000 )); then
      echo "$file ($((size/1024))KB)"
    fi
  fi
done)

if [[ -n "$large_files" ]]; then
  trace -e "\e[33mWarning: Large files detected:\e[0m"
  echo "$large_files" | while read -r line; do
    trace "  $line"
  done
  ((issues++)) || true
fi

trace ""
if (( issues > 0 )); then
  trace -e "\e[33mCode check completed with $issues warnings\e[0m"
else
  trace -e "\e[32mCode check completed successfully\e[0m"
fi

exit 0

