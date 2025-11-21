/*
 * Museum Escape - Room Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Room.h"
#include "Puzzle.h"
#include "Item.h"
#include "Guard.h"

// Room Constructor
Room::Room(int id, const std::string& name, float x, float y, float width, float height)
    : roomID(id),
      roomName(name),
      position(x, y),
      size(width, height),
      isExitRoom(false),
      isVisited(false)
{
    // Setup room background
    background.setSize({width, height});
    background.setPosition(position);
    background.setFillColor(sf::Color(40, 40, 50)); // Dark blue-gray
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);
}

// Add puzzle to room
void Room::addPuzzle(std::shared_ptr<Puzzle> puzzle) {
    puzzles.push_back(puzzle);
}

// Get puzzles in room
std::vector<std::shared_ptr<Puzzle>>& Room::getPuzzles() {
    return puzzles;
}

// Check if all puzzles solved
bool Room::allPuzzlesSolved() const {
    for (const auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            return false;
        }
    }
    return true;
}

// Add item to room
void Room::addItem(std::shared_ptr<Item> item) {
    items.push_back(item);
}

// Remove item from room
void Room::removeItem(std::shared_ptr<Item> item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == item) {
            items.erase(it);
            return;
        }
    }
}

// Get items in room
std::vector<std::shared_ptr<Item>>& Room::getItems() {
    return items;
}

// Add guard to room
void Room::addGuard(std::shared_ptr<Guard> guard) {
    guards.push_back(guard);
}

// Get guards in room
std::vector<std::shared_ptr<Guard>>& Room::getGuards() {
    return guards;
}

// Add door to room
void Room::addDoor(std::shared_ptr<Door> door) {
    doors.push_back(door);
}

// Get doors in room
std::vector<std::shared_ptr<Door>>& Room::getDoors() {
    return doors;
}

// Get room ID
int Room::getRoomID() const {
    return roomID;
}

// Get room name
std::string Room::getRoomName() const {
    return roomName;
}

// Get room position
sf::Vector2f Room::getPosition() const {
    return position;
}

// Get room size
sf::Vector2f Room::getSize() const {
    return size;
}

// Get room bounds
sf::FloatRect Room::getBounds() const {
    return background.getGlobalBounds();
}

// Set if room is exit
void Room::setExitRoom(bool isExit) {
    isExitRoom = isExit;
}

// Check if room is exit
bool Room::isExit() const {
    return isExitRoom;
}

// Set visited status
void Room::setVisited(bool visited) {
    isVisited = visited;
}

// Check if visited
bool Room::hasBeenVisited() const {
    return isVisited;
}

// Update room (guards, puzzles, etc.)
void Room::update(float deltaTime) {
    // Note: Guards need player reference, this will be called from Game::updatePlaying()
    // where it has access to the player
    
    // Update puzzles
    for (auto& puzzle : puzzles) {
        puzzle->update(deltaTime);
    }
}

// Draw room
void Room::draw(sf::RenderWindow& window) {
    // Draw background
    window.draw(background);
    
    // Draw guards (with detection radius visible)
    for (auto& guard : guards) {
        guard->draw(window, true);  // true = show detection radius
    }
    
    // Draw doors
    for (auto& door : doors) {
        door->draw(window);
    }
    
    // Draw items (not collected)
    for (auto& item : items) {
        if (!item->isItemCollected()) {
            item->draw(window);
        }
    }
}

// Check if point is in room
bool Room::containsPoint(const sf::Vector2f& point) const {
    return background.getGlobalBounds().contains(point);
}

// ============================================================================
// Door Class Implementation
// ============================================================================

// Door Constructor
Door::Door(float x, float y, int targetRoom, bool locked, const std::string& keyName)
    : position(x, y),
      targetRoomID(targetRoom),
      isLocked(locked),
      requiredKey(keyName)
{
    // Setup door sprite
    sprite.setSize({30.0f, 60.0f});
    sprite.setPosition(position);
    
    // Color based on lock status
    if (isLocked) {
        sprite.setFillColor(sf::Color::Red);
    } else {
        sprite.setFillColor(sf::Color(100, 100, 100)); // Gray
    }
    
    sprite.setOutlineThickness(2.0f);
    sprite.setOutlineColor(sf::Color::White);
}

// Unlock door
void Door::unlock() {
    isLocked = false;
    sprite.setFillColor(sf::Color(100, 100, 100)); // Change to gray when unlocked
}

// Check if can open with key
bool Door::canOpen(const std::string& keyName) {
    if (!isLocked) {
        return true;
    }
    
    // Check if the key matches
    if (keyName == requiredKey || requiredKey.empty()) {
        unlock();
        return true;
    }
    
    return false;
}

// Check collision with door
bool Door::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

// Get target room ID
int Door::getTargetRoomID() const {
    return targetRoomID;
}

// Get locked status
bool Door::getLockedStatus() const {
    return isLocked;
}

// Get door bounds
sf::FloatRect Door::getBounds() const {
    return sprite.getGlobalBounds();
}

// Draw door
void Door::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}