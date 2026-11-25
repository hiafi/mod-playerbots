#!/usr/bin/env bash
# Custom: builds and runs the standalone host tests of this module's pure headers with every available host compiler.
# Not part of the module build (the core compiles src/ only). The headers use the core's enums, so the core source is
# needed: CORE, or the core this module is checked out in.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(dirname "$here")"
core="${CORE:-$(cd "$root/../.." && pwd)}"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT

if [[ ! -f "$core/src/server/shared/SharedDefines.h" ]]; then
  echo "no core source at $core (set CORE)" >&2
  exit 1
fi

compilers=()
if [[ -n "${CXX:-}" ]]; then
  compilers=("$CXX")
else
  for c in g++ clang++; do
    if command -v "$c" > /dev/null; then
      compilers+=("$c")
    fi
  done
fi

# Core headers as -isystem: -Werror is for this module's code
includes=(-I "$root/src/Ai/Base/Util" -I "$here" -isystem "$core/src/common" -isystem "$core/src/server/shared"
  -isystem "$core/src/server/shared/DataStores" -isystem "$core/src/common/Utilities")

tests=(
  leap_trinket_rules_test
)

status=0
for cxx in "${compilers[@]}"; do
  for t in "${tests[@]}"; do
    echo "== $cxx $t"
    "$cxx" -std=c++20 -Wall -Wextra -Werror -O1 "${includes[@]}" "$here/$t.cpp" -o "$out/$t-$cxx"
    if ! "$out/$t-$cxx"; then
      status=1
    fi
  done
done
exit $status
