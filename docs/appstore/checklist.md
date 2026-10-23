# Falling Fury — v1.0 Shipping Checklist

Ordered checklist for getting the iOS build from this repo onto the App Store. Work top to bottom. Items marked ⭐ must not be skipped.

## Current status (2026-10-18)

The Game Center top-10/current-rank feature is implemented and the unsigned iOS
Release build succeeds. App Store Connect already has the version 1.0 listing,
Game Center enabled, and a matching all-time high-score leaderboard. The App
Privacy answers are published, the reviewer contact is saved, and updated
privacy/support pages are live. The saved category is Games > Casual and the
age rating is 13+ (with regional equivalents). The five uploaded screenshots
predate the leaderboard. App Store Connect availability is set to 133 countries
or regions; the entire Europe group is excluded at launch, as requested.

**Remaining:** make the required EU DSA trader-status declaration in App Store Connect,
resolve Xcode signing (the team is selected, but Xcode reports no registered
devices/profile; Xcode shows one Apple Development certificate, no Apple Distribution
certificate, and the Mac has no usable signing identity), create and upload a signed
build, verify Game Center on a physical iPhone, and refresh and upload screenshots.

## Phase 0 — Prerequisites

- [ ] ⭐ Active **Apple Developer Program** membership ($99/year) — [developer.apple.com/programs](https://developer.apple.com/programs/)
- [x] Xcode is signed in to the Apple Developer team and the App target uses that team. Xcode lists one Apple Development certificate and zero registered devices; no Apple Distribution certificate or usable code-signing identity is installed on this Mac.
- [ ] Node.js + npm installed (needed for the Capacitor workflow in `apps/ios`).
- [x] Updated support and privacy pages are deployed and verified live; the privacy policy explains optional Game Center scores and player names.
- [x] App icon ready: `assets/appstore/icon-1024.png` — 1024 × 1024 PNG, no alpha/transparency, square (no rounded corners). Installed in the asset catalog.
- [x] App Review contact name, phone, and email entered in App Store Connect.

## Phase 1 — Build in Xcode

- [x] `cd apps/ios && npm install`
- [x] `npx cap sync ios` — copies `apps/web/site` into the iOS bundle and installs the native plugins. (If `ios/App` does not exist yet, run `npx cap add ios` first, then sync.)
- [ ] `npx cap open ios` to open the project in Xcode.
- [ ] ⭐ Resolve the signing profile/certificate error in **Signing & Capabilities**, then verify the bundle ID is `com.tamimorif.fallingfury` and create a signed archive.
- [x] Set **Version to `1.0`** and Build to `1`.
- [x] ⭐ Replace the app icon: already installed (`AppIcon-512@2x.png` matches `assets/appstore/icon-1024.png`).
- [x] Confirm the app's `Info.plist`: portrait orientation, display name "Falling Fury", and `ITSAppUsesNonExemptEncryption` is already declared (no encryption used — no export compliance prompt on upload). The app-level `PrivacyInfo.xcprivacy` is also bundled.
- [x] Previous simulator gameplay smoke test and unsigned Release build verified. Game Center itself still needs physical-device verification.

## Phase 2 — Device + TestFlight

- [ ] Run on a physical iPhone to verify haptics, audio, score persistence, and Game Center sign-in, top 10, current rank, and score submission.
- [ ] Product > Archive (destination: "Any iOS Device (arm64)").
- [ ] In the Organizer: **Validate App**, then **Distribute App > TestFlight**.
- [ ] Wait for processing, add yourself as an internal tester, install via TestFlight, and smoke-test the full game once more.

## Phase 3 — App Store Connect record

- [x] App Store Connect app record exists; version 1.0 listing metadata is saved.
- [x] Primary category: **Games > Casual**; free price schedule is saved. Availability is set to **133 countries or regions**, with the full Europe group excluded at launch.
- [x] Age rating questionnaire saved: **13+** in most storefronts, with regional equivalents (see the saved questionnaire answers in App Store Connect).
- [x] Support, Privacy Policy, and Marketing URLs are saved in the version 1.0 listing.
- [x] Updated privacy policy is deployed and verified at its saved URL.
- [x] ⭐ Published App Privacy answers for linked **Gameplay Content** (scores) and **User ID** used for Game Center; no tracking.
- [ ] Complete the account's EU Digital Services Act trader-status declaration. Apple says this declaration is still required even when the app isn't distributed in the EU; the account owner must choose the accurate status.
- [x] Export compliance: already answered via `ITSAppUsesNonExemptEncryption` in `Info.plist`; if App Store Connect still asks, answer **No** (no encryption used).
- [x] App Review notes are saved, sign-in is marked not required, and reviewer contact fields are complete.
- [ ] Advertising identifier question: answer **No** if App Store Connect presents it — the app contains no ads and does not use IDFA.

## Phase 4 — Screenshots

- [x] Initial five-shot 6.9" set (1320 × 2868 portrait) is uploaded to version 1.0.
- [ ] Refresh the opening-screen screenshot to show the leaderboard, then upload the updated set. Existing uploaded images predate this feature. App Store Connect has **no per-screenshot caption field**.

## Phase 5 — Submit

- [ ] Attach the processed TestFlight build to version 1.0 and fill "What's New" ("Initial release").
- [ ] ⭐ **Submit for Review.** Typical first-review time is **1–3 days**.
- [ ] If rejected: fix the issue, bump the build number, re-archive/re-upload, and resubmit with a note about what changed.

## Phase 6 — Post-release update loop

- [ ] After any change in `apps/web/site`, run `npx cap copy ios` from `apps/ios` (use `npx cap sync ios` if native plugins changed) to push the new web build into the Xcode project.
- [ ] Bump the build number (and version for user-facing releases), archive in Xcode, upload via TestFlight, add release notes, and submit the update.
