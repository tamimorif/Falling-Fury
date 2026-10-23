(() => {
  "use strict";

  /* ════════════════════════════════════════════════
     Constants
     ════════════════════════════════════════════════ */
  const STORAGE_KEY = "falling_fury_high";
  const GAME_HEIGHT = 460;             // logical height; width follows device aspect
  const MIN_GAME_WIDTH = 320;
  const MAX_GAME_WIDTH = 1100;
  const MAX_HEALTH = 10;
  const MIN_SPAWN_GAP = 80;
  const INITIAL_SPAWN_INTERVAL = 1.4;
  const MIN_SPAWN_INTERVAL = 0.34;
  const BASE_SPEED_MIN = 65;
  const BASE_SPEED_MAX = 135;
  const SPEED_RAMP = 2.8;
  const COMBO_WINDOW = 1.2;            // seconds to keep combo alive
  const STAR_COUNT = 60;
  const WAVE_DURATION = 20;            // seconds per wave
  const SURGE_EVERY = 5;               // every Nth wave is a surge
  const SURGE_DURATION = 10;           // seconds of heavy spawning
  const HEAL_EVERY = 4;                // heal after this many green catches when injured
  const HIT_STOP = 0.045;              // seconds of impact freeze
  const MAX_ESCAPE_BASE = 3;           // escape damage keeps growing until this cap
  const RESUME_SECONDS = 3;            // countdown after unpausing

  let GAME_WIDTH = 780;

  /* ════════════════════════════════════════════════
     DOM
     ════════════════════════════════════════════════ */
  const canvas       = document.getElementById("game-canvas");
  const ctx          = canvas.getContext("2d");
  const healthEl     = document.getElementById("health");
  const healthBarEl  = document.getElementById("health-bar");
  const scoreEl      = document.getElementById("score");
  const highScoreEl  = document.getElementById("high-score");
  const waveBadge    = document.getElementById("wave-badge");
  const startScreen  = document.getElementById("start-screen");
  const startBtn     = document.getElementById("start-btn");
  const gameOverEl   = document.getElementById("game-over");
  const finalScoreEl = document.getElementById("final-score-line");
  const newHighEl    = document.getElementById("new-high");
  const restartBtn   = document.getElementById("restart-btn");
  const arenaWrap    = document.getElementById("arena-wrap");
  const comboDisplay = document.getElementById("combo-display");
  const comboTextEl  = document.getElementById("combo-text");
  const playerGreetingEl = document.getElementById("player-greeting");
  const leaderboardCard = document.getElementById("leaderboard-card");
  const leaderboardList = document.getElementById("leaderboard-list");
  const leaderboardYou = document.getElementById("leaderboard-you");
  const leaderboardStatus = document.getElementById("leaderboard-status");

  /* ════════════════════════════════════════════════
     Utility
     ════════════════════════════════════════════════ */
  const rand = (lo, hi) => Math.random() * (hi - lo) + lo;
  const clamp = (v, lo, hi) => Math.max(lo, Math.min(hi, v));

  /* ════════════════════════════════════════════════
     DPR-aware Canvas Setup (fills the arena on any aspect)
     ════════════════════════════════════════════════ */
  function setupCanvas() {
    const rect = canvas.getBoundingClientRect();
    if (rect.width < 40 || rect.height < 40) return;

    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    const targetWidth = Math.round(clamp(GAME_HEIGHT * (rect.width / rect.height), MIN_GAME_WIDTH, MAX_GAME_WIDTH));
    const targetBufW = Math.round(targetWidth * dpr);
    const targetBufH = Math.round(GAME_HEIGHT * dpr);

    if (canvas.width === targetBufW && canvas.height === targetBufH) {
      GAME_WIDTH = targetWidth;
      return;
    }

    GAME_WIDTH = targetWidth;
    canvas.width = targetBufW;
    canvas.height = targetBufH;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

    // Keep existing enemies inside the new world bounds
    for (const e of state.enemies) {
      e.x = clamp(e.x, e.size, GAME_WIDTH - e.size);
    }
  }

  /* ════════════════════════════════════════════════
     Native bridge (Capacitor haptics / share — no-op on web)
     ════════════════════════════════════════════════ */
  function haptic(name) {
    const bridge = window.FallingFuryNative;
    if (bridge && typeof bridge.haptic === "function") {
      try { bridge.haptic(name); } catch (_) { /* never break the game */ }
    }
  }

  function renderLeaderboard(data) {
    if (!leaderboardCard || !data || data.available !== true) return;
    leaderboardCard.classList.remove("hidden");
    leaderboardList.replaceChildren();
    const entries = Array.isArray(data.entries) ? data.entries.slice(0, 10) : [];
    entries.forEach((entry, index) => {
      const row = document.createElement("li");
      row.className = "leaderboard-row" + (entry.isPlayer ? " is-player" : "");
      const rank = document.createElement("span");
      rank.className = "leaderboard-rank";
      rank.textContent = "#" + (entry.rank || index + 1);
      const name = document.createElement("span");
      name.className = "leaderboard-name";
      name.textContent = entry.name || "Player";
      const score = document.createElement("span");
      score.className = "leaderboard-score";
      score.textContent = String(entry.score || 0);
      row.append(rank, name, score);
      leaderboardList.appendChild(row);
    });
    if (data.playerName && playerGreetingEl) {
      playerGreetingEl.textContent = "Welcome, " + data.playerName;
      playerGreetingEl.classList.remove("hidden");
    }
    if (data.playerRank && data.playerRank > 10) {
      leaderboardYou.textContent = "You · #" + data.playerRank + " · " + (data.playerName || "Player") + " · " + (data.playerScore || 0);
      leaderboardYou.classList.remove("hidden");
    } else {
      leaderboardYou.classList.add("hidden");
    }
    leaderboardStatus.textContent = entries.length ? "All-time scores" : "Be the first to score!";
  }

  async function refreshLeaderboard(syncLocalScore) {
    const bridge = window.FallingFuryNative;
    if (!bridge || !bridge.isNative || typeof bridge.getLeaderboard !== "function") return;
    leaderboardCard.classList.remove("hidden");
    leaderboardStatus.textContent = "Loading scores…";
    try {
      const data = await bridge.getLeaderboard();
      if (data && data.available) {
        renderLeaderboard(data);
        if (syncLocalScore !== false && state && state.highScore > 0 && typeof bridge.submitHighScore === "function") {
          const submitted = await bridge.submitHighScore(state.highScore);
          if (submitted) renderLeaderboard(await bridge.getLeaderboard());
        }
      } else {
        leaderboardStatus.textContent = "Sign in to Game Center to see global scores.";
      }
    } catch (_) {
      leaderboardStatus.textContent = "Leaderboard unavailable. You can still play offline.";
    }
  }

  /* ════════════════════════════════════════════════
     Audio (Web Audio API — no external files)
     ════════════════════════════════════════════════ */
  let audioCtx = null;
  function ensureAudio() {
    if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    if (audioCtx.state === "suspended") audioCtx.resume();
  }

  function playTone(freq, dur, type = "square", vol = 0.12) {
    if (!audioCtx) return;
    const osc = audioCtx.createOscillator();
    const gain = audioCtx.createGain();
    osc.type = type;
    osc.frequency.value = freq;
    gain.gain.setValueAtTime(vol, audioCtx.currentTime);
    gain.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + dur);
    osc.connect(gain).connect(audioCtx.destination);
    osc.start();
    osc.stop(audioCtx.currentTime + dur);
  }

  function sfxHit()     { playTone(880, 0.08, "square", 0.10); playTone(1320, 0.06, "sine", 0.06); }
  function sfxDanger()  { playTone(1200, 0.06, "square", 0.12); playTone(1600, 0.05, "sine", 0.08); }
  function sfxDamage()  { playTone(120, 0.25, "sawtooth", 0.10); }
  function sfxCombo()   { playTone(660, 0.06, "sine", 0.08); playTone(990, 0.08, "sine", 0.07); }
  function sfxCrack()   { playTone(320, 0.09, "square", 0.09); }
  function sfxBomb()    { playTone(90, 0.35, "sawtooth", 0.13); playTone(60, 0.4, "square", 0.08); }
  function sfxGameOver(){ playTone(220, 0.3, "sawtooth", 0.10); playTone(165, 0.4, "sawtooth", 0.08); }

  /* ════════════════════════════════════════════════
     Stars (background ambiance)
     ════════════════════════════════════════════════ */
  const stars = Array.from({ length: STAR_COUNT }, () => ({
    x: rand(0, GAME_WIDTH),
    y: rand(0, GAME_HEIGHT),
    r: rand(0.4, 1.4),
    speed: rand(4, 15),
    alpha: rand(0.2, 0.6),
  }));

  function updateStars(dt) {
    for (const s of stars) {
      s.y += s.speed * dt;
      if (s.y > GAME_HEIGHT + 2) {
        s.y = -2;
        s.x = rand(0, GAME_WIDTH);
      }
    }
  }

  function drawStars() {
    for (const s of stars) {
      ctx.globalAlpha = s.alpha;
      ctx.fillStyle = "#c4b5fd";
      ctx.beginPath();
      ctx.arc(s.x, s.y, s.r, 0, Math.PI * 2);
      ctx.fill();
    }
    ctx.globalAlpha = 1;
  }

  /* ════════════════════════════════════════════════
     Particles
     ════════════════════════════════════════════════ */
  let particles = [];

  function spawnParticles(x, y, color, count = 10) {
    for (let i = 0; i < count; i++) {
      const angle = rand(0, Math.PI * 2);
      const speed = rand(80, 220);
      particles.push({
        x, y,
        vx: Math.cos(angle) * speed,
        vy: Math.sin(angle) * speed,
        life: rand(0.25, 0.55),
        maxLife: 0,
        size: rand(2, 5),
        color,
      });
      particles[particles.length - 1].maxLife = particles[particles.length - 1].life;
    }
  }

  function spawnRipple(x, y) {
    particles.push({
      x, y,
      vx: 0, vy: 0,
      life: 0.35,
      maxLife: 0.35,
      size: 4,
      color: "rgba(167, 139, 250, 0.5)",
      isRipple: true,
      radius: 0,
    });
  }

  function updateParticles(dt) {
    for (let i = particles.length - 1; i >= 0; i--) {
      const p = particles[i];
      p.life -= dt;
      if (p.life <= 0) { particles.splice(i, 1); continue; }
      if (p.isRipple) {
        p.radius += 180 * dt;
      } else {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vy += 300 * dt; // gravity
      }
    }
  }

  function drawParticles() {
    for (const p of particles) {
      const alpha = Math.max(0, p.life / p.maxLife);
      ctx.globalAlpha = alpha;
      if (p.isRipple) {
        ctx.strokeStyle = p.color;
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.radius, 0, Math.PI * 2);
        ctx.stroke();
      } else {
        ctx.fillStyle = p.color;
        ctx.fillRect(p.x - p.size / 2, p.y - p.size / 2, p.size, p.size);
      }
    }
    ctx.globalAlpha = 1;
  }

  /* ════════════════════════════════════════════════
     Floating Score Text
     ════════════════════════════════════════════════ */
  let floatingTexts = [];

  function spawnFloatingText(x, y, text, color) {
    floatingTexts.push({ x, y, text, color, life: 0.8, maxLife: 0.8 });
  }

  function updateFloatingTexts(dt) {
    for (let i = floatingTexts.length - 1; i >= 0; i--) {
      const ft = floatingTexts[i];
      ft.life -= dt;
      ft.y -= 50 * dt;
      if (ft.life <= 0) floatingTexts.splice(i, 1);
    }
  }

  function drawFloatingTexts() {
    for (const ft of floatingTexts) {
      const alpha = Math.max(0, ft.life / ft.maxLife);
      ctx.globalAlpha = alpha;
      ctx.fillStyle = ft.color;
      ctx.font = "bold 16px Outfit, system-ui, sans-serif";
      ctx.textAlign = "center";
      ctx.fillText(ft.text, ft.x, ft.y);
    }
    ctx.globalAlpha = 1;
    ctx.textAlign = "start";
  }

  /* ════════════════════════════════════════════════
     Wave Banner (created dynamically — styled inline)
     ════════════════════════════════════════════════ */
  let waveBannerEl = null;
  let waveBannerTimer = null;

  function ensureWaveBanner() {
    if (waveBannerEl) return waveBannerEl;
    waveBannerEl = document.createElement("div");
    waveBannerEl.id = "wave-banner";
    waveBannerEl.style.cssText = [
      "position:fixed",
      "top:22%",
      "left:50%",
      "transform:translate(-50%,-50%)",
      "font-family:Outfit,system-ui,sans-serif",
      "font-weight:800",
      "font-size:clamp(1.6rem,7vw,2.4rem)",
      "letter-spacing:4px",
      "text-transform:uppercase",
      "color:#c4b5fd",
      "text-shadow:0 0 24px rgba(167,139,250,0.8)",
      "opacity:0",
      "transition:opacity 0.25s ease",
      "pointer-events:none",
      "z-index:40",
      "text-align:center",
      "white-space:nowrap",
    ].join(";");
    document.body.appendChild(waveBannerEl);
    return waveBannerEl;
  }

  function showWaveBanner(title, sub) {
    const el = ensureWaveBanner();
    el.innerHTML = sub
      ? `${title}<div style="font-size:0.5em;letter-spacing:6px;color:#f87171;margin-top:6px">${sub}</div>`
      : title;
    el.style.opacity = "1";
    clearTimeout(waveBannerTimer);
    waveBannerTimer = setTimeout(() => { el.style.opacity = "0"; }, 1500);
  }

  /* ════════════════════════════════════════════════
     Pause Overlay (created dynamically — styled inline)
     ════════════════════════════════════════════════ */
  let pauseEl = null;

  function ensurePauseOverlay() {
    if (pauseEl) return pauseEl;
    pauseEl = document.createElement("div");
    pauseEl.id = "pause-overlay";
    pauseEl.style.cssText = [
      "position:fixed",
      "inset:0",
      "display:none",
      "align-items:center",
      "justify-content:center",
      "background:rgba(11,10,26,0.85)",
      "backdrop-filter:blur(6px)",
      "-webkit-backdrop-filter:blur(6px)",
      "z-index:60",
      "font-family:Outfit,system-ui,sans-serif",
      "text-align:center",
      "cursor:pointer",
    ].join(";");
    pauseEl.innerHTML = `
      <div>
        <div style="font-size:1.5rem;font-weight:800;letter-spacing:3px;text-transform:uppercase;color:#c4b5fd">Paused</div>
        <div id="pause-sub" style="margin-top:10px;font-size:0.95rem;color:rgba(238,232,245,0.7)">Tap to resume</div>
      </div>`;
    pauseEl.addEventListener("pointerdown", (e) => {
      e.preventDefault();
      beginResume();
    });
    document.body.appendChild(pauseEl);
    return pauseEl;
  }

  function pauseGame() {
    if (state.phase !== "playing") return;
    state.phase = "paused";
    state.resumeIn = 0;
    const el = ensurePauseOverlay();
    el.style.display = "flex";
    const sub = el.querySelector("#pause-sub");
    if (sub) sub.textContent = "Tap to resume";
    if (audioCtx && audioCtx.state === "running") audioCtx.suspend();
  }

  function beginResume() {
    if (state.phase !== "paused" || state.resumeIn > 0) return;
    state.resumeIn = RESUME_SECONDS;
  }

  function resumeGame() {
    state.phase = "playing";
    state.resumeIn = 0;
    state.lastFrame = performance.now();
    if (pauseEl) pauseEl.style.display = "none";
    ensureAudio();
  }

  document.addEventListener("visibilitychange", () => {
    if (document.hidden) pauseGame();
  });

  /* ════════════════════════════════════════════════
     Game State
     ════════════════════════════════════════════════ */
  const state = {
    phase: "start",  // "start" | "playing" | "paused" | "over"
    score: 0,
    highScore: Number(localStorage.getItem(STORAGE_KEY) || 0),
    health: MAX_HEALTH,
    enemies: [],
    spawnTimer: 0,
    elapsed: 0,
    lastFrame: 0,
    combo: 0,
    comboTimer: 0,
    wave: 1,
    greenStreak: 0,      // green catches since last heal
    surgeUntil: 0,       // elapsed time when the current surge ends
    hitStop: 0,          // impact-freeze timer
    vignette: 0,         // damage flash amount 0..1
    resumeIn: 0,         // unpause countdown
    // Dirty flags for HUD
    _prevScore: -1,
    _prevHealth: -1,
    _prevHigh: -1,
    _prevWave: -1,
  };

  /* ════════════════════════════════════════════════
     HUD (dirty-check — only touch DOM on change)
     ════════════════════════════════════════════════ */
  function renderHud() {
    if (state.score !== state._prevScore) {
      scoreEl.textContent = String(state.score);
      state._prevScore = state.score;
    }
    if (state.health !== state._prevHealth) {
      healthEl.textContent = String(state.health);
      const pct = Math.max(0, (state.health / MAX_HEALTH) * 100);
      healthBarEl.style.width = pct + "%";
      state._prevHealth = state.health;
    }
    if (state.highScore !== state._prevHigh) {
      highScoreEl.textContent = String(state.highScore);
      state._prevHigh = state.highScore;
    }
    if (state.wave !== state._prevWave) {
      waveBadge.textContent = "Wave " + state.wave;
      state._prevWave = state.wave;
    }
  }

  /* ════════════════════════════════════════════════
     Screen Shake
     ════════════════════════════════════════════════ */
  function triggerShake() {
    arenaWrap.classList.remove("shake");
    // Force reflow to restart animation
    void arenaWrap.offsetWidth;
    arenaWrap.classList.add("shake");
  }

  /* ════════════════════════════════════════════════
     Combo
     ════════════════════════════════════════════════ */
  function bumpCombo() {
    state.combo++;
    state.comboTimer = COMBO_WINDOW;
    if (state.combo >= 2) {
      const mult = getComboMultiplier();
      comboTextEl.textContent = mult > 1 ? `x${state.combo} Combo! (${mult}x)` : `x${state.combo} Combo!`;
      comboDisplay.classList.remove("hidden");
      // Re-trigger animation
      comboTextEl.style.animation = "none";
      void comboTextEl.offsetWidth;
      comboTextEl.style.animation = "";
      sfxCombo();
    }
  }

  function resetCombo() {
    state.combo = 0;
    state.comboTimer = 0;
    comboDisplay.classList.add("hidden");
  }

  function updateCombo(dt) {
    if (state.combo > 0) {
      state.comboTimer -= dt;
      if (state.comboTimer <= 0) {
        state.combo = 0;
        comboDisplay.classList.add("hidden");
      }
    }
  }

  function getComboMultiplier() {
    if (state.combo >= 15) return 5;
    if (state.combo >= 10) return 4;
    if (state.combo >= 6) return 3;
    if (state.combo >= 3) return 2;
    return 1;
  }

  /* ════════════════════════════════════════════════
     Enemy Kinds
     ════════════════════════════════════════════════ */
  const KIND_CONFIG = {
    green:    { size: [16, 30], speed: 1.0,  points: 1, wobble: 28, wfreq: 1.0 },
    danger:   { size: [16, 30], speed: 1.0,  points: 3, wobble: 28, wfreq: 1.0 },
    armored:  { size: [22, 32], speed: 0.9,  points: 2, wobble: 18, wfreq: 0.9 },
    bomb:     { size: [22, 30], speed: 0.85, points: 0, wobble: 18, wfreq: 0.9 },
    zipper:   { size: [11, 15], speed: 1.55, points: 2, wobble: 80, wfreq: 2.4 },
    splitter: { size: [20, 28], speed: 1.0,  points: 1, wobble: 28, wfreq: 1.0 },
    mini:     { size: [9, 12],  speed: 1.5,  points: 1, wobble: 42, wfreq: 1.8 },
  };

  function waveDifficulty() {
    const w = state.wave - 1;
    return {
      spawnRate: 1 + Math.min(1.0, w * 0.06),
      speed: 1 + w * 0.06,
      danger: Math.min(0.45, 0.10 + state.wave * 0.02),
    };
  }

  function pickKind(dangerChance) {
    const w = state.wave;
    const bombP = w >= 2 ? Math.min(0.12, 0.05 + w * 0.006) : 0;
    const armoredP = w >= 2 ? 0.09 : 0;
    const zipperP = w >= 3 ? 0.09 : 0;
    const splitterP = w >= 3 ? 0.07 : 0;

    const r = Math.random();
    let acc = bombP;
    if (r < acc) return "bomb";
    acc += armoredP;
    if (r < acc) return "armored";
    acc += zipperP;
    if (r < acc) return "zipper";
    acc += splitterP;
    if (r < acc) return "splitter";
    return Math.random() < dangerChance ? "danger" : "green";
  }

  function tooClose(x, y, size) {
    for (const e of state.enemies) {
      const dx = x - e.x;
      const dy = y - e.y;
      const minDist = size + e.size + MIN_SPAWN_GAP;
      if (dx * dx + dy * dy < minDist * minDist) return true;
    }
    return false;
  }

  function makeEnemy(kind, x, y, size, speed) {
    const cfg = KIND_CONFIG[kind];
    return {
      kind,
      x, y, size, speed,
      wobble: rand(0.3, 1.1) * cfg.wfreq,
      wobblePhase: rand(0, Math.PI * 2),
      wobbleAmp: cfg.wobble,
      hp: kind === "armored" ? 2 : 1,
      cracked: false,
      entryProgress: 0,
      mini: kind === "mini",
    };
  }

  function enemySpeed(kind) {
    const d = waveDifficulty();
    return (rand(BASE_SPEED_MIN, BASE_SPEED_MAX) + state.elapsed * SPEED_RAMP) * d.speed * KIND_CONFIG[kind].speed;
  }

  function spawnEnemy() {
    const kind = pickKind(waveDifficulty().danger);
    const cfg = KIND_CONFIG[kind];
    const size = rand(cfg.size[0], cfg.size[1]);

    for (let attempt = 0; attempt < 10; attempt++) {
      const x = rand(size + 12, GAME_WIDTH - size - 12);
      const y = -size - rand(10, 50);
      if (!tooClose(x, y, size)) {
        state.enemies.push(makeEnemy(kind, x, y, size, enemySpeed(kind)));
        return;
      }
    }
  }

  function splitEnemy(e) {
    for (const dir of [-1, 1]) {
      const size = rand(KIND_CONFIG.mini.size[0], KIND_CONFIG.mini.size[1]);
      const x = clamp(e.x + dir * (e.size + 6), size + 4, GAME_WIDTH - size - 4);
      state.enemies.push(makeEnemy("mini", x, e.y, size, enemySpeed("mini")));
    }
  }

  function escapeDamage(e) {
    const base = Math.min(MAX_ESCAPE_BASE, 1 + Math.floor((state.wave - 1) / 4));
    if (e.kind === "danger") return base * 2;
    return base;
  }

  /* ════════════════════════════════════════════════
     Damage
     ════════════════════════════════════════════════ */
  function applyDamage(amount, x, y, label) {
    if (amount <= 0) return;
    state.health = Math.max(0, state.health - amount);
    state.vignette = 1;
    state.greenStreak = 0;
    resetCombo();
    sfxDamage();
    triggerShake();
    haptic(amount >= 2 ? "heavy" : "warning");
    if (x != null) spawnFloatingText(x, y, label || `-${amount}`, "#f87171");
  }

  /* ════════════════════════════════════════════════
     Update
     ════════════════════════════════════════════════ */
  function update(dt) {
    if (state.phase !== "playing") return;

    state.elapsed += dt;
    state.spawnTimer += dt;
    state.vignette = Math.max(0, state.vignette - dt * 2.4);

    // Wave progression
    const newWave = Math.max(1, Math.floor(state.elapsed / WAVE_DURATION) + 1);
    if (newWave !== state.wave) {
      state.wave = newWave;
      if (newWave % SURGE_EVERY === 0) {
        state.surgeUntil = state.elapsed + SURGE_DURATION;
        showWaveBanner(`Wave ${newWave}`, "⚡ Surge!");
      } else {
        showWaveBanner(`Wave ${newWave}`, "Danger rising");
      }
    }

    // Spawn
    const d = waveDifficulty();
    const surging = state.elapsed < state.surgeUntil;
    let interval = Math.max(MIN_SPAWN_INTERVAL, INITIAL_SPAWN_INTERVAL - state.elapsed * 0.0105);
    interval /= d.spawnRate;
    if (surging) interval *= 0.55;
    if (state.spawnTimer >= interval) {
      spawnEnemy();
      state.spawnTimer = 0;
    }

    // Move enemies
    for (let i = state.enemies.length - 1; i >= 0; i--) {
      const e = state.enemies[i];
      e.entryProgress = Math.min(1, e.entryProgress + dt * 3);
      e.y += e.speed * dt;
      e.x += Math.sin(state.elapsed * e.wobble + e.wobblePhase) * e.wobbleAmp * dt;
      e.x = clamp(e.x, e.size, GAME_WIDTH - e.size);

      if (e.y - e.size > GAME_HEIGHT) {
        state.enemies.splice(i, 1);
        // Bombs fizzle out harmlessly; minis are too small to hurt
        if (e.kind === "bomb" || e.kind === "mini") {
          spawnParticles(e.x, GAME_HEIGHT, e.kind === "bomb" ? "#64748b" : "#34d399", 6);
          continue;
        }
        applyDamage(escapeDamage(e), e.x, GAME_HEIGHT - 10, e.kind === "danger" ? "DANGER!" : undefined);
      }
    }

    updateCombo(dt);
    updateStars(dt);
    updateParticles(dt);
    updateFloatingTexts(dt);

    if (state.health <= 0) {
      state.phase = "over";
      const isNewHigh = state.score > state.highScore;
      if (isNewHigh) {
        state.highScore = state.score;
        localStorage.setItem(STORAGE_KEY, String(state.highScore));
      }
      finalScoreEl.textContent = `Your score: ${state.score}`;
      newHighEl.classList.toggle("hidden", !isNewHigh);
      gameOverEl.classList.remove("hidden");
      comboDisplay.classList.add("hidden");
      sfxGameOver();
      haptic(isNewHigh ? "success" : "error");
      updateShareButton(isNewHigh);
      const nativeBridge = window.FallingFuryNative;
      if (isNewHigh && nativeBridge && nativeBridge.isNative && typeof nativeBridge.submitHighScore === "function") {
        nativeBridge.submitHighScore(state.highScore).then((submitted) => {
          if (submitted) refreshLeaderboard(false);
        });
      }
    }

    renderHud();
  }

  /* ════════════════════════════════════════════════
     Render
     ════════════════════════════════════════════════ */
  function drawBackground() {
    const grad = ctx.createLinearGradient(0, 0, 0, GAME_HEIGHT);
    grad.addColorStop(0, "#0b0a1a");
    grad.addColorStop(0.5, "#131136");
    grad.addColorStop(1, "#1c1a3a");
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

    // Grid lines
    ctx.strokeStyle = "rgba(167, 139, 250, 0.03)";
    ctx.lineWidth = 1;
    for (let x = 0; x < GAME_WIDTH; x += 40) {
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, GAME_HEIGHT);
      ctx.stroke();
    }
    for (let y = 0; y < GAME_HEIGHT; y += 40) {
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(GAME_WIDTH, y);
      ctx.stroke();
    }
  }

  function drawDiamondBody(size, fill, stroke, glow) {
    ctx.shadowColor = glow;
    ctx.shadowBlur = 16;
    ctx.fillStyle = fill;
    ctx.strokeStyle = stroke;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(0, -size);
    ctx.lineTo(size, 0);
    ctx.lineTo(0, size);
    ctx.lineTo(-size, 0);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();
    ctx.shadowBlur = 0;
  }

  function drawHighlight(size) {
    ctx.fillStyle = "rgba(255, 255, 255, 0.15)";
    ctx.beginPath();
    ctx.ellipse(
      -size * 0.15, -size * 0.18,
      size * 0.3, size * 0.16,
      -0.3, 0, Math.PI * 2
    );
    ctx.fill();
  }

  function drawGlyph(text, size, color) {
    ctx.shadowBlur = 0;
    ctx.fillStyle = color;
    ctx.font = `bold ${Math.round(size)}px Outfit, system-ui, sans-serif`;
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(text, 0, 1);
    ctx.textAlign = "start";
    ctx.textBaseline = "alphabetic";
  }

  function drawEnemies() {
    for (const e of state.enemies) {
      const alpha = Math.min(1, e.entryProgress);

      // Zipper motion trail
      if (e.kind === "zipper") {
        ctx.save();
        ctx.globalAlpha = alpha * 0.28;
        ctx.translate(e.x, e.y - 14);
        drawDiamondBody(e.size * 0.8, "rgba(52, 211, 153, 0.6)", "rgba(6, 95, 70, 0.5)", "rgba(52, 211, 153, 0.1)");
        ctx.restore();
      }

      ctx.save();
      ctx.globalAlpha = alpha;
      ctx.translate(e.x, e.y);

      const rot = (e.y / GAME_HEIGHT) * 0.6;
      ctx.rotate(rot);

      switch (e.kind) {
        case "danger":
          drawDiamondBody(e.size, "rgba(248, 113, 113, 0.92)", "#991b1b", "rgba(248, 113, 113, 0.3)");
          drawHighlight(e.size);
          drawGlyph("!", e.size * 0.7, "#fff");
          break;

        case "armored":
          drawDiamondBody(e.size, "rgba(96, 165, 250, 0.9)", "#1e40af", "rgba(96, 165, 250, 0.3)");
          drawHighlight(e.size);
          if (e.cracked) {
            ctx.strokeStyle = "rgba(15, 23, 42, 0.9)";
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(-e.size * 0.4, -e.size * 0.1);
            ctx.lineTo(-e.size * 0.05, e.size * 0.15);
            ctx.lineTo(e.size * 0.3, -e.size * 0.2);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(-e.size * 0.1, -e.size * 0.45);
            ctx.lineTo(-e.size * 0.05, e.size * 0.15);
            ctx.stroke();
          }
          break;

        case "bomb":
          drawDiamondBody(e.size, "rgba(30, 27, 75, 0.95)", "#dc2626", "rgba(220, 38, 38, 0.45)");
          drawGlyph("💀", e.size * 0.85, "#fff");
          break;

        case "zipper":
          drawDiamondBody(e.size, "rgba(52, 211, 153, 0.95)", "#065f46", "rgba(52, 211, 153, 0.3)");
          break;

        case "splitter":
          drawDiamondBody(e.size, "rgba(52, 211, 153, 0.88)", "#065f46", "rgba(52, 211, 153, 0.22)");
          drawHighlight(e.size);
          ctx.strokeStyle = "rgba(6, 95, 70, 0.9)";
          ctx.lineWidth = 2;
          ctx.beginPath();
          ctx.moveTo(0, -e.size * 0.7);
          ctx.lineTo(0, e.size * 0.7);
          ctx.stroke();
          break;

        case "mini":
          drawDiamondBody(e.size, "rgba(52, 211, 153, 0.85)", "#065f46", "rgba(52, 211, 153, 0.2)");
          break;

        default: // green
          drawDiamondBody(e.size, "rgba(52, 211, 153, 0.88)", "#065f46", "rgba(52, 211, 153, 0.22)");
          drawHighlight(e.size);
      }

      ctx.restore();
    }
  }

  function drawFloorLine() {
    const grad = ctx.createLinearGradient(0, GAME_HEIGHT - 4, 0, GAME_HEIGHT);
    grad.addColorStop(0, "rgba(167, 139, 250, 0.45)");
    grad.addColorStop(1, "rgba(167, 139, 250, 0)");
    ctx.fillStyle = grad;
    ctx.fillRect(0, GAME_HEIGHT - 4, GAME_WIDTH, 4);
  }

  function drawDamageOverlay() {
    const cx = GAME_WIDTH / 2;
    const cy = GAME_HEIGHT / 2;
    const inner = Math.min(GAME_WIDTH, GAME_HEIGHT) * 0.35;
    const outer = Math.max(GAME_WIDTH, GAME_HEIGHT) * 0.7;

    if (state.vignette > 0) {
      const g = ctx.createRadialGradient(cx, cy, inner, cx, cy, outer);
      g.addColorStop(0, "rgba(248, 113, 113, 0)");
      g.addColorStop(1, `rgba(248, 113, 113, ${(0.35 * state.vignette).toFixed(3)})`);
      ctx.fillStyle = g;
      ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
    }

    // Low-health pulse
    if (state.phase === "playing" && state.health > 0 && state.health <= 3) {
      const pulse = 0.16 + 0.12 * Math.sin(state.elapsed * 6);
      const g = ctx.createRadialGradient(cx, cy, inner, cx, cy, outer);
      g.addColorStop(0, "rgba(248, 113, 113, 0)");
      g.addColorStop(1, `rgba(248, 113, 113, ${pulse.toFixed(3)})`);
      ctx.fillStyle = g;
      ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
    }
  }

  function render() {
    drawBackground();
    drawStars();
    drawEnemies();
    drawParticles();
    drawFloatingTexts();
    drawFloorLine();
    drawDamageOverlay();
  }

  /* ════════════════════════════════════════════════
     Game Loop
     ════════════════════════════════════════════════ */
  function tick(now) {
    // Clamp to [0, 50ms]: protects against clock jitter and long background gaps
    const dt = clamp((now - state.lastFrame) / 1000, 0, 0.05);
    state.lastFrame = now;

    // Impact freeze — world pauses for a few frames on hits
    if (state.hitStop > 0) {
      state.hitStop -= dt;
      render();
      requestAnimationFrame(tick);
      return;
    }

    // Unpause countdown
    if (state.phase === "paused" && state.resumeIn > 0) {
      state.resumeIn -= dt;
      const sub = pauseEl && pauseEl.querySelector("#pause-sub");
      if (sub) sub.textContent = state.resumeIn > 0.4 ? String(Math.ceil(state.resumeIn)) : "GO!";
      if (state.resumeIn <= 0) resumeGame();
    }

    // Always render (stars move even on start/over/pause screen)
    if (state.phase === "start" || state.phase === "over" || state.phase === "paused") {
      updateStars(dt);
      updateParticles(dt);
      updateFloatingTexts(dt);
    }

    update(dt);
    render();
    requestAnimationFrame(tick);
  }

  /* ════════════════════════════════════════════════
     Input
     ════════════════════════════════════════════════ */
  function toCanvasCoords(clientX, clientY) {
    const rect = canvas.getBoundingClientRect();
    const scaleX = GAME_WIDTH / rect.width;
    const scaleY = GAME_HEIGHT / rect.height;
    return {
      x: (clientX - rect.left) * scaleX,
      y: (clientY - rect.top) * scaleY,
    };
  }

  function handleHit(clientX, clientY) {
    if (state.phase !== "playing") return;
    ensureAudio();

    const point = toCanvasCoords(clientX, clientY);
    spawnRipple(point.x, point.y);

    for (let i = state.enemies.length - 1; i >= 0; i--) {
      const e = state.enemies[i];
      const dx = point.x - e.x;
      const dy = point.y - e.y;
      const hitRadius = e.size * 1.15;
      if (dx * dx + dy * dy <= hitRadius * hitRadius) {
        strikeEnemy(i, e);
        break;
      }
    }
  }

  function strikeEnemy(index, e) {
    // Bombs are a trap — hitting them hurts
    if (e.kind === "bomb") {
      state.enemies.splice(index, 1);
      spawnParticles(e.x, e.y, "#f87171", 20);
      state.hitStop = HIT_STOP * 1.6;
      sfxBomb();
      applyDamage(2, e.x, e.y - e.size, "BOOM!");
      return;
    }

    // Armored diamonds crack first, break on the second hit
    if (e.kind === "armored" && e.hp > 1) {
      e.hp -= 1;
      e.cracked = true;
      spawnParticles(e.x, e.y, "#93c5fd", 8);
      sfxCrack();
      haptic("light");
      state.hitStop = HIT_STOP;
      return;
    }

    // Destroyed
    state.enemies.splice(index, 1);
    const cfg = KIND_CONFIG[e.kind];

    bumpCombo();
    const multiplier = getComboMultiplier();
    const totalPoints = cfg.points * multiplier;
    state.score += totalPoints;

    // Heal economy: only while injured, and only every HEAL_EVERY green catches
    if (e.kind === "green" && !e.mini && state.health < MAX_HEALTH) {
      state.greenStreak += 1;
      if (state.greenStreak >= HEAL_EVERY) {
        state.greenStreak = 0;
        state.health = Math.min(MAX_HEALTH, state.health + 1);
        spawnFloatingText(e.x + 20, e.y - e.size - 12, "+1 HP", "#34d399");
      }
    }

    // Effects
    const pColor = e.kind === "danger" ? "#f87171" : e.kind === "armored" ? "#93c5fd" : "#34d399";
    spawnParticles(e.x, e.y, pColor, e.kind === "danger" ? 16 : 10);
    if (e.kind === "danger") {
      sfxDanger();
      haptic("medium");
    } else if (e.kind === "armored") {
      sfxHit();
      haptic("medium");
    } else {
      sfxHit();
      haptic("light");
    }

    const label = multiplier > 1 ? `+${totalPoints} (x${multiplier})` : `+${totalPoints}`;
    spawnFloatingText(e.x, e.y - e.size, label, pColor);
    state.hitStop = HIT_STOP;

    if (e.kind === "splitter") splitEnemy(e);
  }

  // Pointer events only (works for mouse + touch; CSS touch-action:none stops scrolling)
  canvas.addEventListener("pointerdown", (e) => {
    e.preventDefault();
    handleHit(e.clientX, e.clientY);
  });
  canvas.addEventListener("contextmenu", (e) => e.preventDefault());

  /* ════════════════════════════════════════════════
     Share (native bridge when available)
     ════════════════════════════════════════════════ */
  let shareBtn = null;

  function canShare() {
    const bridge = window.FallingFuryNative;
    return (bridge && typeof bridge.shareScore === "function") ||
      (typeof navigator !== "undefined" && typeof navigator.share === "function");
  }

  function updateShareButton(isNewHigh) {
    if (!shareBtn) {
      shareBtn = document.createElement("button");
      shareBtn.type = "button";
      shareBtn.id = "share-btn";
      shareBtn.textContent = "Share Score";
      shareBtn.addEventListener("click", async () => {
        const bridge = window.FallingFuryNative;
        if (bridge && typeof bridge.shareScore === "function") {
          try { await bridge.shareScore(state.score, state.highScore); return; } catch (_) { /* fall through */ }
        }
        if (navigator.share) {
          try {
            await navigator.share({
              title: "Falling Fury",
              text: `I scored ${state.score} in Falling Fury! 💎`,
              url: "https://tamimorif.github.io/Falling-Fury/",
            });
          } catch (_) { /* user dismissed */ }
        }
      });
    }

    const show = state.score > 0 && (isNewHigh || state.score >= 10) && canShare();
    if (show) {
      if (!shareBtn.parentNode) gameOverEl.appendChild(shareBtn);
    } else if (shareBtn.parentNode) {
      shareBtn.remove();
    }
  }

  /* ════════════════════════════════════════════════
     Game Flow
     ════════════════════════════════════════════════ */
  function startGame() {
    ensureAudio();
    state.score = 0;
    state.health = MAX_HEALTH;
    state.enemies = [];
    state.phase = "playing";
    state.spawnTimer = 0;
    state.elapsed = 0;
    state.combo = 0;
    state.comboTimer = 0;
    state.wave = 1;
    state.greenStreak = 0;
    state.surgeUntil = 0;
    state.hitStop = 0;
    state.vignette = 0;
    state.resumeIn = 0;
    state.lastFrame = performance.now();
    particles = [];
    floatingTexts = [];
    state._prevScore = -1;
    state._prevHealth = -1;
    state._prevHigh = -1;
    state._prevWave = -1;
    startScreen.classList.add("hidden");
    gameOverEl.classList.add("hidden");
    comboDisplay.classList.add("hidden");
    newHighEl.classList.add("hidden");
    if (pauseEl) pauseEl.style.display = "none";
    if (shareBtn && shareBtn.parentNode) shareBtn.remove();
    renderHud();
    showWaveBanner("Wave 1", "Survive!");
  }

  startBtn.addEventListener("click", startGame);
  restartBtn.addEventListener("click", startGame);
  window.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      if (state.phase === "start" || state.phase === "over") startGame();
      else if (state.phase === "paused") beginResume();
    }
  });

  /* ════════════════════════════════════════════════
     Init
     ════════════════════════════════════════════════ */
  highScoreEl.textContent = String(state.highScore);
  refreshLeaderboard();
  setupCanvas();
  window.addEventListener("resize", setupCanvas);
  window.addEventListener("orientationchange", setupCanvas);
  if (window.ResizeObserver) {
    new ResizeObserver(() => setupCanvas()).observe(canvas.parentElement || canvas);
  }
  state.lastFrame = performance.now();
  requestAnimationFrame(tick);
})();
