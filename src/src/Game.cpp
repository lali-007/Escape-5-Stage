/*
 * Museum Escape - Game Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Game.h"
#include "Puzzle.h"
#include "Guard.h"
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

// Create game rooms
void Game::createRooms() {
    // ========================================================================
    // Room 1 - Entrance Hall (1 Guard - Easy)
    // ========================================================================
    auto room1 = std::make_shared<Room>(1, "Entrance Hall", 0, 0, 800, 600);
    
    // Add Master Key to room 1
    auto key = std::make_shared<Key>("Master Key", "master_key", 400.0f, 300.0f);
    room1->addItem(key);
    
    // Add 1 guard with simple patrol (Easy difficulty)
    auto guard1 = std::make_shared<Guard>(200.0f, 200.0f, 100.0f);
    guard1->addPatrolPoint(200.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 400.0f);
    guard1->addPatrolPoint(200.0f, 400.0f);
    room1->addGuard(guard1);
    
    rooms[1] = room1;
    
    // ========================================================================
    // Room 2 - Artifact Room (2 Guards - Harder)
    // ========================================================================
    auto room2 = std::make_shared<Room>(2, "Artifact Room", 0, 0, 800, 600);
    
    // Add passcode item
    auto passcode = std::make_shared<Passcode>("Secret Code", "1234", 200.0f, 150.0f);
    room2->addItem(passcode);
    
    // Guard 1: Horizontal patrol (top)
    auto guard2a = std::make_shared<Guard>(150.0f, 200.0f, 110.0f);
    guard2a->addPatrolPoint(150.0f, 200.0f);
    guard2a->addPatrolPoint(650.0f, 200.0f);
    room2->addGuard(guard2a);
    
    // Guard 2: Horizontal patrol (bottom) - opposite direction
    auto guard2b = std::make_shared<Guard>(650.0f, 450.0f, 110.0f);
    guard2b->addPatrolPoint(650.0f, 450.0f);
    guard2b->addPatrolPoint(150.0f, 450.0f);
    room2->addGuard(guard2b);
    
    rooms[2] = room2;
    
    // ========================================================================
    // Room 3 - Exit Hall (No Guards - Victory Room)
    // ========================================================================
    auto room3 = std::make_shared<Room>(3, "Exit Hall", 0, 0, 800, 600);
    room3->setExitRoom(true);
    rooms[3] = room3;
    
    // ========================================================================
    // Connect rooms with doors
    // ========================================================================
    room1->addDoor(std::make_shared<Door>(750.0f, 300.0f, 2, false, ""));
    room2->addDoor(std::make_shared<Door>(50.0f, 300.0f, 1, false, ""));
    room2->addDoor(std::make_shared<Door>(750.0f, 300.0f, 3, true, "master_key"));
    room3->addDoor(std::make_shared<Door>(50.0f, 300.0f, 2, false, ""));
    
    std::cout << "Created " << rooms.size() << " rooms:" << std::endl;
    std::cout << "  - Room 1: 1 guard (Easy)" << std::endl;
    std::cout << "  - Room 2: 2 guards (Hard) + Puzzle" << std::endl;
    std::cout << "  - Room 3: Exit (Safe)" << std::endl;
}

// Setup puzzles in rooms
void Game::setupPuzzles() {
    // Add a riddle puzzle to room 2
    auto riddle = std::make_shared<RiddlePuzzle>(
        "I speak without a mouth and hear without ears.\nI have no body, but come alive with wind.\nWhat am I?",
        "echo"
    );
    riddle->setFont(mainFont);
    rooms[2]->addPuzzle(riddle);
    
    std::cout << "Puzzles setup complete!" << std::endl;
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
            std::cout << "Game started!" << std::endl;
        }
    }
}

// Handle playing state input
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
        // Interact with objects (doors, puzzles, items)
        if (keyPressed->code == sf::Keyboard::Key::E) {
            checkPuzzleInteraction();  // Check puzzles first
            checkDoorInteraction();
            checkItemPickup();
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
            std::cout << "Puzzle solved! +" << activePuzzle->getTimeBonus() << " seconds" << std::endl;
        }
    }
    
    // Exit puzzle with Escape
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            activePuzzle = nullptr;
            currentState = GameState::PLAYING;
            gameTimer->resume();  // Resume timer when exiting puzzle
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
    stateText.setString("MUSEUM ESCAPE\n\nPress ENTER to Start");
    stateText.setPosition({200.0f, 250.0f});
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
    
    // Draw notification if active
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
    stateText.setPosition({250.0f, 250.0f});
    window.draw(stateText);
}

void Game::renderVictory() {
    window.draw(overlay);
    stateText.setString("YOU ESCAPED!\n\nPress ESC to quit");
    stateText.setPosition({230.0f, 250.0f});
    window.draw(stateText);
}

// Change to a different room
void Game::changeRoom(int newRoomID) {
    if (rooms.find(newRoomID) != rooms.end()) {
        currentRoomID = newRoomID;
        rooms[currentRoomID]->setVisited(true);
        std::cout << "Moved to room: " << rooms[currentRoomID]->getRoomName() << std::endl;
    }
}

// Activate a puzzle
void Game::activatePuzzle(std::shared_ptr<Puzzle> puzzle) {
    activePuzzle = puzzle;
    currentState = GameState::PUZZLE_ACTIVE;
    gameTimer->pause();
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
        // detectPlayer() will return true only when detection FIRST happens (not during cooldown)
        if (guard->detectPlayer(*player)) {
            // Player just got detected!
            if (!player->isPlayerWarned()) {
                // First time caught - give warning
                player->warn();
                showNotification("WARNING! Caught by guard!", sf::Color::Yellow, 3.0f);
                std::cout << "WARNING! Caught by guard! Don't get caught again!" << std::endl;
                gameTimer->subtractTime(5.0f); // Small time penalty
            } else {
                // Second time caught - game over
                showNotification("CAUGHT! Game Over!", sf::Color::Red, 2.0f);
                std::cout << "Caught again! Game Over!" << std::endl;
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
                // Door is locked - check for key
                bool hasKey = false;
                
                // Check if player has Master Key
                auto& inv = player->getInventory();
                for (auto* item : inv) {
                    if (item->getName() == "Master Key") {
                        hasKey = true;
                        break;
                    }
                }
                
                if (hasKey) {
                    door->unlock();
                    showNotification("Door unlocked!", sf::Color::Green, 2.0f);
                    std::cout << "Door unlocked with Master Key!" << std::endl;
                    changeRoom(door->getTargetRoomID());
                } else {
                    showNotification("Door is LOCKED! Need Master Key", sf::Color::Red, 2.0f);
                    std::cout << "Door is LOCKED! You need the Master Key." << std::endl;
                }
            } else {
                // Door is unlocked
                changeRoom(door->getTargetRoomID());
            }
            return; // Only interact with one door at a time
        }
    }
}

// Check item pickup
void Game::checkItemPickup() {
    auto& items = rooms[currentRoomID]->getItems();
    auto playerBounds = player->getBounds();
    
    for (auto& item : items) {
        if (!item->isItemCollected() && item->checkCollision(playerBounds)) {
            item->collect();
            player->addItem(item.get()); // Add to player inventory
            inventory->addItem(item); // Add to inventory system
            
            // Show notification
            showNotification("Picked up: " + item->getName(), sf::Color::Cyan, 2.0f);
            std::cout << "Picked up: " << item->getName() << std::endl;
        }
    }
}

// Check puzzle interaction
void Game::checkPuzzleInteraction() {
    auto& puzzles = rooms[currentRoomID]->getPuzzles();
    
    // For now, just activate the first unsolved puzzle when player presses E
    for (auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            activatePuzzle(puzzle);
            showNotification("Puzzle activated!", sf::Color::Magenta, 1.5f);
            std::cout << "Puzzle activated!" << std::endl;
            return;
        }
    }
}

// Check if player has won
void Game::checkWinCondition() {
    if (rooms[currentRoomID]->isExit() && rooms[currentRoomID]->allPuzzlesSolved()) {
        setGameOver(true);
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
        std::cout << "You escaped!" << std::endl;
    } else {
        currentState = GameState::GAME_OVER;
        std::cout << "Game Over!" << std::endl;
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
    // TODO: Implement game reset
    currentState = GameState::MENU;
    gameTimer->reset();
}

// Show notification on screen
void Game::showNotification(const std::string& message, const sf::Color& color, float duration) {
    currentNotification = message;
    notificationColor = color;
    notificationTimer = duration;
}