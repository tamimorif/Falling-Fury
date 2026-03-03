/**
 * @file Enemy.h
 * @brief Enemy class hierarchy with different enemy types
 *
 * Implements proper OOP design with base Enemy class and derived types
 * for extensibility and different gameplay mechanics
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

/**
 * @brief Enemy type enumeration
 */
enum class EnemyType {
    NORMAL,  // Standard enemy
    FAST,    // Moves faster, less health
    TANK,    // Moves slower, more health
    BONUS    // Gives extra points
};

/**
 * @brief Base Enemy class
 */
class Enemy {
   protected:
    sf::RectangleShape mShape;
    EnemyType mType;
    float mSpeed;
    int mHealthValue;  // Health damage when missed
    int mPointValue;   // Points when clicked
    bool mActive;

   public:
    /**
     * @brief Constructor
     * @param type Type of enemy
     * @param position Starting position
     */
    Enemy(EnemyType type, const sf::Vector2f& position)
        : mType(type), mActive(true) {
        mShape.setPosition(position);
        mShape.setSize(sf::Vector2f(100.f, 100.f));
        mShape.setScale(sf::Vector2f(0.5f, 0.5f));

        // Set default values based on type
        switch (mType) {
            case EnemyType::NORMAL:
                mSpeed = 200.f;
                mHealthValue = 1;
                mPointValue = 1;
                mShape.setFillColor(sf::Color::Green);
                break;

            case EnemyType::FAST:
                mSpeed = 350.f;
                mHealthValue = 1;
                mPointValue = 2;
                mShape.setFillColor(sf::Color::Red);
                mShape.setScale(sf::Vector2f(0.4f, 0.4f));
                break;

            case EnemyType::TANK:
                mSpeed = 120.f;
                mHealthValue = 2;
                mPointValue = 3;
                mShape.setFillColor(sf::Color::Blue);
                mShape.setScale(sf::Vector2f(0.7f, 0.7f));
                break;

            case EnemyType::BONUS:
                mSpeed = 250.f;
                mHealthValue = 0;  // Doesn't hurt if missed
                mPointValue = 5;
                mShape.setFillColor(sf::Color::Yellow);
                mShape.setScale(sf::Vector2f(0.45f, 0.45f));
                break;
        }
    }

    virtual ~Enemy() = default;

    /**
     * @brief Update enemy position
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) {
        mShape.move(0.f, mSpeed * deltaTime);
    }

    /**
     * @brief Render the enemy
     * @param window Reference to render window
     */
    virtual void render(sf::RenderWindow& window) { window.draw(mShape); }

    /**
     * @brief Check if enemy is clicked
     * @param mousePos Mouse position
     * @return true if clicked, false otherwise
     */
    bool isClicked(const sf::Vector2f& mousePos) const {
        return mShape.getGlobalBounds().contains(mousePos);
    }

    /**
     * @brief Check if enemy is off screen
     * @param screenHeight Height of the screen
     * @return true if off screen, false otherwise
     */
    bool isOffScreen(float screenHeight) const {
        return mShape.getPosition().y > screenHeight;
    }

    // Getters
    EnemyType getType() const { return mType; }
    int getHealthValue() const { return mHealthValue; }
    int getPointValue() const { return mPointValue; }
    bool isActive() const { return mActive; }
    const sf::Vector2f& getPosition() const { return mShape.getPosition(); }

    // Setters
    void setActive(bool active) { mActive = active; }
    void setPosition(const sf::Vector2f& pos) { mShape.setPosition(pos); }
};

/**
 * @brief Normal enemy - standard gameplay
 */
class NormalEnemy : public Enemy {
   public:
    NormalEnemy(const sf::Vector2f& position)
        : Enemy(EnemyType::NORMAL, position) {}
};

/**
 * @brief Fast enemy - quick but worth more points
 */
class FastEnemy : public Enemy {
   public:
    FastEnemy(const sf::Vector2f& position)
        : Enemy(EnemyType::FAST, position) {}

    void update(float deltaTime) override {
        // Fast enemies might have special movement patterns
        Enemy::update(deltaTime);

        // Add slight horizontal movement for variety
        float wiggle = sin(mShape.getPosition().y * 0.01f) * 50.f * deltaTime;
        mShape.move(wiggle, 0.f);
    }
};

/**
 * @brief Tank enemy - slow but costs more health if missed
 */
class TankEnemy : public Enemy {
   public:
    TankEnemy(const sf::Vector2f& position)
        : Enemy(EnemyType::TANK, position) {}
};

/**
 * @brief Bonus enemy - doesn't hurt if missed, high points
 */
class BonusEnemy : public Enemy {
   private:
    float mLifetime;
    const float MAX_LIFETIME = 5.0f;  // Disappears after 5 seconds

   public:
    BonusEnemy(const sf::Vector2f& position)
        : Enemy(EnemyType::BONUS, position), mLifetime(0.f) {}

    void update(float deltaTime) override {
        Enemy::update(deltaTime);

        mLifetime += deltaTime;

        // Pulsing effect
        float pulse = 0.5f + 0.2f * sin(mLifetime * 10.f);
        mShape.setScale(sf::Vector2f(pulse, pulse));

        // Fade out near end of lifetime
        if (mLifetime > MAX_LIFETIME * 0.7f) {
            float alpha = 255.f * (1.f - (mLifetime - MAX_LIFETIME * 0.7f) /
                                             (MAX_LIFETIME * 0.3f));
            sf::Color color = mShape.getFillColor();
            color.a = static_cast<sf::Uint8>(alpha);
            mShape.setFillColor(color);
        }

        // Deactivate if lifetime exceeded
        if (mLifetime > MAX_LIFETIME) {
            mActive = false;
        }
    }
};

/**
 * @brief Enemy Factory for creating enemies
 */
class EnemyFactory {
   public:
    /**
     * @brief Create an enemy based on type
     * @param type Enemy type to create
     * @param position Starting position
     * @return Unique pointer to created enemy
     */
    static std::unique_ptr<Enemy> createEnemy(EnemyType type,
                                              const sf::Vector2f& position) {
        switch (type) {
            case EnemyType::NORMAL:
                return std::make_unique<NormalEnemy>(position);
            case EnemyType::FAST:
                return std::make_unique<FastEnemy>(position);
            case EnemyType::TANK:
                return std::make_unique<TankEnemy>(position);
            case EnemyType::BONUS:
                return std::make_unique<BonusEnemy>(position);
            default:
                return std::make_unique<NormalEnemy>(position);
        }
    }

    /**
     * @brief Create a random enemy
     * @param position Starting position
     * @return Unique pointer to created enemy
     */
    static std::unique_ptr<Enemy> createRandomEnemy(
        const sf::Vector2f& position) {
        int randomType = rand() % 100;

        // 50% Normal, 25% Fast, 20% Tank, 5% Bonus
        if (randomType < 50)
            return createEnemy(EnemyType::NORMAL, position);
        else if (randomType < 75)
            return createEnemy(EnemyType::FAST, position);
        else if (randomType < 95)
            return createEnemy(EnemyType::TANK, position);
        else
            return createEnemy(EnemyType::BONUS, position);
    }
};
