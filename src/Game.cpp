/*
 * Museum Escape - Game Class Implementation
 * CS/CE 224/272 - Fall 2025
 * ALL BUGS FIXED VERSION
 */

#include "Game.h"
#include "Puzzle.h"
#include "Guard.h"
#include "Item.h"
#include <iostream>

// Constructor
Game::Game() 
    : window(sf::VideoMode({800u, 600u}), "Museum Escape"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      currentRoomID(1),
      activePuzzle(nullptr),
      stateText(defaultFont),
      notificationText(notificationFont),
      notificationTimer(0.0f),
      notificationColor(sf::Color::White)
{
    window.setFramerateLimit(60);
    initialize();
}

// Destructor
Game::~Game() {
    // Cleanup handled by smart pointers
}

// Initialize game components
void Game::initialize() {
    // Load assets FIRST (we need font before creating text)
    loadAssets();
    
    // Create player
    player = std::make_unique<Player>(100.0f, 100.0f);
    
    // Create timer (10 minutes = 600 seconds)
    gameTimer = std::make_unique<Timer>(600.0f);
    gameTimer->setDisplayPosition(650.0f, 20.0f);
    gameTimer->setFont(mainFont);
    
    // Create inventory
    inventory = std::make_unique<Inventory>(10);
    inventory->setFont(mainFont);
    
    // Create rooms and puzzles
    createRooms();
    setupPuzzles();
    
    // Setup UI text (set font after it's loaded)
    stateText.setFont(mainFont);
    stateText.setCharacterSize(30);
    stateText.setFillColor(sf::Color::White);
    stateText.setPosition({250.0f, 250.0f});
    
    // Setup notification text
    notificationText.setFont(mainFont);
    notificationText.setCharacterSize(24);
    notificationText.setPosition({50.0f, 50.0f});
    notificationText.setOutlineThickness(2.0f);
    notificationText.setOutlineColor(sf::Color::Black);
    
    // Setup overlay
    overlay.setSize({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    
    std::cout << "Game initialized successfully!" << std::endl;
}

// Load fonts, sounds, music
void Game::loadAssets() {
    // Try to load font - provide fallback if fails
    // Try multiple paths
    bool fontLoaded = false;
    
    if (mainFont.openFromFile("assets/arial.ttf")) {
        fontLoaded = true;
    } else if (mainFont.openFromFile("arial.ttf")) {
        fontLoaded = true;
    } else if (mainFont.openFromFile("D:/Assignments/Sem3/OOP/Prozect/main/assets/arial.ttf")) {
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font from any path!" << std::endl;
    } else {
        std::cout << "Font loaded successfully!" << std::endl;
    }
    
    std::cout << "Assets loaded!" << std::endl;
}

// Create game rooms - FIXED VERSION with proper flow
void Game::createRooms() {
    // ========================================================================
    // Room 1 - Entrance Hall (Tutorial - Easy)
    // ========================================================================
    auto room1 = std::make_shared<Room>(1, "Entrance Hall", 0, 0, 800, 600);
    
    // Add 1 guard with simple patrol (Tutorial)
    auto guard1 = std::make_shared<Guard>(200.0f, 200.0f, 100.0f);
    guard1->addPatrolPoint(200.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 400.0f);
    guard1->addPatrolPoint(200.0f, 400.0f);
    room1->addGuard(guard1);
    
    rooms[1] = room1;
    
    // ========================================================================
    // Room 2 - Storage Room (Pattern Puzzle → Master Key reward)
    // ========================================================================
    auto room2 = std::make_shared<Room>(2, "Storage Room", 0, 0, 800, 600);
    
    // Add 1 guard (medium difficulty)
    auto guard2 = std::make_shared<Guard>(150.0f, 300.0f, 110.0f);
    guard2->addPatrolPoint(150.0f, 300.0f);
    guard2->addPatrolPoint(650.0f, 300.0f);
    room2->addGuard(guard2);
    
    rooms[2] = room2;
    
    // ========================================================================
    // Room 3 - Artifact Room (Riddle Puzzle + Secret Code item)
    // ========================================================================
    auto room3 = std::make_shared<Room>(3, "Artifact Room", 0, 0, 800, 600);
    
    // Add the SECRET PASSCODE (needed for Lock Puzzle in Room 4!)
    auto secretCode = std::make_shared<Passcode>("Secret Code", "4738", 650.0f, 150.0f);
    room3->addItem(secretCode);
    
    // Add 1 guard
    auto guard3 = std::make_shared<Guard>(300.0f, 200.0f, 100.0f);
    guard3->addPatrolPoint(300.0f, 200.0f);
    guard3->addPatrolPoint(500.0f, 400.0f);
    room3->addGuard(guard3);
    
    rooms[3] = room3;
    
    // ========================================================================
    // Room 4 - Security Office (Lock Puzzle → Security Card reward)
    // ========================================================================
    auto room4 = std::make_shared<Room>(4, "Security Office", 0, 0, 800, 600);
    
    // Add 2 guards (HARD - crossing patrols!)
    auto guard4a = std::make_shared<Guard>(150.0f, 200.0f, 110.0f);
    guard4a->addPatrolPoint(150.0f, 200.0f);
    guard4a->addPatrolPoint(650.0f, 200.0f);
    room4->addGuard(guard4a);
    
    auto guard4b = std::make_shared<Guard>(650.0f, 450.0f, 110.0f);
    guard4b->addPatrolPoint(650.0f, 450.0f);
    guard4b->addPatrolPoint(150.0f, 450.0f);
    room4->addGuard(guard4b);
    
    rooms[4] = room4;
    
    // ========================================================================
    // Room 5 - Exit Hall (Victory Room - Safe!)
    // ========================================================================
    auto room5 = std::make_shared<Room>(5, "Exit Hall", 0, 0, 800, 600);
    room5->setExitRoom(true);
    rooms[5] = room5;
    
    // ========================================================================
    // Connect Rooms - PROPER FLOW: 1 → 2 → 3 → 4 → 5
    // ========================================================================
    
    // Room 1: Entrance → Room 2 only
    room1->addDoor(std::make_shared<Door>(750.0f, 300.0f, 2, false, "")); // To Room 2 (unlocked)
    
    // Room 2: Storage Room
    room2->addDoor(std::make_shared<Door>(50.0f, 300.0f, 1, false, "")); // Back to Room 1
    room2->addDoor(std::make_shared<Door>(750.0f, 300.0f, 3, true, "master_key")); // To Room 3 (LOCKED - need Master Key from puzzle!)
    
    // Room 3: Artifact Room  
    room3->addDoor(std::make_shared<Door>(50.0f, 300.0f, 2, false, "")); // Back to Room 2
    room3->addDoor(std::make_shared<Door>(750.0f, 300.0f, 4, false, "")); // To Room 4 (unlocked - you have the code now!)
    
    // Room 4: Security Office
    room4->addDoor(std::make_shared<Door>(50.0f, 300.0f, 3, false, "")); // Back to Room 3
    room4->addDoor(std::make_shared<Door>(750.0f, 300.0f, 5, true, "security_card")); // To Room 5 (LOCKED - need Security Card from puzzle!)
    
    // Room 5: Exit Hall
    room5->addDoor(std::make_shared<Door>(50.0f, 300.0f, 4, false, "")); // Back to Room 4
    
    std::cout << "\n=== MUSEUM ESCAPE - PROPER GAME FLOW ===" << std::endl;
    std::cout << "Room 1: Entrance Hall (Tutorial)" << std::endl;
    std::cout << "  ↓" << std::endl;
    std::cout << "Room 2: Storage Room (Pattern Puzzle → Master Key)" << std::endl;
    std::cout << "  ↓ (Door locked - need Master Key)" << std::endl;
    std::cout << "Room 3: Artifact Room (Riddle + Secret Code)" << std::endl;
    std::cout << "  ↓" << std::endl;
    std::cout << "Room 4: Security Office (Lock Puzzle → Security Card)" << std::endl;
    std::cout << "  ↓ (Door locked - need Security Card)" << std::endl;
    std::cout << "Room 5: Exit Hall (Victory!)" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// Setup puzzles in rooms - FIXED ORDER
void Game::setupPuzzles() {
    // Room 2: Pattern Puzzle (reward: Master Key to unlock Room 3)
    auto patternPuzzle = std::make_shared<PatternPuzzle>(std::vector<int>{1, 3, 2, 4});
    patternPuzzle->setFont(mainFont);
    rooms[2]->addPuzzle(patternPuzzle);
    
    // Room 3: Riddle Puzzle (no reward - just progression)
    auto riddle = std::make_shared<RiddlePuzzle>(
        "I speak without a mouth and hear without ears.\nI have no body, but come alive with wind.\nWhat am I?",
        "echo"
    );
    riddle->setFont(mainFont);
    rooms[3]->addPuzzle(riddle);
    
    // Room 4: Lock Puzzle (reward: Security Card to unlock Room 5)
    auto lockPuzzle = std::make_shared<LockPuzzle>("4738"); // Code from Room 3
    lockPuzzle->setFont(mainFont);
    rooms[4]->addPuzzle(lockPuzzle);
    
    std::cout << "Puzzles setup:" << std::endl;
    std::cout << "  Room 2: Pattern (Blue→Green→Red→Yellow) → Master Key" << std::endl;
    std::cout << "  Room 3: Riddle (Answer: echo) + Secret Code item" << std::endl;
    std::cout << "  Room 4: Lock (Code: 4738) → Security Card" << std::endl;
}

// Main game loop
void Game::run() {
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update();
        render();
    }
}

// Process input events
void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        // State-specific input handling
        switch (currentState) {
            case GameState::MENU:
                handleMenuInput(*event);
                break;
            case GameState::PLAYING:
                handlePlayingInput(*event);
                break;
            case GameState::PUZZLE_ACTIVE:
                handlePuzzleInput(*event);
                break;
            case GameState::PAUSED:
                handlePauseInput(*event);
                break;
            default:
                break;
        }
    }
}

// Handle menu input
void Game::handleMenuInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            currentState = GameState::PLAYING;
            gameTimer->start();
            std::cout << "\n=== GAME STARTED ===" << std::endl;
            std::cout << "Objective: Solve all puzzles and escape!" << std::endl;
            std::cout << "Controls: WASD=Move, E=Interact, P=Activate Puzzle, I=Inventory, ESC=Pause\n" << std::endl;
        }
    }
}

