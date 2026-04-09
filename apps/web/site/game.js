(() => {
  "use strict";

  /* ── Constants ── */
  const STORAGE_KEY = "falling_fury_high_score";
  const GAME_WIDTH = 780;
  const GAME_HEIGHT = 460;
  const MIN_SPAWN_GAP = 90;       // minimum px between any two enemies
  const INITIAL_SPAWN_INTERVAL = 1.4; // seconds between spawns at start
  const MIN_SPAWN_INTERVAL = 0.45;
  const BASE_SPEED_MIN = 70;
  const BASE_SPEED_MAX = 140;
  const SPEED_RAMP = 3;           // extra px/s per elapsed second

  /* ── DOM Refs ── */
  const canvas       = document.getElementById("game-canvas");
  const ctx          = canvas.getContext("2d");
  const healthEl     = document.getElementById("health");
  const scoreEl      = document.getElementById("score");
  const highScoreEl  = document.getElementById("high-score");
  const gameOverEl   = document.getElementById("game-over");
  const finalScoreEl = document.getElementById("final-score-line");
  const restartBtn   = document.getElementById("restart-btn");

  /* ── State ── */
  const state = {
    score: 0,
    highScore: Number(localStorage.getItem(STORAGE_KEY) || 0),
    health: 10,
    enemies: [],
    running: true,
    spawnTimer: 0,
    elapsed: 0,
    lastFrame: performance.now(),
  };

  const rand = (min, max) => Math.random() * (max - min) + min;

  /* ── Helpers ── */
  function resetGame() {
    state.score = 0;
    state.health = 10;
    state.enemies = [];
    state.running = true;
    state.spawnTimer = 0;
    state.elapsed = 0;
    state.lastFrame = performance.now();
    gameOverEl.classList.add("hidden");
    renderHud();
  }

  function renderHud() {
    healthEl.textContent = String(state.health);
    scoreEl.textContent = String(state.score);
    highScoreEl.textContent = String(state.highScore);
  }

  function saveHighScore() {
    if (state.score > state.highScore) {
      state.highScore = state.score;
      localStorage.setItem(STORAGE_KEY, String(state.highScore));
    }
  }

  /* ── Collision check — no overlapping spawns ── */
  function tooCloseToExisting(x, y, size) {
    for (const e of state.enemies) {
      const dx = x - e.x;
      const dy = y - e.y;
      const minDist = size + e.size + MIN_SPAWN_GAP;
      if (dx * dx + dy * dy < minDist * minDist) return true;
    }
    return false;
  }

  function spawnEnemy() {
    const size = rand(16, 32);
    const danger = Math.random() < 0.15;
    const speedBoost = state.elapsed * SPEED_RAMP;

    // Try up to 8 positions to avoid overlap, then skip this spawn
    for (let attempt = 0; attempt < 8; attempt++) {
      const x = rand(size + 10, GAME_WIDTH - size - 10);
      const y = -size - rand(10, 60);
      if (!tooCloseToExisting(x, y, size)) {
        state.enemies.push({
          x, y, size,
          speed: rand(BASE_SPEED_MIN, BASE_SPEED_MAX) + speedBoost,
          wobble: rand(0.3, 1.2),
          wobblePhase: rand(0, Math.PI * 2),
          danger,
          opacity: 1,
        });
        return;
      }
    }
  }

  /* ── Update ── */
  function update(dt) {
    if (!state.running) return;

    state.elapsed += dt;
    state.spawnTimer += dt;

    const interval = Math.max(MIN_SPAWN_INTERVAL, INITIAL_SPAWN_INTERVAL - state.elapsed * 0.012);
    if (state.spawnTimer >= interval) {
      spawnEnemy();
      state.spawnTimer = 0;
    }

    for (let i = state.enemies.length - 1; i >= 0; i--) {
      const e = state.enemies[i];
      e.y += e.speed * dt;
      e.x += Math.sin(state.elapsed * e.wobble + e.wobblePhase) * 30 * dt;
      e.x = Math.max(e.size, Math.min(GAME_WIDTH - e.size, e.x));

      if (e.y - e.size > GAME_HEIGHT) {
        state.enemies.splice(i, 1);
        state.health -= e.danger ? 2 : 1;
      }
    }

    if (state.health <= 0) {
      state.health = 0;
      state.running = false;
      saveHighScore();
      finalScoreEl.textContent = `Your score: ${state.score}`;
      gameOverEl.classList.remove("hidden");
    }
  }

  /* ── Render ── */
  function drawBackground() {
    const grad = ctx.createLinearGradient(0, 0, 0, GAME_HEIGHT);
    grad.addColorStop(0, "#0f0c29");
    grad.addColorStop(0.5, "#1a1440");
    grad.addColorStop(1, "#24243e");
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

    // Subtle grid lines
    ctx.strokeStyle = "rgba(167, 139, 250, 0.04)";
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
      ctx.save();
      ctx.translate(e.x, e.y);

      const rot = (e.y / GAME_HEIGHT) * 0.7;
      ctx.rotate(rot);

      // Glow
      const glowColor = e.danger ? "rgba(248, 113, 113, 0.3)" : "rgba(52, 211, 153, 0.25)";
      ctx.shadowColor = glowColor;
      ctx.shadowBlur = 18;

      // Body
      const fill = e.danger
        ? "rgba(248, 113, 113, 0.92)"
        : "rgba(52, 211, 153, 0.88)";
      const stroke = e.danger ? "#991b1b" : "#065f46";

      ctx.fillStyle = fill;
      ctx.strokeStyle = stroke;
      ctx.lineWidth = 2;

      // Diamond shape
      ctx.beginPath();
      ctx.moveTo(0, -e.size);
      ctx.lineTo(e.size, 0);
      ctx.lineTo(0, e.size);
      ctx.lineTo(-e.size, 0);
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      // Inner highlight
      ctx.shadowBlur = 0;
      ctx.fillStyle = "rgba(255, 255, 255, 0.15)";
      ctx.beginPath();
      ctx.ellipse(-e.size * 0.15, -e.size * 0.18, e.size * 0.3, e.size * 0.16, -0.3, 0, Math.PI * 2);
      ctx.fill();

      ctx.restore();
    }
  }

  function drawFloorLine() {
    const grad = ctx.createLinearGradient(0, GAME_HEIGHT - 4, 0, GAME_HEIGHT);
    grad.addColorStop(0, "rgba(167, 139, 250, 0.5)");
    grad.addColorStop(1, "rgba(167, 139, 250, 0)");
    ctx.fillStyle = grad;
    ctx.fillRect(0, GAME_HEIGHT - 4, GAME_WIDTH, 4);
  }

  function render() {
    drawBackground();
    drawEnemies();
    drawFloorLine();
    renderHud();
  }

  /* ── Loop ── */
  function tick(now) {
    const dt = Math.min(0.05, (now - state.lastFrame) / 1000);
    state.lastFrame = now;
    update(dt);
    render();
    requestAnimationFrame(tick);
  }

  /* ── Input ── */
  function toCanvasCoords(event) {
    const rect = canvas.getBoundingClientRect();
    const scaleX = GAME_WIDTH / rect.width;
    const scaleY = GAME_HEIGHT / rect.height;
    return {
      x: (event.clientX - rect.left) * scaleX,
      y: (event.clientY - rect.top) * scaleY,
    };
  }

  function onPointerDown(event) {
    if (!state.running) return;
    const point = toCanvasCoords(event);

    for (let i = state.enemies.length - 1; i >= 0; i--) {
      const e = state.enemies[i];
      const dx = point.x - e.x;
      const dy = point.y - e.y;
      const hitRadius = e.size * 1.1;
      if (dx * dx + dy * dy <= hitRadius * hitRadius) {
        state.enemies.splice(i, 1);
        state.score += e.danger ? 3 : 1;
        state.health = Math.min(15, state.health + 1);
        saveHighScore();
        break;
      }
    }
  }

  canvas.addEventListener("pointerdown", onPointerDown);
  restartBtn.addEventListener("click", resetGame);
  window.addEventListener("keydown", (e) => {
    if (e.key === "Enter" && !state.running) resetGame();
  });

  renderHud();
  requestAnimationFrame(tick);
})();
