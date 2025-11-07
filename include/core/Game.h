#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "managers/ResourceManager.h"

class Game {
    // Local variable: variableName;
    // Global variable: mVariableName;
    // CONSTANT variable: MAX_ENEMIES
    static const int WINDOW_HEIGH = 1000;
    static const int WINDOW_WIDTH = 1000;

   private:
    // Variables
    // Window
    sf::VideoMode mVideoMode;
    std::unique_ptr<sf::RenderWindow> mWindow;
    sf::Event mEvent;

    // Mouse Position
    sf::Vector2i mMousePosWindow;
    sf::Vector2f mMousePosView;

    // Text
    sf::Text mUiText;
    sf::Text mMaxpointText;

    // Game Logic
    unsigned mMaxPoint;
    unsigned mPoints;
    int mHealth;

    float mEnemySpawnTimer;
    float mEnemySpawnTimerMax;
    const int MAX_ENEMIES = 30;
    bool mMouseHeld;
    bool mEndGame;
    float mGravity = 200.f;  // Pixels per second
    unsigned mDistance = 0;

    // Delta Time
    sf::Clock mDeltaClock;
    float mDeltaTime;

    bool mRed2 = 1, mGreen2 = 1, mBlue2 = 1;
    int mRed = 0, mGreen = 0, mBlue = 0;
    int mSpeed = 50;

    // Game object
    std::vector<sf::RectangleShape> mEnemies;
    sf::RectangleShape mEnemy;

    // Privet Functions
    void initWindow();
    void initText();
    void initMaxPoint();
    void initEnemies();

   public:
    // Constructors
    Game();
    virtual ~Game();

    // accessors
    const bool running() const;
    const bool getEndGame() const;

    // functions
    void spawnEnemy();

    void nextColor();
    void updateDeltaTime();

    void pollEvent();
    void update();
    void updateEnemies();
    void updateMousePositions();
    void updateText();
    std::string saveData();
    std::string getData();
    // std::string resetData();
    void renderMaxPoint();
    void renderText();
    void renderEnemies();
    void render();
};