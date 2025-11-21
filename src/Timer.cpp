/*
 * Museum Escape - Timer Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Timer.h"
#include <sstream>
#include <iomanip>

// Constructor
Timer::Timer(float totalSeconds)
    : totalTime(totalSeconds),
      remainingTime(totalSeconds),
      isRunning(false),
      hasExpired(false),
      normalColor(sf::Color::White),
      warningColor(sf::Color::Yellow),
      criticalColor(sf::Color::Red),
      warningThreshold(60.0f),
      criticalThreshold(30.0f),
      displayPosition(10.0f, 10.0f),
      timerText(defaultFont),
      background({200.0f, 50.0f})
{
    // Setup background
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);
    background.setPosition(displayPosition);
    
    // Setup text (will set font later)
    timerText.setCharacterSize(24);
    timerText.setFillColor(normalColor);
    timerText.setPosition({displayPosition.x + 10.0f, displayPosition.y + 10.0f});
}

// Start timer
void Timer::start() {
    isRunning = true;
    hasExpired = false;
}

// Pause timer
void Timer::pause() {
    isRunning = false;
}

// Resume timer
void Timer::resume() {
    if (!hasExpired) {
        isRunning = true;
    }
}

// Reset timer
void Timer::reset() {
    remainingTime = totalTime;
    isRunning = false;
    hasExpired = false;
}

// Stop timer
void Timer::stop() {
    isRunning = false;
}

// Update timer
void Timer::update(float deltaTime) {
    if (!isRunning || hasExpired) {
        return;
    }
    
    // Decrease time
    remainingTime -= deltaTime;
    
    // Check if expired
    if (remainingTime <= 0.0f) {
        remainingTime = 0.0f;
        hasExpired = true;
        isRunning = false;
    }
    
    // Update text color based on remaining time
    if (remainingTime <= criticalThreshold) {
        timerText.setFillColor(criticalColor);
    } else if (remainingTime <= warningThreshold) {
        timerText.setFillColor(warningColor);
    } else {
        timerText.setFillColor(normalColor);
    }
    
    // Update text
    timerText.setString("Time: " + getFormattedTime());
}

// Add time (bonus)
void Timer::addTime(float seconds) {
    remainingTime += seconds;
    if (remainingTime > totalTime) {
        remainingTime = totalTime;
    }
}

// Subtract time (penalty)
void Timer::subtractTime(float seconds) {
    remainingTime -= seconds;
    if (remainingTime < 0.0f) {
        remainingTime = 0.0f;
        hasExpired = true;
        isRunning = false;
    }
}

// Get remaining time
float Timer::getRemainingTime() const {
    return remainingTime;
}

// Get total time
float Timer::getTotalTime() const {
    return totalTime;
}

// Check if running
bool Timer::isTimerRunning() const {
    return isRunning;
}

// Check if expired
bool Timer::isExpired() const {
    return hasExpired;
}

// Get formatted time string (MM:SS)
std::string Timer::getFormattedTime() const {
    int minutes = static_cast<int>(remainingTime) / 60;
    int seconds = static_cast<int>(remainingTime) % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    
    return oss.str();
}

// Set display position
void Timer::setDisplayPosition(float x, float y) {
    displayPosition = {x, y};
    background.setPosition(displayPosition);
    timerText.setPosition({displayPosition.x + 10.0f, displayPosition.y + 10.0f});
}

// Set font
void Timer::setFont(const sf::Font& f) {
    timerText.setFont(f);
}

// Set warning threshold
void Timer::setWarningThreshold(float seconds) {
    warningThreshold = seconds;
}

// Set critical threshold
void Timer::setCriticalThreshold(float seconds) {
    criticalThreshold = seconds;
}

// Draw timer
void Timer::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(timerText);
}