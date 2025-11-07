#include "core/Game.h"

#include <iostream>
// Constructor
Game::Game()
    : mVideoMode(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGH)),
      mWindow(std::make_unique<sf::RenderWindow>(
          mVideoMode, "Falling Fury", sf::Style::Titlebar | sf::Style::Close)),
      mPoints(0),
      mMaxPoint(0),
      mHealth(10),
      mEndGame(false),
      mEnemySpawnTimerMax(10.f),
      mEnemySpawnTimer(10.f),
      mMouseHeld(false) {
    mWindow->setFramerateLimit(60);

    // Load resources via ResourceManager
    ResourceManager::getInstance().loadFont(
        "main", "assets/fonts/1/BebasNeue-Regular.ttf");

    // Load high score from file
    std::string savedScore = getData();
    try {
        mMaxPoint = std::stoi(savedScore);
    } catch (const std::exception& e) {
        std::cerr
            << "ERROR::GAME::CONSTRUCTOR::Invalid score data, resetting to 0\n";
        mMaxPoint = 0;
    }

    initText();
    initMaxPoint();
    initEnemies();
}

// Destructor
Game::~Game() {
    // Smart pointer automatically cleans up mWindow
    // Cleanup ResourceManager
    ResourceManager::destroy();
}

void Game::update() {
    updateDeltaTime();
    pollEvent();

    if (mEndGame) return;
    updateMousePositions();
    updateEnemies();
    updateText();
}

void Game::updateDeltaTime() {
    // Update delta time (time since last frame)
    mDeltaTime = mDeltaClock.restart().asSeconds();
}

void Game::render() {
    mWindow->clear();

    renderEnemies();

    renderText();

    if (mHealth <= 0) {
        mWindow->clear();
        renderMaxPoint();
    }

    mWindow->display();
}

void Game::updateMousePositions() {
    /*
    @return void
    Update Mouse Positions:
    -Mouse Position relative to mWindow (Vector2i);
    */
    mMousePosWindow = sf::Mouse::getPosition(*mWindow);
    mMousePosView = mWindow->mapPixelToCoords(mMousePosWindow);
}

void Game::updateEnemies() {
    /*
    @return void
    Updates the mEnemy swapn timer and spawn mEnemies
    When the total amount of mEnemies is smaller than the maximum.
    Moves the mEnemies downwords.
    Removes the mEnemies at the edge of the screen. // TODO
    */

    // Updating the timer for mEnemy swapwning
    if (mEnemies.size() < MAX_ENEMIES) {
        if (mEnemySpawnTimer >= mEnemySpawnTimerMax) {
            // Spawn the mEnemy and reset the timer
            spawnEnemy();
            mEnemySpawnTimer = 0.f;
        } else
            mEnemySpawnTimer += 1.f;
    }
    // Move the updating mEnemies (frame-independent with deltaTime)
    for (int i = 0; i < mEnemies.size(); i++) {
        bool deleted = false;
        mEnemies[i].move(0.f, mGravity * mDeltaTime);

        if (mEnemies[i].getPosition().y > mWindow->getSize().y) {
            mHealth--;
            mEnemies.erase(mEnemies.begin() + i);
        }
    }
    // Check if clicked upon
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!mMouseHeld) {
            mMouseHeld = true;
            bool deleted = false;
            for (int i = 0; i < mEnemies.size() && !deleted; i++) {
                if (mEnemies[i].getGlobalBounds().contains(mMousePosView)) {
                    // Deleted the enemy
                    deleted = true;
                    mEnemies.erase(mEnemies.begin() + i);

                    // Gain Points
                    mHealth++;
                    mPoints++;
                }
            }
        }
    } else {
        mMouseHeld = false;
    }
}

void Game::updateText() {
    std::stringstream ss;
    ss << "Health = " << mHealth << "     "
       << "Points = " << mPoints << "     "
       << "Max Point = " << getData();
    mUiText.setString(ss.str());
}

void Game::initEnemies() {
    mEnemy.setPosition(10.f, 10.f);
    mEnemy.setSize(sf::Vector2f(100.f, 100.f));
    mEnemy.setScale(sf::Vector2f(0.5f, 0.5f));
    mEnemy.setFillColor(sf::Color::Green);
}

