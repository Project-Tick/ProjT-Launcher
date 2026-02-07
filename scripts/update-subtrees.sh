#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

REMOTE="${REMOTE:-origin}"
BASE_BRANCH="${BASE_BRANCH:-main}"
UPDATE_BRANCH="${UPDATE_BRANCH:-chore/update-subtrees}"
SUBTREES_FILE="${SUBTREES_FILE:-.github/subtrees.txt}"

GIT_USER_NAME="${GIT_USER_NAME:-YongDo-Hyun}"
GIT_USER_EMAIL="${GIT_USER_EMAIL:-froster12@naver.com}"

git config user.name "$GIT_USER_NAME"
git config user.email "$GIT_USER_EMAIL"

if [ ! -f "$SUBTREES_FILE" ]; then
  echo "Missing $SUBTREES_FILE"
  exit 1
fi

git fetch "$REMOTE" "$BASE_BRANCH"
git switch -C "$UPDATE_BRANCH" "$REMOTE/$BASE_BRANCH"

while read -r name path repo branch; do
  [ -z "${name:-}" ] && continue
  case "$name" in
    \#*) continue ;;
  esac
  if [ -z "${branch:-}" ]; then
    branch="main"
  fi

  # LLVM needs --squash due to its massive unrelated history
  squash_flag=""
  if [ "$name" = "llvm" ]; then
    squash_flag="--squash"
  fi

  echo "Updating $name ($path) from $repo ($branch)${squash_flag:+ [squash]}"
  if [ -d "$path" ]; then
    git subtree pull --prefix="$path" "$repo" "$branch" $squash_flag -m "chore(subtrees): update $name"
  else
    git subtree add --prefix="$path" "$repo" "$branch" $squash_flag -m "chore(subtrees): add $name"
  fi
done < "$SUBTREES_FILE"

if git diff --quiet "$REMOTE/$BASE_BRANCH"..HEAD --; then
  echo "No subtree changes detected; not pushing."
  exit 0
fi

git push "$REMOTE" "$UPDATE_BRANCH"
echo "Pushed updates to $REMOTE/$UPDATE_BRANCH"
