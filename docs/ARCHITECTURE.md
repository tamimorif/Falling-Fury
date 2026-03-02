# 🎮 Falling Fury - Professional Game Architecture

> Note (March 2, 2026): folder paths in this document are partially legacy. For current layout, use `apps/native` and `apps/web/site`.

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-2.5+-green.svg)](https://www.sfml-dev.org/)
[![CMake](https://img.shields.io/badge/CMake-3.25+-red.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A professionally architected falling objects game showcasing modern C++ design patterns, clean code architecture, and advanced game development techniques.

---

## 🌟 **Key Features**

### **Gameplay**
- 🎯 **Multiple Enemy Types**: Normal, Fast, Tank, and Bonus enemies with unique behaviors
- 💥 **Particle Effects**: Stunning visual feedback for clicks, combos, and misses
- 🎵 **Sound System**: Comprehensive audio management with sound effects and music
- 🔥 **Combo System**: Score multipliers for consecutive hits
- ❤️ **Health System**: Dynamic difficulty with health management
- 📊 **Leaderboard**: Persistent high score tracking

### **Architecture Highlights**
- 🏗️ **Design Patterns**: Singleton, Factory, Object Pool, State Machine
- 🧩 **Separation of Concerns**: Modular, maintainable codebase
- 💎 **Modern C++17**: Smart pointers, RAII, STL algorithms
- ⚡ **Performance Optimized**: Object pooling, efficient memory management
- 🎨 **Professional UI**: Reusable UI components (Buttons, Panels, Sliders)
- 🔄 **Frame-Independent**: DeltaTime-based movement

---

## 📁 **Project Structure**

```
Falling-Fury/
├── CMakeLists.txt           # Modern CMake configuration
├── .gitignore               # Comprehensive ignore rules
├── README.md                # This file
│
├── Core Game Files
│   ├── main.cpp             # Entry point
│   ├── Game.h / Game.cpp    # Main game class
│   └── GameState.h          # State machine base classes
│
├── Game States
│   ├── PlayingState.h       # Main gameplay state
│   ├── MenuState.h          # Menu state (to be implemented)
│   └── GameOverState.h      # Game over state (to be implemented)
│
├── Game Systems
│   ├── ResourceManager.h    # Singleton resource management
│   ├── ScoreManager.h       # Score tracking & persistence
│   ├── SoundManager.h       # Audio management
│   └── ParticleSystem.h     # Particle effects engine
│
├── Game Objects
│   ├── Enemy.h              # Enemy class hierarchy & factory
│   └── ObjectPool.h         # Generic object pooling
│
├── UI System
│   └── UI.h                 # Button, Panel, Label, Slider components
│
├── Assets
│   ├── Fonts/               # Game fonts
│   └── database/            # Persistent data storage
│
└── Build
    └── cmake-build-debug/   # Build artifacts (gitignored)
```

---

## 🎨 **Architecture Overview**

### **Design Patterns Implemented**

#### **1. Singleton Pattern**
```cpp
ResourceManager::getInstance()  // Centralized resource loading
ScoreManager::getInstance()     // Score management
SoundManager::getInstance()     // Audio control
```

#### **2. Factory Pattern**
```cpp
EnemyFactory::createEnemy(EnemyType::FAST, position)
EnemyFactory::createRandomEnemy(position)
```

#### **3. Object Pool Pattern**
```cpp
ObjectPool<Enemy> enemyPool(50, factory, resetFunction);
Enemy* enemy = enemyPool.acquire();
enemyPool.release(enemy);
```

#### **4. State Machine Pattern**
```cpp
StateManager stateManager;
stateManager.changeState(std::make_unique<PlayingState>());
```

#### **5. RAII (Resource Acquisition Is Initialization)**
```cpp
std::unique_ptr<sf::RenderWindow> mWindow;  // Auto cleanup
PooledObject<Enemy> autoRelease(enemy, &pool);  // Auto release
```

---

## 🚀 **Getting Started**

### **Prerequisites**
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.25** or higher
- **SFML 2.5+** (graphics, window, system, audio)

### **macOS Installation**
```bash
# Install dependencies
brew install cmake sfml

# Clone repository
git clone https://github.com/tamimorif/Falling-Fury.git
cd Falling-Fury

# Build project
mkdir build && cd build
cmake ..
make

# Run game
./bin/FallingFury
```

### **Linux Installation**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake libsfml-dev

# Build and run
mkdir build && cd build
cmake ..
make
./bin/FallingFury
```

### **Windows Installation**
```bash
# Install SFML and CMake
# Set SFML_DIR environment variable

# Using CMake GUI or command line
mkdir build
cd build
cmake ..
cmake --build .
.\bin\FallingFury.exe
```

---

## 🎯 **Class Descriptions**

### **Core Classes**

#### **Game** (Main Game Manager)
- Window management with smart pointers
- Game loop with deltaTime
- Resource initialization
- State coordination

#### **GameState** (State Machine)
- Abstract base class for all game states
- Handles update, render, and input
- State transitions (Menu → Playing → GameOver)

### **Manager Classes**

#### **ResourceManager** (Singleton)
- Centralized loading of fonts, textures, sounds
- Prevents resource duplication
- Exception-safe resource access

#### **ScoreManager** (Singleton)
- Current score tracking
- High score persistence
- Combo multiplier system
- Leaderboard management

#### **SoundManager** (Singleton)
- Sound effect playback
- Background music control
- Volume management
- Enable/disable audio

### **Game Object Classes**

#### **Enemy** (Base Class + Hierarchy)
- **NormalEnemy**: Standard gameplay
- **FastEnemy**: Quick movement with horizontal wiggle
- **TankEnemy**: Slow but costs more health
- **BonusEnemy**: High points, no health penalty, limited lifetime

#### **ObjectPool\<T\>** (Generic Template)
- Pre-allocated object reuse
- Eliminates allocation overhead
- Automatic growth support
- RAII-friendly PooledObject wrapper

### **Visual Effects**

#### **ParticleSystem**
- Burst emissions for clicks
- Trail effects for misses
- Combo sparkles
- Color interpolation and fading

#### **ScreenShake**
- Intensity-based camera shake
- Decay over time
- Game feel enhancement

### **UI System**

#### **UIElement** (Base Class)
- Position and size management
- Visibility and enabled states
- Update and render interface

#### **Button**
- Hover and press states
- Color transitions
- Callback system
- Centered text rendering

#### **Panel**
- Container for UI elements
- Background with border
- Child management

#### **Label**
- Text rendering
- Color and size control
- Center positioning

#### **Slider**
- Value adjustment (min/max range)
- Draggable handle
- Real-time callback
- Volume or setting control

---

## 💡 **Advanced Features**

### **Frame-Independent Movement**
```cpp
// Movement scales with deltaTime
enemy.move(0.f, speed * deltaTime);

// Ensures consistent behavior across different frame rates
```

### **Combo System**
```cpp
// Consecutive hits increase multiplier
// 3+ hits: x1.5, x2.0, x2.5, etc.
scoreManager.addPoints(basePoints);  // Automatically applies multiplier
```

### **Smart Pointer Usage**
```cpp
// No manual memory management
std::unique_ptr<sf::RenderWindow> mWindow;
std::unique_ptr<GameState> mCurrentState;
std::unique_ptr<Enemy> enemy = EnemyFactory::create();
```

### **Exception-Safe Resource Loading**
```cpp
try {
    ResourceManager::getInstance().loadFont("main", "path/to/font.ttf");
} catch (const std::exception& e) {
    std::cerr << "Failed to load resource: " << e.what() << "\n";
}
```

---

## 🎮 **Controls**

| Key | Action |
|-----|--------|
| **Left Click** | Click enemies to score points |
| **ESC** | Pause game / Return to menu |
| **Q** | Quit game |
| **Space** | Resume (when paused) |

---

## 📊 **Performance Considerations**

- **Object Pooling**: Reuses enemy objects, reducing allocation overhead
- **Smart Pointers**: Automatic memory management, no leaks
- **DeltaTime**: Consistent performance across different hardware
- **Spatial Optimization**: Efficient collision detection
- **Resource Caching**: Single load for multiple uses

---

## 🔧 **Configuration**

### **CMake Build Options**
```bash
# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Custom SFML path
cmake -DSFML_DIR=/path/to/sfml ..
```

### **Game Settings** (Future: config.ini)
```ini
[Audio]
SoundVolume=70
MusicVolume=50
SoundEnabled=true
MusicEnabled=true

[Gameplay]
Difficulty=Medium
StartingHealth=10

[Graphics]
WindowWidth=1000
WindowHeight=1000
VSync=true
```

---

## 🚧 **Future Enhancements**

- [ ] Complete menu system implementation
- [ ] Game over state with stats
- [ ] Pause state with settings
- [ ] Config file (JSON/INI) support
- [ ] Sprite textures instead of colored shapes
- [ ] Power-up system (shield, slow-mo, multi-click)
- [ ] Achievement system
- [ ] Online leaderboard
- [ ] Level progression
- [ ] Boss enemies
- [ ] Touch controls for mobile

---

## 🧪 **Testing**

### **Unit Tests** (Recommended)
```cpp
// Example test structure
TEST(EnemyFactory, CreatesCorrectTypes) {
    auto enemy = EnemyFactory::createEnemy(EnemyType::FAST, {0,0});
    ASSERT_EQ(enemy->getType(), EnemyType::FAST);
}
```

### **Manual Testing Checklist**
- ✅ Enemies spawn and fall correctly
- ✅ Click detection works accurately
- ✅ Score increases on successful clicks
- ✅ Health decreases on misses
- ✅ Combo system activates after 3+ hits
- ✅ Particles appear on clicks
- ✅ High score persists between sessions

---

## 📝 **Code Style Guidelines**

### **Naming Conventions**
```cpp
// Member variables: m prefix
int mScore;
float mDeltaTime;

// Static variables: s prefix
static ResourceManager* sInstance;

// Constants: UPPER_SNAKE_CASE
const int MAX_ENEMIES = 30;

// Functions: camelCase
void updateEnemies();

// Classes: PascalCase
class EnemyFactory;
```

### **Documentation**
```cpp
/**
 * @brief Brief description
 * @param paramName Parameter description
 * @return Return value description
 */
void functionName(int paramName);
```

---

## 🤝 **Contributing**

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Follow the code style guidelines
4. Add appropriate documentation
5. Test your changes
6. Commit with descriptive messages
7. Push to the branch
8. Open a Pull Request

---

## 📜 **License**

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 **Author**

**Tamimorif**
- GitHub: [@tamimorif](https://github.com/tamimorif)
- Project: [Falling-Fury](https://github.com/tamimorif/Falling-Fury)

---

## 🙏 **Acknowledgments**

- **SFML Team** for the excellent multimedia library
- **C++ Community** for modern C++ best practices
- **Game Programming Patterns** by Robert Nystrom for design pattern inspiration

---

## 📚 **Resources & References**

- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
- [CMake Documentation](https://cmake.org/documentation/)

---

**⭐ If you found this project helpful, please consider giving it a star!**
