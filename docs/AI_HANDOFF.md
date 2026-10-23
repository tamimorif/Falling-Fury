# AI Handoff — Falling Fury

**Last updated:** 2026-10-23
**Status:** Analysis complete. **No feature code has been changed yet.** This document records what exists, what the user asked for, and what is missing.
**Read order for the next AI:** this file → `apps/web/site/game.js` → `apps/web/site/native-bridge.js` → `apps/ios/ios/App/App/GameCenterPlugin.swift`

---

## 1. TL;DR

The shipped product is an **HTML5 canvas game** in `apps/web/site`, wrapped by **Capacitor** for iOS and Android and deployed to GitHub Pages. It is **not** the C++/SFML app in `apps/native` (that is a separate desktop implementation).

The user wants:

1. On first launch, **ask only for a player name**, and **save it to a database**.
2. Show the **global top 10**, with **11th row = the player's own rank**.
3. First **audit existing features** (done — see §3).
4. **Fix Game Center sign-in**, which currently does not work.

What is actually implemented today:

- Gameplay, scoring, combos, waves, effects, audio, haptics, share, local high score.
- A **partial Game Center leaderboard on iOS only** (top 10 + player rank shown as a separate paragraph).
- **No name prompt**, **no database**, **no backend**, **no web/Android leaderboard**, **no Game Center sign-in verification**.

Biggest blocker: there is **no server or database in this repo**, and no provider has been chosen yet. See §7.

---

## 2. What the app is

| Layer | Location | Notes |
|---|---|---|
| Game (source of truth) | `apps/web/site/` | `index.html`, `styles.css`, `game.js`, `native-bridge.js` |
| iOS shell | `apps/ios/` | Capacitor 8.5.3, SPM, Xcode project `App.xcodeproj` |
| Android shell | `apps/android/` | Capacitor 8.5.3, Gradle 8.14.3, requires JDK 21 |
| Native C++/SFML | `apps/native/` | Separate desktop build; **not** used by iOS/Android |
| Web deploy | GitHub Pages | Deploys `apps/web/site/` via Actions |
| Docs | `docs/appstore/`, `docs/playstore/` | Store checklists, review notes, runbook |

Important: `apps/ios/ios/App/App/public/` and `apps/android/android/app/src/main/assets/public/` are **generated snapshots** of `apps/web/site`. Edit the source, then run `npm run copy` in the shell directory. Never edit the snapshots.

---

## 3. Feature inventory (verified by reading code)

### Gameplay — implemented and self-contained

| Feature | Evidence / behaviour |
|---|---|
| Tap-to-pop diamonds | `game.js` pointer handling (`handleHit`, `strikeEnemy`) |
| Green +1 pt, heals every 4th catch while injured | `KIND_CONFIG`, `HEAL_EVERY` |
| Red danger +3 pts, double escape damage | `KIND_CONFIG`, `escapeDamage` |
| Skull bomb: −2 HP when tapped | `strikeEnemy` bomb branch |
| Armored diamond: 2 taps | `armored` branch |
| Zipper (fast zigzag) | `KIND_CONFIG.zipper` |
| Splitter → two minis | `splitEnemy` |
| Combo multipliers x2–x5 (3/6/10/15) | `getComboMultiplier`, `COMBO_WINDOW` |
| Waves every 20 s; surge every 5th wave | `WAVE_DURATION`, `SURGE_EVERY` |
| Difficulty ramp / escape damage ramp | `waveDifficulty`, `escapeDamage` |
| Hit-stop, particles, ripples, screen shake, damage vignette, floating text | `HIT_STOP`, `spawnParticles`, `triggerShake`, `drawDamageOverlay` |
| Procedural Web Audio SFX | `playTone` + `sfx*` functions, no audio files |
| Local high score | `localStorage["falling_fury_high"]` |
| Auto-pause on background + 3-2-1 resume | `visibilitychange`, `beginResume`, `RESUME_SECONDS` |
| DPR-aware responsive canvas | `setupCanvas` |
| Start / Game Over overlays; Enter restarts | `startGame`, `keydown` listener |

