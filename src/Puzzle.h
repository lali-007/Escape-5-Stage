#ifndef PUZZLE_H
#define PUZZLE_H

#include <SFML/Graphics.hpp>
#include <string>

// Abstract base class for all puzzles
class Puzzle {
protected:
    bool isSolved;
    std::string description;
    std::string hint;
    int timeBonus; // Time bonus for solving
    int timePenalty; // Time penalty for failing
    
public:
    // Constructor
    Puzzle(const std::string& desc, const std::string& hintText, int bonus = 30, int penalty = 10);
    
    // Virtual destructor for proper cleanup
    virtual ~Puzzle() = default;
    
    // Pure virtual functions (must be implemented by derived classes)
    virtual bool solve(const std::string& answer) = 0;
    virtual void display(sf::RenderWindow& window) = 0;
    virtual void handleInput(sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    
    // Common functions
    bool isSolvedStatus() const;
    std::string getDescription() const;
    std::string getHint() const;
    int getTimeBonus() const;
    int getTimePenalty() const;
    void setSolved(bool status);
};

// Riddle Puzzle - Answer a logic riddle
class RiddlePuzzle : public Puzzle {
private:
    std::string riddle;
    std::string correctAnswer;
    std::string userAnswer;
    sf::Font defaultFont;
    sf::Font font;
    sf::Text riddleText;
    sf::Text inputText;
    bool showFeedback;
    std::string feedbackMessage;
    
public:
    RiddlePuzzle(const std::string& riddleText, const std::string& answer);
    
    bool solve(const std::string& answer) override;
    void display(sf::RenderWindow& window) override;
    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    
    void setFont(const sf::Font& f);
};

// Pattern Puzzle - Replicate a pattern using switches
class PatternPuzzle : public Puzzle {
private:
    std::vector<int> correctPattern; // e.g., {1, 3, 2, 4}
    std::vector<int> playerPattern;
    std::vector<sf::RectangleShape> switches;
    sf::Font defaultFont;
    sf::Font font;
    sf::Text instructionText;
    int maxSwitches;
    
public:
    PatternPuzzle(const std::vector<int>& pattern);
    
    bool solve(const std::string& answer) override;
    void display(sf::RenderWindow& window) override;
    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    
    void setFont(const sf::Font& f);
    bool checkPattern();
    void resetPattern();
};

// Lock Puzzle - Input a numeric code
class LockPuzzle : public Puzzle {
private:
    std::string correctCode;
    std::string enteredCode;
    sf::Font defaultFont;
    sf::Font font;
    sf::Text codeDisplay;
    sf::Text instructionText;
    int maxDigits;
    
public:
    LockPuzzle(const std::string& code);
    
    bool solve(const std::string& answer) override;
    void display(sf::RenderWindow& window) override;
    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    
    void setFont(const sf::Font& f);
    void addDigit(char digit);
    void removeDigit();
    void clearCode();
};

#endif // PUZZLE_H