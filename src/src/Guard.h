#ifndef GUARD_H
#define GUARD_H

#include <SFML/Graphics.hpp>
#include <vector>

class Player; // Forward declaration

class Guard {
private:
    sf::Vector2f position;
    sf::RectangleShape sprite;
    float speed;
    
    // Patrol system
    std::vector<sf::Vector2f> patrolPoints;
    int currentPatrolIndex;
    bool movingForward; // Direction along patrol path
    
    // Detection
    float detectionRadius;
    sf::CircleShape detectionCircle; // Visual representation
    bool hasDetectedPlayer;
    float detectionCooldown; // Cooldown timer to prevent spam
    float cooldownTime;
    
    // Room bounds (guard stays within room)
    sf::FloatRect roomBounds;
    
public:
    // Constructor
    Guard(float x, float y, float detectionRange = 100.0f);
    
    // Patrol management
    void addPatrolPoint(float x, float y);
    void setPatrolPoints(const std::vector<sf::Vector2f>& points);
    void patrol(float deltaTime);
    
    // Room bounds
    void setRoomBounds(const sf::FloatRect& bounds);
    
    // Detection and collision
    bool detectPlayer(const Player& player);
    bool checkCollision(const sf::FloatRect& bounds);
    sf::FloatRect getBounds() const;
    
    // Position
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    
    // Update and render
    void update(float deltaTime, const Player& player);
    void draw(sf::RenderWindow& window, bool showDetectionRadius = false);
    
    // AI behavior
    void moveTowards(const sf::Vector2f& target, float deltaTime);
    float distanceTo(const sf::Vector2f& point) const;
};

#endif // GUARD_H