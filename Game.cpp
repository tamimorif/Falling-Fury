#include "Game.h"
void Game::initEnemies()
{
    mEnemy.setPosition(10.f, 10.f);
    mEnemy.setSize(sf::Vector2f(100.f, 100.f));
    mEnemy.setScale(sf::Vector2f(0.5f, 0.5f));
    mEnemy.setFillColor(sf::Color::Cyan);
    // mEnemy.setOutlineColor(sf::Color::Green);
    // mEnemy.setOutlineThickness(1.f);
}
void Game::initFonts()
{
    if (!mFont.loadFromFile("../Fonts/1/BebasNeue-Regular.ttf"))
    {
        std::cout << "ERROR::GAME::INITFONTS::Failed to load fonts!\n";
        throw std::runtime_error("Cannot load font");
    }
}

void Game::initText()
{
    mUiText.setFont(mFont);
    mUiText.setCharacterSize(50);
    mUiText.setColor(sf::Color::Red);
    mUiText.setPosition(300.f, 30.f);
    mUiText.setString("NONE");
}

// Constructor
Game::Game() : mVideoMode(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGH)),
               mWindow(new sf::RenderWindow(mVideoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close)),
               mPoints(0),
               mHealth(10),
               mEndGame(false),
               mEnemySpawnTimerMax(10.f),
               mEnemySpawnTimer(10.f),
               mMouseHeld(false)
{
    mWindow->setFramerateLimit(60);
    initFonts();
    initText();
    initEnemies();
}

// Destructor
Game::~Game()
{
    delete mWindow;
}

const bool Game::running() const
{
    return mWindow->isOpen();
}

const bool Game::getEndGame() const
{
    return mEndGame;
}

// Functions

void Game::spawnEnemy()
{
    /*
    @return void
    Spawns mEnemies and sets their color and positions.
    -Sets a random position.
    -Sets random color.
    -Adds mEnemy to the vector.
    */
    float x, y;
    x = rand() % 900;
    y = 100.f;
    mEnemy.setPosition(x, y);
    mEnemy.setFillColor(sf::Color::Green);

    // Spawn the mEnemy
    mEnemies.push_back(mEnemy);
}
void Game::updateEnemies()
{
    /*
    @return void
    Updates the mEnemy swapn timer and spawn mEnemies
    When the total amount of mEnemies is smaller than the maximum.
    Moves the mEnemies downwords.
    Removes the mEnemies at the edge of the screen. // TODO
    */

    // Updating the timer for mEnemy swapwning
    if (mEnemies.size() < MAX_ENEMIES)
    {
        if (mEnemySpawnTimer >= mEnemySpawnTimerMax)
        {
            // Spawn the mEnemy and reset the timer
            spawnEnemy();
            mEnemySpawnTimer = 0.f;
        }
        else
            mEnemySpawnTimer += 1.f;
    }
    // Move the updating mEnemies
    for (int i = 0; i < mEnemies.size(); i++)
    {
        bool deleted = false;
        mEnemies[i].move(0.f, 3.f);

        if (mEnemies[i].getPosition().y > mWindow->getSize().y)
        {
            mHealth--;
            mEnemies.erase(mEnemies.begin() + i);
        }
    }
    // Check if clicked upon
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (!mMouseHeld)
        {
            mMouseHeld = true;
            bool deleted = false;
            for (auto i = 0; i < mEnemies.size() && !deleted; i++)
            {
                if (mEnemies[i].getGlobalBounds().contains(mMousePosView))
                {
                    // Deleted the enemy
                    deleted = true;
                    mEnemies.erase(mEnemies.begin() + i);

                    // Gain Points
                    mHealth++;
                    mPoints++;
                }
            }
        }
    }
    else
    {
        mMouseHeld = false;
    }
}

void Game::renderEnemies()
{
    // Rendering all the mEnemies
    for (auto &i : mEnemies)
    {
        mWindow->draw(i);
    }
}
void Game::renderText()
{
    mWindow->draw(mUiText);
}

void Game::renderCounter()
{
    // draw RectangleShape for counter in window
}

void Game::pollEvent()
{
    // Event Poling
    while (mWindow->pollEvent(mEvent))
    {
        if (mEvent.type == sf::Event::Closed)
            mWindow->close();
        else if (mEvent.type == sf::Event::KeyPressed)
        {
            if (mEvent.key.code == sf::Keyboard::Escape)
                mWindow->close();
        }
    }
}
void Game::updateMousePositions()
{
    /*
    @return void
    Update Mouse Positions:
    -Mouse Position relative to mWindow (Vector2i);
    */
    mMousePosWindow = sf::Mouse::getPosition(*mWindow);
    mMousePosView = mWindow->mapPixelToCoords(mMousePosWindow);
}

void Game::updateText()
{
    std::stringstream ss;
    ss << "Health = " << mHealth << "     "
       << "Points = " << mPoints;
    mUiText.setString(ss.str());
}

void Game::update()
{
    pollEvent();

    if (mEndGame == false)
    {
        updateMousePositions();
        updateEnemies();
        updateText();
    }

    // End game condition
    if (mHealth <= 0)
    {
        mEndGame = true;
    }
}
void Game::render()
{
    mWindow->clear();

    renderEnemies();

    renderText();

    mWindow->display();
}
