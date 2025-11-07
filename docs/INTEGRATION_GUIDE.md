# 🚀 Falling Fury - Quick Integration Guide

This guide shows you how to integrate all the new systems into your existing game.

---

## 📝 **Integration Checklist**

### **Step 1: Update Game.h includes**

```cpp
#include "ResourceManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include "ParticleSystem.h"
#include "Enemy.h"
#include "ObjectPool.h"
#include "GameState.h"
#include "UI.h"
```

### **Step 2: Update Game class to use new systems**

```cpp
class Game {
private:
    std::unique_ptr<sf::RenderWindow> mWindow;  // Use smart pointer
    StateManager mStateManager;
    ParticleSystem mParticles;
    ScreenShake mScreenShake;
    ObjectPool<Enemy> mEnemyPool;
    float mDeltaTime;
    sf::Clock mDeltaClock;
    
public:
    Game();
    void update();
    void render();
    void handleInput();
};
```

### **Step 3: Initialize all systems in constructor**

```cpp
Game::Game() 
    : mWindow(std::make_unique<sf::RenderWindow>(...)),
      mParticles(200),  // 200 particle pool
      mEnemyPool(50, []() { 
          return EnemyFactory::createRandomEnemy({0, 0}); 
      })
{
    // Load resources
    ResourceManager::getInstance().loadFont("main", "../Fonts/1/BebasNeue-Regular.ttf");
    
    // Initialize score manager
    ScoreManager::getInstance().resetScore();
    
    // Initialize sound system (when you have sound files)
    // SoundManager::getInstance().loadMusic("bgm", "path/to/music.ogg");
    
    // Set initial state
    mStateManager.changeState(std::make_unique<PlayingState>(this));
}
```

### **Step 4: Update game loop**

```cpp
void Game::update() {
    // Update delta time
    mDeltaTime = mDeltaClock.restart().asSeconds();
    
    // Update current state
    mStateManager.update(mDeltaTime);
    
    // Update particles
    mParticles.update(mDeltaTime);
    
    // Update screen shake
    mScreenShake.update(mDeltaTime);
}

void Game::render() {
    mWindow->clear();
    
    // Apply screen shake if active
    if (mScreenShake.isActive()) {
        sf::View view = mWindow->getDefaultView();
        view.move(mScreenShake.getOffset());
        mWindow->setView(view);
    }
    
    // Render current state
    mStateManager.render(*mWindow);
    
    // Render particles on top
    mParticles.render(*mWindow);
    
    // Reset view
    mWindow->setView(mWindow->getDefaultView());
    
    mWindow->display();
}
```

---

## 🎯 **Using Individual Systems**

### **Enemy System**

```cpp
// Create specific enemy type
auto enemy = EnemyFactory::createEnemy(EnemyType::FAST, sf::Vector2f(100, 0));

// Create random enemy
auto randomEnemy = EnemyFactory::createRandomEnemy(sf::Vector2f(200, 0));

// Update enemy
enemy->update(deltaTime);

// Render enemy
enemy->render(window);

// Check if clicked
if (enemy->isClicked(mousePosition)) {
    int points = enemy->getPointValue();
    ScoreManager::getInstance().addPoints(points);
}

// Check if off screen
if (enemy->isOffScreen(window.getSize().y)) {
    int healthDamage = enemy->getHealthValue();
    // Reduce health...
}
```

### **Object Pool**

```cpp
// Create pool
auto factory = []() { 
    return EnemyFactory::createRandomEnemy({0, 0}); 
};

auto reset = [](Enemy* e) { 
    e->setActive(false); 
    e->setPosition({0, 0}); 
};

ObjectPool<Enemy> pool(50, factory, reset);

// Acquire object
Enemy* enemy = pool.acquire();
if (enemy) {
    enemy->setPosition({rand() % 900, 0});
    enemy->setActive(true);
}

// Release when done
pool.release(enemy);

// Or use RAII wrapper
{
    PooledObject<Enemy> autoEnemy(pool.acquire(), &pool);
    autoEnemy->update(deltaTime);
    // Automatically released when scope exits
}
```

### **Particle System**

```cpp
// Initialize
ParticleSystem particles(200);

// On enemy click
particles.emitClickEffect(clickPosition, sf::Color::Green);

// On enemy miss
particles.emitMissEffect(enemyPosition);

// On combo milestone
if (comboCount % 5 == 0) {
    particles.emitComboEffect(position);
}

// Update and render
particles.update(deltaTime);
particles.render(window);
```

### **Score Manager**

```cpp
// Reset at game start
ScoreManager::getInstance().resetScore();

// Add points (with combo multiplier)
ScoreManager::getInstance().addPoints(enemy->getPointValue());

// Break combo on miss
ScoreManager::getInstance().breakCombo();

// Get current values
unsigned score = ScoreManager::getInstance().getCurrentScore();
unsigned highScore = ScoreManager::getInstance().getHighScore();
std::string comboText = ScoreManager::getInstance().getComboString();

// Save high score
ScoreManager::getInstance().saveHighScore();

// Add to leaderboard
if (ScoreManager::getInstance().qualifiesForLeaderboard()) {
    ScoreManager::getInstance().addToLeaderboard("Player", score, "2025-11-07");
}
```

### **Sound Manager**

