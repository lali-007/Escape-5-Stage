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
// LockPuzzle - Fully Interactive Numeric Keypad
// ============================================================================

LockPuzzle::LockPuzzle(const std::string& code)
    : Puzzle("Enter the code", "Look for clues...", 35, 10),
      correctCode(code),
      codeDisplay(defaultFont),
      instructionText(defaultFont),
      maxDigits(code.length()) {}

bool LockPuzzle::solve(const std::string& answer) {
    if (enteredCode == correctCode) {
        isSolved = true;
        return true;
    }
    return false;
}

void LockPuzzle::display(sf::RenderWindow& window) {
    // Dark overlay
    sf::RectangleShape overlay({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Puzzle box - SMALLER to fit everything
    sf::RectangleShape puzzleBox({550.0f, 550.0f});
    puzzleBox.setPosition({125.0f, 25.0f});
    puzzleBox.setFillColor(sf::Color(40, 40, 60));
    puzzleBox.setOutlineThickness(3.0f);
    puzzleBox.setOutlineColor(sf::Color::White);
    window.draw(puzzleBox);
    
    // Title
    sf::Text title(font);
    title.setString("LOCK PUZZLE");
    title.setCharacterSize(26);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition({310.0f, 40.0f});
    window.draw(title);
    
    // Instructions
    sf::Text instructions(font);
    instructions.setString("Enter the 4-digit code:");
    instructions.setCharacterSize(18);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition({280.0f, 80.0f});
    window.draw(instructions);
    
    // Code display box
    sf::RectangleShape displayBox({300.0f, 50.0f});
    displayBox.setPosition({250.0f, 115.0f});
    displayBox.setFillColor(sf::Color(20, 20, 30));
    displayBox.setOutlineThickness(3.0f);
    displayBox.setOutlineColor(sf::Color::Cyan);
    window.draw(displayBox);
    
    // Display entered code
    std::string displayCode = "";
    for (size_t i = 0; i < enteredCode.length(); i++) {
        displayCode += enteredCode[i];
        displayCode += " ";
    }
    
    // Add underscores for remaining digits
    for (size_t i = enteredCode.length(); i < (size_t)maxDigits; i++) {
        displayCode += "_ ";
    }
    
    codeDisplay.setFont(font);
    codeDisplay.setString(displayCode);
    codeDisplay.setCharacterSize(32);
    codeDisplay.setFillColor(sf::Color::White);
    codeDisplay.setPosition({290.0f, 125.0f});
    window.draw(codeDisplay);
    
    // Draw numeric keypad - ADJUSTED POSITIONS AND SMALLER BUTTONS
    float keypadStartX = 235.0f;
    float keypadStartY = 200.0f;
    float buttonSize = 65.0f;  // Smaller buttons
    float spacing = 85.0f;     // Tighter spacing
    
    // Buttons 1-9
    for (int i = 1; i <= 9; i++) {
        int row = (i - 1) / 3;
        int col = (i - 1) % 3;
        
        float x = keypadStartX + col * spacing;
        float y = keypadStartY + row * spacing;
        
        // Button background
        sf::RectangleShape button({buttonSize, buttonSize});
        button.setPosition({x, y});
        button.setFillColor(sf::Color(60, 60, 80));
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(sf::Color::White);
        window.draw(button);
        
        // Button number
        sf::Text buttonText(font);
        buttonText.setString(std::to_string(i));
        buttonText.setCharacterSize(28);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition({x + 23.0f, y + 15.0f});
        window.draw(buttonText);
    }
    
    // Bottom row: Clear, 0, Enter
    // Clear button
    sf::RectangleShape clearButton({buttonSize, buttonSize});
    clearButton.setPosition({keypadStartX, keypadStartY + 3 * spacing});
    clearButton.setFillColor(sf::Color(100, 50, 50));
    clearButton.setOutlineThickness(2.0f);
    clearButton.setOutlineColor(sf::Color::White);
    window.draw(clearButton);
    
    sf::Text clearText(font);
    clearText.setString("C");
    clearText.setCharacterSize(26);
    clearText.setFillColor(sf::Color::White);
    clearText.setPosition({keypadStartX + 23.0f, keypadStartY + 3 * spacing + 16.0f});
    window.draw(clearText);
    
    // 0 button
    sf::RectangleShape zeroButton({buttonSize, buttonSize});
    zeroButton.setPosition({keypadStartX + spacing, keypadStartY + 3 * spacing});
    zeroButton.setFillColor(sf::Color(60, 60, 80));
    zeroButton.setOutlineThickness(2.0f);
    zeroButton.setOutlineColor(sf::Color::White);
    window.draw(zeroButton);
    
    sf::Text zeroText(font);
    zeroText.setString("0");
    zeroText.setCharacterSize(28);
    zeroText.setFillColor(sf::Color::White);
    zeroText.setPosition({keypadStartX + spacing + 23.0f, keypadStartY + 3 * spacing + 15.0f});
    window.draw(zeroText);
    
    // OK button
    sf::RectangleShape enterButton({buttonSize, buttonSize});
    enterButton.setPosition({keypadStartX + 2 * spacing, keypadStartY + 3 * spacing});
    enterButton.setFillColor(sf::Color(50, 100, 50));
    enterButton.setOutlineThickness(2.0f);
    enterButton.setOutlineColor(sf::Color::White);
    window.draw(enterButton);
    
    sf::Text enterText(font);
    enterText.setString("OK");
    enterText.setCharacterSize(22);
    enterText.setFillColor(sf::Color::White);
    enterText.setPosition({keypadStartX + 2 * spacing + 15.0f, keypadStartY + 3 * spacing + 18.0f});
    window.draw(enterText);
    
    // Feedback message
    if (isSolved) {
        sf::Text feedback(font);
        feedback.setString("Correct! +" + std::to_string(timeBonus) + " seconds!");
        feedback.setCharacterSize(20);
        feedback.setFillColor(sf::Color::Green);
        feedback.setPosition({235.0f, 510.0f});
        window.draw(feedback);
    }
    
    // Instructions at bottom
    sf::Text controls(font);
    controls.setString("Click keypad or use keyboard | ESC to exit");
    controls.setCharacterSize(15);
    controls.setFillColor(sf::Color(150, 150, 150));
    controls.setPosition({220.0f, 545.0f});
    window.draw(controls);
}
void LockPuzzle::handleInput(sf::Event& event) {
    if (isSolved) return;  // Don't accept input if already solved
    
    // Handle mouse clicks on keypad - UPDATED POSITIONS
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float mouseX = static_cast<float>(mousePressed->position.x);
            float mouseY = static_cast<float>(mousePressed->position.y);
            
            // UPDATED to match display() positions
            float keypadStartX = 235.0f;
            float keypadStartY = 200.0f;
            float buttonSize = 65.0f;
            float spacing = 85.0f;
            
            // Check buttons 1-9
            for (int i = 1; i <= 9; i++) {
                int row = (i - 1) / 3;
                int col = (i - 1) % 3;
                
                float x = keypadStartX + col * spacing;
                float y = keypadStartY + row * spacing;
                
                if (mouseX >= x && mouseX <= x + buttonSize &&
                    mouseY >= y && mouseY <= y + buttonSize) {
                    addDigit('0' + i);
                    return;
                }
            }
            
            // Check Clear button (bottom left)
            float clearX = keypadStartX;
            float clearY = keypadStartY + 3 * spacing;
            if (mouseX >= clearX && mouseX <= clearX + buttonSize &&
                mouseY >= clearY && mouseY <= clearY + buttonSize) {
                clearCode();
                return;
            }
            
            // Check 0 button (bottom middle)
            float zeroX = keypadStartX + spacing;
            float zeroY = keypadStartY + 3 * spacing;
            if (mouseX >= zeroX && mouseX <= zeroX + buttonSize &&
                mouseY >= zeroY && mouseY <= zeroY + buttonSize) {
                addDigit('0');
                return;
            }
            
            // Check OK button (bottom right)
            float enterX = keypadStartX + 2 * spacing;
            float enterY = keypadStartY + 3 * spacing;
            if (mouseX >= enterX && mouseX <= enterX + buttonSize &&
                mouseY >= enterY && mouseY <= enterY + buttonSize) {
                solve(enteredCode);
                return;
            }
        }
    }
    
    // Handle keyboard input
    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        char entered = static_cast<char>(textEntered->unicode);
        
        // Handle backspace
        if (entered == 8) {  // Backspace
            removeDigit();
        }
        // Handle digits 0-9
        else if (entered >= '0' && entered <= '9') {
            addDigit(entered);
        }
    }
    
    // Handle Enter key
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter && enteredCode.length() == (size_t)maxDigits) {
            solve(enteredCode);
        }
    }
}

void LockPuzzle::update(float deltaTime) {
    // Animation or timer logic could go here
}

void LockPuzzle::setFont(const sf::Font& f) {
    font = f;
    codeDisplay.setFont(font);
    instructionText.setFont(font);
}

void LockPuzzle::addDigit(char digit) {
    if (enteredCode.length() < (size_t)maxDigits && digit >= '0' && digit <= '9') {
        enteredCode += digit;
    }
}

void LockPuzzle::removeDigit() {
    if (!enteredCode.empty()) {
        enteredCode.pop_back();
    }
}

void LockPuzzle::clearCode() {
    enteredCode.clear();
}