void Game::initText() {
    mUiText.setFont(ResourceManager::getInstance().getFont("main"));
    mUiText.setCharacterSize(50);
    mUiText.setFillColor(sf::Color::Cyan);
    mUiText.setPosition(170.f, 30.f);
    mUiText.setString("NONE");
}

void Game::initMaxPoint() {
    mMaxpointText.setFont(ResourceManager::getInstance().getFont("main"));
    mMaxpointText.setCharacterSize(50);
    mMaxpointText.setColor(sf::Color::White);
    mMaxpointText.setPosition(-200.f, (mWindow->getSize().y / 2) - 50.f);
}

const bool Game::running() const { return mWindow->isOpen(); }

const bool Game::getEndGame() const { return mEndGame; }

// Functions

void Game::spawnEnemy() {
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
    mDistance += mGravity;
    if (mDistance >= 8) {
        mEnemy.setPosition(x, y);
        mDistance = 0;
        mEnemy.setFillColor(sf::Color::Green);
        // Spawn the mEnemy
        mEnemies.push_back(mEnemy);
    }
}

void Game::renderEnemies() {
    // Rendering all the mEnemies
    for (auto& i : mEnemies) {
        mWindow->draw(i);
    }
}
void Game::renderText() { mWindow->draw(mUiText); }
void Game::nextColor() {
    mBlue += (mBlue2 ? mSpeed : -mSpeed);
    if (mBlue >= 250 || mBlue <= 0) {
        mBlue2 = !mBlue2;
        mGreen += (mGreen2 ? mSpeed : -mSpeed);
    }
    if (mGreen >= 250 || mGreen <= 0) {
        mGreen2 = !mGreen2;
        mRed += (mRed2 ? mSpeed : -mSpeed);
    }
    if (mRed >= 250 || mRed <= 0) {
        mRed2 = !mRed2;
    }
}
void Game::renderMaxPoint() {
    mMaxpointText.move(5.f, 0.f);
    if (mMaxpointText.getPosition().x > mWindow->getSize().x)
        mMaxpointText.setPosition(-200.f, mMaxpointText.getPosition().y);
    nextColor();
    mMaxpointText.setColor(sf::Color(mRed, mGreen, mBlue, 255));
    mWindow->draw(mMaxpointText);
}

void Game::pollEvent() {
    // Event Poling
    while (mWindow->pollEvent(mEvent)) {
        if (mEvent.type == sf::Event::Closed)
            mWindow->close();
        else if (mEvent.type == sf::Event::KeyPressed) {
            if (mEvent.key.code == sf::Keyboard::Escape) mWindow->close();
        }
    }
}

std::string Game::getData() {
    static const std::string FILE_PATH = "data/data.txt";
    std::ifstream input_file(FILE_PATH);

    // If file doesn't exist, create it with default value
    if (!input_file.is_open()) {
        std::ofstream create_file(FILE_PATH);
        if (!create_file.is_open()) {
            std::cerr << "ERROR::GAME::GETDATA::Cannot create data file!\n";
            return "0";
        }
        create_file << "0";
        create_file.close();
        return "0";
    }

    unsigned savedMaxPoint = 0;
    input_file >> savedMaxPoint;
    input_file.close();

    // Update mMaxPoint if current score is higher
    if (savedMaxPoint > mMaxPoint) mMaxPoint = savedMaxPoint;

    return std::to_string(mMaxPoint);
}

std::string Game::saveData() {
    static const std::string FILE_PATH = "data/data.txt";

    // Update max point if current score is higher
    if (mPoints > mMaxPoint) mMaxPoint = mPoints;

    // Write the max point to file
    std::ofstream output_file(FILE_PATH);
    if (!output_file.is_open()) {
        std::cerr << "ERROR::GAME::SAVEDATA::Cannot write to data file!\n";
        return std::to_string(mMaxPoint);
    }

    output_file << mMaxPoint;
    output_file.close();

    return std::to_string(mMaxPoint);
}

// std::string Game::resetData()
// {
//     static const std::string FILE_PATH = "../database/data.txt";
//     std::ifstream input_file(FILE_PATH);

//     if (!input_file.is_open())
//         throw std::runtime_error("No input file GET_DATA");

//     input_file.close();
//     return std::to_string(0);
// }