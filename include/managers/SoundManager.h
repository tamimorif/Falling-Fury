/**
 * @file SoundManager.h
 * @brief Singleton class for managing game audio
 *
 * Handles sound effects and background music with volume control
 */

#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class SoundManager {
   private:
    inline static SoundManager* sInstance = nullptr;

    // Sound resources (managed by ResourceManager)
    std::map<std::string, sf::Sound> mSounds;
    std::map<std::string, std::unique_ptr<sf::Music>> mMusic;

    // Volume settings
    float mSoundVolume;
    float mMusicVolume;
    bool mSoundEnabled;
    bool mMusicEnabled;

    sf::Music* mCurrentMusic;

    // Private constructor for singleton
    SoundManager()
        : mSoundVolume(70.f),
          mMusicVolume(50.f),
          mSoundEnabled(true),
          mMusicEnabled(true),
          mCurrentMusic(nullptr) {
        std::cout << "SoundManager initialized\n";
    }

    // Delete copy constructor and assignment
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

   public:
    /**
     * @brief Get singleton instance
     */
    static SoundManager& getInstance() {
        if (sInstance == nullptr) {
            sInstance = new SoundManager();
        }
        return *sInstance;
    }

    /**
     * @brief Destroy singleton instance
     */
    static void destroy() {
        if (sInstance != nullptr) {
            delete sInstance;
            sInstance = nullptr;
        }
    }

    /**
     * @brief Register a sound with a buffer
     * @param name Sound identifier
     * @param buffer Sound buffer to use
     */
    void registerSound(const std::string& name, const sf::SoundBuffer& buffer) {
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.setVolume(mSoundVolume);
        mSounds[name] = sound;
        std::cout << "Registered sound: " << name << "\n";
    }

    /**
     * @brief Load and register music
     * @param name Music identifier
     * @param filepath Path to music file
     * @return true if loaded successfully
     */
    bool loadMusic(const std::string& name, const std::string& filepath) {
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(filepath)) {
            std::cerr << "ERROR::SOUNDMANAGER::Failed to load music: "
                      << filepath << "\n";
            return false;
        }

        music->setVolume(mMusicVolume);
        music->setLoop(true);
        mMusic[name] = std::move(music);
        std::cout << "Loaded music: " << name << "\n";
        return true;
    }

    /**
     * @brief Play a sound effect
     * @param name Sound identifier
     */
    void playSound(const std::string& name) {
        if (!mSoundEnabled) return;

        auto it = mSounds.find(name);
        if (it != mSounds.end()) {
            it->second.play();
        } else {
            std::cerr << "Sound not found: " << name << "\n";
        }
    }

    /**
     * @brief Play music by name
     * @param name Music identifier
     */
    void playMusic(const std::string& name) {
        if (!mMusicEnabled) return;

        auto it = mMusic.find(name);
        if (it != mMusic.end()) {
            // Stop current music if playing
            if (mCurrentMusic &&
                mCurrentMusic->getStatus() == sf::Music::Playing) {
                mCurrentMusic->stop();
            }

            mCurrentMusic = it->second.get();
            mCurrentMusic->play();
            std::cout << "Playing music: " << name << "\n";
        } else {
            std::cerr << "Music not found: " << name << "\n";
        }
    }

    /**
     * @brief Stop current music
     */
    void stopMusic() {
        if (mCurrentMusic) {
            mCurrentMusic->stop();
            mCurrentMusic = nullptr;
        }
    }

    /**
     * @brief Pause current music
     */
    void pauseMusic() {
        if (mCurrentMusic) {
            mCurrentMusic->pause();
        }
    }

    /**
     * @brief Resume current music
     */
    void resumeMusic() {
        if (mCurrentMusic && mMusicEnabled) {
            mCurrentMusic->play();
        }
    }

    /**
     * @brief Set sound effects volume
     * @param volume Volume (0-100)
     */
    void setSoundVolume(float volume) {
        mSoundVolume = std::max(0.f, std::min(100.f, volume));
        for (auto& pair : mSounds) {
            pair.second.setVolume(mSoundVolume);
        }
    }

    /**
     * @brief Set music volume
     * @param volume Volume (0-100)
     */
    void setMusicVolume(float volume) {
        mMusicVolume = std::max(0.f, std::min(100.f, volume));
        for (auto& pair : mMusic) {
            pair.second->setVolume(mMusicVolume);
        }
    }

    /**
     * @brief Enable/disable sound effects
     */
    void setSoundEnabled(bool enabled) {
        mSoundEnabled = enabled;
        if (!enabled) {
            // Stop all playing sounds
            for (auto& pair : mSounds) {
                pair.second.stop();
            }
        }
    }

    /**
     * @brief Enable/disable music
     */
    void setMusicEnabled(bool enabled) {
        mMusicEnabled = enabled;
        if (!enabled && mCurrentMusic) {
            mCurrentMusic->stop();
        } else if (enabled && mCurrentMusic) {
            mCurrentMusic->play();
        }
    }

    // Getters
    float getSoundVolume() const { return mSoundVolume; }
    float getMusicVolume() const { return mMusicVolume; }
    bool isSoundEnabled() const { return mSoundEnabled; }
    bool isMusicEnabled() const { return mMusicEnabled; }
    bool isMusicPlaying() const {
        return mCurrentMusic &&
               mCurrentMusic->getStatus() == sf::Music::Playing;
    }

    /**
     * @brief Stop all sounds
     */
    void stopAllSounds() {
        for (auto& pair : mSounds) {
            pair.second.stop();
        }
    }

    /**
     * @brief Create placeholder sounds for testing (using sin wave generation)
     * This is a simple implementation - in production you'd load actual sound
     * files
     */
    void createPlaceholderSounds() {
        // Note: SFML doesn't have built-in procedural audio generation
        // You would typically load actual sound files here
        // This is just a placeholder to show the structure

        std::cout << "Note: Load actual sound files in production:\n";
        std::cout << "  - click.wav (successful click sound)\n";
        std::cout << "  - miss.wav (enemy reaches bottom)\n";
        std::cout << "  - combo.wav (combo milestone reached)\n";
        std::cout << "  - gameover.wav (game over sound)\n";
        std::cout << "  - background.ogg (background music)\n";
    }

    /**
     * @brief Destructor
     */
    ~SoundManager() {
        stopAllSounds();
        stopMusic();
        std::cout << "SoundManager destroyed\n";
    }
};
