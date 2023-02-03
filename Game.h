#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

class Game
{
    // Local variable: variableName;
    // Global variable: mVariableName;
    // CONSTANT variable: MAX_ENEMIES
    static const int WINDOW_HEIGH = 1000;
    static const int WINDOW_WIDTH = 1000;

private:
    // Ceriables
    // Window
    sf::VideoMode mVideoMode;
    sf::RenderWindow *mWindow;
    sf::Event mEvent;

    // Mouse Position
    sf::Vector2i mMousePosWindow;
    sf::Vector2f mMousePosView;

    // Resources
    sf::Font mFont;

    // Border for text
    sf::Sprite mSprite;

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
    float mGravity = 4.f;
    unsigned mDistance = 0;

    // Comparing Veriables
    // unsigned

    bool mRed2 = 1, mGreen2 = 1, mBlue2 = 1;
    int mRed = 0, mGreen = 0, mBlue = 0;
    int mSpeed = 50;

    // Game object
    std::vector<sf::RectangleShape> mEnemies;
    sf::RectangleShape mEnemy;

    // Privet Functions
    void initWindow();
    void initFonts();
    void initMaxPoint();
    void initText();
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

    void pollEvent();
    void update();
    void updateEnemies();
    void saveData(int lineNumber);
    std::string getData(int lineNumber);
    void updateMousePositions();
    void updateText();

    void renderMaxPoint();
    void renderCounter();
    void renderText();
    void renderEnemies();
    void render();
};