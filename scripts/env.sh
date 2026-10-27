#!/usr/bin/env bash
# Optional machine-local SDK paths. No dependency on another application.
brettspiel_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -f "$brettspiel_root/.tools/env.sh" ]]; then
  source "$brettspiel_root/.tools/env.sh"
fi
if [[ -n "${BRETTSPIELREGAL_QT_ROOT:-}" ]]; then
  export CMAKE_PREFIX_PATH="$BRETTSPIELREGAL_QT_ROOT${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"
  export PATH="$BRETTSPIELREGAL_QT_ROOT/bin:$PATH"
fi
