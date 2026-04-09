(() => {
  "use strict";

  /* ════════════════════════════════════════════════
     Constants
     ════════════════════════════════════════════════ */
  const STORAGE_KEY = "falling_fury_high";
  const GAME_WIDTH = 780;
  const GAME_HEIGHT = 460;
  const MAX_HEALTH = 10;
  const MIN_SPAWN_GAP = 80;
  const INITIAL_SPAWN_INTERVAL = 1.4;
  const MIN_SPAWN_INTERVAL = 0.38;
  const BASE_SPEED_MIN = 65;
  const BASE_SPEED_MAX = 135;
  const SPEED_RAMP = 2.8;
  const COMBO_WINDOW = 1.2;       // seconds to keep combo alive
  const STAR_COUNT = 60;
  const WAVE_DURATION = 20;       // seconds per wave

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

  /* ════════════════════════════════════════════════
     DPR-aware Canvas Setup
     ════════════════════════════════════════════════ */
  function setupCanvas() {
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = GAME_WIDTH * dpr;
    canvas.height = GAME_HEIGHT * dpr;
    canvas.style.width = "100%";
    canvas.style.height = "auto";
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  }
  setupCanvas();
  window.addEventListener("resize", setupCanvas);

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

  function sfxHit()    { playTone(880, 0.08, "square", 0.10); playTone(1320, 0.06, "sine", 0.06); }
  function sfxDanger() { playTone(1200, 0.06, "square", 0.12); playTone(1600, 0.05, "sine", 0.08); }
  function sfxDamage() { playTone(120, 0.25, "sawtooth", 0.10); }
  function sfxCombo()  { playTone(660, 0.06, "sine", 0.08); playTone(990, 0.08, "sine", 0.07); }
  function sfxGameOver() { playTone(220, 0.3, "sawtooth", 0.10); playTone(165, 0.4, "sawtooth", 0.08); }

  /* ════════════════════════════════════════════════
     Utility
     ════════════════════════════════════════════════ */
  const rand = (lo, hi) => Math.random() * (hi - lo) + lo;
  const randInt = (lo, hi) => Math.floor(rand(lo, hi + 1));

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
      ctx.font = "bold 16px Outfit, sans-serif";
      ctx.textAlign = "center";
      ctx.fillText(ft.text, ft.x, ft.y);
    }
    ctx.globalAlpha = 1;
    ctx.textAlign = "start";
  }

  /* ════════════════════════════════════════════════
     Game State
     ════════════════════════════════════════════════ */
  const state = {
    phase: "start",  // "start" | "playing" | "over"
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
      comboTextEl.textContent = `x${state.combo} Combo!`;
      comboDisplay.classList.remove("hidden");
      // Re-trigger animation
      comboTextEl.style.animation = "none";
      void comboTextEl.offsetWidth;
      comboTextEl.style.animation = "";
      sfxCombo();
    }
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
    if (state.combo >= 10) return 4;
    if (state.combo >= 6) return 3;
    if (state.combo >= 3) return 2;
    return 1;
  }

  /* ════════════════════════════════════════════════
     Enemy Spawning
     ════════════════════════════════════════════════ */
  function tooClose(x, y, size) {
    for (const e of state.enemies) {
      const dx = x - e.x;
      const dy = y - e.y;
      const minDist = size + e.size + MIN_SPAWN_GAP;
      if (dx * dx + dy * dy < minDist * minDist) return true;
    }
    return false;
  }

  function spawnEnemy() {
    const size = rand(16, 30);
    const danger = Math.random() < Math.min(0.35, 0.12 + state.wave * 0.03);
    const speedBoost = state.elapsed * SPEED_RAMP;

    for (let attempt = 0; attempt < 10; attempt++) {
      const x = rand(size + 12, GAME_WIDTH - size - 12);
      const y = -size - rand(10, 50);
      if (!tooClose(x, y, size)) {
        state.enemies.push({
          x, y, size,
          speed: rand(BASE_SPEED_MIN, BASE_SPEED_MAX) + speedBoost,
          wobble: rand(0.3, 1.1),
          wobblePhase: rand(0, Math.PI * 2),
          danger,
          entryProgress: 0,  // 0→1 fade-in
        });
        return;
      }
    }
  }

  /* ════════════════════════════════════════════════
     Update
     ════════════════════════════════════════════════ */
  function update(dt) {
    if (state.phase !== "playing") return;

    state.elapsed += dt;
    state.spawnTimer += dt;

    // Wave progression
    const newWave = Math.floor(state.elapsed / WAVE_DURATION) + 1;
    if (newWave !== state.wave) {
      state.wave = newWave;
    }

    // Spawn
    const interval = Math.max(MIN_SPAWN_INTERVAL, INITIAL_SPAWN_INTERVAL - state.elapsed * 0.011);
    if (state.spawnTimer >= interval) {
      spawnEnemy();
      state.spawnTimer = 0;
    }

    // Move enemies
    for (let i = state.enemies.length - 1; i >= 0; i--) {
      const e = state.enemies[i];
      e.entryProgress = Math.min(1, e.entryProgress + dt * 3);
      e.y += e.speed * dt;
      e.x += Math.sin(state.elapsed * e.wobble + e.wobblePhase) * 28 * dt;
      e.x = Math.max(e.size, Math.min(GAME_WIDTH - e.size, e.x));

      if (e.y - e.size > GAME_HEIGHT) {
        state.enemies.splice(i, 1);
        const dmg = e.danger ? 2 : 1;
        state.health = Math.max(0, state.health - dmg);
        sfxDamage();
        triggerShake();
        // Reset combo on miss
        state.combo = 0;
        state.comboTimer = 0;
        comboDisplay.classList.add("hidden");
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

  function drawEnemies() {
    for (const e of state.enemies) {
      const alpha = Math.min(1, e.entryProgress);
      ctx.save();
      ctx.globalAlpha = alpha;
      ctx.translate(e.x, e.y);

      const rot = (e.y / GAME_HEIGHT) * 0.6;
      ctx.rotate(rot);

      // Glow
      const glowColor = e.danger
        ? "rgba(248, 113, 113, 0.3)"
        : "rgba(52, 211, 153, 0.22)";
      ctx.shadowColor = glowColor;
      ctx.shadowBlur = 16;

      // Body
      const fill = e.danger
        ? "rgba(248, 113, 113, 0.92)"
        : "rgba(52, 211, 153, 0.88)";
      const stroke = e.danger ? "#991b1b" : "#065f46";

      ctx.fillStyle = fill;
      ctx.strokeStyle = stroke;
      ctx.lineWidth = 2;

      // Diamond
      ctx.beginPath();
      ctx.moveTo(0, -e.size);
      ctx.lineTo(e.size, 0);
      ctx.lineTo(0, e.size);
      ctx.lineTo(-e.size, 0);
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      // Highlight
      ctx.shadowBlur = 0;
      ctx.fillStyle = "rgba(255, 255, 255, 0.15)";
      ctx.beginPath();
      ctx.ellipse(
        -e.size * 0.15, -e.size * 0.18,
        e.size * 0.3, e.size * 0.16,
        -0.3, 0, Math.PI * 2
      );
      ctx.fill();

      // Danger marker
      if (e.danger) {
        ctx.shadowBlur = 0;
        ctx.fillStyle = "#fff";
        ctx.font = `bold ${Math.round(e.size * 0.7)}px Outfit, sans-serif`;
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillText("!", 0, 0);
        ctx.textAlign = "start";
        ctx.textBaseline = "alphabetic";
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

  function render() {
    drawBackground();
    drawStars();
    drawEnemies();
    drawParticles();
    drawFloatingTexts();
    drawFloorLine();
  }

  /* ════════════════════════════════════════════════
     Game Loop
     ════════════════════════════════════════════════ */
  function tick(now) {
    const dt = Math.min(0.05, (now - state.lastFrame) / 1000);
    state.lastFrame = now;

    // Always render (stars move even on start/over screen)
    if (state.phase === "start" || state.phase === "over") {
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
        state.enemies.splice(i, 1);

        // Combo + scoring
        bumpCombo();
        const basePoints = e.danger ? 3 : 1;
        const multiplier = getComboMultiplier();
        const totalPoints = basePoints * multiplier;
        state.score += totalPoints;

        // Heal slightly
        state.health = Math.min(MAX_HEALTH, state.health + (e.danger ? 0 : 1));

        // Effects
        const pColor = e.danger ? "#f87171" : "#34d399";
        spawnParticles(e.x, e.y, pColor, e.danger ? 16 : 10);
        if (e.danger) sfxDanger(); else sfxHit();

        // Floating text
        const label = multiplier > 1 ? `+${totalPoints} (x${multiplier})` : `+${totalPoints}`;
        spawnFloatingText(e.x, e.y - e.size, label, pColor);

        break;
      }
    }
  }

  // Pointer (mouse)
  canvas.addEventListener("pointerdown", (e) => {
    e.preventDefault();
    handleHit(e.clientX, e.clientY);
  });

  // Touch (mobile — prevent scroll)
  canvas.addEventListener("touchstart", (e) => {
    e.preventDefault();
    for (const touch of e.changedTouches) {
      handleHit(touch.clientX, touch.clientY);
    }
  }, { passive: false });

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
    renderHud();
  }

  startBtn.addEventListener("click", startGame);
  restartBtn.addEventListener("click", startGame);
  window.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      if (state.phase === "start" || state.phase === "over") startGame();
    }
  });

  /* ════════════════════════════════════════════════
     Init
     ════════════════════════════════════════════════ */
  highScoreEl.textContent = String(state.highScore);
  state.lastFrame = performance.now();
  requestAnimationFrame(tick);
})();
