/**
 * @file ScoreManager.h
 * @brief Singleton class for managing game scores and persistence
 *
 * Handles score tracking, high score persistence, and combo multipliers
 */

#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Score entry structure for leaderboard
 */
struct ScoreEntry {
    std::string playerName;
    unsigned score;
    std::string date;

    ScoreEntry(const std::string& name = "Player", unsigned s = 0,
               const std::string& d = "")
        : playerName(name), score(s), date(d) {}

    bool operator<(const ScoreEntry& other) const {
        return score > other.score;  // Descending order
    }
};

/**
 * @brief Score Manager singleton class
 */
class ScoreManager {
   private:
    inline static ScoreManager* sInstance = nullptr;

    const std::string DATA_FILE_PATH = "../data/data.txt";
    const std::string LEADERBOARD_FILE_PATH = "../data/leaderboard.txt";
    const int MAX_LEADERBOARD_ENTRIES = 10;

    unsigned mCurrentScore;
    unsigned mHighScore;
    unsigned mComboCount;
    float mComboMultiplier;
    const float BASE_MULTIPLIER = 1.0f;
    const float MULTIPLIER_INCREMENT = 0.5f;
    const int COMBO_THRESHOLD = 3;  // Combo starts after 3 consecutive hits

    std::vector<ScoreEntry> mLeaderboard;

    // Private constructor for singleton
    ScoreManager()
        : mCurrentScore(0),
          mHighScore(0),
          mComboCount(0),
          mComboMultiplier(BASE_MULTIPLIER) {
        loadHighScore();
        loadLeaderboard();
    }

    // Delete copy constructor and assignment
    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    /**
     * @brief Load high score from file
     */
    void loadHighScore() {
        std::ifstream file(DATA_FILE_PATH);
        if (file.is_open()) {
            file >> mHighScore;
            file.close();
            std::cout << "Loaded high score: " << mHighScore << "\n";
        } else {
            // Create file with default value
            std::ofstream newFile(DATA_FILE_PATH);
            if (newFile.is_open()) {
                newFile << 0;
                newFile.close();
                mHighScore = 0;
                std::cout << "Created new score file\n";
            } else {
                std::cerr
                    << "ERROR::SCOREMANAGER::Could not create data file\n";
            }
        }
    }

    /**
     * @brief Load leaderboard from file
     */
    void loadLeaderboard() {
        std::ifstream file(LEADERBOARD_FILE_PATH);
        if (file.is_open()) {
            std::string name, date;
            unsigned score;

            while (file >> name >> score >> date) {
                mLeaderboard.emplace_back(name, score, date);
            }
            file.close();
            std::cout << "Loaded " << mLeaderboard.size()
                      << " leaderboard entries\n";
        } else {
            std::cout << "No leaderboard file found, will create on save\n";
        }
    }

   public:
    /**
     * @brief Get singleton instance
     */
    static ScoreManager& getInstance() {
        if (sInstance == nullptr) {
            sInstance = new ScoreManager();
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
     * @brief Reset current score
     */
    void resetScore() {
        mCurrentScore = 0;
        mComboCount = 0;
        mComboMultiplier = BASE_MULTIPLIER;
    }

    /**
     * @brief Add points with combo multiplier
     * @param basePoints Base points to add
     */
    void addPoints(int basePoints) {
        mComboCount++;

        // Update multiplier based on combo
        if (mComboCount >= COMBO_THRESHOLD) {
            mComboMultiplier =
                BASE_MULTIPLIER +
                ((mComboCount - COMBO_THRESHOLD) * MULTIPLIER_INCREMENT);
        }

        int pointsToAdd = static_cast<int>(basePoints * mComboMultiplier);
        mCurrentScore += pointsToAdd;

        std::cout << "Added " << pointsToAdd << " points (combo x"
                  << mComboMultiplier << ")\n";
    }

    /**
     * @brief Break combo (called when missing an enemy)
     */
    void breakCombo() {
        if (mComboCount > 0) {
            std::cout << "Combo broken! Was at " << mComboCount << " hits\n";
        }
        mComboCount = 0;
        mComboMultiplier = BASE_MULTIPLIER;
    }

    /**
     * @brief Save high score to file
     */
    void saveHighScore() {
        if (mCurrentScore > mHighScore) {
            mHighScore = mCurrentScore;

            std::ofstream file(DATA_FILE_PATH);
            if (file.is_open()) {
                file << mHighScore;
                file.close();
                std::cout << "New high score saved: " << mHighScore << "\n";
            } else {
                std::cerr << "ERROR::SCOREMANAGER::Could not save high score\n";
            }
        }
    }

    /**
     * @brief Add entry to leaderboard
     * @param playerName Name of the player
     * @param score Score achieved
     * @param date Date string
     */
    void addToLeaderboard(const std::string& playerName, unsigned score,
                          const std::string& date) {
        mLeaderboard.emplace_back(playerName, score, date);

        // Sort by score (descending)
        std::sort(mLeaderboard.begin(), mLeaderboard.end());

        // Keep only top entries
        if (mLeaderboard.size() > MAX_LEADERBOARD_ENTRIES) {
            mLeaderboard.resize(MAX_LEADERBOARD_ENTRIES);
        }

        saveLeaderboard();
    }

    /**
     * @brief Save leaderboard to file
     */
    void saveLeaderboard() {
        std::ofstream file(LEADERBOARD_FILE_PATH);
        if (file.is_open()) {
            for (const auto& entry : mLeaderboard) {
                file << entry.playerName << " " << entry.score << " "
                     << entry.date << "\n";
            }
            file.close();
            std::cout << "Leaderboard saved\n";
        } else {
            std::cerr << "ERROR::SCOREMANAGER::Could not save leaderboard\n";
        }
    }

    /**
     * @brief Check if current score qualifies for leaderboard
     */
    bool qualifiesForLeaderboard() const {
        if (mLeaderboard.size() < MAX_LEADERBOARD_ENTRIES) return true;

        return mCurrentScore > mLeaderboard.back().score;
    }

    // Getters
    unsigned getCurrentScore() const { return mCurrentScore; }
    unsigned getHighScore() const { return mHighScore; }
    unsigned getComboCount() const { return mComboCount; }
    float getComboMultiplier() const { return mComboMultiplier; }
    const std::vector<ScoreEntry>& getLeaderboard() const {
        return mLeaderboard;
    }

    /**
     * @brief Get formatted combo string for display
     */
    std::string getComboString() const {
        if (mComboCount < COMBO_THRESHOLD) return "";

        return "COMBO x" + std::to_string(static_cast<int>(mComboMultiplier));
    }

    /**
     * @brief Destructor
     */
    ~ScoreManager() { saveHighScore(); }
};
