# Falling Fury — App Review Notes

Fields to fill in App Store Connect > App Review Information.

## Summary (one sentence)

Falling Fury is a fully offline HTML5 canvas arcade tapper bundled inside the app — tap falling gems, build combos, and survive endless waves.

## Notes for App Review

**Game Center sign-in is optional.** It is only used on iOS for global leaderboard scores and the player's display name. The game is fully playable without signing in, and no demo credentials are needed.

1. Falling Fury is a self-contained HTML5 game bundled locally in the app. The `apps/web/site` web assets are copied into the app bundle with Capacitor and loaded by `WKWebView` from local files.
2. On iOS, players may sign in through Apple's Game Center sheet. The app submits a player's best score to the configured global, all-time leaderboard and reads its top 10 plus the signed-in player's rank. Game Center manages the account, display name, and leaderboard data. No separate game account or developer-operated leaderboard server is used.
3. Core gameplay remains available without Game Center or an internet connection; the local high score stays on-device.
3. Nothing in the app links outside the bundle except the App Store listing's Support/Privacy pages.

## How to play

- Tap green gems as they fall to score points and heal — miss one and you lose health.
- Red danger gems are worth triple points, but a miss costs double damage.
- Chain fast taps to build combo multipliers (x2 up to x5); survive as waves get faster and beat your high score.

## Pre-emptive note: Guideline 4.2 (Minimum Functionality)

Falling Fury is not a website wrapper — it is a complete, purpose-built game app:

- **Native integration:** Capacitor `Haptics` provide device feedback on hits, danger taps, and damage, going beyond what a web page can do.
- **Offline gameplay:** the entire game ships in the bundle; only optional Game Center leaderboard features need network access.
- **Replayability:** endless wave-based difficulty, a combo-multiplier scoring system, and a persistent on-device high score give the game real replay value.
- **Game Center leaderboard:** the opening screen shows the global top 10, highlights the signed-in player, and shows their rank below the top 10.

## Review contact

| Field | Value |
|---|---|
| Contact email (required by App Store Connect) | `tamimarif03@gmail.com` |
| Demo account | Not required — no login in the app |

## Compliance

- **Export compliance:** no encryption used; `ITSAppUsesNonExemptEncryption` is already declared in the app's `Info.plist`.
- **App Privacy:** update the App Store Connect privacy answers to reflect optional Game Center leaderboard score/display-name handling before submission.
