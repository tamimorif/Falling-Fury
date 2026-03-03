/**
 * @file ResourceManager.h
 * @brief Singleton class for managing game resources (fonts, textures, sounds)
 *
 * This class ensures only one instance exists and provides centralized
 * resource loading and access throughout the game.
 */

#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class ResourceManager {
   private:
    // Singleton instance
    inline static ResourceManager* sInstance = nullptr;

    // Resource containers
    std::map<std::string, std::unique_ptr<sf::Font>> mFonts;
    std::map<std::string, std::unique_ptr<sf::Texture>> mTextures;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> mSoundBuffers;

    // Private constructor for singleton
    ResourceManager() = default;

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

   public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the ResourceManager instance
     */
    static ResourceManager& getInstance() {
        if (sInstance == nullptr) {
            sInstance = new ResourceManager();
        }
        return *sInstance;
    }

    /**
     * @brief Destroy the singleton instance
     */
    static void destroy() {
        if (sInstance != nullptr) {
            delete sInstance;
            sInstance = nullptr;
        }
    }

    /**
     * @brief Load a font from file
     * @param name Identifier for the font
     * @param filepath Path to the font file
     * @return true if loaded successfully, false otherwise
     */
    bool loadFont(const std::string& name, const std::string& filepath) {
        auto font = std::make_unique<sf::Font>();
        if (!font->loadFromFile(filepath)) {
            std::cerr << "ERROR::RESOURCEMANAGER::Failed to load font: "
                      << filepath << "\n";
            return false;
        }
        mFonts[name] = std::move(font);
        std::cout << "Successfully loaded font: " << name << "\n";
        return true;
    }

    /**
     * @brief Load a texture from file
     * @param name Identifier for the texture
     * @param filepath Path to the texture file
     * @return true if loaded successfully, false otherwise
     */
    bool loadTexture(const std::string& name, const std::string& filepath) {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(filepath)) {
            std::cerr << "ERROR::RESOURCEMANAGER::Failed to load texture: "
                      << filepath << "\n";
            return false;
        }
        mTextures[name] = std::move(texture);
        std::cout << "Successfully loaded texture: " << name << "\n";
        return true;
    }

    /**
     * @brief Load a sound buffer from file
     * @param name Identifier for the sound
     * @param filepath Path to the sound file
     * @return true if loaded successfully, false otherwise
     */
    bool loadSound(const std::string& name, const std::string& filepath) {
        auto soundBuffer = std::make_unique<sf::SoundBuffer>();
        if (!soundBuffer->loadFromFile(filepath)) {
            std::cerr << "ERROR::RESOURCEMANAGER::Failed to load sound: "
                      << filepath << "\n";
            return false;
        }
        mSoundBuffers[name] = std::move(soundBuffer);
        std::cout << "Successfully loaded sound: " << name << "\n";
        return true;
    }

    /**
     * @brief Get a font by name
     * @param name Font identifier
     * @return Reference to the font
     * @throws std::runtime_error if font not found
     */
    sf::Font& getFont(const std::string& name) {
        auto it = mFonts.find(name);
        if (it == mFonts.end()) {
            throw std::runtime_error("Font not found: " + name);
        }
        return *it->second;
    }

    /**
     * @brief Get a texture by name
     * @param name Texture identifier
     * @return Reference to the texture
     * @throws std::runtime_error if texture not found
     */
    sf::Texture& getTexture(const std::string& name) {
        auto it = mTextures.find(name);
        if (it == mTextures.end()) {
            throw std::runtime_error("Texture not found: " + name);
        }
        return *it->second;
    }

    /**
     * @brief Get a sound buffer by name
     * @param name Sound identifier
     * @return Reference to the sound buffer
     * @throws std::runtime_error if sound not found
     */
    sf::SoundBuffer& getSound(const std::string& name) {
        auto it = mSoundBuffers.find(name);
        if (it == mSoundBuffers.end()) {
            throw std::runtime_error("Sound not found: " + name);
        }
        return *it->second;
    }

    /**
     * @brief Clear all loaded resources
     */
    void clearAll() {
        mFonts.clear();
        mTextures.clear();
        mSoundBuffers.clear();
        std::cout << "All resources cleared\n";
    }

    /**
     * @brief Destructor
     */
    ~ResourceManager() { clearAll(); }
};
