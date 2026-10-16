# Falling Fury — Google Play Shipping Checklist

Ordered checklist for getting the Android build from this repo into Google Play. Work top to bottom. Items marked ⭐ must not be skipped.

> **Plan for time:** new personal developer accounts (created after Nov 13, 2023) **must run a closed test with 12+ testers for 14 continuous days before applying for production access** (Phase 5). That is the longest pole — start it as early as possible and expect **2+ weeks** before production.

## Phase 0 — Prerequisites

- [ ] Play Console developer account: register at [play.google.com/console](https://play.google.com/console) ($25 one-time fee) and complete **identity verification** — approval can take a few days, so start now.
- [ ] ⭐ **JDK 21** installed and used for all builds — this machine's default Java 25 cannot run Gradle 8.14.3 (`Unsupported class file major version 69`). See `apps/android/SETUP.md` §3.
- [ ] Node.js + npm installed (needed for the Capacitor workflow in `apps/android`).
- [ ] ⭐ **Back up the upload keystore and its credentials.** Keystore: `~/keystores/falling-fury-upload.jks` (alias `upload`). Credentials: `apps/android/android/keystore.properties` (**gitignored — never commit it**). Store both together somewhere safe; losing the upload key requires a Play Console upload-key reset.
- [ ] Support and privacy pages are deployed: confirm `https://tamimorif.github.io/Falling-Fury/support.html` and `.../privacy.html` return 200. The privacy policy URL is **required for all apps**.
- [ ] Store graphics ready: `assets/playstore/icon-512.png` (512 × 512 PNG, ≤ 1 MB) and `assets/playstore/feature-graphic-1024x500.png` (1024 × 500, no alpha).
- [ ] Contact email `tamimarif03@gmail.com` is live and monitored — it appears in the store listing and on the privacy/support pages.

## Phase 1 — Build the signed AAB

- [ ] `cd apps/android && npm install` (first time only).
- [ ] If anything changed in `apps/web/site`: `npm run copy` (or `npm run sync`) so the app picks up the new web assets.
- [ ] Set the JDK and build the release bundle:

```bash
cd apps/android
export JAVA_HOME="/opt/homebrew/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home"
npm run bundle
```

- [ ] Confirm the output exists: `apps/android/android/app/build/outputs/bundle/release/app-release.aab`.
- [ ] ⭐ Verify the signature (must print `jar verified`):

```bash
"$JAVA_HOME/bin/jarsigner" -verify apps/android/android/app/build/outputs/bundle/release/app-release.aab
```

- [ ] Confirm `versionCode 1` / `versionName "1.0"` in `apps/android/android/app/build.gradle`.

## Phase 2 — Create the app in Play Console

- [ ] Play Console > **All apps > Create app**: name **Falling Fury**, default language English, type **Game**, **Free**.
- [ ] Package name must be `com.tamimorif.fallingfury` — it is locked in on first upload.
- [ ] Upload the `.aab` (the first upload can go straight to a **closed testing** track — see Phase 5).
- [ ] ⭐ First upload enables **Play App Signing** automatically. Your local keystore is the *upload key*; Google holds the app-signing key. Keep the keystore backed up (Phase 0) — you sign every future upload with it.

## Phase 3 — Store listing

- [ ] ⭐ Paste all metadata from `listing.md`: app name, short description, full description, What's new.
- [ ] Upload graphics: `assets/playstore/icon-512.png` and `assets/playstore/feature-graphic-1024x500.png` (the feature graphic is required — it appears in top charts).
- [ ] Upload the phone screenshots per `screenshots.md` (2–8 required; we ship 5).
- [ ] Category: **Games > Arcade**. Add tags from the picker (suggestions in `listing.md`).
- [ ] Contact details: email `tamimarif03@gmail.com`; website `https://tamimorif.github.io/Falling-Fury/`.
- [ ] Privacy policy URL: `https://tamimorif.github.io/Falling-Fury/privacy.html`.

## Phase 4 — App content declarations (Play Console > App content)

- [ ] Privacy policy: paste the URL above.
- [ ] App access: **All functionality is available without special access** — no login, no restrictions.
- [ ] Ads: **No ads**.
- [ ] Content rating (IARC questionnaire) → **Everyone / PEGI 3 / ESRB Everyone** — exact answers in `data-safety.md`.
- [ ] Target audience: **13+** (keeps the app outside the stricter Families policy flow).
- [ ] ⭐ Data safety: **no data collected, no data shared** — walkthrough in `data-safety.md`.
- [ ] Government apps / news / health / financial features: **No** to all.
- [ ] DSA trader status: required if you distribute in the EU — complete the declaration in Play Console.

## Phase 5 — Closed testing: 12 testers, 14 continuous days ⭐

- [ ] ⭐ **This is required for production access** on personal developer accounts created after Nov 13, 2023. Do not skip it; it cannot be shortened.
- [ ] Create a **Closed testing** track and add the AAB.
- [ ] Add **12 or more testers** (Gmail accounts) who will actually opt in. A simple email list in Play Console works; testers must accept the invite and install the app.
- [ ] Keep **12+ testers opted in for 14 continuous days** — if the count drops below 12, the clock restarts. Track progress in Play Console > Testing > Closed testing.
- [ ] While waiting: run the full QA pass yourself (start, combo, danger gems, wave change, game over, restart — and once in Airplane Mode), and finish Phases 3–4.

## Phase 6 — Production access + release

- [ ] Apply for **production access** (Dashboard prompt / Testing > Production access) and complete the questionnaire about your closed test.
- [ ] Create a **Production** release: upload the AAB (rebuild with a higher `versionCode` if you changed anything since the closed test build).
- [ ] Fill release notes ("What's new", ≤ 500 chars — `Initial release` for v1.0).
- [ ] Roll out and submit. ⭐ First-app review can take longer than updates — typically **a few days** after production access is granted.
- [ ] If rejected: fix the issue, bump `versionCode`, rebuild, re-upload, and resubmit with a short note about what changed.

## Phase 7 — Post-release update loop

- [ ] Bump `versionCode` in `apps/android/android/app/build.gradle` — **every Play upload needs a higher `versionCode`** (bump `versionName` for user-facing releases).
- [ ] After any change in `apps/web/site`, run `npm run copy` from `apps/android` (use `npm run sync` if native plugins changed).
- [ ] Rebuild (`npm run bundle` with JDK 21), verify the signature with `jarsigner`, upload to the production track, add release notes (≤ 500 chars), review the pre-launch report, and roll out.