// Handle playing state input - FIXED: Separate keys for items and puzzles
void Game::handlePlayingInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // Pause game
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            pauseGame();
        }
        // Toggle inventory
        if (keyPressed->code == sf::Keyboard::Key::I) {
            inventory->toggleVisibility();
        }
        // E = Pick up items and interact with doors
        if (keyPressed->code == sf::Keyboard::Key::E) {
            checkDoorInteraction();
            checkItemPickup();
        }
        // P = Activate puzzles (separate key!)
        if (keyPressed->code == sf::Keyboard::Key::P) {
            checkPuzzleInteraction();
        }
    }
}

// Handle puzzle input
void Game::handlePuzzleInput(const sf::Event& event) {
    if (activePuzzle) {
        // Check if puzzle is solved BEFORE handling input
        bool wasSolved = activePuzzle->isSolvedStatus();
        
        activePuzzle->handleInput(const_cast<sf::Event&>(event));
        
        // Check if puzzle was just solved
        if (!wasSolved && activePuzzle->isSolvedStatus()) {
            gameTimer->addTime(activePuzzle->getTimeBonus());
            showNotification("Puzzle Solved! +" + std::to_string(activePuzzle->getTimeBonus()) + "s", sf::Color::Green, 3.0f);
            std::cout << "\n*** PUZZLE SOLVED! ***" << std::endl;
            std::cout << "Time bonus: +" << activePuzzle->getTimeBonus() << " seconds" << std::endl;
            
            // Give rewards based on current room
            if (currentRoomID == 2) {
                // Room 2 Pattern Puzzle → Master Key
                auto masterKey = std::make_shared<Key>("Master Key", "master_key", 650.0f, 500.0f);
                rooms[2]->addItem(masterKey);
                showNotification("Master Key appeared in bottom-right corner!", sf::Color::Yellow, 4.0f);
                std::cout << "🔑 MASTER KEY unlocked! Go to bottom-right corner to pick it up!" << std::endl;
                std::cout << "Use it to unlock the door to Room 3!\n" << std::endl;
            } else if (currentRoomID == 4) {
                // Room 4 Lock Puzzle → Security Card
                auto securityCard = std::make_shared<Key>("Security Card", "security_card", 650.0f, 500.0f);
                rooms[4]->addItem(securityCard);
                showNotification("Security Card appeared in bottom-right corner!", sf::Color::Cyan, 4.0f);
                std::cout << "🎫 SECURITY CARD unlocked! Go to bottom-right corner to pick it up!" << std::endl;
                std::cout << "Use it to unlock the door to Room 5 (Exit)!\n" << std::endl;
            }
        }
    }
    
    // Exit puzzle with Escape
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            activePuzzle = nullptr;
            currentState = GameState::PLAYING;
            gameTimer->resume();
            std::cout << "Puzzle closed. Resume game." << std::endl;
        }
    }
}

