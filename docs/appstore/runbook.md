# Falling Fury — Run, Test & Ship (iOS)

Step-by-step runbook for the iOS app in `apps/ios`. Written for a human **or an agent**
on this Mac — Xcode 27.1, with iOS 27.0 + 26.5 simulator runtimes installed.

> There is no "App Store simulator". The release path is:
> **iOS Simulator** (test now) → **Xcode Archive** → **TestFlight / App Store Connect** (upload).

---

## 0) Refresh the app bundle (only needed after web changes)

```sh
cd apps/ios
npm install          # first time on a machine only
npx cap sync ios     # copies apps/web/site/* into the iOS app bundle
```

## 1) Run it in the iOS Simulator

### Option A — Xcode (simplest)

```sh
cd apps/ios
npx cap open ios
```

In Xcode: pick scheme **App** + an **iPhone 18 Pro Max** (or iPhone 17 Pro Max)
simulator in the toolbar, then press **⌘R**. The game starts — tap to play.

### Option B — fully from the terminal

```sh
cd apps/ios/ios/App
xcodebuild -project App.xcodeproj -scheme App -configuration Debug \
  -destination 'platform=iOS Simulator,name=iPhone 18 Pro Max' \
  -derivedDataPath build build

xcrun simctl boot "iPhone 18 Pro Max" 2>/dev/null || true
open "/Applications/Xcode.app/Contents/Applications/DeviceHub.app"   # Xcode 27's simulator window (older Xcode: "Simulator")
xcrun simctl install booted build/Build/Products/Debug-iphonesimulator/App.app
xcrun simctl launch booted com.tamimorif.fallingfury
```

Take a screenshot any time:

```sh
xcrun simctl io booted screenshot shot.png
```

## 2) Smoke test (5 minutes)

- [ ] Start screen appears; **Start** begins a run
- [ ] Green gems: +1; combo multiplier after 3 fast catches (×2 → ×5)
- [ ] Red gems: +3 (a miss costs double); tapping a skull bomb costs 2 HP
- [ ] Wave banner + difficulty ramp; every 5th wave is a ⚡ surge
- [ ] Game over → **Play Again** restarts
- [ ] Portrait only; no scrolling / rubber-banding
- [ ] Go Home and reopen the app: auto-pause + 3-2-1 resume countdown
- [ ] Force-quit (swipe up) and relaunch: **high score persists**
- [ ] Sound effects play; no console errors
- Haptics need a real iPhone — verify on TestFlight (step 5)

## 3) Screenshots for the listing (initial set uploaded; refresh required)

App Store Connect needs the **6.9" set: 1320 × 2868** portrait. The final five-shot set
is already in `assets/appstore/screenshots/` (`01-start.png` … `05-gameover.png`), all
1320 × 2868 RGB PNGs with no alpha. This set was uploaded to App Store Connect before
the Game Center leaderboard was added, so refresh the opening screen and upload the
updated images before submission. Captions and the shot plan are in `screenshots.md`.

To re-capture from the simulator with the game running:

```sh
xcrun simctl io booted screenshot 01-start.png
sips -g pixelWidth -g pixelHeight 01-start.png   # must print 1320 / 2868
```

## 4) Signing (one-time)

1. `npx cap open ios` → select the **App** target → **Signing & Capabilities**
2. ✔ **Automatically manage signing** → **Team** = your Apple Developer team
3. Bundle ID must stay `com.tamimorif.fallingfury`

Requires a paid Apple Developer Program membership, signed in at
Xcode ▸ Settings ▸ Accounts (`tamimarif03@gmail.com`).

## 5) Archive → TestFlight → App Store

1. Xcode toolbar destination: **Any iOS Device (arm64)**
2. **Product ▸ Archive**
3. Organizer: **Validate App** → **Distribute App ▸ App Store Connect ▸ Upload**
4. TestFlight: add yourself as an internal tester, install on a real iPhone, and
   re-run the smoke test (haptics!).
5. The app record and version 1.0 listing already exist in App Store Connect; review the saved copy in `listing.md`.
6. The saved Support, Privacy, and Marketing URLs are:
   - Support `https://tamimorif.github.io/Falling-Fury/support.html`
   - Privacy `https://tamimorif.github.io/Falling-Fury/privacy.html`
   - Marketing `https://tamimorif.github.io/Falling-Fury/`
7. **App Privacy:** published declarations for linked Gameplay Content (scores) and User ID (Game Center player identity); no tracking. If practices change, update these answers and the live privacy policy together.
8. Export compliance is declared in `Info.plist` (no encryption).
9. App Review contact name, phone, and email are saved. The review notes are saved; sign-in is marked not required.
10. Select the Apple Developer signing Team, archive and upload a signed build, then verify Game Center on a physical iPhone before submitting the version and leaderboard for review.

## 6) Rules for every future update

- Bump **CURRENT_PROJECT_VERSION** (target ▸ General tab) before every upload — App Store
  Connect rejects duplicate build numbers.
- After editing anything in `apps/web/site`: run `npx cap copy ios`, re-archive, re-upload.
- Item-by-item submission checklist: `checklist.md`.

## Verified on this Mac

- 2026-10-16 — simulator build via `xcodebuild` → **BUILD SUCCEEDED** (Xcode 27.1).
- 2026-10-16 — app installed and launched on the **iPhone 18 Pro Max** simulator
  (`com.tamimorif.fallingfury`); start screen renders correctly (screenshot checked,
  1320 × 2868 — the same size the App Store listing needs).
- 2026-10-17 — **Release** configuration build via `xcodebuild` → **BUILD SUCCEEDED**;
  the app-level `PrivacyInfo.xcprivacy` is present in the built `.app`.
- 2026-10-17 — five App Store screenshots generated from the shipped web build at
  440 × 956 CSS / 3× (1320 × 2868), RGB with no alpha, stored in
  `assets/appstore/screenshots/`. These predate the Game Center leaderboard and must be refreshed.
- 2026-10-18 — Release configuration build after adding Game Center → **BUILD SUCCEEDED**
  with `CODE_SIGNING_ALLOWED=NO`; this verifies compilation only, not an App Store archive or upload.
- Tip: a screenshot taken within the first ~5 seconds of launch can be a black frame while
  the web view boots — wait a moment or relaunch before capturing.
