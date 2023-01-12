#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <ctime>

class Game
{
private:
    // Ceriables
    // Window
    sf::RenderWindow *window;
    sf::Event ev;
    sf::VideoMode videoMode;

    // Mouse Position
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;

    // Game Logic
    int points;
    float enemySpawnTimer;
    float enemySpawnTimerMax;
    int maxEnemies;

    // Game object
    std::vector<sf::RectangleShape> enemies;
    sf::RectangleShape enemy;

    // Privet Functions
    void initVeriables();
    void initWindow();
    void initEnemies();

public:
    // Constructors
    Game();
    virtual ~Game();

    // accessors
    const bool running() const;

    // functions
    void spawnEnemy();

    void updateEnemies();
    void renderEnemies();
    void update();
    void render();
    void pollEvent();
    void updateMousePositions();
};