// Handle pause input
void Game::handlePauseInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            resumeGame();
        }
    }
}

// Update game state
void Game::update() {
    switch (currentState) {
        case GameState::MENU:
            updateMenu();
            break;
        case GameState::PLAYING:
            updatePlaying();
            break;
        case GameState::PUZZLE_ACTIVE:
            updatePuzzle();
            break;
        case GameState::GAME_OVER:
            updateGameOver();
            break;
        default:
            break;
    }
}

void Game::updateMenu() {
    // Menu doesn't need updates
}

void Game::updatePlaying() {
    // Update timer
    gameTimer->update(deltaTime);
    
    // Update notification timer
    if (notificationTimer > 0) {
        notificationTimer -= deltaTime;
    }
    
    // Update player
    player->handleInput(deltaTime);
    player->update(deltaTime);
    
    // Update current room
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->update(deltaTime);
        
        // Update guards in current room (they need player reference)
        auto& guards = rooms[currentRoomID]->getGuards();
        for (auto& guard : guards) {
            guard->update(deltaTime, *player);
        }
    }
    
    // Check collisions
    checkCollisions();
    checkGuardDetection();
    
    // Check win/lose conditions
    checkWinCondition();
    checkLoseCondition();
}

void Game::updatePuzzle() {
    if (activePuzzle) {
        activePuzzle->update(deltaTime);
    }
}

