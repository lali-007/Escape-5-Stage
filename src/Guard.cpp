/*
 * Museum Escape - Guard Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Guard.h"
#include "Player.h"
#include <cmath>

// Guard Constructor
Guard::Guard(float x, float y, float detectionRange)
    : position(x, y),
      speed(80.0f),
      currentPatrolIndex(0),
      movingForward(true),
      detectionRadius(detectionRange),
      hasDetectedPlayer(false),
      detectionCooldown(0.0f),
      cooldownTime(2.0f),
      detectionCircle(detectionRange) {
    
    // Setup guard sprite (red square)
    sprite.setSize({30.0f, 30.0f});
    sprite.setPosition(position);
    sprite.setFillColor(sf::Color::Red);
    sprite.setOutlineThickness(2.0f);
    sprite.setOutlineColor(sf::Color::White);
    
    // Setup detection circle (semi-transparent red)
    detectionCircle.setFillColor(sf::Color(255, 0, 0, 30));
    detectionCircle.setOutlineThickness(1.0f);
    detectionCircle.setOutlineColor(sf::Color(255, 0, 0, 100));
    detectionCircle.setOrigin({detectionRange, detectionRange});
    detectionCircle.setPosition(position);
}

// Add patrol point
void Guard::addPatrolPoint(float x, float y) {
    patrolPoints.push_back({x, y});
}

// Set all patrol points at once
void Guard::setPatrolPoints(const std::vector<sf::Vector2f>& points) {
    patrolPoints = points;
    currentPatrolIndex = 0;
}

// Patrol between waypoints
void Guard::patrol(float deltaTime) {
    if (patrolPoints.empty()) return;
    
    // If only one point, just stay there
    if (patrolPoints.size() == 1) {
        position = patrolPoints[0];
        sprite.setPosition(position);
        detectionCircle.setPosition(position);
        return;
    }
    
    // Get current target
    sf::Vector2f target = patrolPoints[currentPatrolIndex];
    
    // Calculate direction to target
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // If close enough to target, move to next waypoint
    if (distance < 5.0f) {
        // Reached waypoint, move to next
        if (movingForward) {
            currentPatrolIndex++;
            if (currentPatrolIndex >= patrolPoints.size()) {
                currentPatrolIndex = patrolPoints.size() - 2;
                movingForward = false;
            }
        } else {
            currentPatrolIndex--;
            if (currentPatrolIndex < 0) {
                currentPatrolIndex = 1;
                movingForward = true;
            }
        }
    } else {
        // Move towards target
        float normalizedX = dx / distance;
        float normalizedY = dy / distance;
        
        position.x += normalizedX * speed * deltaTime;
        position.y += normalizedY * speed * deltaTime;
        
        sprite.setPosition(position);
        detectionCircle.setPosition(position);
    }
}

// Set room boundaries
void Guard::setRoomBounds(const sf::FloatRect& bounds) {
    roomBounds = bounds;
}

// Detect if player is in range
bool Guard::detectPlayer(const Player& player) {
    // Update cooldown
    if (detectionCooldown > 0) {
        return false; // Still in cooldown, don't detect
    }
    
    float distance = distanceTo(player.getPosition());
    
    if (distance < detectionRadius) {
        // Player detected! Start cooldown
        hasDetectedPlayer = true;
        detectionCooldown = cooldownTime;
        return true;
    }
    
    hasDetectedPlayer = false;
    return false;
}

// Check collision with guard body
bool Guard::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

// Get guard bounds
sf::FloatRect Guard::getBounds() const {
    return sprite.getGlobalBounds();
}

// Get position
sf::Vector2f Guard::getPosition() const {
    return position;
}

// Set position
void Guard::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
    detectionCircle.setPosition(position);
}

// Update guard AI
void Guard::update(float deltaTime, const Player& player) {
    // Update cooldown timer
    if (detectionCooldown > 0) {
        detectionCooldown -= deltaTime;
    }
    
    // Patrol movement
    patrol(deltaTime);
    
    // Don't call detectPlayer here - let Game call it in checkGuardDetection()
    // This way the detection happens once per frame in the right order
}

// Draw guard
void Guard::draw(sf::RenderWindow& window, bool showDetectionRadius) {
    // Draw detection radius first (behind guard)
    if (showDetectionRadius) {
        window.draw(detectionCircle);
    }
    
    // Draw guard sprite
    window.draw(sprite);
}

// Move towards target
void Guard::moveTowards(const sf::Vector2f& target, float deltaTime) {
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance > 1.0f) {
        float normalizedX = dx / distance;
        float normalizedY = dy / distance;
        
        position.x += normalizedX * speed * deltaTime;
        position.y += normalizedY * speed * deltaTime;
        
        sprite.setPosition(position);
        detectionCircle.setPosition(position);
    }
}

// Calculate distance to point
float Guard::distanceTo(const sf::Vector2f& point) const {
    float dx = point.x - position.x;
    float dy = point.y - position.y;
    return std::sqrt(dx * dx + dy * dy);
}