# Falling Fury — Google Play artwork

Generated artwork for the Google Play submission. Palette matches the game and the
iOS set in `assets/appstore/`: background `#0b0a1a → #131136 → #1c1a3a`, emerald gem
`#34d399` (facets `#6ee7b7 → #047857`, stroke `#065f46`), lilac accent `#a78bfa`.

## Files

| File | What it is |
| --- | --- |
| `feature-graphic-1024x500.png` | 1024×500 opaque feature graphic (Play requires this). Full-bleed rectangle, no border, no rounded corners, no device mockups. Faceted emerald gem on the left, "FALLING FURY" in Bebas Neue with the short tagline "Tap the gems before they fall" on the right. All text and key art sit inside the central safe area, and the exact centre of the canvas is kept clear of art (Play may overlay a play button there on the app detail page). |
| `icon-512.png` | 512×512 opaque Play Store icon, scaled down from `assets/appstore/icon-1024.png` with `sips`. Full square, no rounded corners — Play applies its own mask. |
| `feature-graphic.html` | Self-contained source for the feature graphic: inline SVG (same gem art and palette as `assets/appstore/icon.svg`) plus base64 `@font-face` data URIs for Bebas Neue (title) and Outfit 600 (tagline), so headless Chrome renders it with no network access and no local font dependency. |
| `README.md` | This file. |

`screenshots/` is populated separately (phone/tablet screenshots are produced by a
different tool) — it is not part of this workflow.

## Where each file goes in Play Console

Play Console → your app → **Store listing → Store graphics**:

| Asset | Field |
| --- | --- |
| `icon-512.png` | **App icon** |
| `feature-graphic-1024x500.png` | **Feature graphic** |

## Requirements summary

| Asset | Requirement |
| --- | --- |
| App icon | 512×512 PNG, ≤ 1 MB, no alpha; 32-bit PNG fine, full-bleed square (Play masks it) |
| Feature graphic | 1024×500, 24-bit PNG or JPEG, **no alpha**; text and key art inside the central safe area (~10 % of each edge can be cropped and a play button may be overlaid at the centre of the app detail page); no device frames/mockups, no rankings, no fake buttons, no misleading imagery, no borders or rounded corners; text must stay legible when scaled down for top-charts/banner placements |

## Re-rendering (macOS, Chrome headless — no ImageMagick)

Feature graphic — render `feature-graphic.html` at exactly 1024×500:

```sh
CHROME="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"

"$CHROME" --headless=new --disable-gpu --hide-scrollbars --force-device-scale-factor=1 \
  --user-data-dir="$TMPDIR/fury-play-chrome-a" --no-first-run --no-default-browser-check \
  --window-size=1024,500 --screenshot=feature-graphic-1024x500.png \
  "file://$PWD/feature-graphic.html"
```

(Use a fresh `--user-data-dir` per run; reusing one can make Chrome's teardown watchdog
abort the screenshot.)

Verify dimensions and alpha, and flatten via a max-quality JPEG round-trip only if
`hasAlpha` reports `yes` (the checked-in render is already opaque):

```sh
sips -g pixelWidth -g pixelHeight -g hasAlpha feature-graphic-1024x500.png
# if needed:
sips -s format jpeg -s formatOptions 100 in.png --out tmp.jpg
sips -s format png tmp.jpg --out out.png
```

Icon — derived from the App Store icon, no re-render needed:

```sh
sips -z 512 512 ../appstore/icon-1024.png --out icon-512.png
sips -g pixelWidth -g pixelHeight -g hasAlpha icon-512.png
```

The fonts embedded in the HTML come from `apps/web/site/assets/fonts/BebasNeue-Regular.ttf`
and `apps/web/site/assets/fonts/outfit/outfit-600.woff2`; re-embed them with
`base64 -i <file>` if the fonts change.

## Verification (2026-10-16)

- `feature-graphic-1024x500.png` — 1024×500, `hasAlpha: no` (`samplesPerPixel: 3`), 403 KB. No flattening required; Chrome wrote a fully opaque PNG. Rendered from `feature-graphic.html` with the command above (Google Chrome 155.0.8059.40).
- `icon-512.png` — 512×512, `hasAlpha: no`, 168 KB (well under the 1 MB limit).
