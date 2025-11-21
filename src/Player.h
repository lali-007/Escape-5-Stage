#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Item; // Forward declaration
class Room; // Forward declaration

class Player {
private:
    sf::Vector2f position;
    sf::RectangleShape sprite;
    float speed;
    int health;
    bool isWarned; // True if caught by guard once
    std::vector<Item*> inventory;
    
public:
    // Constructor
    Player(float x, float y);
    
    // Movement
    void move(float dx, float dy);
    void handleInput(float deltaTime);
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    
    // Collision
    bool checkCollision(const sf::FloatRect& bounds);
    sf::FloatRect getBounds() const;
    
    // Inventory management
    void addItem(Item* item);
    bool hasItem(const std::string& itemName);
    void removeItem(const std::string& itemName);
    std::vector<Item*>& getInventory();
    
    // Health/Warning system
    void warn();
    bool isPlayerWarned() const;
    void resetWarning();
    
    // Rendering
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);
};

#endif // PLAYER_H