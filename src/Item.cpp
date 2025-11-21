/*
 * Museum Escape - Item Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Item.h"

// Item Constructor
Item::Item(const std::string& itemName, const std::string& desc, float x, float y)
    : name(itemName), description(desc), position(x, y), isCollected(false) {
    sprite.setSize({20.0f, 20.0f});
    sprite.setPosition(position);
    sprite.setFillColor(sf::Color::Yellow);
}

std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
sf::Vector2f Item::getPosition() const { return position; }
bool Item::isItemCollected() const { return isCollected; }
sf::FloatRect Item::getBounds() const { return sprite.getGlobalBounds(); }

void Item::collect() { isCollected = true; }
void Item::draw(sf::RenderWindow& window) { 
    if (!isCollected) {
        window.draw(sprite); 
    }
}
bool Item::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

// Key Constructor
Key::Key(const std::string& keyName, const std::string& doorIdentifier, float x, float y)
    : Item(keyName, "A key to unlock doors", x, y), doorID(doorIdentifier) {
    sprite.setFillColor(sf::Color::Cyan); // Keys are cyan
}

void Key::use() {}
std::string Key::getDoorID() const { return doorID; }

// Passcode Constructor
Passcode::Passcode(const std::string& passcodeName, const std::string& codeValue, float x, float y)
    : Item(passcodeName, "A numeric passcode", x, y), code(codeValue) {
    sprite.setFillColor(sf::Color::Magenta); // Passcodes are magenta
}

void Passcode::use() {}
std::string Passcode::getCode() const { return code; }

// BasicItem Constructor
BasicItem::BasicItem(const std::string& itemName, const std::string& desc, float x, float y)
    : Item(itemName, desc, x, y) {
    sprite.setFillColor(sf::Color::White); // Basic items are white
}

void BasicItem::use() {
    // Does nothing - just a collectible
}

// Inventory Constructor
Inventory::Inventory(int capacity)
    : maxCapacity(capacity), isVisible(false), background({400.0f, 500.0f}) {
    background.setFillColor(sf::Color(0, 0, 0, 200));
    background.setOutlineThickness(3.0f);
    background.setOutlineColor(sf::Color::White);
    background.setPosition({200.0f, 50.0f});
}

bool Inventory::addItem(std::shared_ptr<Item> item) {
    if (items.size() < static_cast<size_t>(maxCapacity)) {
        items.push_back(item);
        return true;
    }
    return false;
}

bool Inventory::removeItem(const std::string& itemName) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if ((*it)->getName() == itemName) {
            items.erase(it);
            return true;
        }
    }
    return false;
}

bool Inventory::hasItem(const std::string& itemName) const {
    for (const auto& item : items) {
        if (item->getName() == itemName) return true;
    }
    return false;
}

std::shared_ptr<Item> Inventory::getItem(const std::string& itemName) {
    for (auto& item : items) {
        if (item->getName() == itemName) return item;
    }
    return nullptr;
}

int Inventory::getItemCount() const { return items.size(); }
int Inventory::getMaxCapacity() const { return maxCapacity; }
bool Inventory::isFull() const { return items.size() >= static_cast<size_t>(maxCapacity); }
std::vector<std::shared_ptr<Item>>& Inventory::getItems() { return items; }

void Inventory::toggleVisibility() { isVisible = !isVisible; }
void Inventory::setVisible(bool visible) { isVisible = visible; }
bool Inventory::getVisible() const { return isVisible; }
void Inventory::setFont(const sf::Font& f) { font = f; }

void Inventory::draw(sf::RenderWindow& window) {
    if (!isVisible) return;
    
    // Draw background
    window.draw(background);
    
    // Draw title
    sf::Text title(font);
    title.setString("INVENTORY (" + std::to_string(items.size()) + "/" + std::to_string(maxCapacity) + ")");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::White);
    title.setPosition({220.0f, 70.0f});
    window.draw(title);
    
    // Draw items
    float yOffset = 120.0f;
    for (size_t i = 0; i < items.size(); ++i) {
        // Item name
        sf::Text itemText(font);
        itemText.setString(std::to_string(i + 1) + ". " + items[i]->getName());
        itemText.setCharacterSize(20);
        itemText.setFillColor(sf::Color::Yellow);
        itemText.setPosition({230.0f, yOffset});
        window.draw(itemText);
        
        // Item description
        sf::Text descText(font);
        descText.setString("   " + items[i]->getDescription());
        descText.setCharacterSize(16);
        descText.setFillColor(sf::Color(200, 200, 200));
        descText.setPosition({230.0f, yOffset + 25.0f});
        window.draw(descText);
        
        yOffset += 60.0f;
    }
    
    // Empty inventory message
    if (items.empty()) {
        sf::Text emptyText(font);
        emptyText.setString("No items collected yet...");
        emptyText.setCharacterSize(18);
        emptyText.setFillColor(sf::Color(150, 150, 150));
        emptyText.setPosition({250.0f, 250.0f});
        window.draw(emptyText);
    }
}

void Inventory::clear() { items.clear(); }