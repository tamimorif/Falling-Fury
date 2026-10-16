/*!
 * Falling Fury — native bridge (Capacitor iOS/Android)
 *
 * Exposes window.FallingFuryNative:
 *   .isNative                  -> boolean, true inside the Capacitor app
 *   .haptic(name)              -> fire a haptic (no-op on web)
 *                                 name: "light" | "medium" | "heavy" |
 *                                       "success" | "warning" | "error" | "vibrate"
 *   .shareScore(score, high)   -> Promise<boolean>, never rejects
 *
 * Plain ES2017, no imports, fully defensive: every plugin call is wrapped in
 * try/catch so the game can never be broken by a missing plugin or bridge.
 */
(function (window) {
  "use strict";

  if (!window || typeof window !== "object") {
    return;
  }

  var SHARE_TITLE = "Falling Fury";
  var SHARE_URL = "https://tamimorif.github.io/Falling-Fury/";
  var SHARE_DIALOG_TITLE = "Share your score";
  var FALLBACK_TEXT = "I'm playing Falling Fury! Beat my score \uD83D\uDC8E";

  /* Uppercase enum values match @capacitor/haptics v4-v8; the capitalized
     variants are only used as a last-ditch retry for older plugin versions. */
  var IMPACT_STYLES = {
    light: ["LIGHT", "Light"],
    medium: ["MEDIUM", "Medium"],
    heavy: ["HEAVY", "Heavy"]
  };
  var NOTIFICATION_TYPES = {
    success: ["SUCCESS", "Success"],
    warning: ["WARNING", "Warning"],
    error: ["ERROR", "Error"]
  };

  function capacitor() {
    try {
      return window.Capacitor || null;
    } catch (_) {
      return null;
    }
  }

  function isNativePlatform() {
    try {
      var cap = capacitor();
      return !!(cap && typeof cap.isNativePlatform === "function" && cap.isNativePlatform());
    } catch (_) {
      return false;
    }
  }

  function plugin(name) {
    try {
      var cap = capacitor();
      if (!cap || !cap.Plugins) {
        return null;
      }
      return cap.Plugins[name] || null;
    } catch (_) {
      return null;
    }
  }

  /* Swallow rejections from plugin promises so nothing surfaces as an
     unhandled rejection in the game. */
  function swallow(result) {
    try {
      if (result && typeof result.catch === "function") {
        result.catch(function () { /* ignore */ });
      }
    } catch (_) { /* ignore */ }
    return result;
  }

  function call(context, method, args) {
    try {
      if (!context || typeof context[method] !== "function") {
        return undefined;
      }
      var out = context[method].apply(context, args || []);
      return swallow(out);
    } catch (_) {
      return undefined;
    }
  }

  function toFiniteNumber(value) {
    var n = typeof value === "number" ? value : parseFloat(value);
    if (typeof n !== "number" || !isFinite(n)) {
      return null;
    }
    return Math.max(0, Math.floor(n));
  }

  function shareText(score, highScore) {
    var value = toFiniteNumber(score);
    if (value === null) {
      value = toFiniteNumber(highScore);
    }
    if (value === null) {
      return FALLBACK_TEXT;
    }
    return "I scored " + value + " in Falling Fury! Beat that \uD83D\uDC8E";
  }

  /* ── Haptics ─────────────────────────────────────────────── */

  /* Fires `method` with the modern enum value, retrying once with the legacy
     capitalized value if the call throws or the returned promise rejects. */
  function hapticCall(Haptics, method, optionsKey, values) {
    var modern = values[0];
    var legacy = values[1];
    var retryLegacy = function () {
      try {
        var options = {};
        options[optionsKey] = legacy;
        swallow(Haptics[method](options));
      } catch (_) { /* ignore */ }
    };
    try {
      var options = {};
      options[optionsKey] = modern;
      var result = Haptics[method](options);
      if (result && typeof result.catch === "function") {
        result.catch(retryLegacy);
      }
      return true;
    } catch (_) {
      retryLegacy();
      return true;
    }
  }

  function hapticImpact(style) {
    var Haptics = plugin("Haptics");
    if (!Haptics || typeof Haptics.impact !== "function") {
      return false;
    }
    return hapticCall(Haptics, "impact", "style", style);
  }

  function hapticNotification(type) {
    var Haptics = plugin("Haptics");
    if (!Haptics || typeof Haptics.notification !== "function") {
      return false;
    }
    return hapticCall(Haptics, "notification", "type", type);
  }

  function hapticVibrate() {
    var Haptics = plugin("Haptics");
    if (!Haptics) {
      return false;
    }
    call(Haptics, "vibrate", [{ duration: 300 }]);
    return true;
  }

  function haptic(name) {
    try {
      if (!isNativePlatform()) {
        return; // no-op on web
      }
      var key = String(name === undefined || name === null ? "" : name).toLowerCase();
      if (IMPACT_STYLES[key]) {
        hapticImpact(IMPACT_STYLES[key]);
      } else if (NOTIFICATION_TYPES[key]) {
        hapticNotification(NOTIFICATION_TYPES[key]);
      } else if (key === "vibrate") {
        hapticVibrate();
      }
    } catch (_) { /* haptics must never break gameplay */ }
  }

  /* ── Share ───────────────────────────────────────────────── */

  function nativeShare(text) {
    var Share = plugin("Share");
    if (!Share) {
      return null;
    }
    var result = call(Share, "share", [{
      title: SHARE_TITLE,
      text: text,
      url: SHARE_URL,
      dialogTitle: SHARE_DIALOG_TITLE
    }]);
    if (!result || typeof result.then !== "function") {
      return null;
    }
    return result.then(function () {
      return true;
    }, function () {
      return false; // user dismissed or share failed
    });
  }

  function webShare(text) {
    try {
      if (!window.navigator || typeof window.navigator.share !== "function") {
        return null;
      }
      var result = window.navigator.share({
        title: SHARE_TITLE,
        text: text,
        url: SHARE_URL
      });
      if (!result || typeof result.then !== "function") {
        return null;
      }
      return result.then(function () {
        return true;
      }, function () {
        return false; // user dismissed
      });
    } catch (_) {
      return null;
    }
  }

  function shareScore(score, highScore) {
    return new Promise(function (resolve) {
      var settle = function (value) {
        try {
          resolve(value === true);
        } catch (_) { /* ignore */ }
      };
      var fallback = function () {
        settle(false);
      };
      try {
        var text = shareText(score, highScore);

        if (isNativePlatform()) {
          var native = nativeShare(text);
          if (native) {
            native.then(settle, fallback);
            return;
          }
          // Native shell but no Share plugin bundled: try the Web Share API.
          var web = webShare(text);
          if (web) {
            web.then(settle, fallback);
            return;
          }
          settle(false);
          return;
        }

        var shared = webShare(text);
        if (shared) {
          shared.then(settle, fallback);
          return;
        }
        settle(false);
      } catch (_) {
        fallback();
      }
    });
  }

  /* ── Status bar ──────────────────────────────────────────── */

  function hideStatusBar() {
    try {
      if (!isNativePlatform()) {
        return;
      }
      call(plugin("StatusBar"), "hide", []);
    } catch (_) { /* ignore */ }
  }

  function scheduleStatusBarHide() {
    try {
      hideStatusBar();
      if (typeof document === "undefined" || !document.addEventListener) {
        return;
      }
      var once = function () {
        hideStatusBar();
      };
      // Plugins may not be registered yet at parse time; retry on lifecycle events.
      document.addEventListener("deviceready", once);
      if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", once);
      }
      window.addEventListener("load", once);
    } catch (_) { /* ignore */ }
  }

  /* ── Public API ──────────────────────────────────────────── */

  try {
    window.FallingFuryNative = {
      isNative: isNativePlatform(),
      haptic: haptic,
      shareScore: shareScore
    };
  } catch (_) { /* ignore */ }

  scheduleStatusBarHide();
})(typeof window !== "undefined" ? window : null);
