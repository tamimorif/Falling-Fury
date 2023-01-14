#include "Game.h"

void Game::initFonts()
{
    // Fonts/Dosis-Light.ttf
    if (mFont.loadFromFile("./Fonts2/BebasNeue-Regular.ttf"))
    {
        std::cout << "ERROR::GAME::INITFONTS::Failed to load fonts!\n";
        throw;
    }
}

void Game::initEnemies()
{
    mEnemy.setPosition(10.f, 10.f);
    mEnemy.setSize(sf::Vector2f(100.f, 100.f));
    mEnemy.setScale(sf::Vector2f(0.5f, 0.5f));
    mEnemy.setFillColor(sf::Color::Cyan);
    // mEnemy.setOutlineColor(sf::Color::Green);
    // mEnemy.setOutlineThickness(1.f);
}

void Game::initText()
{
    mUiText.setFont(mFont);

    // set the string to display
    mUiText.setString("Hello world");

    // set the character size
    mUiText.setCharacterSize(200);

    // set the color
    mUiText.setFillColor(sf::Color::Red);

    mUiText.setPosition(100.f, 100.f);
    mUiText.setScale(10.f, 10.f);
    mUiText.setOutlineThickness(5.f);
    mUiText.setOutlineColor(sf::Color::Blue);

    mUiText.setOrigin(10.f, 10.f);
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
    initEnemies();
    initText();
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
    mEnemy.setPosition(static_cast<float>(rand() % static_cast<int>(mWindow->getSize().x) - mEnemy.getSize().x), 0.f);
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
            std::cout << "Health " << mHealth << '\n';
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
            for (size_t i = 0; i < mEnemies.size() && !deleted; i++)
            {
                if (mEnemies[i].getGlobalBounds().contains(mMousePosView))
                {
                    // Deleted the enemy
                    deleted = true;
                    mEnemies.erase(mEnemies.begin() + i);

                    // Gain Points
                    // points++;
                    std::cout << "Health " << mHealth << '\n';
                    mHealth++;
                    // std::cout << "Points " << points << '\n';
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

void Game::renderCounter()
{
    // draw RectangleShape for counter in window
}

void Game::updateText()
{
}

void Game::renderText()
{
    mUiText.setPosition(300, 300);
    mUiText.setString("Hello World");
    std::cout << mUiText.getPosition().x << ' ' << mUiText.getPosition().y << std::endl;
    std::string a = mUiText.getString();
    std::cout << a << std::endl;
    mWindow->draw(mUiText);
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

void Game::update()
{
    pollEvent();

    if (!mEndGame)
    {
        updateMousePositions();
        updateEnemies();
    }

    // End game condition
    if (!mHealth)
    {
        mEndGame = true;
    }
    //  Update mouse position

    // Relative to Screen
    // std::cout << "Mouse pos: " << sf::Mouse::getPosition().x << ' ' << sf::Mouse::getPosition().y << '\n';

    // Relative to the Window
    // std::cout << "Mouse pos: " << sf::Mouse::getPosition(*window).x << ' ' << sf::Mouse::getPosition(*window).y << '\n';
}
void Game::render()
{
    mWindow->clear();
    // draw game object

    // renderEnemies();

    renderText();

    mWindow->display();
}