### Native integrations — implemented

| Feature | Location | Notes |
|---|---|---|
| Haptics | `native-bridge.js` → `@capacitor/haptics` | no-op on web |
| Native share sheet | `native-bridge.js` → `@capacitor/share`, Web Share fallback | never rejects |
| Status bar hide | `native-bridge.js` → `@capacitor/status-bar` | |
| Game Center plugin (iOS) | `apps/ios/ios/App/App/GameCenterPlugin.swift` | methods `getLeaderboard`, `submitScore` |
| Game Center entitlement | `apps/ios/ios/App/App/App.entitlements` | `com.apple.developer.game-center = true` |
| Custom bridge VC registering plugin | `FallingFuryBridgeViewController.swift` | `bridge?.registerPluginInstance(GameCenterPlugin())` |
| Privacy manifest | `App/PrivacyInfo.xcprivacy` | |

### Leaderboard — partially implemented, iOS only

- `game.js` → `renderLeaderboard()`:
  - Renders max **10** entries (`data.entries.slice(0, 10)`).
  - Shows player rank in a **separate `<p id="leaderboard-you">`**, only when `playerRank > 10`.
  - Hides the card entirely unless `data.available === true`.
- `refreshLeaderboard()` returns immediately when `!bridge.isNative`, so **the web build never shows a leaderboard**.
- On Android, `isNative` is true but `Capacitor.Plugins.GameCenter` does not exist, so the card ends up showing “Sign in to Game Center…” — **wrong message** on Android.
- The native plugin already returns: `entries[10]`, `playerRank`, `playerScore`, `playerName`. So the data for the “11th row” exists on iOS; only the UI composition is missing.

### Not implemented anywhere

- Name entry / player profile.
- Any database, API, or backend.
- Global leaderboard on web or Android.
- Player name persistence (beyond Game Center display name).
- Cross-platform ranking.
- Anti-cheat / score validation.
- Automated tests or linting.

---

## 4. Before vs. now

| Area | Before this session (repo state) | Now (2026-10-23) |
|---|---|---|
| Name prompt | Absent. Start screen has only logo, legend, leaderboard card, Start button. | Unchanged |
| DB persistence | Absent. Only `localStorage` high score. | Unchanged |
| Top 10 UI | Present but iOS/Game Center only | Unchanged |
| Player rank | Separate paragraph, not an 11th row | Unchanged |
| Game Center sign-in | Code exists; reported broken by user | Unchanged (root cause not yet proven — see §6) |
| Analysis | — | **Done**: feature inventory, gap list, failure hypotheses |
| This handoff | — | **Added** (`docs/AI_HANDOFF.md`) |

**No functional changes were made.** Treat §3 as “done”, §5 as “missing”.

---

## 5. Gap analysis for the current request

| # | Requirement | Status | Detail |
|---|---|---|---|
| 1 | Ask **just a name** at the beginning | ❌ Missing | `index.html` start overlay has no input. `playerGreetingEl` only displays a Game Center name after fetch. |
| 2 | Save name to a **database** | ❌ Missing | No DB/API exists. No `fetch()` calls outside share URLs. |
| 3 | Show **global top 10** | ⚠️ Partial | Game Center only (iOS); web/Android see nothing or a misleading message. |
| 4 | **11th row = player's rank** | ⚠️ Partial | Rank exists in GC payload but renders below the list as text, not as row 11. Also only shown when rank > 10. |
| 5 | **Game Center sign-in works** | ❌ Broken / unverified | Plugin is wired, but auth is lazy, error paths are silent, and runtime behaviour has not been tested. |
| 6 | Docs consistent with new data flow | ❌ Out of date after DB work | `privacy.html`, `support.html`, `docs/appstore/review-notes.md`, `README.md` all claim **no developer-operated server**. Adding a DB requires updating these (App Store privacy implications). |

### Required behaviour (proposed contract)

