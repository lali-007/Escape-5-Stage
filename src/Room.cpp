/*
 * Museum Escape - Room Class Implementation
 * CS/CE 224/272 - Fall 2025
 * SFML 3.0 COMPATIBLE
 */

#include "Room.h"
#include "Puzzle.h"
#include "Item.h"
#include "Guard.h"

// --- CHANGED: Constructor implementation to load image ---
Room::Room(int id, const std::string& name, float x, float y, float width, float height, const std::string& imagePath)
    : roomID(id),
      roomName(name),
      position(x, y),
      size(width, height),
      bgSprite(bgTexture), // <--- FIXED: Initialize sprite with the texture member here
      isExitRoom(false),
      isVisited(false)
{
    // Attempt to load background texture
    if (!bgTexture.loadFromFile(imagePath)) {
        // Fallback if image missing: Create a colored background
        sf::Image img;
        // SFML 3.0: Use resize() instead of create()
        img.resize({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}, sf::Color(40, 40, 50));
        
        // Check result to fix [[nodiscard]] warning
        if (!bgTexture.loadFromImage(img)) {
            std::cerr << "Error: Failed to create fallback texture for room " << id << std::endl;
        }
        std::cout << "Warning: Could not load " << imagePath << ". Using default color." << std::endl;
    }
    
    // We don't need setTextur(bgTexture) because we did it in the initializer list.
    bgSprite.setPosition(position);
    
    // Scale sprite to fit the room dimensions if image size differs
    sf::Vector2u texSize = bgTexture.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        bgSprite.setScale({width / texSize.x, height / texSize.y});
    }
}

void Room::addPuzzle(std::shared_ptr<Puzzle> puzzle) { puzzles.push_back(puzzle); }
std::vector<std::shared_ptr<Puzzle>>& Room::getPuzzles() { return puzzles; }

bool Room::allPuzzlesSolved() const {
    for (const auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) return false;
    }
    return true;
}

void Room::addItem(std::shared_ptr<Item> item) { items.push_back(item); }
void Room::removeItem(std::shared_ptr<Item> item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == item) {
            items.erase(it);
            return;
        }
    }
}
std::vector<std::shared_ptr<Item>>& Room::getItems() { return items; }

void Room::addGuard(std::shared_ptr<Guard> guard) { guards.push_back(guard); }
std::vector<std::shared_ptr<Guard>>& Room::getGuards() { return guards; }

void Room::addDoor(std::shared_ptr<Door> door) { doors.push_back(door); }
std::vector<std::shared_ptr<Door>>& Room::getDoors() { return doors; }

int Room::getRoomID() const { return roomID; }
std::string Room::getRoomName() const { return roomName; }
sf::Vector2f Room::getPosition() const { return position; }
sf::Vector2f Room::getSize() const { return size; }
sf::FloatRect Room::getBounds() const { return bgSprite.getGlobalBounds(); }

void Room::setExitRoom(bool isExit) { isExitRoom = isExit; }
bool Room::isExit() const { return isExitRoom; }
void Room::setVisited(bool visited) { isVisited = visited; }
bool Room::hasBeenVisited() const { return isVisited; }

void Room::update(float deltaTime) {
    for (auto& puzzle : puzzles) {
        puzzle->update(deltaTime);
    }
}

void Room::draw(sf::RenderWindow& window) {
    // --- CHANGED: Draw Sprite instead of Rectangle ---
    window.draw(bgSprite);
    
    for (auto& guard : guards) guard->draw(window, true);
    for (auto& door : doors) door->draw(window);
    for (auto& item : items) {
        if (!item->isItemCollected()) item->draw(window);
    }
}

bool Room::containsPoint(const sf::Vector2f& point) const {
    return bgSprite.getGlobalBounds().contains(point);
}

// ============================================================================
// Door Class Implementation
// ============================================================================

Door::Door(float x, float y, int targetRoom, bool locked, const std::string& keyName)
    : position(x, y),
      targetRoomID(targetRoom),
      isLocked(locked),
      requiredKey(keyName)
{
    sprite.setSize({30.0f, 60.0f});
    sprite.setPosition(position);
    
    if (isLocked) sprite.setFillColor(sf::Color::Red);
    else sprite.setFillColor(sf::Color(100, 100, 100)); // Gray
    
    sprite.setOutlineThickness(2.0f);
    sprite.setOutlineColor(sf::Color::White);
}

void Door::unlock() {
    isLocked = false;
    sprite.setFillColor(sf::Color::Blue);
}

bool Door::canOpen(const std::string& keyName) {
    if (!isLocked) return true;
    if (keyName == requiredKey || requiredKey.empty()) {
        unlock();
        return true;
    }
    return false;
}

bool Door::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

int Door::getTargetRoomID() const { return targetRoomID; }
bool Door::getLockedStatus() const { return isLocked; }
sf::FloatRect Door::getBounds() const { return sprite.getGlobalBounds(); }

// --- NEW IMPLEMENTATIONS ---
std::string Door::getRequiredKey() const { return requiredKey; }
void Door::setColor(const sf::Color& color) { sprite.setFillColor(color); }

void Door::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}