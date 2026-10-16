# Falling Fury — Android wrapper (Capacitor)

Native Android shell for the HTML5 game in `apps/web/site`. Built with **Capacitor 8.5.3**
(Gradle wrapper 8.14.3, AGP 8.13.0) and shipped to Google Play as an **Android App Bundle (.aab)**.

| Item | Value |
| --- | --- |
| App ID / applicationId | `com.tamimorif.fallingfury` |
| App name / display name | Falling Fury |
| Web assets dir (`webDir`) | `../web/site` |
| Native project | `android/` (module `app`) under `apps/android` |
| min / target / compile SDK | 24 / 36 / 36 (Android 7.0+; meets Play's API 36 requirement) |
| versionCode / versionName | 1 / 1.0 |
| Orientation | Portrait only (`android:screenOrientation="portrait"`) |
| Plugins | `@capacitor/haptics`, `@capacitor/share`, `@capacitor/status-bar` |
| Icons / splash | Adaptive icons + splash generated via `npm run assets` |
| Signing | `android/keystore.properties` (gitignored) → `signingConfigs.release` applied to the release build type |
| Launch activity | `com.tamimorif.fallingfury/.MainActivity` |

## 1. Install dependencies and sync web assets

```bash
cd apps/android
npm install
npm run copy          # cap copy android — copies apps/web/site into the native project (fast)
npm run sync          # cap sync android — copy + update native plugins (use after plugin changes)
```

The native app ships a **snapshot** of `apps/web/site` — it does not read the source live.
**Re-run `npm run copy` (or `npm run sync`) after every change to `apps/web/site`.**

## 2. Open in Android Studio or build from the CLI

```bash
npm run open          # cap open android — opens the project in Android Studio
```

Or skip Android Studio entirely and build from the terminal (see §3 for the JDK):

```bash
npm run apk:debug     # debug APK for emulator/device testing
npm run bundle        # signed release .aab for Google Play
```

> The first Gradle build downloads dependencies — an internet connection is required once.

## 3. JDK requirement (important)

Gradle 8.14.3 does **not** run on this machine's default **Java 25** — builds fail with:

```
Unsupported class file major version 69
```

Use **JDK 21** (or 17) for every build.

**Terminal fix:**

```bash
export JAVA_HOME="/opt/homebrew/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home"
```

**Android Studio fix:** Settings > Build, Execution, Deployment > Build Tools > Gradle >
**Gradle JDK = 21**. The bundled JBR 25 also fails — it must be switched to a JDK 21.

## 4. Release signing and build

Signing is already wired up:

- **Keystore:** `~/keystores/falling-fury-upload.jks` (alias `upload`).
- **Credentials:** `apps/android/android/keystore.properties` — **gitignored, never commit it.**
  `app/build.gradle` loads this file and applies `signingConfigs.release` to the release build type.
- ⭐ **Back up the keystore and `keystore.properties` together.** Losing the upload key requires a
  Play Console upload-key reset.
- ⭐ **Bump `versionCode` in `apps/android/android/app/build.gradle` for every Play upload**
  (bump `versionName` for user-facing releases).

Build the signed bundle:

```bash
cd apps/android
export JAVA_HOME="/opt/homebrew/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home"
npm run bundle
```

Output: `apps/android/android/app/build/outputs/bundle/release/app-release.aab`

Verify the signature (must print `jar verified`):

```bash
"$JAVA_HOME/bin/jarsigner" -verify apps/android/android/app/build/outputs/bundle/release/app-release.aab
```

## 5. Useful commands

| Command (from `apps/android`) | What it does |
| --- | --- |
| `npm run copy` | `cap copy android` — copy web assets only (fast) |
| `npm run sync` | `cap sync android` — copy web assets + update native plugins |
| `npm run open` | `cap open android` — open Android Studio |
| `npm run assets` | Regenerate adaptive icons + splash from `assets/` |
| `npm run bundle` | Build the signed release `.aab` for Play |
| `npm run apk` | Build the signed release `.apk` (device testing) |
| `npm run apk:debug` | Build the debug `.apk` (`android/app/build/outputs/apk/debug/app-debug.apk`) |
| `adb install -r android/app/build/outputs/apk/debug/app-debug.apk` | Install the debug APK on a running emulator/device |

Full instructions for shipping to Google Play: `docs/playstore/checklist.md`.

## 6. Troubleshooting

- **Game changes not showing in the app** → you forgot `npm run copy` (`cap copy android`).
- **`Unsupported class file major version 69`** → Gradle is running on Java 25. Set `JAVA_HOME`
  to JDK 21 (terminal) or **Gradle JDK = 21** (Android Studio) — see §3.
- **Gradle daemon lock / "Timeout waiting to lock"** → another build is still running. Wait for it
  to finish and retry; if it's stuck, run `./gradlew --stop` from `apps/android/android` (with
  JDK 21 set) and rebuild.
- **Unsigned AAB** → `apps/android/android/keystore.properties` is missing or unreadable.
  Restore it from your backup (and confirm `storeFile` points at the keystore).
- **`cap` complains about TypeScript** → keep `typescript` in `devDependencies`
  (required to read `capacitor.config.ts`).
