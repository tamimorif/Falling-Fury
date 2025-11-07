/**
 * @file UI.h
 * @brief Modern UI system with buttons, panels, and menus
 *
 * Provides reusable UI components for professional menu systems
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "managers/ResourceManager.h"

/**
 * @brief Base UI Element class
 */
class UIElement {
   protected:
    sf::Vector2f mPosition;
    sf::Vector2f mSize;
    bool mVisible;
    bool mEnabled;

   public:
    UIElement() : mVisible(true), mEnabled(true) {}
    virtual ~UIElement() = default;

    virtual void update(const sf::Vector2f& mousePos, bool mousePressed) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual void setPosition(const sf::Vector2f& pos) { mPosition = pos; }
    virtual void setSize(const sf::Vector2f& size) { mSize = size; }
    virtual void setVisible(bool visible) { mVisible = visible; }
    virtual void setEnabled(bool enabled) { mEnabled = enabled; }

    bool isVisible() const { return mVisible; }
    bool isEnabled() const { return mEnabled; }
    const sf::Vector2f& getPosition() const { return mPosition; }
    const sf::Vector2f& getSize() const { return mSize; }
};

/**
 * @brief Button class with hover and click effects
 */
class Button : public UIElement {
   private:
    sf::RectangleShape mShape;
    sf::Text mText;

    sf::Color mNormalColor;
    sf::Color mHoverColor;
    sf::Color mPressedColor;
    sf::Color mDisabledColor;

    std::function<void()> mCallback;

    bool mIsHovered;
    bool mWasPressed;

   public:
    /**
     * @brief Constructor
     * @param text Button text
     * @param position Button position
     * @param size Button size
     * @param callback Function to call when clicked
     */
    Button(const std::string& text, const sf::Vector2f& position,
           const sf::Vector2f& size, std::function<void()> callback = nullptr)
        : mCallback(callback), mIsHovered(false), mWasPressed(false) {
        mPosition = position;
        mSize = size;

        // Setup shape
        mShape.setPosition(position);
        mShape.setSize(size);

        // Setup colors
        mNormalColor = sf::Color(70, 70, 70);
        mHoverColor = sf::Color(100, 100, 100);
        mPressedColor = sf::Color(50, 50, 50);
        mDisabledColor = sf::Color(40, 40, 40);
        mShape.setFillColor(mNormalColor);
        mShape.setOutlineThickness(2.f);
        mShape.setOutlineColor(sf::Color::White);

        // Setup text
        mText.setFont(ResourceManager::getInstance().getFont("main"));
        mText.setString(text);
        mText.setCharacterSize(24);
        mText.setFillColor(sf::Color::White);

        // Center text
        sf::FloatRect textBounds = mText.getLocalBounds();
        mText.setOrigin(textBounds.left + textBounds.width / 2.f,
                        textBounds.top + textBounds.height / 2.f);
        mText.setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f);
    }

    void update(const sf::Vector2f& mousePos, bool mousePressed) override {
        if (!mVisible || !mEnabled) {
            mShape.setFillColor(mDisabledColor);
            return;
        }

        // Check if mouse is over button
        sf::FloatRect bounds = mShape.getGlobalBounds();
        mIsHovered = bounds.contains(mousePos);

        if (mIsHovered) {
            if (mousePressed) {
                mShape.setFillColor(mPressedColor);
                mWasPressed = true;
            } else {
                mShape.setFillColor(mHoverColor);

                // Trigger callback on release
                if (mWasPressed && mCallback) {
                    mCallback();
                }
                mWasPressed = false;
            }
        } else {
            mShape.setFillColor(mNormalColor);
            mWasPressed = false;
        }
    }

    void render(sf::RenderWindow& window) override {
        if (!mVisible) return;

        window.draw(mShape);
        window.draw(mText);
    }

    void setText(const std::string& text) {
        mText.setString(text);
        sf::FloatRect textBounds = mText.getLocalBounds();
        mText.setOrigin(textBounds.left + textBounds.width / 2.f,
                        textBounds.top + textBounds.height / 2.f);
        mText.setPosition(mPosition.x + mSize.x / 2.f,
                          mPosition.y + mSize.y / 2.f);
    }

    void setCallback(std::function<void()> callback) { mCallback = callback; }

    void setColors(const sf::Color& normal, const sf::Color& hover,
                   const sf::Color& pressed) {
        mNormalColor = normal;
        mHoverColor = hover;
        mPressedColor = pressed;
    }
};

/**
 * @brief Panel/Container class
 */
class Panel : public UIElement {
   private:
    sf::RectangleShape mBackground;
    std::vector<std::shared_ptr<UIElement>> mChildren;
    sf::Color mBackgroundColor;

   public:
    /**
     * @brief Constructor
     * @param position Panel position
     * @param size Panel size
     * @param backgroundColor Background color
     */
    Panel(const sf::Vector2f& position, const sf::Vector2f& size,
          const sf::Color& backgroundColor = sf::Color(30, 30, 30, 200))
        : mBackgroundColor(backgroundColor) {
        mPosition = position;
        mSize = size;

        mBackground.setPosition(position);
        mBackground.setSize(size);
        mBackground.setFillColor(backgroundColor);
        mBackground.setOutlineThickness(3.f);
        mBackground.setOutlineColor(sf::Color(100, 100, 100));
    }