void Game::updateGameOver() {
    // Game over doesn't need updates
}

// Render everything
void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::PLAYING:
            renderPlaying();
            break;
        case GameState::PUZZLE_ACTIVE:
            renderPuzzle();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::VICTORY:
            renderVictory();
            break;
        default:
            break;
    }
    
    window.display();
}

void Game::renderMenu() {
    stateText.setString("MUSEUM ESCAPE\n\nPress ENTER to Start\n\nControls:\nWASD = Move\nE = Pick items/doors\nP = Activate puzzles\nI = Inventory");
    stateText.setCharacterSize(24);
    stateText.setPosition({150.0f, 200.0f});
    window.draw(stateText);
}

void Game::renderPlaying() {
    // Draw current room
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->draw(window);
    }
    
    // Draw player
    player->draw(window);
    
    // Draw timer
    gameTimer->draw(window);
    
    // Draw inventory if visible
    if (inventory->getVisible()) {
        inventory->draw(window);
    }
    
    // Draw notification if active (ALWAYS ON TOP)
    if (notificationTimer > 0) {
        notificationText.setString(currentNotification);
        notificationText.setFillColor(notificationColor);
        window.draw(notificationText);
    }
}

void Game::renderPuzzle() {
    // Draw dimmed game background
    renderPlaying();
    window.draw(overlay);
    
    // Draw active puzzle
    if (activePuzzle) {
        activePuzzle->display(window);
    }
}

