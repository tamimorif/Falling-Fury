# Falling Fury — v1.0 Shipping Checklist

Ordered checklist for getting the iOS build from this repo onto the App Store. Work top to bottom. Items marked ⭐ must not be skipped.

## Phase 0 — Prerequisites

- [ ] ⭐ Active **Apple Developer Program** membership ($99/year) — [developer.apple.com/programs](https://developer.apple.com/programs/)
- [ ] ⭐ **Xcode** installed (latest stable) and signed in: Xcode > Settings > Accounts > add your Apple ID; a development signing certificate is created automatically.
- [ ] Node.js + npm installed (needed for the Capacitor workflow in `apps/ios`).
- [ ] Support and privacy pages are deployed: confirm `https://tamimorif.github.io/Falling-Fury/support.html` and `.../privacy.html` return 200 (committed and deployed October 2026).
- [ ] App icon ready: `assets/appstore/icon-1024.png` — 1024 × 1024 PNG, no alpha/transparency, square (no rounded corners).
- [ ] Contact email set in the privacy/support pages and `review-notes.md` (`tamimarif03@gmail.com`) — App Review will contact it.

## Phase 1 — Build in Xcode

- [ ] `cd apps/ios && npm install`
- [ ] `npx cap sync ios` — copies `apps/web/site` into the iOS bundle and installs the native plugins. (If `ios/App` does not exist yet, run `npx cap add ios` first, then sync.)
- [ ] `npx cap open ios` to open the project in Xcode.
- [ ] Select the **App** target > Signing & Capabilities: enable **Automatically manage signing**, pick your **Team**, and verify the **Bundle Identifier is `com.tamimorif.fallingfury`**.
- [ ] Set **Version to `1.0`** and Build to `1`.
- [ ] ⭐ Replace the app icon: in Xcode, open `App/App/Assets.xcassets/AppIcon.appiconset` and drop in `assets/appstore/icon-1024.png` (or drag it onto the App Icon slot in the target's General tab).
- [ ] Confirm the app's `Info.plist`: portrait orientation, display name "Falling Fury", and `ITSAppUsesNonExemptEncryption` is already declared (no encryption used — no export compliance prompt on upload).
- [ ] Run on a simulator (⌘R) — play a full round: start, combo, red gems, wave change, game over, restart. Repeat with Airplane Mode on.

## Phase 2 — Device + TestFlight

- [ ] Run on a physical iPhone to verify haptics, audio, and that the high score survives a force-quit.
- [ ] Product > Archive (destination: "Any iOS Device (arm64)").
- [ ] In the Organizer: **Validate App**, then **Distribute App > TestFlight**.
- [ ] Wait for processing, add yourself as an internal tester, install via TestFlight, and smoke-test the full game once more.

## Phase 3 — App Store Connect record

- [ ] [appstoreconnect.apple.com](https://appstoreconnect.apple.com) > My Apps > **+ New App**: platform iOS, name "Falling Fury", bundle ID `com.tamimorif.fallingfury`, any SKU (e.g. `FALLINGFURY1`).
- [ ] ⭐ Paste all metadata from `listing.md`: name, subtitle, promotional text, description, keywords.
- [ ] Categories: **Games > Arcade** (primary), Games > Action (secondary, optional). Price: **Free**.
- [ ] Age rating questionnaire → **4+** (nothing objectionable; see justification in `listing.md`).
- [ ] URLs: Support, Privacy Policy, and Marketing exactly as listed in `listing.md`.
- [ ] ⭐ App Privacy: choose **"Data Not Collected"** (no accounts, no analytics, no ads, no third-party data SDKs).
- [ ] Export compliance: already answered via `ITSAppUsesNonExemptEncryption` in `Info.plist`; if App Store Connect still asks, answer **No** (no encryption used).
- [ ] App Review Information: contact email `tamimarif03@gmail.com`; "no sign-in required" for demo account; paste the notes from `review-notes.md` into the Notes field.
- [ ] Advertising identifier question: answer **No** — the app contains no ads and does not use IDFA.

## Phase 4 — Screenshots

- [ ] ⭐ Capture the 5 shots per `screenshots.md` (iPhone 6.9", 1320 × 2868 portrait, required set; 6.5" optional).
- [ ] Upload them to the 1.0 version in order with the suggested captions.

## Phase 5 — Submit

- [ ] Attach the processed TestFlight build to version 1.0 and fill "What's New" ("Initial release").
- [ ] ⭐ **Submit for Review.** Typical first-review time is **1–3 days**.
- [ ] If rejected: fix the issue, bump the build number, re-archive/re-upload, and resubmit with a note about what changed.

## Phase 6 — Post-release update loop

- [ ] After any change in `apps/web/site`, run `npx cap copy ios` from `apps/ios` (use `npx cap sync ios` if native plugins changed) to push the new web build into the Xcode project.
- [ ] Bump the build number (and version for user-facing releases), archive in Xcode, upload via TestFlight, add release notes, and submit the update.
