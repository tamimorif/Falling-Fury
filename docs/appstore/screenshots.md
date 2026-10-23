# Falling Fury — App Store Screenshots

## Final set (committed 2026-10-17)

The required 6.9" set is ready in [`assets/appstore/screenshots/`](../assets/appstore/screenshots/):

| File | Shot | Caption |
|---|---|---|
| `01-start.png` | Start screen | *Tap targets. Survive waves. Beat your score.* |
| `02-early-game.png` | Early gameplay | *Green gems score and heal — red gems pay triple.* |
| `03-mid-game-combo.png` | Combo + danger | *Chain fast taps for x2 … x5 combo multipliers.* |
| `04-wave-surge.png` | Wave 5 / surge | *Every wave is faster. How long can you last?* |
| `05-gameover.png` | New high score | *New high score! One more run?* |

All five are **1320 × 2868**, RGB, no alpha, no device frames or added text. They were
rendered from the exact web build that ships in the app, at 440 × 956 CSS pixels with a
3× device pixel ratio and iPhone Pro Max safe-area insets.

> App Store Connect has **no per-screenshot caption field**. The captions above are the
> intended messaging/order; they are not typed into App Store Connect. If you want caption
> text on the images themselves, add it to the PNGs before uploading.

## Required sizes

| Display size | Resolution (portrait) | Required? | Notes |
|---|---|---|---|
| iPhone 6.9" | **1320 × 2868** | **Yes — required** | iPhone 16/15 Pro Max class. App Store Connect will scale this set down for smaller iPhones. |
| iPhone 6.5" | 1242 × 2688 | Optional | Only upload if you want a hand-picked set for older 6.5" devices; otherwise skip. |
| iPad 13" / 12.9" | — | Not needed | This release is **iPhone-only**; do not submit iPad screenshots or iPad builds. |

Format: PNG or JPG, RGB, no alpha channel, no rounded corners or device frames (Apple applies those). 1–10 screenshots per size. Suggested final storage: `assets/appstore/screenshots/`.

## Capture instructions (only needed when the UI changes)

1. Build and run the app in Xcode: from `apps/ios`, run `npx cap open ios`, select an **iPhone 18 Pro Max** simulator (1320 × 2868), then Run (⌘R).
2. Play to the moment you want, then capture:
   - **Simulator menu:** Device > Save Screen (⌘S) — saves a full-resolution PNG to your Desktop.
   - **Terminal alternative:** `xcrun simctl io booted screenshot shot.png`
3. Repeat for each shot. Rename files in the order below (e.g. `01-start.png` … `05-gameover.png`).
4. Check each file is exactly 1320 × 2868. If the Simulator window was resized, set the window to physical size (Window > Physical Size) or re-capture.
5. Upload in order under the 1.0 version's "App Previews and Screenshots" — the first 2–3 shots are what people see in search results, so lead with gameplay.

## Suggested 5 shots (with captions)

Upload order = display order. Keep captions short; long text gets cut off on device.

1. **Start screen** — *"Tap targets. Survive waves. Beat your score."* — the clean start screen with the green/red legend and Start button.
2. **Mid-game with combo** — *"Chain fast taps for x2, x3 and x4 combo multipliers."* — health and score visible, several green gems falling, combo banner on screen.
3. **Red danger + bomb moment** — *"Red danger gems are worth triple — but a miss hurts double."* — a cluster of red danger gems with the "!" marker mid-drop; the high-risk moment.
4. **Wave banner / surge** — *"Every wave is faster. How long can you last?"* — late-wave shot with the wave badge high, gems dense and quick.
5. **Game over with new high score** — *"New high score! One more run?"* — the game-over overlay showing the 🏆 New High Score line and Play Again button.
