# Falling Fury — App Store artwork

Generated artwork for the iOS App Store submission. Palette matches the game:
background `#0b0a1a → #131136 → #1c1a3a`, emerald gem `#34d399` (facets `#6ee7b7 → #047857`,
stroke `#065f46`), lilac accent `#a78bfa`.

## Files

| File | What it is |
| --- | --- |
| `icon.svg` | 1024×1024 vector source of the app icon. Full square (no rounded corners, no alpha, no text) — iOS applies the rounded mask itself. Bold faceted emerald diamond slightly below center, lilac glow, top-left facet highlight. Reads down to 40 px. |
| `icon-1024.png` | 1024×1024 opaque PNG render of `icon.svg` (App Store icon source). |
| `splash-2732.png` | 2732×2732 opaque splash/launch background: same background gradient, same diamond logo scaled to ~30 % of the width and centered (819 px gem width). |
| `README.md` | This file. |

## Where each file goes in Xcode

Copy the PNGs into the app's asset catalog (`App/App/Assets.xcassets` in the Capacitor iOS project):

### `icon-1024.png` → `AppIcon.appiconset/`

Drop the file into `AppIcon.appiconset`, then replace `Contents.json` with the modern
single-size form (Xcode 14+):

```json
{
  "images" : [
    {
      "filename" : "icon-1024.png",
      "idiom" : "universal",
      "platform" : "ios",
      "size" : "1024x1024"
    }
  ],
  "info" : { "author" : "xcode", "version" : 1 }
}
```

### `splash-2732.png` → `Splash.imageset/`

Create an image set named `Splash`, drop the file in, and use "Single Scale":

```json
{
  "images" : [
    { "filename" : "splash-2732.png", "idiom" : "universal" }
  ],
  "info" : { "author" : "xcode", "version" : 1 }
}
```

Reference it from the launch screen — either a `UIImageView` named `Splash` in
`LaunchScreen.storyboard`, or the `UILaunchScreen` Info.plist dictionary with
`UIImageName = Splash`. iOS aspect-fills and crops launch images, so the centered logo
stays visible in both portrait and landscape.

## Re-rendering (macOS, Chrome headless — no ImageMagick)

1. Build temp HTML files in `$TMPDIR` that inline the SVG with `body { margin:0 }` and an
   opaque background rect at the exact canvas size:
   - `icon.html`: `icon.svg` markup verbatim, 1024×1024 canvas.
   - `splash.html`: same markup on a 2732×2732 canvas — background/vignette/glow rects grown
     to 2732, glow ellipse re-centered at `(1366,1366)`, and the gem group transformed to
     `transform="translate(1366,1366) scale(1.205) translate(0,-40)"` (gem bbox center in
     local coordinates is `(0,40)`, which yields a centered gem at ~30 % of the width).
2. Screenshot with Chrome (versions used: Google Chrome 155.0.8059.40):

```sh
CHROME="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"

"$CHROME" --headless=new --disable-gpu --hide-scrollbars --force-device-scale-factor=1 \
  --user-data-dir="$TMPDIR/fury-chrome-a" --no-first-run --no-default-browser-check \
  --window-size=1024,1024 --screenshot="$TMPDIR/icon1024.png" "file://$TMPDIR/icon.html"

"$CHROME" --headless=new --disable-gpu --hide-scrollbars --force-device-scale-factor=1 \
  --user-data-dir="$TMPDIR/fury-chrome-b" --no-first-run --no-default-browser-check \
  --window-size=2732,2732 --screenshot="$TMPDIR/splash2732.png" "file://$TMPDIR/splash.html"
```

(Use a fresh `--user-data-dir` per run; reusing one can make Chrome's teardown watchdog
abort the second screenshot.)

3. Verify dimensions and alpha:

```sh
sips -g pixelWidth -g pixelHeight -g hasAlpha "$TMPDIR/icon1024.png" "$TMPDIR/splash2732.png"
```

If `hasAlpha` reports `yes` even though the art is opaque, flatten via a max-quality JPEG
round-trip:

```sh
sips -s format jpeg -s formatOptions 100 in.png --out tmp.jpg
sips -s format png tmp.jpg --out out.png
```

Then copy the results next to this README as `icon-1024.png` / `splash-2732.png`.

## Verification (2026-10-14)

- `icon-1024.png` — 1024×1024, `hasAlpha: no` (gem bbox center measured at 511.5,569 ≈ as designed).
- `splash-2732.png` — 2732×2732, `hasAlpha: no` (gem bbox 842×722 px, center 1365.5,1365.5 ≈ exact center, ~30 % of width).
- No flattening was required; Chrome wrote fully opaque PNGs.
