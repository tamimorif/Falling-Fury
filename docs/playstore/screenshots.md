# Falling Fury — Google Play Screenshots

## Requirements

| Requirement | Value |
|---|---|
| Phone screenshots | **2–8 required** — we ship **5** |
| Resolution | 320–3840 px per side; our set is **1080 × 1920** (9:16 portrait) |
| Aspect ratio | 16:9 or 9:16 — 1080 × 1920 is 9:16 |
| Format | JPEG or 24-bit PNG, **no alpha channel** |
| Tablet (7" / 10") | Optional — 2–8 each; skip for this release |
| Promo video | Optional — not required |

Shot files live in `assets/playstore/screenshots/` (captured from a **Medium_Phone** AVD at 1080 × 1920).

## The captured set (with captions)

Upload order = display order. Keep captions short; Play truncates long text.

1. `01-start.png` — **Start screen** — *"Tap targets. Survive waves. Beat your score."* — clean start screen with the green/red legend and Start button.
2. `02-early-game.png` — **Early game** — *"Green gems score and heal. Miss one and you lose health."* — first wave, sparse gems, full health bar.
3. `03-mid-game-combo.png` — **Mid-game with combo** — *"Chain fast taps for x2, x3 and x4 combo multipliers."* — several gems falling with the combo banner on screen.
4. `04-gameplay.png` — **Danger gems in play** — *"Red danger gems are worth triple — but a miss hurts double."* — red danger gems mid-drop, the high-risk moment.
5. `05-gameover.png` — **Game over / high score** — *"New high score! One more run?"* — game-over overlay with the 🏆 New High Score line and Play Again button.

## Recapture instructions (emulator + adb)

1. Build and install the debug APK on a booted Medium_Phone AVD:

```bash
cd apps/android
npm install          # first time only
npm run copy         # after any change in apps/web/site
npm run apk:debug    # output: android/app/build/outputs/apk/debug/app-debug.apk

adb devices          # confirm the emulator is listed
adb install -r android/app/build/outputs/apk/debug/app-debug.apk
adb shell wm size 1080x1920
```

2. Launch the game and play to the moment you want:

```bash
adb shell am start -n com.tamimorif.fallingfury/.MainActivity
```

3. Capture each shot (repeat for each moment, use the exact filenames above):

```bash
adb exec-out screencap -p > assets/playstore/screenshots/01-start.png
```

4. Verify every file is exactly 1080 × 1920 (and ideally alpha-free):

```bash
sips -g pixelWidth -g pixelHeight -g hasAlpha assets/playstore/screenshots/01-start.png
```

5. If `hasAlpha: yes`, flatten by round-tripping through JPEG (Play accepts JPEG directly, so you may ship the `.jpg` as-is, or convert back to PNG):

```bash
sips -s format jpeg assets/playstore/screenshots/01-start.png --out /tmp/01-start.jpg
sips -s format png /tmp/01-start.jpg --out assets/playstore/screenshots/01-start.png
```

6. When done, restore the emulator's display: `adb shell wm size reset`.

7. Upload in order under **Store listing > Phone screenshots** — the first shots are what people see in search results, so lead with the start screen and gameplay, not game over.
