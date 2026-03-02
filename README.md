# Falling Fury

Falling Fury is an arcade clicker game with two delivery targets:
- Native desktop game in C++ with SFML.
- Browser deployment for GitHub Pages.

## Project Structure

```text
.
├── apps
│   ├── native
│   │   ├── src/            # C++ source files (SFML)
│   │   ├── include/        # C++ headers
│   │   ├── assets/         # Fonts, sounds, textures
│   │   └── data/           # Score/leaderboard files
│   └── web
│       └── site/           # Files deployed to GitHub Pages
├── third_party
│   ├── sfml-macos/         # Local SFML frameworks for macOS
│   └── sfml-web/           # Reserved for web SFML experiments
├── .github/workflows
│   ├── cmake-single-platform.yml  # Native CI
│   └── pages.yml                  # GitHub Pages deploy
├── CMakeLists.txt
└── build.sh
```

## Native Build (SFML)

### macOS
This repository uses local frameworks from `third_party/sfml-macos`.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bin/FallingFury
```

### Linux
Install SFML first:

```bash
sudo apt-get update
sudo apt-get install -y libsfml-dev
```

Then build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bin/FallingFury
```

## Web Deployment (GitHub Pages)

1. In GitHub repo settings, open **Pages**.
2. Set **Build and deployment** source to **GitHub Actions**.
3. Push to `main`.

The workflow `.github/workflows/pages.yml` publishes `apps/web/site` automatically.

Project URL format:
- `https://<username>.github.io/<repo-name>/`

## Important SFML Note

The native desktop game is implemented with SFML in `apps/native`.
The browser deployment uses a dedicated web runtime in `apps/web/site` so it can run directly on GitHub Pages.

## Quick Start Script

```bash
./build.sh
```
