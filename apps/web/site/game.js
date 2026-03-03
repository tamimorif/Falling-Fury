(() => {
  const STORAGE_KEY = "falling_fury_high_score";
  const GAME_WIDTH = 1000;
  const GAME_HEIGHT = 700;

  const canvas = document.getElementById("game-canvas");
  const ctx = canvas.getContext("2d");
  const healthEl = document.getElementById("health");
  const scoreEl = document.getElementById("score");
  const highScoreEl = document.getElementById("high-score");
  const gameOverEl = document.getElementById("game-over");
  const restartBtn = document.getElementById("restart-btn");

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

  function spawnEnemy() {
    const size = rand(28, 82);
    const danger = Math.random() < 0.18;
    const speedBoost = state.elapsed * 7.5;
    state.enemies.push({
      x: rand(size, GAME_WIDTH - size),
      y: -size - rand(20, 140),
      size,
      speed: rand(120, 250) + speedBoost,
      wobble: rand(0.4, 1.8),
      wobblePhase: rand(0, Math.PI * 2),
      danger,
    });
  }

  function update(dt) {
    if (!state.running) return;

    state.elapsed += dt;
    state.spawnTimer += dt;

    const spawnInterval = Math.max(0.16, 0.72 - state.elapsed * 0.015);
    while (state.spawnTimer >= spawnInterval) {
      spawnEnemy();
      state.spawnTimer -= spawnInterval;
    }

    for (let i = state.enemies.length - 1; i >= 0; i -= 1) {
      const enemy = state.enemies[i];
      enemy.y += enemy.speed * dt;
      enemy.x += Math.sin(state.elapsed * enemy.wobble + enemy.wobblePhase) * 60 * dt;
      enemy.x = Math.max(enemy.size, Math.min(GAME_WIDTH - enemy.size, enemy.x));

      if (enemy.y - enemy.size > GAME_HEIGHT) {
        state.enemies.splice(i, 1);
        state.health -= enemy.danger ? 2 : 1;
      }
    }

    if (state.health <= 0) {
      state.health = 0;
      state.running = false;
      saveHighScore();
      gameOverEl.classList.remove("hidden");
    }
  }

  function drawBackground() {
    const grad = ctx.createLinearGradient(0, 0, 0, GAME_HEIGHT);
    grad.addColorStop(0, "#fff9ec");
    grad.addColorStop(1, "#ffd79a");
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

    ctx.strokeStyle = "rgba(56, 34, 20, 0.08)";
    ctx.lineWidth = 1;
    for (let x = 0; x < GAME_WIDTH; x += 36) {
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x + 140, GAME_HEIGHT);
      ctx.stroke();
    }
  }

  function drawEnemies() {
    for (const enemy of state.enemies) {
      ctx.save();
      ctx.translate(enemy.x, enemy.y);
      ctx.rotate((enemy.y / GAME_HEIGHT) * 0.9);

      const inner = enemy.danger ? "#ff5e2f" : "#20ad54";
      const outer = enemy.danger ? "#8f1d12" : "#0d5530";
      ctx.fillStyle = inner;
      ctx.strokeStyle = outer;
      ctx.lineWidth = 3;

      ctx.beginPath();
      ctx.moveTo(0, -enemy.size);
      ctx.lineTo(enemy.size, 0);
      ctx.lineTo(0, enemy.size);
      ctx.lineTo(-enemy.size, 0);
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      ctx.fillStyle = "rgba(255, 255, 255, 0.25)";
      ctx.beginPath();
      ctx.ellipse(-enemy.size * 0.2, -enemy.size * 0.2, enemy.size * 0.36, enemy.size * 0.2, -0.4, 0, Math.PI * 2);
      ctx.fill();
      ctx.restore();
    }
  }

  function drawStatusLine() {
    ctx.fillStyle = "rgba(47, 33, 22, 0.92)";
    ctx.fillRect(0, GAME_HEIGHT - 14, GAME_WIDTH, 14);
  }

  function render() {
    drawBackground();
    drawEnemies();
    drawStatusLine();
    renderHud();
  }

  function tick(now) {
    const dt = Math.min(0.05, (now - state.lastFrame) / 1000);
    state.lastFrame = now;
    update(dt);
    render();
    requestAnimationFrame(tick);
  }

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

    for (let i = state.enemies.length - 1; i >= 0; i -= 1) {
      const enemy = state.enemies[i];
      const dx = point.x - enemy.x;
      const dy = point.y - enemy.y;
      const hitRadius = enemy.size * 0.9;
      if (dx * dx + dy * dy <= hitRadius * hitRadius) {
        state.enemies.splice(i, 1);
        state.score += enemy.danger ? 2 : 1;
        state.health = Math.min(20, state.health + 1);
        saveHighScore();
        break;
      }
    }
  }

  canvas.addEventListener("pointerdown", onPointerDown);
  restartBtn.addEventListener("click", resetGame);
  window.addEventListener("keydown", (event) => {
    if (event.key === "Enter" && !state.running) {
      resetGame();
    }
  });

  renderHud();
  requestAnimationFrame(tick);
})();
