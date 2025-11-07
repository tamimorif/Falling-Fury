/**
 * @file ParticleSystem.h
 * @brief Particle effects system for visual feedback
 *
 * Creates particle effects for clicks, explosions, and other visual feedback
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <vector>

/**
 * @brief Single particle
 */
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color startColor;
    sf::Color endColor;
    float startSize;
    float endSize;
    bool active;

    Particle()
        : lifetime(0.f),
          maxLifetime(1.f),
          startSize(5.f),
          endSize(0.f),
          active(false) {
        shape.setRadius(startSize);
        shape.setOrigin(startSize, startSize);
    }

    void update(float deltaTime) {
        if (!active) return;

        lifetime += deltaTime;

        if (lifetime >= maxLifetime) {
            active = false;
            return;
        }

        // Update position
        shape.move(velocity * deltaTime);

        // Apply gravity
        velocity.y += 300.f * deltaTime;

        // Calculate interpolation factor (0 to 1)
        float t = lifetime / maxLifetime;

        // Interpolate size
        float size = startSize + (endSize - startSize) * t;
        shape.setRadius(size);
        shape.setOrigin(size, size);

        // Interpolate color
        sf::Color color;
        color.r = static_cast<sf::Uint8>(startColor.r +
                                         (endColor.r - startColor.r) * t);
        color.g = static_cast<sf::Uint8>(startColor.g +
                                         (endColor.g - startColor.g) * t);
        color.b = static_cast<sf::Uint8>(startColor.b +
                                         (endColor.b - startColor.b) * t);
        color.a = static_cast<sf::Uint8>(255 * (1.f - t));  // Fade out
        shape.setFillColor(color);
    }
};

/**
 * @brief Particle emitter/system
 */
class ParticleSystem {
   private:
    std::vector<Particle> mParticles;
    size_t mPoolSize;

   public:
    /**
     * @brief Constructor
     * @param poolSize Number of particles to pre-allocate
     */
    ParticleSystem(size_t poolSize = 100) : mPoolSize(poolSize) {
        mParticles.resize(mPoolSize);
        std::cout << "ParticleSystem created with " << mPoolSize
                  << " particles\n";
    }

    /**
     * @brief Emit a burst of particles
     * @param position Center position
     * @param count Number of particles
     * @param color Particle color
     * @param speed Speed multiplier
     */
    void emitBurst(const sf::Vector2f& position, int count,
                   const sf::Color& color, float speed = 1.0f) {
        int emitted = 0;
        for (auto& particle : mParticles) {
            if (emitted >= count) break;
            if (!particle.active) {
                emitParticle(particle, position, color, speed);
                emitted++;
            }
        }
    }

    /**
     * @brief Emit click effect (explosion-like)
     * @param position Click position
     * @param enemyColor Color of clicked enemy
     */
    void emitClickEffect(const sf::Vector2f& position,
                         const sf::Color& enemyColor) {
        emitBurst(position, 20, enemyColor, 200.f);
    }

    /**
     * @brief Emit miss effect (downward trail)
     * @param position Enemy position
     */
    void emitMissEffect(const sf::Vector2f& position) {
        emitBurst(position, 10, sf::Color(255, 100, 100), 100.f);
    }

    /**
     * @brief Emit combo effect (stars/sparkles)
     * @param position Position
     */
    void emitComboEffect(const sf::Vector2f& position) {
        for (int i = 0; i < 5; i++) {
            int available = 0;
            for (auto& particle : mParticles) {
                if (!particle.active && available < 3) {
                    sf::Color comboColor = sf::Color(255, 215, 0);  // Gold
                    emitParticle(particle, position, comboColor, 150.f);
                    particle.startSize = 8.f;
                    particle.maxLifetime = 1.5f;
                    available++;
                }
            }
        }
    }

    /**
     * @brief Update all particles
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime) {
        for (auto& particle : mParticles) {
            if (particle.active) {
                particle.update(deltaTime);
            }
        }
    }

    /**
     * @brief Render all active particles
     * @param window Reference to render window
     */
    void render(sf::RenderWindow& window) {
        for (const auto& particle : mParticles) {
            if (particle.active) {
                window.draw(particle.shape);
            }
        }
    }

    /**
     * @brief Clear all particles
     */
    void clear() {
        for (auto& particle : mParticles) {
            particle.active = false;
        }
    }

    /**
     * @brief Get number of active particles
     */
    int getActiveCount() const {
        int count = 0;
        for (const auto& particle : mParticles) {
            if (particle.active) count++;
        }
        return count;
    }

   private:
    /**
     * @brief Emit a single particle
     */
    void emitParticle(Particle& particle, const sf::Vector2f& position,
                      const sf::Color& color, float speed) {
        particle.active = true;
        particle.lifetime = 0.f;
        particle.maxLifetime =
            0.5f + static_cast<float>(rand() % 100) / 200.f;  // 0.5-1.0s

        // Random direction
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
        float velocityMag = speed + static_cast<float>(rand() % 100);

        particle.velocity.x = cos(angle) * velocityMag;
        particle.velocity.y =
            sin(angle) * velocityMag - 100.f;  // Slight upward bias

        particle.shape.setPosition(position);

        particle.startColor = color;
        particle.endColor = color;
        particle.endColor.a = 0;

        particle.startSize = 3.f + static_cast<float>(rand() % 5);
        particle.endSize = 0.5f;

        particle.shape.setRadius(particle.startSize);
        particle.shape.setOrigin(particle.startSize, particle.startSize);
        particle.shape.setFillColor(particle.startColor);
    }
};

/**
 * @brief Screen shake effect manager
 */
class ScreenShake {
   private:
    float mDuration;
    float mIntensity;
    float mTimer;
    sf::Vector2f mOriginalPosition;
    bool mActive;

   public:
    ScreenShake()
        : mDuration(0.f), mIntensity(0.f), mTimer(0.f), mActive(false) {}

    /**
     * @brief Start screen shake
     * @param duration Duration in seconds
     * @param intensity Shake intensity
     */
    void start(float duration, float intensity) {
        mDuration = duration;
        mIntensity = intensity;
        mTimer = 0.f;
        mActive = true;
    }

    /**
     * @brief Update shake
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime) {
        if (!mActive) return;

        mTimer += deltaTime;
        if (mTimer >= mDuration) {
            mActive = false;
        }
    }

    /**
     * @brief Get shake offset
     * @return Offset vector for camera/view
     */
    sf::Vector2f getOffset() const {
        if (!mActive) return sf::Vector2f(0.f, 0.f);

        // Decrease intensity over time
        float factor = 1.f - (mTimer / mDuration);
        float currentIntensity = mIntensity * factor;

        // Random offset
        float x = (static_cast<float>(rand() % 200) - 100.f) / 100.f *
                  currentIntensity;
        float y = (static_cast<float>(rand() % 200) - 100.f) / 100.f *
                  currentIntensity;

        return sf::Vector2f(x, y);
    }

    /**
     * @brief Check if shake is active
     */
    bool isActive() const { return mActive; }

    /**
     * @brief Stop shake immediately
     */
    void stop() { mActive = false; }
};