- First launch (and any launch without a saved name): show a name form **before** gameplay.
- Validate: trim, 1–16 chars, allow letters/numbers/space/`_`/`-`; reject empty; optionally filter profanity.
- Persist locally (`localStorage`) **and** upsert to the DB.
- On submit → fetch leaderboard.
- Render **top 10** from the DB.
- Render an **11th “You” row**:
  - If the player is outside the top 10 → `#<rank> <name> <score>` with a divider/highlight.
  - If the player is inside the top 10 → highlight their existing row instead of duplicating it (confirm with user).
- Offline: game must remain playable; show last cached board or “Offline” status.

---

## 6. Game Center sign-in investigation

### Verified statically

- `GameCenterPlugin.swift` is in the App target’s **Sources** build phase (`project.pbxproj`, `A111…11`).
- `FallingFuryBridgeViewController` is also in Sources and registers the plugin in `capacitorDidLoad()`.
- `App.entitlements` contains the Game Center entitlement.
- `Info.plist` has no Game Center–specific keys (none required).
- `capacitor.config.json`’s `packageClassList` is `["HapticsPlugin","SharePlugin","StatusBarPlugin"]` — **the custom GameCenter plugin is not listed**; it relies on manual registration.
- `Main.storyboard`’s initial VC is a plain `CAPBridgeViewController`.
- `SceneDelegate.swift` **creates a new window and replaces the root VC** with `FallingFuryBridgeViewController()`.
- JS only calls `getLeaderboard` from `refreshLeaderboard()` at page init.
- `getLeaderboard` installs `GKLocalPlayer.local.authenticateHandler` only when the call arrives; if authentication fails with `viewController == nil`, it resolves `{available:false}` **with no user-visible error**.
- Leaderboard ID is hard-coded: `com.tamimorif.fallingfury.highscore`.

### Ranked hypotheses (most likely first)

1. **Leaderboard not configured in App Store Connect** (or ID mismatch). `GKLeaderboard.loadLeaderboards` then errors → `{available:false}` → the UI blames sign-in. The required ID is `com.tamimorif.fallingfury.highscore`.
2. **App not actually signed with Game Center capability.** The `.entitlements` file is correct, but automatic signing must attach the capability to the App ID/provisioning profile. Verify in the Apple Developer portal and in the built app’s entitlements.
3. **No Game Center account on the test device / simulator.** Sign in via Settings → Game Center (sandbox Apple ID when testing a development build). Without it, auth can resolve silently as unauthenticated.
4. **Double bridge view controller.** The storyboard creates `CAPBridgeViewController`, then `SceneDelegate` replaces it with `FallingFuryBridgeViewController`. If the first VC is the one that owns the web view (or the replacement happens after the bridge loads), the JS proxy may not see the manually registered plugin.
5. **Registration timing / lookup.** `registerPluginInstance` runs in `capacitorDidLoad()`, but the JS may call `Capacitor.Plugins.GameCenter` before registration completes. A missing plugin yields `undefined` → `gameCenterCall` resolves `null` → “Sign in…” message.
6. **Auth is lazy.** Game Center auth is only triggered by the JS leaderboard call. Apple expects the handler to be installed early; if the call is skipped or fails, there is no explicit “Sign in” button and no retry.
7. **Simulator/Game Center availability.** Game Center sign-in is unreliable on simulators; the SETUP docs say physical-device verification is still outstanding.

### Debug checklist (do this before changing code)

1. Run on a **physical device** with a sandbox Game Center account.
2. In Safari Web Inspector (or Xcode console), check:
   - `window.Capacitor.isNativePlatform()` → `true`
   - `window.FallingFuryNative.isNative` → `true`
   - `Capacitor.Plugins.GameCenter` → object, not `undefined`
   - Call `Capacitor.Plugins.GameCenter.getLeaderboard()` manually and inspect the resolved object.
3. Add temporary logging to `GameCenterPlugin.getLeaderboard` for:
   - `GKLocalPlayer.local.isAuthenticated` at entry
   - the `authenticateHandler` `viewController`/`error` values
   - the `loadLeaderboards` error
