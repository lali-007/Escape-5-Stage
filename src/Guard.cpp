/*
 * Museum Escape - Guard Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Guard.h"
#include "Player.h"
#include <cmath>

// Constructor - CHANGED to use Texture
Guard::Guard(float x, float y, float detectionRange, const sf::Texture& texture)
    : position(x, y),
      sprite(texture), // <--- FIXED: Initialize sprite with texture here
      speed(80.0f),
      currentPatrolIndex(0),
      movingForward(true),
      detectionRadius(detectionRange),
      hasDetectedPlayer(false),
      detectionCooldown(0.0f),
      cooldownTime(2.0f),
      detectionCircle(detectionRange) 
{
    sprite.setPosition(position);
    sprite.setColor(sf::Color(255, 200, 200)); 
    
    detectionCircle.setFillColor(sf::Color(255, 0, 0, 30));
    detectionCircle.setOutlineThickness(1.0f);
    detectionCircle.setOutlineColor(sf::Color(255, 0, 0, 100));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    detectionCircle.setOrigin({detectionRange - bounds.size.x/2.0f, detectionRange - bounds.size.y/2.0f});
    detectionCircle.setPosition(position);
    sprite.setScale({0.05f, 0.05f});
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
    
    if (patrolPoints.size() == 1) {
        position = patrolPoints[0];
        sprite.setPosition(position);
        detectionCircle.setPosition(position);
        return;
    }
    
    sf::Vector2f target = patrolPoints[currentPatrolIndex];
    
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 5.0f) {
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
        float normalizedX = dx / distance;
        float normalizedY = dy / distance;
        
        position.x += normalizedX * speed * deltaTime;
        position.y += normalizedY * speed * deltaTime;
        
        sprite.setPosition(position);
        detectionCircle.setPosition(position);
    }
}

void Guard::setRoomBounds(const sf::FloatRect& bounds) {
    roomBounds = bounds;
}

bool Guard::detectPlayer(const Player& player) {
    if (detectionCooldown > 0) return false;
    
    float distance = distanceTo(player.getPosition());
    
    if (distance < detectionRadius) {
        hasDetectedPlayer = true;
        detectionCooldown = cooldownTime;
        return true;
    }
    
    hasDetectedPlayer = false;
    return false;
}

bool Guard::checkCollision(const sf::FloatRect& bounds) {
    return sprite.getGlobalBounds().findIntersection(bounds).has_value();
}

sf::FloatRect Guard::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Vector2f Guard::getPosition() const {
    return position;
}

void Guard::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
    detectionCircle.setPosition(position);
}

void Guard::update(float deltaTime, const Player& player) {
    if (detectionCooldown > 0) {
        detectionCooldown -= deltaTime;
    }
    patrol(deltaTime);
}

void Guard::draw(sf::RenderWindow& window, bool showDetectionRadius) {
    if (showDetectionRadius) {
        window.draw(detectionCircle);
    }
    window.draw(sprite);
}

float Guard::distanceTo(const sf::Vector2f& point) const {
    float dx = point.x - position.x;
    float dy = point.y - position.y;
    return std::sqrt(dx * dx + dy * dy);
}