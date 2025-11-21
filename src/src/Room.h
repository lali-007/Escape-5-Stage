#ifndef ROOM_H
#define ROOM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

class Puzzle;
class Item;
class Guard;
class Door;

class Room {
private:
    int roomID;
    std::string roomName;
    sf::Vector2f position; // Position in world coordinates
    sf::Vector2f size;
    sf::RectangleShape background;
    
    std::vector<std::shared_ptr<Puzzle>> puzzles;
    std::vector<std::shared_ptr<Item>> items;
    std::vector<std::shared_ptr<Guard>> guards;
    std::vector<std::shared_ptr<Door>> doors;
    
    bool isExitRoom;
    bool isVisited;
    
public:
    // Constructor
    Room(int id, const std::string& name, float x, float y, float width, float height);
    
    // Puzzle management
    void addPuzzle(std::shared_ptr<Puzzle> puzzle);
    std::vector<std::shared_ptr<Puzzle>>& getPuzzles();
    bool allPuzzlesSolved() const;
    
    // Item management
    void addItem(std::shared_ptr<Item> item);
    void removeItem(std::shared_ptr<Item> item);
    std::vector<std::shared_ptr<Item>>& getItems();
    
    // Guard management
    void addGuard(std::shared_ptr<Guard> guard);
    std::vector<std::shared_ptr<Guard>>& getGuards();
    
    // Door management
    void addDoor(std::shared_ptr<Door> door);
    std::vector<std::shared_ptr<Door>>& getDoors();
    
    // Room properties
    int getRoomID() const;
    std::string getRoomName() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    sf::FloatRect getBounds() const;
    
    void setExitRoom(bool isExit);
    bool isExit() const;
    
    void setVisited(bool visited);
    bool hasBeenVisited() const;
    
    // Update and render
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    // Collision check
    bool containsPoint(const sf::Vector2f& point) const;
};

// Door class - Connects rooms
class Door {
private:
    sf::Vector2f position;
    sf::RectangleShape sprite;
    int targetRoomID;
    bool isLocked;
    std::string requiredKey; // Key name required to unlock
    
public:
    Door(float x, float y, int targetRoom, bool locked = false, const std::string& keyName = "");
    
    void unlock();
    bool canOpen(const std::string& keyName);
    bool checkCollision(const sf::FloatRect& bounds);
    
    int getTargetRoomID() const;
    bool getLockedStatus() const;
    sf::FloatRect getBounds() const;
    
    void draw(sf::RenderWindow& window);
};

#endif // ROOM_H