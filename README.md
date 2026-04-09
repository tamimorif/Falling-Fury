# 💎 Falling Fury

A fast-paced arcade clicker game — click the falling diamond targets before they escape the arena!

**[▶ Play Now](https://tamimorif.github.io/Falling-Fury/)**

## Gameplay

- **Green diamonds** are worth **1 point** — click them to score and heal.
- **Red danger diamonds** are worth **3 points** but deal double damage if missed.
- Chain rapid clicks for **combo multipliers** (x2, x3, x4).
- Difficulty ramps up across **waves** — enemies fall faster and spawn more frequently.
- Survive as long as you can and beat your **high score**!

## Features

- Canvas-rendered smooth 60 fps gameplay
- DPR-aware rendering (crisp on Retina / HiDPI)
- Particle effects & floating score text
- Combo system with multiplier
- Screen shake on damage
- Animated health bar & wave indicator
- Procedural sound effects (Web Audio API — no external audio files)
- Touch support for mobile
- High score saved in `localStorage`
- Zero external dependencies

## Project Structure

```
├── apps/
│   ├── native/          # C++ / SFML desktop build
│   │   ├── src/
│   │   ├── include/
│   │   └── assets/
│   └── web/
│       └── site/        # ← Deployed to GitHub Pages
│           ├── index.html
│           ├── styles.css
│           └── game.js
├── .github/workflows/
│   ├── pages.yml        # GitHub Pages deploy
│   └── cmake-single-platform.yml
├── CMakeLists.txt
└── README.md
```

## Deployment (GitHub Pages)

1. In your repo settings, go to **Pages**.
2. Set **Build and deployment** source to **GitHub Actions**.
3. Push to `main` — the workflow deploys `apps/web/site/` automatically.

## Native Build (C++ / SFML)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bin/FallingFury
```

## License

MIT
