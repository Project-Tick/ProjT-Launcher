#!/usr/bin/env bash
# Deprecated wrapper kept for backwards compatibility.
set -euo pipefail
echo "ci/nixpkgs-vet.sh is deprecated; use ci/code-quality.sh instead." >&2
exec "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/code-quality.sh" "$@"

