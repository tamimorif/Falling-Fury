#include "Game.h"
void Game::initVeriables()
{
    window = nullptr;

    // Game logic
    int points;
    float enemySpawnTimer;
    float enemySpawnTimerMax;
    int maxEnemies;
    this->points = 0;
    this->enemySpawnTimerMax = 1000.f;
    this->enemySpawnTimer = this->enemySpawnTimerMax;
    this->maxEnemies = 5;
}

void Game::initWindow()
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;
    this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close);
    this->window->setFramerateLimit(60);
}

void Game::initEnemies()
{
    this->enemy.setPosition(10.f, 10.f);
    this->enemy.setSize(sf::Vector2f(100.f, 100.f));
    this->enemy.setScale(sf::Vector2f(0.5f, 0.5f));
    this->enemy.setFillColor(sf::Color::Cyan);
    this->enemy.setOutlineColor(sf::Color::Green);
    this->enemy.setOutlineThickness(1.f);
}

// Constructor
Game::Game()
{
    this->initVeriables();
    this->initWindow();
    this->initEnemies();
}

// Destructor
Game::~Game()
{
    delete window;
}

const bool Game::running() const
{
    return window->isOpen();
}

// Functions

void Game::spawnEnemy()
{
    /*
    @return void
    Spawns enemies and sets their color and positions.
    -Sets a random position.
    -Sets random color.
    -Adds enemy to the vector.
    */
    this->enemy.setPosition(
        static_cast<float>(rand() % static_cast<int>(this->window->getSize().x) - this->enemy.getSize().x),
        static_cast<float>(rand() % static_cast<int>(this->window->getSize().y) - this->enemy.getSize().y));

    this->enemy.setFillColor(sf::Color::Green);

    // Spawn the enemy
    this->enemies.push_back(this->enemy);
}
void Game::updateEnemies()
{

    /*
    @return void
    Updates the enemy swapn timer and spawn enemies
    when the total amount of enemies is smaller than the maximum.
    Moves the enemies downwords.
    Removes the enemies at the edge of the screen. // TODO
    */

    // Updating the timer for enemy swapwning
    if (this->enemies.size() < this->maxEnemies)
    {
        if (this->enemySpawnTimer >= this->enemySpawnTimerMax)
        {
            // Spawn the enemy and reset the timer
            this->spawnEnemy();
            this->enemySpawnTimer = 0.f;
        }
        else
            this->enemySpawnTimer += 1.f;
    }
    // Move the enemies
    for (auto &i : this->enemies)
    {
        i.move(0.f, 5.f);
    }
}
void Game::renderEnemies()
{
    // Rendering all the enemies
    for (auto &i : this->enemies)
    {
        this->window->draw(i);
    }
}

void Game::pollEvent()
{
    // Event Poling
    while (window->pollEvent(ev))
    {
        if (ev.type == sf::Event::Closed)
            window->close();
        else if (ev.type == sf::Event::KeyPressed)
        {
            if (ev.key.code == sf::Keyboard::Escape)
                window->close();
        }
    }
}
void Game::updateMousePositions()
{
    /*
    @return void
    Update Mouse Positions:
    -Mouse Position relative to window (Vector2i);
    */
    this->mousePosWindow = sf::Mouse::getPosition(*this->window);
}

void Game::update()
{
    this->pollEvent();

    this->updateMousePositions();

    this->updateEnemies();

    //  Update mouse position

    // Relative to Screen
    // std::cout << "Mouse pos: " << sf::Mouse::getPosition().x << ' ' << sf::Mouse::getPosition().y << '\n';

    // Relative to the Window
    // std::cout << "Mouse pos: " << sf::Mouse::getPosition(*this->window).x << ' ' << sf::Mouse::getPosition(*this->window).y << '\n';
}
void Game::render()
{
    window->clear();

    // draw game object
    this->renderEnemies();

    window->display();
}
