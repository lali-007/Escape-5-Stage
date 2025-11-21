#ifndef ITEM_H
#define ITEM_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

// Base Item class
class Item {
protected:
    std::string name;
    std::string description;
    sf::Vector2f position;
    sf::RectangleShape sprite;
    bool isCollected;
    
public:
    // Constructor
    Item(const std::string& itemName, const std::string& desc, float x, float y);
    virtual ~Item() = default;
    
    // Getters
    std::string getName() const;
    std::string getDescription() const;
    sf::Vector2f getPosition() const;
    bool isItemCollected() const;
    sf::FloatRect getBounds() const;
    
    // Actions
    void collect();
    virtual void use() = 0; // Pure virtual - each item type has unique use
    
    // Rendering
    void draw(sf::RenderWindow& window);
    
    // Collision
    bool checkCollision(const sf::FloatRect& bounds);
};

// Key item - Unlocks doors
class Key : public Item {
private:
    std::string doorID; // Which door this key unlocks
    
public:
    Key(const std::string& keyName, const std::string& doorIdentifier, float x, float y);
    
    void use() override;
    std::string getDoorID() const;
};

// Passcode item - Used for digital locks
class Passcode : public Item {
private:
    std::string code; // The numeric code
    
public:
    Passcode(const std::string& passcodeName, const std::string& codeValue, float x, float y);
    
    void use() override;
    std::string getCode() const;
};

// BasicItem - Simple collectible with no special use
class BasicItem : public Item {
public:
    BasicItem(const std::string& itemName, const std::string& desc, float x, float y);
    
    void use() override; // Does nothing, just for collection
};

// Inventory class - Manages player's collected items
class Inventory {
private:
    std::vector<std::shared_ptr<Item>> items;
    int maxCapacity;
    sf::Font font;
    sf::RectangleShape background;
    bool isVisible;
    
public:
    // Constructor
    Inventory(int capacity = 10);
    
    // Item management
    bool addItem(std::shared_ptr<Item> item);
    bool removeItem(const std::string& itemName);
    bool hasItem(const std::string& itemName) const;
    std::shared_ptr<Item> getItem(const std::string& itemName);
    
    // Inventory properties
    int getItemCount() const;
    int getMaxCapacity() const;
    bool isFull() const;
    std::vector<std::shared_ptr<Item>>& getItems();
    
    // Display
    void toggleVisibility();
    void setVisible(bool visible);
    bool getVisible() const;
    void setFont(const sf::Font& f);
    
    // Rendering
    void draw(sf::RenderWindow& window);
    
    // Clear inventory
    void clear();
};

#endif // ITEM_H