```cpp
// Load sounds (do this once at startup)
ResourceManager::getInstance().loadSound("click", "../Sounds/click.wav");
SoundManager::getInstance().registerSound("click", 
    ResourceManager::getInstance().getSound("click"));

// Load music
SoundManager::getInstance().loadMusic("bgm", "../Music/background.ogg");

// Play sound effect
SoundManager::getInstance().playSound("click");

// Play music
SoundManager::getInstance().playMusic("bgm");

// Control volume
SoundManager::getInstance().setSoundVolume(70.f);  // 0-100
SoundManager::getInstance().setMusicVolume(50.f);

// Mute/unmute
SoundManager::getInstance().setSoundEnabled(false);
SoundManager::getInstance().setMusicEnabled(false);
```

### **UI System**

```cpp
// Create button
auto playButton = std::make_shared<Button>(
    "PLAY",
    sf::Vector2f(400, 400),
    sf::Vector2f(200, 60),
    []() { 
        std::cout << "Play clicked!\n"; 
        // Start game...
    }
);

// Create panel
auto menuPanel = std::make_shared<Panel>(
    sf::Vector2f(300, 200),
    sf::Vector2f(400, 600),
    sf::Color(30, 30, 30, 230)
);

// Add button to panel
menuPanel->addChild(playButton);

// Create label
auto titleLabel = std::make_shared<Label>(
    "FALLING FURY",
    sf::Vector2f(500, 100),
    48,
    sf::Color::Yellow
);
titleLabel->centerOnPosition();

// Create slider for volume
auto volumeSlider = std::make_shared<Slider>(
    "Volume",
    sf::Vector2f(350, 500),
    300.f,
    0.f, 100.f,
    70.f
);

volumeSlider->setCallback([](float value) {
    SoundManager::getInstance().setSoundVolume(value);
});

// Update UI (call each frame)
sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

playButton->update(mousePos, mousePressed);
menuPanel->update(mousePos, mousePressed);
volumeSlider->update(mousePos, mousePressed);

// Render UI
menuPanel->render(window);
titleLabel->render(window);
```

### **Screen Shake**

```cpp
// Initialize
ScreenShake shake;

// Trigger shake on events
shake.start(0.3f, 10.f);  // 0.3 seconds, intensity 10

// Update each frame
shake.update(deltaTime);

// Apply to view
if (shake.isActive()) {
    sf::View view = window.getDefaultView();
    view.move(shake.getOffset());
    window.setView(view);
}
```

---

## 🎮 **Complete Example: Modernized Game Loop**

```cpp
class ModernGame {
private:
    std::unique_ptr<sf::RenderWindow> mWindow;
    StateManager mStateManager;
    ParticleSystem mParticles;
    ScreenShake mScreenShake;
    sf::Clock mDeltaClock;
    float mDeltaTime;
    
public:
    ModernGame() 
        : mWindow(std::make_unique<sf::RenderWindow>(
              sf::VideoMode(1000, 1000), "Falling Fury")),
          mParticles(200)
    {
        mWindow->setFramerateLimit(60);
        
        // Initialize all managers
        ResourceManager::getInstance().loadFont("main", "../Fonts/1/BebasNeue-Regular.ttf");
        ScoreManager::getInstance().resetScore();
        
        // Start with playing state
        mStateManager.changeState(std::make_unique<PlayingState>(this));
    }
    
    void run() {
        while (mWindow->isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    
    void handleEvents() {
        sf::Event event;
        while (mWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mWindow->close();
            }
            mStateManager.handleInput(event);
        }
    }
    
    void update() {
        mDeltaTime = mDeltaClock.restart().asSeconds();
        mStateManager.update(mDeltaTime);
        mParticles.update(mDeltaTime);
        mScreenShake.update(mDeltaTime);
    }
    
    void render() {
        mWindow->clear(sf::Color::Black);
        
        // Apply screen shake
        sf::View view = mWindow->getDefaultView();
        if (mScreenShake.isActive()) {
            view.move(mScreenShake.getOffset());
        }
        mWindow->setView(view);
        
        // Render everything
        mStateManager.render(*mWindow);
        mParticles.render(*mWindow);
        
        mWindow->setView(mWindow->getDefaultView());
        mWindow->display();
    }
    
    ~ModernGame() {
        ResourceManager::destroy();
        ScoreManager::destroy();
        SoundManager::destroy();
    }
};
```

---

## ✅ **Verification**

After integration, verify:
- [ ] Game compiles without errors
- [ ] Smart pointers work (no manual delete needed)
- [ ] DeltaTime makes movement consistent
- [ ] Particles appear on clicks
- [ ] Score/combo system works
- [ ] High score persists between runs
- [ ] Different enemy types spawn
- [ ] UI buttons respond to clicks

---

## 🐛 **Common Issues**

### Issue: Font not found
```cpp
// Make sure path is correct relative to executable
ResourceManager::getInstance().loadFont("main", "../Fonts/1/BebasNeue-Regular.ttf");
```

### Issue: Enemies not showing
```cpp
// Make sure enemy is active and positioned correctly
enemy->setActive(true);
enemy->setPosition(sf::Vector2f(x, 0));
```

### Issue: Clicks not registering
```cpp
// Use mapPixelToCoords for correct coordinates
sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
```

---

**🎉 You're all set! Your game now has professional-grade architecture!**