4. Confirm the leaderboard exists in App Store Connect → Services → Game Center, with the exact ID, **global**, **all-time**, and score format **integer**.
5. Confirm the built app’s entitlements include `com.apple.developer.game-center` (`codesign -d --entitlements :- <App.app>`).
6. Confirm the App ID in the developer portal has Game Center enabled.
7. If the plugin is `undefined`, add `GameCenterPlugin`-style registration **before** the bridge loads (e.g. `CAPBridgeViewController` subclass init) or add it to the config’s plugin list if Capacitor 8 requires it.

### Recommended code fixes (after diagnosis)

- Authenticate explicitly and expose an `authenticate`/`signIn` method that **surfaces errors** (`{available:false, error:{code,message}}`).
- Install `authenticateHandler` once at startup (e.g. in `capacitorDidLoad`/`viewDidAppear`), not lazily.
- Return structured errors and render a retry button in the UI.
- Consider replacing the storyboard-created VC with `FallingFuryBridgeViewController` as the storyboard’s custom class, removing the duplicate VC.
- Submit scores with proper error handling and re-query the leaderboard after submission.

---

## 7. Backend decision (required before implementation)

There is **no backend**. The deployed web build is static (GitHub Pages), and the apps are local WKWebView/WebView bundles. Options:

| Option | Pros | Cons | Notes |
|---|---|---|---|
| **A. Supabase (recommended)** | Postgres + REST in minutes; free tier; works from web/iOS/Android; simple RLS | Public anon key in client; needs CORS/RLS setup | Tables: `players`, `scores`; RLS: `select` public, `insert`/`update` limited |
| B. Cloudflare Worker + D1 | Full control; cheap; can add server-side validation | More code to build and deploy; new runtime | Good if anti-cheat/validation matters |
| C. Firebase Firestore | Mature SDK; anonymous auth | Heavier SDK; bundle size; rules complexity | Requires native SDK or JS SDK |
| D. Game Center only | No backend work | iOS-only; no custom names; **does not meet the request** | Not viable for web/Android |

**Recommendation:** Supabase, with a small `leaderboard` module in the web source. Keep Game Center as an optional iOS-native layer only if the user wants both.

### Proposed schema

```sql
-- name is the display identity; one row per name (case-insensitive unique)
create table public.players (
  id          uuid primary key default gen_random_uuid(),
  name        text not null,
  name_key    text not null unique,      -- lower(trim(name))
  score       integer not null default 0 check (score >= 0),
  platform    text,
  created_at  timestamptz not null default now(),
  updated_at  timestamptz not null default now()
);

-- Upsert best score by name_key
-- Top 10:
select name, score, created_at
from public.players
order by score desc, created_at asc
limit 10;

-- Player rank (1-based):
select 1 + count(*) as rank
from public.players
where score > (select score from public.players where name_key = $1);
```

### API shape for the client

- `POST /scores` `{ name, score, platform }` → creates or updates only if `score` is higher.
- `GET /leaderboard?name=<name>` → `{ entries: top10, player: { rank, name, score } | null }`.
- Return a **single query/response** so the 11th row is consistent with the top 10.

### Security / privacy notes

- Client-submitted scores are spoofable. For a casual arcade game this may be acceptable; document it. If not, add server-side plausibility checks (max score rate, session tokens).
- A public leaderboard exposes chosen names. Add a profanity/length filter and a reporting path, and update `privacy.html`.
- The App Store privacy answers and `docs/appstore/review-notes.md` currently state **“No separate game account or developer-operated leaderboard server is used.”** This becomes false once the DB ships — update before submission.

---

## 8. UI/UX specification for the name + leaderboard

### Start screen changes (`index.html`)

```
Start overlay
├── title
├── name form (shown until a name is saved)
│   ├── label "Your name"
│   ├── input  maxlength=16, autocomplete=off, spellcheck=false
│   ├── error text (aria-live)
│   └── Submit
├── greeting (after save): "Playing as <name>" + small "change" link
├── legend
├── leaderboard card
│   ├── heading "Global Top 10"
│   ├── <ol> rows 1–10
│   ├── divider
│   └── row 11: your rank/name/score (`.is-player`)
├── leaderboard status / offline / retry
└── Start Game (disabled until name saved)
```

