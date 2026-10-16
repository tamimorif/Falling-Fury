# Falling Fury — iOS wrapper (Capacitor)

Native iOS shell for the HTML5 game in `apps/web/site`. Built with **Capacitor 8.5.3**,
which uses **Swift Package Manager** (there is **no CocoaPods Podfile and no
`App.xcworkspace`** in this project — open `App.xcodeproj` instead).

| Item | Value |
| --- | --- |
| App ID / bundle identifier | `com.tamimorif.fallingfury` |
| App name / display name | Falling Fury |
| Web assets dir (`webDir`) | `../web/site` (used directly — the `www/` fallback is **not** used) |
| Native project | `ios/App/App.xcodeproj` (scheme `App`) |
| Deployment target | iOS 15.0 |
| Device family | iPhone only (`TARGETED_DEVICE_FAMILY = 1`) |
| Orientation | Portrait only |
| Status bar | Hidden (`UIStatusBarHidden`, `UIViewControllerBasedStatusBarAppearance = false`, plus `StatusBar.hide()` from the bridge) |
| Encryption | `ITSAppUsesNonExemptEncryption = false` (no export-compliance prompt) |
| Plugins | `@capacitor/haptics`, `@capacitor/share`, `@capacitor/status-bar` |

---

## 1. Remaining human steps (in order)

### a. Install dependencies and sync web assets

```bash
cd apps/ios
npm install
npx cap sync ios          # or: npm run sync
```

`cap sync` copies `apps/web/site/**` into `ios/App/App/public/` (gitignored) and refreshes
the Swift package list. **Run `npm run copy` (or `npm run sync`) after every web change** —
the native app ships a snapshot of the web assets, it does not read `apps/web/site` live.

### b. Open the project in Xcode

```bash
npx cap open ios          # or: npm run open
# or, equivalently:
xed ios/App/App.xcodeproj
```

> On the first open Xcode resolves the Swift package `capacitor-swift-pm` from GitHub —
> an internet connection is required once.

### c. Signing

In Xcode: select the **App** target → **Signing & Capabilities** → check
*Automatically manage signing* → choose your **Team**. Bundle identifier is already
`com.tamimorif.fallingfury` (change it here if you use a different App Store record).

### d. App icon (required before submission)

Overwrite this exact file:

```
apps/ios/ios/App/App/Assets.xcassets/AppIcon.appiconset/AppIcon-512@2x.png
```

`Assets.xcassets/AppIcon.appiconset/Contents.json` declares a single icon entry
(idiom `universal`, platform `ios`, size `1024x1024`) whose filename is
**`AppIcon-512@2x.png`** — keep that filename and just replace the pixels.
(The file currently in the project is Capacitor's default blue icon; the final art is
expected at `assets/appstore/icon-1024.png`, which was still empty when this was written.)

```bash
# from the repo root; icon must be 1024x1024 PNG with NO alpha channel
cp assets/appstore/icon-1024.png \
   apps/ios/ios/App/App/Assets.xcassets/AppIcon.appiconset/AppIcon-512@2x.png

# verify it is 1024x1024 and hasAlpha: no
sips -g pixelWidth -g pixelHeight -g hasAlpha \
   apps/ios/ios/App/App/Assets.xcassets/AppIcon.appiconset/AppIcon-512@2x.png
```

If the provided PNG has alpha, flatten it first, e.g.:

```bash
sips -s format png --deleteColorManagementProperties -s hasAlpha no \
   assets/appstore/icon-1024.png --out /tmp/icon-1024-flat.png
```

Optional: the launch screen image is
`ios/App/App/Assets.xcassets/Splash.imageset/splash-2732x2732(-1|-2).png` (2732x2732).

### e. Confirm device + orientation

The project is already configured for **iPhone only** and **portrait only**
(`TARGETED_DEVICE_FAMILY = 1`, and `Info.plist` has a single
`UIInterfaceOrientationPortrait` entry with no `~ipad` key). Sanity-check in Xcode:
target **General** tab → *Deployment Info* → Device = iPhone, Orientation = Portrait.

### f. Archive and upload

Xcode → select **Any iOS Device (arm64)** → **Product ▸ Archive** →
Organizer → **Distribute App ▸ App Store Connect ▸ Upload**.

Remember to bump `MARKETING_VERSION` / `CURRENT_PROJECT_VERSION` (target General tab)
for every uploaded build.

---

## 2. Web-side companion file

`apps/web/site/native-bridge.js` backs `window.FallingFuryNative`:

```js
window.FallingFuryNative = {
  isNative,                  // boolean, true inside the Capacitor app
  haptic(name),              // "light"|"medium"|"heavy"|"success"|"warning"|"error"|"vibrate"
  shareScore(score, highScore) // Promise<boolean>, never rejects
};
```

It is referenced by `apps/web/site/index.html` (`<script src="./native-bridge.js"></script>`
before `game.js`) and is a no-op on the plain web build, so the GitHub Pages site is
unaffected. Verify it is present in `ios/App/App/public/native-bridge.js` after `cap copy`.

---

## 3. Useful commands

```bash
npm run copy    # cap copy ios    — copy web assets only (fast)
npm run sync    # cap sync ios    — copy web assets + update plugins
npm run open    # cap open ios    — open Xcode
npx cap doctor  # environment diagnostics
```

## 4. Troubleshooting

- **Xcode cannot resolve `capacitor-swift-pm`** → you are offline; connect and
  *File ▸ Packages ▸ Reset Package Caches*.
- **Game changes not showing in the simulator** → you forgot `npm run copy` (`cap copy ios`).
- **`cap` complains about TypeScript** → keep `typescript` in `devDependencies`
  (required to read `capacitor.config.ts`).
- **No `App.xcworkspace`** → expected. Capacitor 8 uses SPM; build from `App.xcodeproj`.
