#!/usr/bin/env bash
set -euo pipefail

# Build Falling Fury (native desktop)
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

mkdir -p "${PROJECT_ROOT}/build"
cmake -S "${PROJECT_ROOT}" -B "${PROJECT_ROOT}/build" -DCMAKE_BUILD_TYPE=Debug
cmake --build "${PROJECT_ROOT}/build"

echo "Build complete! Run: ./build/bin/FallingFury"