void Game::renderGameOver() {
    window.draw(overlay);
    stateText.setString("GAME OVER\n\nPress ESC to quit");
    stateText.setCharacterSize(30);
    stateText.setPosition({250.0f, 250.0f});
    window.draw(stateText);
}

void Game::renderVictory() {
    window.draw(overlay);
    stateText.setString("YOU ESCAPED!\n\nPress ESC to quit");
    stateText.setCharacterSize(30);
    stateText.setPosition({230.0f, 250.0f});
    window.draw(stateText);
}

// Change to a different room - SIMPLIFIED (spawn at default position)
void Game::changeRoom(int newRoomID) {
    if (rooms.find(newRoomID) != rooms.end()) {
        currentRoomID = newRoomID;
        rooms[currentRoomID]->setVisited(true);
        
        // Spawn player at a safe default position
        player->setPosition(100.0f, 300.0f);
        
        std::cout << "\n→ Moved to: " << rooms[currentRoomID]->getRoomName() << std::endl;
    }
}

// Activate a puzzle
void Game::activatePuzzle(std::shared_ptr<Puzzle> puzzle) {
    activePuzzle = puzzle;
    currentState = GameState::PUZZLE_ACTIVE;
    gameTimer->pause();
    std::cout << "Puzzle activated! (Press ESC to exit without solving)" << std::endl;
}

// Check collisions with room boundaries
void Game::checkCollisions() {
    // Basic collision with room bounds
    auto playerBounds = player->getBounds();
    auto roomBounds = rooms[currentRoomID]->getBounds();
    
    // Keep player inside room (simple version)
    sf::Vector2f pos = player->getPosition();
    if (pos.x < 0) player->setPosition(0, pos.y);
    if (pos.y < 0) player->setPosition(pos.x, 0);
    if (pos.x > 800 - playerBounds.size.x) player->setPosition(800 - playerBounds.size.x, pos.y);
    if (pos.y > 600 - playerBounds.size.y) player->setPosition(pos.x, 600 - playerBounds.size.y);
}

// Check if guards detect player
void Game::checkGuardDetection() {
    auto& guards = rooms[currentRoomID]->getGuards();
    
    for (auto& guard : guards) {
        if (guard->detectPlayer(*player)) {
            if (!player->isPlayerWarned()) {
                player->warn();
                showNotification("WARNING! Caught by guard!", sf::Color::Yellow, 3.0f);
                std::cout << "⚠️  WARNING! Caught by guard! Don't get caught again!" << std::endl;
                gameTimer->subtractTime(5.0f);
            } else {
                showNotification("CAUGHT! Game Over!", sf::Color::Red, 2.0f);
                std::cout << "💀 CAUGHT AGAIN! GAME OVER!" << std::endl;
                setGameOver(false);
                return;
            }
        }
    }
}

// Check door interactions
void Game::checkDoorInteraction() {
    auto& doors = rooms[currentRoomID]->getDoors();
    auto playerBounds = player->getBounds();
    
    for (auto& door : doors) {
        if (door->checkCollision(playerBounds)) {
            if (door->getLockedStatus()) {
                bool hasKey = false;
                std::string requiredKey = "";
                
                int targetRoom = door->getTargetRoomID();
                if (targetRoom == 3) {
                    requiredKey = "Master Key";
                } else if (targetRoom == 5) {
                    requiredKey = "Security Card";
                }
                
                auto& inv = player->getInventory();
                for (auto* item : inv) {
                    if (item->getName() == requiredKey) {
                        hasKey = true;
                        break;
                    }
                }
                
                if (hasKey) {
                    door->unlock();
                    showNotification("Door unlocked with " + requiredKey + "!", sf::Color::Green, 2.0f);
                    std::cout << "🔓 Door unlocked with " << requiredKey << "!" << std::endl;
                    changeRoom(door->getTargetRoomID());
                } else {
                    showNotification("LOCKED! Need " + requiredKey, sf::Color::Red, 2.0f);
                    std::cout << "🔒 Door is LOCKED! You need: " << requiredKey << std::endl;
                }
            } else {
                changeRoom(door->getTargetRoomID());
            }
            return;
        }
    }
}