### Rendering rules

- Fetch `top 10 + player` together.
- Always render **11 rows** when a player exists:
  - Rows 1–10 from the server.
  - Row 11 = player's own entry if outside top 10.
  - If inside top 10, highlight the player's row and **do not duplicate** (confirm; the literal request says 11th row is the player's rank, but duplication looks wrong).
- Empty DB: show “Be the first to score!” and still show the player's row at rank 1.
- Offline/error: keep the game playable, show cached board if available, and a retry control.
- Names: escape via `textContent` (already done in `renderLeaderboard`), never `innerHTML`.

### Suggested constants

- `NAME_STORAGE_KEY = "falling_fury_name"`
- `NAME_MAX_LENGTH = 16`
- Optional `LEADERBOARD_SIZE = 10`

---

## 9. Implementation plan (file-by-file)

1. **`apps/web/site/index.html`**
   - Add the name form and 11th-row container inside `#start-screen`.
   - Keep `#leaderboard-list` as the top-10 `<ol>`; add `#leaderboard-player-row` after it.
   - Disable `#start-btn` until a valid name exists.
2. **`apps/web/site/styles.css`**
   - Style the input, error state, “playing as” chip, player row highlight/divider, offline/retry state.
3. **`apps/web/site/leaderboard.js` (new, recommended)**
   - Encapsulates provider config, `getOrCreateName()`, `saveScore()`, `fetchLeaderboard()`, caching, retries, error shaping.
   - Keep it provider-agnostic so Supabase/Worker/Firebase can be swapped.
4. **`apps/web/site/game.js`**
   - Load/save the name; gate `startGame()`.
   - Replace `refreshLeaderboard()`/`renderLeaderboard()` with the DB-backed module.
   - On game over, submit best score (only when improved) and refresh.
   - Keep Game Center submit/refresh **only if** the user wants it in addition; otherwise remove/feature-flag.
5. **`apps/web/site/native-bridge.js`**
   - Keep haptics/share. Either add DB fetch helpers here (works in WKWebView) or leave networking to `leaderboard.js`.
   - If keeping Game Center: add `authenticate`, structured error results, and a retry path.
6. **`apps/ios/ios/App/App/GameCenterPlugin.swift`**
   - Fix auth timing and error surfacing (see §6).
   - Optionally expose `authenticate`.
   - Make sure plugin is visible to JS (verify `Capacitor.Plugins.GameCenter`).
7. **`apps/ios/ios/App/App/Base.lproj/Main.storyboard`** (optional cleanup)
   - Set the initial VC custom class to `FallingFuryBridgeViewController` to avoid the duplicate bridge VC.
8. **Docs**
   - `README.md` features list, `privacy.html`, `support.html`, `docs/appstore/review-notes.md`, `docs/appstore/checklist.md` — reflect DB usage and the new UI.
9. **Sync copies**
   - `cd apps/ios && npm run copy`
   - `cd apps/android && npm run copy` (JDK 21 requirement only matters for Gradle builds)
10. **Verify** (see §10).

---

## 10. Acceptance criteria / QA checklist

Functional:

- [ ] Fresh browser profile: name form appears; Start is disabled until a valid name is entered.
- [ ] Name survives reload (`localStorage`) and is shown on the start screen.
- [ ] Submitting a name creates/updates exactly one DB row.
- [ ] Leaderboard shows 10 server rows, sorted by score desc.
- [ ] 11th row shows the player's rank/name/score when outside the top 10.
- [ ] Player inside the top 10 is highlighted (no duplicate row).
- [ ] New high score updates the DB and the board after Game Over.
- [ ] Offline: game playable; board shows cached data or a clear offline state; name submission retries later.
- [ ] Web, iOS, and Android all show the same global board.
- [ ] Long/HTML/emoji names render safely and truncate with ellipsis.

Game Center:

- [ ] On a physical iOS device, launching triggers the Game Center sign-in sheet (or an explicit “Sign in” action).
- [ ] After sign-in, `getLeaderboard()` returns `available:true` with 10 entries and `playerRank`.
- [ ] Score submission succeeds and the refreshed board reflects it.
- [ ] Auth denial/error is displayed with a retry option (not a silent failure).
- [ ] `Capacitor.Plugins.GameCenter` is defined in Safari Web Inspector.

