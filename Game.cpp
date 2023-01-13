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
    this->health = 10;
    this->endGame = false;
    this->enemySpawnTimerMax = 10.f;
    this->enemySpawnTimer = this->enemySpawnTimerMax;
    this->maxEnemies = 10;
    this->mouseHeld = false;
}

void Game::initWindow()
{
    this->videoMode.height = 1000;
    this->videoMode.width = 1500;
    this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close);
    this->window->setFramerateLimit(60);
}

void Game::initEnemies()
{
    this->enemy.setPosition(10.f, 10.f);
    this->enemy.setSize(sf::Vector2f(100.f, 100.f));
    this->enemy.setScale(sf::Vector2f(0.5f, 0.5f));
    this->enemy.setFillColor(sf::Color::Cyan);
    // this->enemy.setOutlineColor(sf::Color::Green);
    // this->enemy.setOutlineThickness(1.f);
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

const bool Game::getEndGame() const
{
    return this->endGame;
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
    this->enemy.setPosition(static_cast<float>(rand() % static_cast<int>(this->window->getSize().x) - this->enemy.getSize().x), 0.f);

    this->enemy.setFillColor(sf::Color::Green);

    // Spawn the enemy
    this->enemies.push_back(this->enemy);
}
void Game::updateEnemies()
{

    /*
    @return void
    Updates the enemy swapn timer and spawn enemies
    When the total amount of enemies is smaller than the maximum.
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
    // Move the updating enemies
    for (int i = 0; i < this->enemies.size(); i++)
    {
        bool deleted = false;
        this->enemies[i].move(0.f, 2.f);

        if (this->enemies[i].getPosition().y > this->window->getSize().y)
        {
            this->enemies.erase(this->enemies.begin() + i);
            this->health--;
            std::cout << "Health " << this->health << '\n';
        }
    }
    // Check if clicked upon
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (!this->mouseHeld)
        {
            this->mouseHeld = true;
            bool deleted = false;
            for (size_t i = 0; i < this->enemies.size() && !deleted; i++)
            {
                if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
                {
                    // Deleted the enemy
                    deleted = true;
                    this->enemies.erase(this->enemies.begin() + i);

                    // Gain Points
                    // points++;
                    std::cout << "Health " << health << '\n';
                    health++;
                    // std::cout << "Points " << points << '\n';
                }
            }
        }
    }
    else
    {
        this->mouseHeld = false;
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
    this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::update()
{
    this->pollEvent();

    if (!this->endGame)
    {
        this->updateMousePositions();
        this->updateEnemies();
    }

    // End game condition
    if (!this->health)
    {
        this->endGame = true;
    }
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
