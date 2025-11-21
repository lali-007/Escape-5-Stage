/*
 * Museum Escape - Puzzle Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Puzzle.h"
#include <algorithm>
#include <cctype>

// Puzzle Base Class
Puzzle::Puzzle(const std::string& desc, const std::string& hintText, int bonus, int penalty)
    : isSolved(false), description(desc), hint(hintText), timeBonus(bonus), timePenalty(penalty) {}

bool Puzzle::isSolvedStatus() const { return isSolved; }
std::string Puzzle::getDescription() const { return description; }
std::string Puzzle::getHint() const { return hint; }
int Puzzle::getTimeBonus() const { return timeBonus; }
int Puzzle::getTimePenalty() const { return timePenalty; }
void Puzzle::setSolved(bool status) { isSolved = status; }

// ============================================================================
// RiddlePuzzle - Fully Interactive
// ============================================================================

RiddlePuzzle::RiddlePuzzle(const std::string& riddleText, const std::string& answer)
    : Puzzle(riddleText, "Think carefully...", 30, 10),
      riddle(riddleText),
      correctAnswer(answer),
      riddleText(defaultFont),
      inputText(defaultFont),
      showFeedback(false) {
    
    // Convert answer to lowercase for case-insensitive comparison
    std::transform(correctAnswer.begin(), correctAnswer.end(), correctAnswer.begin(), ::tolower);
}

bool RiddlePuzzle::solve(const std::string& answer) {
    // Convert input to lowercase
    std::string lowerAnswer = answer;
    std::transform(lowerAnswer.begin(), lowerAnswer.end(), lowerAnswer.begin(), ::tolower);
    
    // Trim whitespace
    lowerAnswer.erase(0, lowerAnswer.find_first_not_of(" \t\n\r"));
    lowerAnswer.erase(lowerAnswer.find_last_not_of(" \t\n\r") + 1);
    
    if (lowerAnswer == correctAnswer) {
        isSolved = true;
        feedbackMessage = "Correct! +" + std::to_string(timeBonus) + " seconds!";
        showFeedback = true;
        return true;
    } else {
        feedbackMessage = "Wrong! -" + std::to_string(timePenalty) + " seconds. Try again.";
        showFeedback = true;
        return false;
    }
}

void RiddlePuzzle::display(sf::RenderWindow& window) {
    // Dark overlay
    sf::RectangleShape overlay({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Puzzle box
    sf::RectangleShape puzzleBox({600.0f, 400.0f});
    puzzleBox.setPosition({100.0f, 100.0f});
    puzzleBox.setFillColor(sf::Color(40, 40, 60));
    puzzleBox.setOutlineThickness(3.0f);
    puzzleBox.setOutlineColor(sf::Color::White);
    window.draw(puzzleBox);
    
    // Title
    sf::Text title(font);
    title.setString("RIDDLE PUZZLE");
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition({250.0f, 120.0f});
    window.draw(title);
    
    // Riddle text
    riddleText.setFont(font);
    riddleText.setString(riddle);
    riddleText.setCharacterSize(20);
    riddleText.setFillColor(sf::Color::White);
    riddleText.setPosition({130.0f, 180.0f});
    window.draw(riddleText);
    
    // Input prompt
    sf::Text promptText(font);
    promptText.setString("Your Answer:");
    promptText.setCharacterSize(18);
    promptText.setFillColor(sf::Color::Cyan);
    promptText.setPosition({130.0f, 320.0f});
    window.draw(promptText);
    
    // Input box
    sf::RectangleShape inputBox({540.0f, 40.0f});
    inputBox.setPosition({130.0f, 350.0f});
    inputBox.setFillColor(sf::Color(20, 20, 30));
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color::White);
    window.draw(inputBox);
    
    // User input text
    inputText.setFont(font);
    inputText.setString(userAnswer + "_");  // Cursor
    inputText.setCharacterSize(20);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition({140.0f, 357.0f});
    window.draw(inputText);
    
    // Feedback message
    if (showFeedback) {
        sf::Text feedback(font);
        feedback.setString(feedbackMessage);
        feedback.setCharacterSize(18);
        
        if (isSolved) {
            feedback.setFillColor(sf::Color::Green);
        } else {
            feedback.setFillColor(sf::Color::Red);
        }
        
        feedback.setPosition({130.0f, 410.0f});
        window.draw(feedback);
    }
    
    // Instructions
    sf::Text instructions(font);
    instructions.setString("Press ENTER to submit | ESC to exit");
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color(150, 150, 150));
    instructions.setPosition({200.0f, 460.0f});
    window.draw(instructions);
}

void RiddlePuzzle::handleInput(sf::Event& event) {
    if (isSolved) return;  // Don't accept input if already solved
    
    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        char entered = static_cast<char>(textEntered->unicode);
        
        // Handle backspace
        if (entered == 8 && !userAnswer.empty()) {  // 8 is backspace
            userAnswer.pop_back();
            showFeedback = false;
        }
        // Handle regular characters (letters, numbers, spaces)
        else if (entered >= 32 && entered < 127 && userAnswer.length() < 30) {
            userAnswer += entered;
            showFeedback = false;
        }
    }
    
    // Handle Enter key to submit
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter && !userAnswer.empty()) {
            solve(userAnswer);
        }
    }
}

void RiddlePuzzle::update(float deltaTime) {
    // Animation or timer logic could go here
}

void RiddlePuzzle::setFont(const sf::Font& f) {
    font = f;
    riddleText.setFont(font);
    inputText.setFont(font);
}

// ============================================================================
// PatternPuzzle - Click switches in correct order
// ============================================================================

PatternPuzzle::PatternPuzzle(const std::vector<int>& pattern)
    : Puzzle("Match the pattern", "Watch carefully...", 40, 15),
      correctPattern(pattern),
      instructionText(defaultFont),
      maxSwitches(pattern.size()) {
    
    // Create 4 colored switches
    std::vector<sf::Color> colors = {
        sf::Color::Blue,   // 1
        sf::Color::Red,    // 2
        sf::Color::Green,  // 3
        sf::Color::Yellow  // 4
    };
    
    float startX = 200.0f;
    float spacing = 120.0f;
    float y = 350.0f;
    
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape button({80.0f, 80.0f});
        button.setPosition({startX + i * spacing, y});
        button.setFillColor(colors[i]);
        button.setOutlineThickness(3.0f);
        button.setOutlineColor(sf::Color::White);
        switches.push_back(button);
    }
}

bool PatternPuzzle::solve(const std::string& answer) {
    return checkPattern();
}

void PatternPuzzle::display(sf::RenderWindow& window) {
    // Dark overlay
    sf::RectangleShape overlay({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Puzzle box
    sf::RectangleShape puzzleBox({600.0f, 450.0f});
    puzzleBox.setPosition({100.0f, 75.0f});
    puzzleBox.setFillColor(sf::Color(40, 40, 60));
    puzzleBox.setOutlineThickness(3.0f);
    puzzleBox.setOutlineColor(sf::Color::White);
    window.draw(puzzleBox);
    
    // Title
    sf::Text title(font);
    title.setString("PATTERN PUZZLE");
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition({250.0f, 95.0f});
    window.draw(title);
    
    // Instructions
    sf::Text instructions(font);
    instructions.setString("Click the switches in this order:\nBlue -> Green -> Red -> Yellow");
    instructions.setCharacterSize(20);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition({150.0f, 150.0f});
    window.draw(instructions);
    
    // Your sequence
    sf::Text sequenceText(font);
    std::string seq = "Your sequence: ";
    for (size_t i = 0; i < playerPattern.size(); i++) {
        std::vector<std::string> names = {"Blue", "Red", "Green", "Yellow"};
        if (i > 0) seq += " -> ";
        seq += names[playerPattern[i] - 1];
    }
    sequenceText.setString(seq);
    sequenceText.setCharacterSize(18);
    sequenceText.setFillColor(sf::Color::Cyan);
    sequenceText.setPosition({150.0f, 240.0f});
    window.draw(sequenceText);
    
    // Draw switches
    for (auto& sw : switches) {
        window.draw(sw);
    }
    
    // Instructions
    sf::Text controls(font);
    controls.setString("Click buttons in order | Press R to reset | ESC to exit");
    controls.setCharacterSize(16);
    controls.setFillColor(sf::Color(150, 150, 150));
    controls.setPosition({180.0f, 480.0f});
    window.draw(controls);
}

void PatternPuzzle::handleInput(sf::Event& event) {
    if (isSolved) return;
    
    // Handle mouse clicks
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);
            
            // Check which switch was clicked
            for (size_t i = 0; i < switches.size(); i++) {
                if (switches[i].getGlobalBounds().contains(mousePos)) {
                    // Add to player pattern (1-indexed)
                    playerPattern.push_back(i + 1);
                    
                    // Check if pattern is complete
                    if (playerPattern.size() >= correctPattern.size()) {
                        if (checkPattern()) {
                            isSolved = true;
                        } else {
                            // Wrong! Auto-reset after a moment
                            playerPattern.clear();
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Handle reset key
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R) {
            resetPattern();
        }
    }
}

void PatternPuzzle::update(float deltaTime) {
    // Could add animations here
}

void PatternPuzzle::setFont(const sf::Font& f) {
    font = f;
    instructionText.setFont(font);
}

bool PatternPuzzle::checkPattern() {
    if (playerPattern.size() != correctPattern.size()) {
        return false;
    }
    
    for (size_t i = 0; i < playerPattern.size(); i++) {
        if (playerPattern[i] != correctPattern[i]) {
            return false;
        }
    }
    
    return true;
}

void PatternPuzzle::resetPattern() {
    playerPattern.clear();
}

// ============================================================================
// LockPuzzle - STUB (to be implemented later)
// ============================================================================

LockPuzzle::LockPuzzle(const std::string& code)
    : Puzzle("Enter the code", "Look for clues...", 35, 10),
      correctCode(code),
      codeDisplay(defaultFont),
      instructionText(defaultFont),
      maxDigits(code.length()) {}

bool LockPuzzle::solve(const std::string& answer) { return false; }
void LockPuzzle::display(sf::RenderWindow& window) {
    // TODO: Implement lock puzzle display
}
void LockPuzzle::handleInput(sf::Event& event) {
    // TODO: Implement lock input
}
void LockPuzzle::update(float deltaTime) {}
void LockPuzzle::setFont(const sf::Font& f) { font = f; }
void LockPuzzle::addDigit(char digit) {}
void LockPuzzle::removeDigit() {}
void LockPuzzle::clearCode() {}