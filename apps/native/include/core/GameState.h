/**
 * @file GameState.h
 * @brief Base class and state machine for game states
 *
 * Implements a proper state pattern for managing different game screens
 * (Menu, Playing, Paused, GameOver)
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class Game;

/**
 * @brief State enumeration
 */
enum class StateType { MENU, PLAYING, PAUSED, GAME_OVER };

/**
 * @brief Abstract base class for all game states
 */
class GameState {
   protected:
    Game* mGame;
    bool mQuit;
    bool mPaused;

   public:
    GameState(Game* game) : mGame(game), mQuit(false), mPaused(false) {}
    virtual ~GameState() = default;

    /**
     * @brief Update state logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render state visuals
     * @param window Reference to render window
     */
    virtual void render(sf::RenderWindow& window) = 0;

    /**
     * @brief Handle input events
     * @param event SFML event
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Called when entering this state
     */
    virtual void onEnter() {}

    /**
     * @brief Called when leaving this state
     */
    virtual void onExit() {}

    /**
     * @brief Check if state should quit
     */
    bool shouldQuit() const { return mQuit; }

    /**
     * @brief Check if state is paused
     */
    bool isPaused() const { return mPaused; }

    /**
     * @brief Get the state type
     */
    virtual StateType getType() const = 0;
};

/**
 * @brief State manager/machine
 */
class StateManager {
   private:
    std::unique_ptr<GameState> mCurrentState;

   public:
    StateManager() = default;
    ~StateManager() = default;

    /**
     * @brief Change to a new state
     * @param newState The new state to transition to
     */
    void changeState(std::unique_ptr<GameState> newState) {
        if (mCurrentState) {
            mCurrentState->onExit();
        }

        mCurrentState = std::move(newState);

        if (mCurrentState) {
            mCurrentState->onEnter();
        }
    }

    /**
     * @brief Update current state
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime) {
        if (mCurrentState) {
            mCurrentState->update(deltaTime);
        }
    }

    /**
     * @brief Render current state
     * @param window Reference to render window
     */
    void render(sf::RenderWindow& window) {
        if (mCurrentState) {
            mCurrentState->render(window);
        }
    }

    /**
     * @brief Handle input for current state
     * @param event SFML event
     */
    void handleInput(const sf::Event& event) {
        if (mCurrentState) {
            mCurrentState->handleInput(event);
        }
    }

    /**
     * @brief Get current state
     */
    GameState* getCurrentState() const { return mCurrentState.get(); }

    /**
     * @brief Check if there's a current state
     */
    bool hasState() const { return mCurrentState != nullptr; }
};
