/**
 * Post-processes the adaptive-icon XMLs produced by @capacitor/assets.
 *
 * The generator insets BOTH layers by 16.7%, which leaves a transparent ring
 * around the icon that shows through launcher masks (the background layer must
 * be full-bleed). The foreground keeps its inset so the gem stays inside the
 * safe zone. Idempotent — safe to re-run after every `capacitor-assets generate`.
 */
import { readFileSync, writeFileSync, existsSync } from 'node:fs';

const FILES = [
  'android/app/src/main/res/mipmap-anydpi-v26/ic_launcher.xml',
  'android/app/src/main/res/mipmap-anydpi-v26/ic_launcher_round.xml',
];

let patched = 0;
for (const file of FILES) {
  if (!existsSync(file)) continue;
  const before = readFileSync(file, 'utf8');
  const after = before.replace(
    /<background>\s*<inset android:drawable="@mipmap\/ic_launcher_background"[^/]*\/>\s*<\/background>/,
    '<background android:drawable="@mipmap/ic_launcher_background" />'
  );
  if (after !== before) {
    writeFileSync(file, after);
    patched++;
  }
}
console.log(`patch-adaptive-icons: ${patched} file(s) updated`);
