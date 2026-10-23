#!/bin/sh
#
# Xcode Cloud post-clone setup for Falling Fury.
#
# Capacitor's generated Swift package (apps/ios/ios/App/CapApp-SPM/Package.swift)
# references the local node_modules packages for Haptics, Share, and StatusBar.
# node_modules is gitignored, so a fresh Xcode Cloud clone does not contain it
# and SwiftPM fails with "cannot be accessed ... doesn't exist in file system".
#
# This script installs the JS dependencies and regenerates the Capacitor iOS
# project (web assets, capacitor.config.json, and the SPM manifest) before Xcode
# resolves packages and builds.
set -eux

REPO_ROOT="${CI_PRIMARY_REPOSITORY_PATH:-$(cd "$(dirname "$0")/.." && pwd)}"

if ! command -v node >/dev/null 2>&1; then
  echo "Node.js not found; installing via Homebrew"
  brew install node
fi

node --version
npm --version

cd "$REPO_ROOT/apps/ios"

npm ci
npx cap sync ios
