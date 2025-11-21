#ifndef TIMER_H
#define TIMER_H

#include <SFML/Graphics.hpp>
#include <string>

class Timer {
private:
    float totalTime; // Total time in seconds
    float remainingTime;
    bool isRunning;
    bool hasExpired;
    
    // Display
    sf::Font defaultFont; // Default font for initialization
    sf::Font font;
    sf::Text timerText;
    sf::RectangleShape background;
    sf::Vector2f displayPosition;
    
    // Warning colors
    sf::Color normalColor;
    sf::Color warningColor; // When time is low
    sf::Color criticalColor; // When time is very low
    float warningThreshold; // Show warning below this time (e.g., 60 seconds)
    float criticalThreshold; // Show critical below this time (e.g., 30 seconds)
    
public:
    // Constructor
    Timer(float totalSeconds = 600.0f); // Default 10 minutes
    
    // Timer controls
    void start();
    void pause();
    void resume();
    void reset();
    void stop();
    
    // Time management
    void update(float deltaTime);
    void addTime(float seconds); // Bonus time for solving puzzles
    void subtractTime(float seconds); // Penalty for failing
    
    // Time queries
    float getRemainingTime() const;
    float getTotalTime() const;
    bool isTimerRunning() const;
    bool isExpired() const;
    
    // Display formatting
    std::string getFormattedTime() const; // Returns "MM:SS" format
    void setDisplayPosition(float x, float y);
    void setFont(const sf::Font& f);
    
    // Thresholds
    void setWarningThreshold(float seconds);
    void setCriticalThreshold(float seconds);
    
    // Rendering
    void draw(sf::RenderWindow& window);
};

#endif // TIMER_H