    void update(const sf::Vector2f& mousePos, bool mousePressed) override {
        if (!mVisible) return;

        for (auto& child : mChildren) {
            child->update(mousePos, mousePressed);
        }
    }

    void render(sf::RenderWindow& window) override {
        if (!mVisible) return;

        window.draw(mBackground);

        for (auto& child : mChildren) {
            child->render(window);
        }
    }

    void addChild(std::shared_ptr<UIElement> element) {
        mChildren.push_back(element);
    }

    void clearChildren() { mChildren.clear(); }
};

/**
 * @brief Label/Text display
 */
class Label : public UIElement {
   private:
    sf::Text mText;

   public:
    /**
     * @brief Constructor
     * @param text Label text
     * @param position Position
     * @param fontSize Font size
     * @param color Text color
     */
    Label(const std::string& text, const sf::Vector2f& position,
          unsigned int fontSize = 24,
          const sf::Color& color = sf::Color::White) {
        mPosition = position;

        mText.setFont(ResourceManager::getInstance().getFont("main"));
        mText.setString(text);
        mText.setCharacterSize(fontSize);
        mText.setFillColor(color);
        mText.setPosition(position);

        sf::FloatRect bounds = mText.getLocalBounds();
        mSize = sf::Vector2f(bounds.width, bounds.height);
    }

    void update(const sf::Vector2f& mousePos, bool mousePressed) override {}

    void render(sf::RenderWindow& window) override {
        if (!mVisible) return;
        window.draw(mText);
    }

    void setText(const std::string& text) {
        mText.setString(text);
        sf::FloatRect bounds = mText.getLocalBounds();
        mSize = sf::Vector2f(bounds.width, bounds.height);
    }

    void setColor(const sf::Color& color) { mText.setFillColor(color); }

    void setCharacterSize(unsigned int size) { mText.setCharacterSize(size); }

    void centerOnPosition() {
        sf::FloatRect bounds = mText.getLocalBounds();
        mText.setOrigin(bounds.left + bounds.width / 2.f,
                        bounds.top + bounds.height / 2.f);
    }
};

/**
 * @brief Slider for value adjustment
 */
class Slider : public UIElement {
   private:
    sf::RectangleShape mTrack;
    sf::CircleShape mHandle;
    sf::Text mLabel;

    float mValue;  // 0.0 to 1.0
    float mMin;
    float mMax;
    bool mDragging;

    std::function<void(float)> mCallback;

   public:
    /**
     * @brief Constructor
     * @param label Slider label
     * @param position Position
     * @param width Slider width
     * @param min Minimum value
     * @param max Maximum value
     * @param initialValue Initial value
     */
    Slider(const std::string& label, const sf::Vector2f& position, float width,
           float min, float max, float initialValue = 0.5f)
        : mMin(min), mMax(max), mDragging(false) {
        mPosition = position;
        mSize = sf::Vector2f(width, 30.f);
        mValue = (initialValue - min) / (max - min);

        // Setup track
        mTrack.setPosition(position.x, position.y + 20.f);
        mTrack.setSize(sf::Vector2f(width, 5.f));
        mTrack.setFillColor(sf::Color(100, 100, 100));

        // Setup handle
        mHandle.setRadius(10.f);
        mHandle.setOrigin(10.f, 10.f);
        mHandle.setFillColor(sf::Color::White);
        updateHandlePosition();

        // Setup label
        mLabel.setFont(ResourceManager::getInstance().getFont("main"));
        mLabel.setString(label);
        mLabel.setCharacterSize(18);
        mLabel.setFillColor(sf::Color::White);
        mLabel.setPosition(position);
    }

    void update(const sf::Vector2f& mousePos, bool mousePressed) override {
        if (!mVisible || !mEnabled) return;

        sf::FloatRect handleBounds = mHandle.getGlobalBounds();

        if (mousePressed && handleBounds.contains(mousePos)) {
            mDragging = true;
        }

        if (!mousePressed) {
            mDragging = false;
        }

        if (mDragging) {
            // Update value based on mouse X position
            float relativeX = mousePos.x - mPosition.x;
            mValue = std::max(0.f, std::min(1.f, relativeX / mSize.x));
            updateHandlePosition();

            if (mCallback) {
                mCallback(getValue());
            }
        }
    }

    void render(sf::RenderWindow& window) override {
        if (!mVisible) return;

        window.draw(mLabel);
        window.draw(mTrack);
        window.draw(mHandle);
    }

    float getValue() const { return mMin + mValue * (mMax - mMin); }

    void setValue(float value) {
        mValue = (value - mMin) / (mMax - mMin);
        mValue = std::max(0.f, std::min(1.f, mValue));
        updateHandlePosition();
    }

    void setCallback(std::function<void(float)> callback) {
        mCallback = callback;
    }

   private:
    void updateHandlePosition() {
        float x = mPosition.x + mValue * mSize.x;
        float y = mPosition.y + 22.5f;
        mHandle.setPosition(x, y);
    }
};