// Check item pickup - FIXED: Longer notification for Secret Code
void Game::checkItemPickup() {
    auto& items = rooms[currentRoomID]->getItems();
    auto playerBounds = player->getBounds();
    
    for (auto& item : items) {
        if (!item->isItemCollected() && item->checkCollision(playerBounds)) {
            item->collect();
            player->addItem(item.get());
            inventory->addItem(item);
            
            // Special handling for Secret Code - show for longer and pause hint
            if (item->getName() == "Secret Code") {
                Passcode* passcode = dynamic_cast<Passcode*>(item.get());
                if (passcode) {
                    std::string code = passcode->getCode();
                    // Show for 10 seconds! Much longer than before
                    showNotification("SECRET CODE: " + code + " - Remember this for Lock Puzzle!", sf::Color::Yellow, 10.0f);
                    std::cout << "\n" << std::string(50, '=') << std::endl;
                    std::cout << "📜 FOUND SECRET CODE: " << code << std::endl;
                    std::cout << "💡 Write this down! You need it for Room 4 Lock Puzzle!" << std::endl;
                    std::cout << std::string(50, '=') << "\n" << std::endl;
                }
            } else {
                showNotification("Picked up: " + item->getName(), sf::Color::Cyan, 2.0f);
                std::cout << "📦 Picked up: " << item->getName() << std::endl;
            }
        }
    }
}

// Check puzzle interaction - FIXED: Better hint for Lock Puzzle
void Game::checkPuzzleInteraction() {
    auto& puzzles = rooms[currentRoomID]->getPuzzles();
    
    for (auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            activatePuzzle(puzzle);
            
            // Special hint for Lock Puzzle in Room 4
            if (currentRoomID == 4) {
                showNotification("Lock Puzzle! Enter code from Room 3 Secret Code item!", sf::Color::Magenta, 4.0f);
                std::cout << "\n💡 HINT: Use the Secret Code you found in Room 3!" << std::endl;
                std::cout << "💡 The code is 4 digits. Check your inventory if you forgot!\n" << std::endl;
            } else {
                showNotification("Puzzle activated! Press ESC to close", sf::Color::Magenta, 2.0f);
            }
            return;
        }
    }
}

// Check if player has won
void Game::checkWinCondition() {
    if (rooms[currentRoomID]->isExit()) {
        bool allPuzzlesSolved = true;
        
        for (auto& roomPair : rooms) {
            auto& puzzles = roomPair.second->getPuzzles();
            for (auto& puzzle : puzzles) {
                if (!puzzle->isSolvedStatus()) {
                    allPuzzlesSolved = false;
                    break;
                }
            }
            if (!allPuzzlesSolved) break;
        }
        
        if (allPuzzlesSolved) {
            setGameOver(true);
        } else {
            showNotification("Solve ALL puzzles to escape!", sf::Color::Red, 2.0f);
            std::cout << "❌ You must solve ALL puzzles first!" << std::endl;
        }
    }
}

// Check if player has lost
void Game::checkLoseCondition() {
    if (gameTimer->isExpired()) {
        setGameOver(false);
    }
}

// Set game over state
void Game::setGameOver(bool victory) {
    if (victory) {
        currentState = GameState::VICTORY;
        std::cout << "\n🎉 YOU ESCAPED! CONGRATULATIONS! 🎉" << std::endl;
    } else {
        currentState = GameState::GAME_OVER;
        std::cout << "\n💀 GAME OVER! 💀" << std::endl;
    }
    gameTimer->stop();
}

// Pause the game
void Game::pauseGame() {
    currentState = GameState::PAUSED;
    gameTimer->pause();
}

// Resume the game
void Game::resumeGame() {
    currentState = GameState::PLAYING;
    gameTimer->resume();
}

// Reset game to initial state
void Game::resetGame() {
    currentState = GameState::MENU;
    gameTimer->reset();
}

// Show notification on screen
void Game::showNotification(const std::string& message, const sf::Color& color, float duration) {
    currentNotification = message;
    notificationColor = color;
    notificationTimer = duration;
}