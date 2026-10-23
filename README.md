# 💎 Falling Fury

A fast-paced arcade clicker game — tap the falling targets before they escape the arena!

**[▶ Play Now](https://tamimorif.github.io/Falling-Fury/)** · iOS App Store preparation in progress · Android (Google Play) build in progress

## Gameplay

- **Green diamonds** are worth **1 point** and heal every 4th catch while you're hurt.
- **Red danger diamonds** are worth **3 points** but deal double damage if missed.
- **💀 Skull bombs** are traps — tap one and you lose 2 health. Let them fall.
- **Blue armored diamonds** take two taps (the first cracks them, no points).
- **Zippers** are small, fast zigzaggers; **splitters** pop into two mini diamonds.
- Chain rapid clicks for **combo multipliers** (x2 at 3, up to x5 at 15).
- Difficulty ramps across **waves** — every 5th wave triggers a **⚡ surge** of heavy spawns.
- Missed targets cost more health the deeper you get. Survive and beat your **high score**!

## Features

- Canvas-rendered 60 fps gameplay, DPR-aware (crisp on Retina / HiDPI)
- Fullscreen mobile layout with safe-area support, pointer/touch input, no scroll
- Auto-pause when the app goes to background + 3-2-1 resume countdown
- Hit-stop, particles, screen shake, damage vignette, floating score text
- Procedural sound effects (Web Audio API — no external audio files)
- Native iOS haptics + share sheet through the Capacitor bridge (no-op on web)
- High score saved in `localStorage`
- Optional iOS Game Center sign-in, top 10, and your rank on the opening screen
- Core gameplay is offline; the optional iOS Game Center leaderboard needs an internet connection

## Project Structure

```
├── apps/
│   ├── android/          # Capacitor Android wrapper (Google Play build)
│   │   ├── capacitor.config.ts
│   │   ├── android/      # Gradle project
│   │   └── SETUP.md      # signing + building steps
│   ├── ios/              # Capacitor iOS wrapper (App Store build)
│   │   ├── capacitor.config.ts
│   │   ├── ios/          # Xcode project (SPM)
│   │   └── SETUP.md      # signing + archiving steps
│   ├── native/           # C++ / SFML desktop build
│   └── web/
│       └── site/         # ← Deployed to GitHub Pages + bundled into the app
│           ├── index.html
│           ├── styles.css
│           ├── game.js
│           ├── native-bridge.js
│           ├── privacy.html / support.html
│           └── assets/fonts/
├── assets/appstore/      # App icon + splash artwork (generated)
├── docs/appstore/        # Listing copy, review notes, screenshots, checklist
├── .github/workflows/    # GitHub Pages deploy + CMake build
└── CMakeLists.txt
```

## Web Deployment (GitHub Pages)

1. In your repo settings, go to **Pages**.
2. Set **Build and deployment** source to **GitHub Actions**.
3. Push to `main` — the workflow deploys `apps/web/site/` automatically.

The privacy policy (`privacy.html`) and support page (`support.html`) ship with the
site so the App Store listing URLs resolve after the next deploy.

## iOS App Store

Prerequisites: Xcode + an active Apple Developer Program membership.

```bash
cd apps/ios
npm install
npx cap sync ios        # copies ../web/site into the app
npx cap open ios        # opens the Xcode project
```

Then: pick your Apple Developer **signing Team** in Xcode and
Product ▸ Archive ▸ Distribute App ▸ App Store Connect. The production icon
(`assets/appstore/icon-1024.png`), the five 6.9" screenshots
(`assets/appstore/screenshots/`), and the app-level privacy manifest are in place.
A Release build has been verified without distribution signing; the app still
needs a signed upload, physical-device Game Center verification, and refreshed
screenshots. App Store Connect privacy answers and review contact details are
saved; the category and age rating are also set.

See the checklist for the remaining signing, build upload, screenshot, and
physical-device verification steps:
[`docs/appstore/checklist.md`](docs/appstore/checklist.md) ·
[`docs/appstore/runbook.md`](docs/appstore/runbook.md)

After any change to `apps/web/site`, re-run `npm run copy` (or `npx cap copy ios`)
so the native app picks up the new files. (Android: `npx cap copy android`.)

## Google Play (Android)

Prerequisites: Android Studio (or the Android SDK) and **JDK 21** — Gradle 8.14
cannot run on this machine's default Java 25. A Google Play Console account
($25 one-time) is required to publish.

```bash
cd apps/android
npm install
npx cap sync android    # copies ../web/site into the app
npm run open            # opens the project in Android Studio
```

Release signing credentials live in `android/keystore.properties` (gitignored —
back it up together with the keystore; never commit either). To produce a
Play-ready bundle:

```bash
export JAVA_HOME="/opt/homebrew/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home"
npm run bundle          # -> android/app/build/outputs/bundle/release/app-release.aab (signed)
```

Full step-by-step (Play Console fields, declarations, required 12-tester
closed test): [`docs/playstore/checklist.md`](docs/playstore/checklist.md) ·
build details: [`apps/android/SETUP.md`](apps/android/SETUP.md)

## Native Build (C++ / SFML)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bin/FallingFury
```

## License

MIT
