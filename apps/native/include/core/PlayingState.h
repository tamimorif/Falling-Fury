/**
 * @file PlayingState.h
 * @brief The main gameplay state
 */

#pragma once
#include <sstream>
#include <vector>

#include "core/GameState.h"
#include "managers/ResourceManager.h"

class PlayingState : public GameState {
   private:
    // Game objects
    std::vector<sf::RectangleShape> mEnemies;
    sf::RectangleShape mEnemy;

    // UI
    sf::Text mUiText;

    // Game logic
    unsigned mPoints;
    int mHealth;
    float mEnemySpawnTimer;
    float mEnemySpawnTimerMax;
    const int MAX_ENEMIES = 30;
    bool mMouseHeld;
    float mGravity = 200.f;

    // Mouse position
    sf::Vector2i mMousePosWindow;
    sf::Vector2f mMousePosView;

    void initEnemies() {
        mEnemy.setPosition(10.f, 10.f);
        mEnemy.setSize(sf::Vector2f(100.f, 100.f));
        mEnemy.setScale(sf::Vector2f(0.5f, 0.5f));
        mEnemy.setFillColor(sf::Color::Green);
    }

    void initText() {
        mUiText.setFont(ResourceManager::getInstance().getFont("main"));
        mUiText.setCharacterSize(50);
        mUiText.setFillColor(sf::Color::Cyan);
        mUiText.setPosition(170.f, 30.f);
        mUiText.setString("Health: 10 | Points: 0");
    }

    void spawnEnemy(sf::RenderWindow& window) {
        float x = static_cast<float>(rand() % 900);
        float y = -100.f;

        mEnemy.setPosition(x, y);
        mEnemy.setFillColor(sf::Color::Green);
        mEnemies.push_back(mEnemy);
    }

    void updateEnemies(float deltaTime, sf::RenderWindow& window) {
        // Spawn enemies
        if (mEnemies.size() < MAX_ENEMIES) {
            if (mEnemySpawnTimer >= mEnemySpawnTimerMax) {
                spawnEnemy(window);
                mEnemySpawnTimer = 0.f;
            } else {
                mEnemySpawnTimer += 1.f;
            }
        }

        // Move and check enemies
        for (int i = 0; i < mEnemies.size(); i++) {
            mEnemies[i].move(0.f, mGravity * deltaTime);

            // Remove if off screen
            if (mEnemies[i].getPosition().y > window.getSize().y) {
                mHealth--;
                mEnemies.erase(mEnemies.begin() + i);
                i--;

                // Check for game over
                if (mHealth <= 0) {
                    // TODO: Transition to Game Over state
                    mQuit = true;
                }
            }
        }
    }

    void handleClick(sf::RenderWindow& window) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (!mMouseHeld) {
                mMouseHeld = true;
                bool deleted = false;

                mMousePosWindow = sf::Mouse::getPosition(window);
                mMousePosView = window.mapPixelToCoords(mMousePosWindow);

                for (int i = 0; i < mEnemies.size() && !deleted; i++) {
                    if (mEnemies[i].getGlobalBounds().contains(mMousePosView)) {
                        deleted = true;
                        mEnemies.erase(mEnemies.begin() + i);
                        mHealth++;
                        mPoints++;
                    }
                }
            }
        } else {
            mMouseHeld = false;
        }
    }

    void updateText() {
        std::stringstream ss;
        ss << "Health: " << mHealth << " | Points: " << mPoints;
        mUiText.setString(ss.str());
    }

   public:
    PlayingState(Game* game)
        : GameState(game),
          mPoints(0),
          mHealth(10),
          mEnemySpawnTimer(0.f),
          mEnemySpawnTimerMax(10.f),
          mMouseHeld(false) {
        initEnemies();
        initText();
    }

    void update(float deltaTime) override {
        if (mPaused) return;

        // Access window through Game class would be needed
        // For now, we'll handle this in Game class
    }

    void updateWithWindow(float deltaTime, sf::RenderWindow& window) {
        if (mPaused) return;

        updateEnemies(deltaTime, window);
        handleClick(window);
        updateText();
    }

    void render(sf::RenderWindow& window) override {
        // Render enemies
        for (auto& enemy : mEnemies) {
            window.draw(enemy);
        }

        // Render UI
        window.draw(mUiText);
    }

    void handleInput(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                mPaused = !mPaused;
            } else if (event.key.code == sf::Keyboard::Q) {
                mQuit = true;
            }
        }
    }

    StateType getType() const override { return StateType::PLAYING; }

    unsigned getPoints() const { return mPoints; }
    int getHealth() const { return mHealth; }
};
