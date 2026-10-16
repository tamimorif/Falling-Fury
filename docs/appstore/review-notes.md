# Falling Fury — App Review Notes

Fields to fill in App Store Connect > App Review Information.

## Summary (one sentence)

Falling Fury is a fully offline HTML5 canvas arcade tapper bundled inside the app — tap falling gems, build combos, and survive endless waves.

## Notes for App Review

**No login required.** The app has no accounts, no sign-in, no IAP, no ads, and no analytics. It is fully usable immediately after launch — no demo credentials are needed.

1. Falling Fury is a self-contained HTML5 game bundled locally in the app. The `apps/web/site` web assets are copied into the app bundle with Capacitor and loaded by `WKWebView` from local files only. There is **no remote content, no server calls, and no network access of any kind** — the game works in Airplane Mode.
2. The app collects **no data** and stores only the player's local high score on-device (localStorage).
3. Nothing in the app links outside the bundle except the App Store listing's Support/Privacy pages.

## How to play

- Tap green gems as they fall to score points and heal — miss one and you lose health.
- Red danger gems are worth triple points, but a miss costs double damage.
- Chain fast taps to build combo multipliers (x2, x3, x4); survive as waves get faster and beat your high score.

## Pre-emptive note: Guideline 4.2 (Minimum Functionality)

Falling Fury is not a website wrapper — it is a complete, purpose-built game app:

- **Native integration:** Capacitor `Haptics` provide device feedback on hits, danger taps, and damage, going beyond what a web page can do.
- **Fully offline gameplay:** the entire game ships in the bundle and requires no network connection at any point.
- **Replayability:** endless wave-based difficulty, a combo-multiplier scoring system, and a persistent on-device high score give the game real replay value.
- **Planned native features:** Game Center support (leaderboards and achievements) is in development for an upcoming update.

## Review contact

| Field | Value |
|---|---|
| Contact email (required by App Store Connect) | `tamimarif03@gmail.com` |
| Demo account | Not required — no login in the app |

## Compliance

- **Export compliance:** no encryption used; `ITSAppUsesNonExemptEncryption` is already declared in the app's `Info.plist`.
- **App Privacy:** "Data Not Collected" — no accounts, analytics, ads, or third-party SDKs that collect data.
