#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
source scripts/env.sh
brettspiel_preset="${1:-dev}"
mapfile -d '' brettspiel_files < <(find src tests -type f \( -name '*.h' -o -name '*.cpp' \) -print0)
clang-format --dry-run --Werror "${brettspiel_files[@]}"
cmake --preset "$brettspiel_preset"
cmake --build --preset "$brettspiel_preset" --parallel 4
ctest --preset "$brettspiel_preset"
if [[ "$brettspiel_preset" != asan ]]; then
  mapfile -d '' brettspiel_sources < <(find src -name '*.cpp' -print0)
  clang-tidy -p "build/$brettspiel_preset" "${brettspiel_sources[@]}" --quiet
fi
git diff --check
