#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
source scripts/env.sh
brettspiel_preset="${1:-dev}"
brettspiel_files=()
while IFS= read -r -d '' brettspiel_file; do
  brettspiel_files+=("$brettspiel_file")
done < <(find src tests -type f \( -name '*.h' -o -name '*.cpp' \) -print0)
clang-format --dry-run --Werror "${brettspiel_files[@]}"
cmake --preset "$brettspiel_preset"
cmake --build --preset "$brettspiel_preset" --parallel 4
ctest --preset "$brettspiel_preset"
if [[ "$brettspiel_preset" != asan ]]; then
  if command -v clang-tidy >/dev/null 2>&1; then
    brettspiel_sources=()
    while IFS= read -r -d '' brettspiel_source; do
      brettspiel_sources+=("$brettspiel_source")
    done < <(find src -name '*.cpp' -print0)
    clang-tidy -p "build/$brettspiel_preset" "${brettspiel_sources[@]}" --quiet
  elif [[ "$(uname -s)" == Darwin ]]; then
    python3 scripts/analyze_macos.py "build/$brettspiel_preset/compile_commands.json"
  else
    echo "clang-tidy is required for static analysis" >&2
    exit 1
  fi
fi
git diff --check
