# Falling Fury — Google Play Data Safety & App Content

Exact answers for every **Play Console > App content** form, in order. Falling Fury collects nothing, shows no ads, and needs no login — every declaration below reflects that.

## Quick reference

| Declaration | Answer |
|---|---|
| Privacy policy | `https://tamimorif.github.io/Falling-Fury/privacy.html` |
| App access | All functionality available without special access (no login) |
| Ads | No ads |
| Content rating | Everyone / PEGI 3 / ESRB Everyone |
| Target audience | 13+ |
| Data safety | No data collected, no data shared |
| Government / news / health / financial | No to all |
| DSA trader status | Required for EU distribution — complete the declaration |

## Data safety form

Go through the questionnaire with these answers:

- **Does your app collect or share any of the required user data types?** → **No.**
- **Is all of the user data collected by your app encrypted in transit?** → not shown once collection is answered "No".
- **Do you provide a way for users to request that their data is deleted?** → not shown once collection is answered "No".

Result: **Data collected: No. Data shared: No.** The form completes with no data types selected in either section.

Why this is correct:

- The only thing the app stores is the player's **high score, saved locally via `localStorage`** inside the app's own WebView storage. It never leaves the device, is not visible to us, and is removed when the app is uninstalled.
- No accounts, no analytics, no crash SDKs, no ad SDKs, no third-party data libraries of any kind.
- The game ships entirely inside the bundle — it is fully playable in Airplane Mode.

### Security practices section

- **No account creation** — nothing to sign in to.
- **All data stays on-device** — a single local high score, stored by the app itself; nothing is transmitted anywhere.
- **The INTERNET permission is not used for network calls.** The Capacitor Android template declares `android.permission.INTERNET`; here it only exists because Capacitor serves the bundled web assets to the WebView through an on-device local server. The app makes **no outbound requests** — verified by playing a full round in Airplane Mode. If Play's review asks, this is the explanation.

## Content rating questionnaire (IARC)

- **Category:** Game.
- **Violence:** No — the app shows abstract falling shapes (green/red diamonds), no characters, no blood, no combat.
- **Sexuality, language, controlled substances, gambling:** No to all.
- **User-generated content / user interaction / location sharing:** No — the game is single-player and offline.
- **Digital purchases / ads:** No.
- **Miscellaneous:** nothing objectionable.

Expected result: **Everyone (ESRB), PEGI 3, rated for all ages.**

## Target audience

- Select **13+** (not under-13). The game is family-friendly, but 13+ keeps the app **outside the Families policy flow**, which adds extra requirements and review steps for a one-person release.
- Do not opt into the Designed for Families program.

## Ads declaration

- **No ads** — the app contains no advertising and no ad SDKs. Answer **No** in App content > Ads.

## App access

- **All functionality is available without special access.** There is no login, no account, no paywall, and no restricted area — reviewers can play immediately after install.

## Government apps, news, health, financial features

- Answer **No** to every one of these declarations.

## DSA trader status (EU)

- Required if you distribute in the EU. If you are not registered as a trader with the EU (or are an individual acting outside a trade), declare that status in Play Console; if you are distributing as a trader, provide the required contact and verification details. Complete this declaration before submitting — releases can be blocked for EU users without it.

## Compliance summary

- **No data collected, no data shared** — matches the privacy policy at `https://tamimorif.github.io/Falling-Fury/privacy.html`.
- **No ads, no IAP, no accounts** — matches the store listing copy in `listing.md`.
- **No network access in practice** — the game is fully offline.