Docs:

- [ ] Privacy policy/support/review notes no longer claim there is no developer server.
- [ ] README feature list matches reality.

---

## 11. Commands & environment

```bash
# Web preview
cd apps/web/site && python3 -m http.server 8080

# iOS
cd apps/ios && npm install && npm run copy && npx cap open ios

# Android (JDK 21 required for Gradle)
export JAVA_HOME="/opt/homebrew/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home"
cd apps/android && npm install && npm run copy
```

Other environment facts:

- macOS, workspace `/Users/tamimorif/Documents/GitHub/Falling-Fury`.
- Android: min/target/compile SDK 24/36/36, app ID `com.tamimorif.fallingfury`.
- iOS: deployment target 15.0, iPhone/portrait only, SPM (no Podfile/workspace).
- No test suite, no linter, no CI test job (Pages deploy + CMake build workflows only).

---

## 12. Open questions for the user (blocking)

1. **Backend provider:** Supabase (recommended), Cloudflare Worker + D1, Firebase, or something else? Need the project URL + public anon key (never a service-role key).
2. **Scope of the board:** one global board across web + iOS + Android? (Assumed yes.)
3. **Game Center's role:** keep it as an additional iOS board, or make the DB the only board? The user asked to fix sign-in, so assume keep, but it now duplicates functionality.
4. **Name identity:** should the typed name be the displayed name everywhere (including iOS), replacing the Game Center display name? If both exist, which wins?
5. **Duplicate names:** allow multiple players with the same name, or treat name as a unique identity (upsert best score)? (Schema above assumes unique.)
6. **Rename policy:** allow changing the name later? Does it keep the score or create a new player?
7. **Moderation:** profanity filter? name length? reporting?
8. **Anti-cheat:** accept client-submitted scores, or add validation?
9. **Existing scores:** none to migrate (only local high scores exist).

---

## 13. Key file map

| File | Role | Hot spots |
|---|---|---|
| `apps/web/site/index.html` | Markup/overlays | start screen ~L58–80, leaderboard card ~L67–75 |
| `apps/web/site/game.js` | Entire game + UI flow | DOM refs L40–55, `renderLeaderboard` L99–130, `refreshLeaderboard` L132–153, state L410–437, game over L700–725, init L1130–1145 |
| `apps/web/site/native-bridge.js` | Capacitor bridge | Game Center calls L266–284, public API L318–324 |
| `apps/web/site/styles.css` | Styling | overlay L241+, leaderboard L275–360 |
| `apps/ios/ios/App/App/GameCenterPlugin.swift` | GameKit leaderboard | auth L16–35, submit L37–57, load L64–108 |
| `apps/ios/ios/App/App/FallingFuryBridgeViewController.swift` | Plugin registration | whole file (5 lines) |
| `apps/ios/ios/App/App/SceneDelegate.swift` | Window/root VC | L8–14 |
| `apps/ios/ios/App/App/Base.lproj/Main.storyboard` | Storyboard | initial VC is `CAPBridgeViewController` |
| `apps/ios/ios/App/App/App.entitlements` | Game Center capability | `com.apple.developer.game-center` |
| `apps/ios/ios/App/App/capacitor.config.json` | Built config | `packageClassList` lacks `GameCenterPlugin` |
| `apps/android/android/app/src/main/java/.../MainActivity.java` | Android entry | empty `BridgeActivity` subclass — **no leaderboard** |
| `docs/appstore/review-notes.md` | App Review copy | claims no developer server — must change with DB |
| `apps/web/site/privacy.html` | Privacy policy | Game Center-only claims — must change with DB |

---

## 14. Confidence / caveats

- Feature inventory and gap analysis: **high confidence** (direct code reading).
- Game Center root cause: **not proven** — no device run was performed in this session; §6 lists hypotheses only.
- Backend design: **proposal**, not approved by the user.
- No code was modified except adding this document.
