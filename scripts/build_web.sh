#!/usr/bin/env bash
set -euo pipefail

# Build Falling Fury for the web (Emscripten)
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="${PROJECT_ROOT}/web"

mkdir -p "${OUT_DIR}"

em++ \
  "${PROJECT_ROOT}/src/core/main.cpp" \
  "${PROJECT_ROOT}/src/core/Game.cpp" \
  -I"${PROJECT_ROOT}/include" \
  -std=c++17 \
  -O3 \
  -s USE_SFML2=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  --preload-file "${PROJECT_ROOT}/assets@assets" \
  --preload-file "${PROJECT_ROOT}/data@data" \
  -o "${OUT_DIR}/index.html"

echo "Web build complete! Output: ${OUT_DIR}/index.html"
