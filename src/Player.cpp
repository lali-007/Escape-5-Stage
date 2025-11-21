/*
 * Museum Escape - Player Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Player.h"
#include "Item.h"
#include <SFML/Window/Keyboard.hpp>

// Constructor
Player::Player(float x, float y) 
    : position(x, y),
      speed(200.0f),
      health(100),
      isWarned(false)
{
    // Setup player sprite (simple rectangle for now)
    sprite.setSize({40.0f, 40.0f});
    sprite.setFillColor(sf::Color::Green);
    sprite.setPosition(position);
}

// Move player by delta amounts
void Player::move(float dx, float dy) {
    position.x += dx;
    position.y += dy;
    sprite.setPosition(position);
}

// Handle keyboard input for movement
void Player::handleInput(float deltaTime) {
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    // Check WASD keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        moveY -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        moveY += speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        moveX -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        moveX += speed * deltaTime;
    }
    
    // Apply movement
    if (moveX != 0.0f || moveY != 0.0f) {
        move(moveX, moveY);
    }
}

// Set player position
void Player::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    sprite.setPosition(position);
}

// Get player position
sf::Vector2f Player::getPosition() const {
    return position;
}

// Check collision with bounds
bool Player::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

// Get player bounding box
sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

// Add item to inventory
void Player::addItem(Item* item) {
    inventory.push_back(item);
}

// Check if player has specific item
bool Player::hasItem(const std::string& itemName) {
    for (const auto& item : inventory) {
        if (item->getName() == itemName) {
            return true;
        }
    }
    return false;
}

// Remove item from inventory
void Player::removeItem(const std::string& itemName) {
    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if ((*it)->getName() == itemName) {
            inventory.erase(it);
            return;
        }
    }
}

// Get inventory reference
std::vector<Item*>& Player::getInventory() {
    return inventory;
}

// Warn player (caught by guard once)
void Player::warn() {
    isWarned = true;
}

// Check if player is warned
bool Player::isPlayerWarned() const {
    return isWarned;
}

// Reset warning
void Player::resetWarning() {
    isWarned = false;
}

// Draw player
void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Update player (for animations, etc.)
void Player::update(float deltaTime) {
    // Future: Add animations, sprite updates, etc.
    // For now, just update sprite position
    sprite.setPosition(position);
}