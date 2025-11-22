/*
 * Museum Escape - Game Class Implementation
 * CS/CE 224/272 - Fall 2025
 * SFML 3.0 COMPATIBLE (Door Color Fix)
 */

#include "Game.h"
#include "Puzzle.h"
#include "Guard.h"
#include "Item.h"
#include <iostream>
#include <cmath>

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

Game::~Game() {}

void Game::initialize() {
    loadAssets();
    player = std::make_unique<Player>(100.0f, 100.0f, playerTexture);
    gameTimer = std::make_unique<Timer>(600.0f);
    gameTimer->setDisplayPosition(650.0f, 20.0f);
    gameTimer->setFont(mainFont);
    inventory = std::make_unique<Inventory>(10);
    inventory->setFont(mainFont);
    createRooms();
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    std::cout << "Looking for assets at: " << std::filesystem::current_path() / "assets" << std::endl;
    // --------------------------------
    
    bool fontLoaded = false;
    setupPuzzles();
    stateText.setFont(mainFont);
    stateText.setCharacterSize(30);
    stateText.setFillColor(sf::Color::White);
    stateText.setPosition({250.0f, 250.0f});
    notificationText.setFont(mainFont);
    notificationText.setCharacterSize(24);
    notificationText.setPosition({50.0f, 50.0f});
    notificationText.setOutlineThickness(2.0f);
    notificationText.setOutlineColor(sf::Color::Black);
    overlay.setSize({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::loadAssets() {
    bool fontLoaded = false;
    if (mainFont.openFromFile("assets/arial.ttf")) fontLoaded = true;
    else if (mainFont.openFromFile("arial.ttf")) fontLoaded = true;
    else if (mainFont.openFromFile("D:/Assignments/Sem3/OOP/Prozect/main/assets/arial.ttf")) fontLoaded = true;
    
    if (!fontLoaded) std::cerr << "Warning: Could not load font!" << std::endl;
    
    if (!playerTexture.loadFromFile("assets/player.png")) {
        sf::Image img; img.resize({40, 40}, sf::Color::Green);
        if(!playerTexture.loadFromImage(img)) {}
        std::cout << "Warning: player.png not found." << std::endl;
    }
    if (!guardTexture.loadFromFile("assets/guard.png")) {
        sf::Image img; img.resize({40, 40}, sf::Color::Red);
        if(!guardTexture.loadFromImage(img)) {}
        std::cout << "Warning: guard.png not found." << std::endl;
    }
    std::cout << "Assets loaded!" << std::endl;
}

void Game::createRooms() {
    // Create Rooms with Backgrounds
    auto room1 = std::make_shared<Room>(1, "Entrance Hall", 0, 0, 800, 600, "assets/room1.png");
    auto guard1 = std::make_shared<Guard>(200.0f, 200.0f, 100.0f, guardTexture);
    guard1->addPatrolPoint(200.0f, 200.0f); guard1->addPatrolPoint(600.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 400.0f); guard1->addPatrolPoint(200.0f, 400.0f);
    room1->addGuard(guard1);
    rooms[1] = room1;
    
    auto room2 = std::make_shared<Room>(2, "Storage Room", 0, 0, 800, 600, "assets/room2.png");
    auto guard2 = std::make_shared<Guard>(150.0f, 300.0f, 110.0f, guardTexture);
    guard2->addPatrolPoint(150.0f, 300.0f); guard2->addPatrolPoint(650.0f, 300.0f);
    room2->addGuard(guard2);
    rooms[2] = room2;
    
    auto room3 = std::make_shared<Room>(3, "Artifact Room", 0, 0, 800, 600, "assets/room3.png");
    auto secretCode = std::make_shared<Passcode>("Secret Code", "4738", 650.0f, 150.0f);
    room3->addItem(secretCode);
    auto guard3 = std::make_shared<Guard>(300.0f, 200.0f, 100.0f, guardTexture);
    guard3->addPatrolPoint(300.0f, 200.0f); guard3->addPatrolPoint(500.0f, 400.0f);
    room3->addGuard(guard3);
    rooms[3] = room3;
    
    auto room4 = std::make_shared<Room>(4, "Security Office", 0, 0, 800, 600, "assets/room4.png");
    auto guard4a = std::make_shared<Guard>(150.0f, 200.0f, 110.0f, guardTexture);
    guard4a->addPatrolPoint(150.0f, 200.0f); guard4a->addPatrolPoint(650.0f, 200.0f);
    room4->addGuard(guard4a);
    auto guard4b = std::make_shared<Guard>(650.0f, 450.0f, 110.0f, guardTexture);
    guard4b->addPatrolPoint(650.0f, 450.0f); guard4b->addPatrolPoint(150.0f, 450.0f);
    room4->addGuard(guard4b);
    rooms[4] = room4;
    
    auto room5 = std::make_shared<Room>(5, "Exit Hall", 0, 0, 800, 600, "assets/room5.png");
    room5->setExitRoom(true);
    rooms[5] = room5;
    
    // Connect Rooms with Doors
    // --- FIXED: Using "Master Key" instead of "master_key" to match Item Name ---
    
    room1->addDoor(std::make_shared<Door>(750.0f, 300.0f, 2, false, "")); 
    
    room2->addDoor(std::make_shared<Door>(50.0f, 300.0f, 1, false, ""));
    room2->addDoor(std::make_shared<Door>(750.0f, 300.0f, 3, true, "Master Key")); // FIXED
    
    room3->addDoor(std::make_shared<Door>(50.0f, 300.0f, 2, false, "")); 
    room3->addDoor(std::make_shared<Door>(750.0f, 300.0f, 4, false, ""));
    
    room4->addDoor(std::make_shared<Door>(50.0f, 300.0f, 3, false, "")); 
    room4->addDoor(std::make_shared<Door>(750.0f, 300.0f, 5, true, "Security Card")); // FIXED
    
    room5->addDoor(std::make_shared<Door>(50.0f, 300.0f, 4, false, ""));
}

void Game::setupPuzzles() {
    auto patternPuzzle = std::make_shared<PatternPuzzle>(std::vector<int>{1, 3, 2, 4});
    patternPuzzle->setFont(mainFont); rooms[2]->addPuzzle(patternPuzzle);
    auto riddle = std::make_shared<RiddlePuzzle>("I speak without a mouth and hear without ears.\nI have no body, but come alive with wind.\nWhat am I?", "echo");
    riddle->setFont(mainFont); rooms[3]->addPuzzle(riddle);
    auto lockPuzzle = std::make_shared<LockPuzzle>("4738");
    lockPuzzle->setFont(mainFont); rooms[4]->addPuzzle(lockPuzzle);
}

void Game::run() {
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        switch (currentState) {
            case GameState::MENU: handleMenuInput(*event); break;
            case GameState::PLAYING: handlePlayingInput(*event); break;
            case GameState::PUZZLE_ACTIVE: handlePuzzleInput(*event); break;
            case GameState::PAUSED: handlePauseInput(*event); break;
            default: break;
        }
    }
}

void Game::handleMenuInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            currentState = GameState::PLAYING;
            gameTimer->start();
            std::cout << "Game Started!" << std::endl;
        }
    }
}

void Game::handlePlayingInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) pauseGame();
        if (keyPressed->code == sf::Keyboard::Key::I) inventory->toggleVisibility();
        if (keyPressed->code == sf::Keyboard::Key::E) { checkDoorInteraction(); checkItemPickup(); }
        if (keyPressed->code == sf::Keyboard::Key::P) checkPuzzleInteraction();
    }
}

void Game::handlePuzzleInput(const sf::Event& event) {
    if (activePuzzle) {
        bool wasSolved = activePuzzle->isSolvedStatus();
        activePuzzle->handleInput(const_cast<sf::Event&>(event));
        if (!wasSolved && activePuzzle->isSolvedStatus()) {
            gameTimer->addTime(activePuzzle->getTimeBonus());
            showNotification("Puzzle Solved! +" + std::to_string(activePuzzle->getTimeBonus()) + "s", sf::Color::Green, 3.0f);
            if (currentRoomID == 2) {
                // FIXED: Using "Master Key" for internal ID to match Item Name
                auto masterKey = std::make_shared<Key>("Master Key", "Master Key", 650.0f, 500.0f);
                rooms[2]->addItem(masterKey);
                showNotification("Master Key appeared!", sf::Color::Yellow, 4.0f);
            } else if (currentRoomID == 4) {
                // FIXED: Using "Security Card" for internal ID to match Item Name
                auto securityCard = std::make_shared<Key>("Security Card", "Security Card", 650.0f, 500.0f);
                rooms[4]->addItem(securityCard);
                showNotification("Security Card appeared!", sf::Color::Cyan, 4.0f);
            }
        }
    }
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            activePuzzle = nullptr;
            currentState = GameState::PLAYING;
            gameTimer->resume();
        }
    }
}

void Game::handlePauseInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) resumeGame();
    }
}

void Game::update() {
    switch (currentState) {
        case GameState::MENU: updateMenu(); break;
        case GameState::PLAYING: updatePlaying(); break;
        case GameState::PUZZLE_ACTIVE: updatePuzzle(); break;
        case GameState::GAME_OVER: updateGameOver(); break;
        default: break;
    }
}

void Game::updateMenu() {}

void Game::updatePlaying() {
    gameTimer->update(deltaTime);
    if (notificationTimer > 0) notificationTimer -= deltaTime;
    player->handleInput(deltaTime);
    player->update(deltaTime);
    
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->update(deltaTime);
        
        // 1. Update Guards (Keep moving!)
        auto& guards = rooms[currentRoomID]->getGuards();
        for (auto& guard : guards) guard->update(deltaTime, *player);
        
        // 2. Update Door Colors
        auto& doors = rooms[currentRoomID]->getDoors();
        for (auto& door : doors) {
            if (door->getLockedStatus()) {
                std::string keyNeeded = door->getRequiredKey();
                // Now that names match ("Master Key" == "Master Key"), this will return TRUE
                if (player->hasItem(keyNeeded)) {
                    door->setColor(sf::Color::Blue); // READY
                } else {
                    door->setColor(sf::Color::Red); // LOCKED
                }
            }
        }
    }
    
    checkCollisions();
    checkGuardDetection();
    checkWinCondition();
    checkLoseCondition();
}

void Game::updatePuzzle() { if (activePuzzle) activePuzzle->update(deltaTime); }
void Game::updateGameOver() {}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    switch (currentState) {
        case GameState::MENU: renderMenu(); break;
        case GameState::PLAYING: renderPlaying(); break;
        case GameState::PUZZLE_ACTIVE: renderPuzzle(); break;
        case GameState::GAME_OVER: renderGameOver(); break;
        case GameState::VICTORY: renderVictory(); break;
        default: break;
    }
    window.display();
}

void Game::renderMenu() {
    sf::RectangleShape bg({800.0f, 600.0f});
    bg.setFillColor(sf::Color(20, 20, 35));
    window.draw(bg);
    sf::RectangleShape titleBar({800.0f, 120.0f});
    titleBar.setPosition({0.0f, 100.0f});
    titleBar.setFillColor(sf::Color(0, 0, 0, 100));
    titleBar.setOutlineThickness(2.0f);
    titleBar.setOutlineColor(sf::Color::Yellow);
    window.draw(titleBar);
    stateText.setString("MUSEUM ESCAPE");
    stateText.setCharacterSize(60);
    stateText.setStyle(sf::Text::Bold);
    stateText.setFillColor(sf::Color::Yellow);
    sf::FloatRect bounds = stateText.getLocalBounds();
    stateText.setPosition({400.0f - bounds.size.x / 2.0f, 120.0f});
    window.draw(stateText);
    sf::RectangleShape controlsBox({400.0f, 200.0f});
    controlsBox.setPosition({200.0f, 300.0f});
    controlsBox.setFillColor(sf::Color(50, 50, 50));
    controlsBox.setOutlineThickness(1.0f);
    controlsBox.setOutlineColor(sf::Color::White);
    window.draw(controlsBox);
    sf::Text controls(mainFont);
    controls.setString("CONTROLS\n\nWASD  - Move\nE     - Interact / Pickup\nP     - Puzzle\nI     - Inventory");
    controls.setCharacterSize(20);
    controls.setFillColor(sf::Color::White);
    controls.setPosition({220.0f, 320.0f});
    window.draw(controls);
    float time = clock.getElapsedTime().asSeconds();
    int alpha = static_cast<int>((sin(time * 3.0f) + 1.0f) / 2.0f * 255);
    sf::Text startText(mainFont);
    startText.setString("- Press ENTER to Start -");
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color(255, 255, 255, alpha));
    sf::FloatRect startBounds = startText.getLocalBounds();
    startText.setPosition({400.0f - startBounds.size.x / 2.0f, 530.0f});
    window.draw(startText);
}

void Game::renderPlaying() {
    if (rooms.find(currentRoomID) != rooms.end()) rooms[currentRoomID]->draw(window);
    player->draw(window);
    sf::RectangleShape topBar({800.0f, 40.0f});
    topBar.setFillColor(sf::Color(30, 30, 30));
    topBar.setOutlineThickness(1.0f);
    topBar.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(topBar);
    sf::Text roomText(mainFont);
    if (rooms.find(currentRoomID) != rooms.end()) roomText.setString("LOCATION: " + rooms[currentRoomID]->getRoomName());
    roomText.setCharacterSize(18);
    roomText.setFillColor(sf::Color::Cyan);
    roomText.setPosition({10.0f, 8.0f});
    window.draw(roomText);
    sf::Text timeText(mainFont);
    timeText.setString("TIME REMAINING: " + gameTimer->getFormattedTime());
    timeText.setCharacterSize(18);
    if(gameTimer->getRemainingTime() < 30.0f) timeText.setFillColor(sf::Color::Red);
    else timeText.setFillColor(sf::Color::White);
    sf::FloatRect timeBounds = timeText.getLocalBounds();
    timeText.setPosition({790.0f - timeBounds.size.x, 8.0f});
    window.draw(timeText);
    sf::Text invHint(mainFont);
    invHint.setString("[I] Inventory  [P] Puzzle  [ESC] Pause");
    invHint.setCharacterSize(14);
    invHint.setFillColor(sf::Color(150, 150, 150));
    invHint.setPosition({550.0f, 580.0f});
    window.draw(invHint);
    if (inventory->getVisible()) inventory->draw(window);
    if (notificationTimer > 0) {
        notificationText.setString(currentNotification);
        notificationText.setFillColor(notificationColor);
        sf::FloatRect notifBounds = notificationText.getLocalBounds();
        notificationText.setPosition({400.0f - notifBounds.size.x/2.0f, 60.0f});
        window.draw(notificationText);
    }
}

void Game::renderPuzzle() {
    renderPlaying();
    window.draw(overlay);
    if (activePuzzle) activePuzzle->display(window);
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

void Game::changeRoom(int newRoomID) {
    if (rooms.find(newRoomID) != rooms.end()) {
        currentRoomID = newRoomID;
        rooms[currentRoomID]->setVisited(true);
        player->setPosition(100.0f, 300.0f);
        std::cout << "\n→ Moved to: " << rooms[currentRoomID]->getRoomName() << std::endl;
    }
}

void Game::activatePuzzle(std::shared_ptr<Puzzle> puzzle) {
    activePuzzle = puzzle;
    currentState = GameState::PUZZLE_ACTIVE;
    gameTimer->pause();
}

void Game::checkCollisions() {
    auto playerBounds = player->getBounds();
    sf::Vector2f pos = player->getPosition();
    if (pos.x < 0) player->setPosition(0, pos.y);
    if (pos.y < 0) player->setPosition(pos.x, 0);
    if (pos.x > 800 - playerBounds.size.x) player->setPosition(800 - playerBounds.size.x, pos.y);
    if (pos.y > 600 - playerBounds.size.y) player->setPosition(pos.x, 600 - playerBounds.size.y);
}

void Game::checkGuardDetection() {
    auto& guards = rooms[currentRoomID]->getGuards();
    for (auto& guard : guards) {
        if (guard->detectPlayer(*player)) {
            if (!player->isPlayerWarned()) {
                player->warn();
                showNotification("WARNING! Caught by guard!", sf::Color::Yellow, 3.0f);
                gameTimer->subtractTime(5.0f);
            } else {
                showNotification("CAUGHT! Game Over!", sf::Color::Red, 2.0f);
                setGameOver(false);
                return;
            }
        }
    }
}

void Game::checkDoorInteraction() {
    auto& doors = rooms[currentRoomID]->getDoors();
    auto playerBounds = player->getBounds();
    for (auto& door : doors) {
        if (door->checkCollision(playerBounds)) {
            if (door->getLockedStatus()) {
                bool hasKey = false;
                // --- FIXED: Use the actual required key from the door logic ---
                std::string requiredKey = door->getRequiredKey(); 
                
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
                    changeRoom(door->getTargetRoomID());
                } else {
                    showNotification("LOCKED! Need " + requiredKey, sf::Color::Red, 2.0f);
                }
            } else {
                changeRoom(door->getTargetRoomID());
            }
            return;
        }
    }
}

void Game::checkItemPickup() {
    auto& items = rooms[currentRoomID]->getItems();
    auto playerBounds = player->getBounds();
    for (auto& item : items) {
        if (!item->isItemCollected() && item->checkCollision(playerBounds)) {
            item->collect();
            player->addItem(item.get());
            inventory->addItem(item);
            
            if (item->getName() == "Secret Code") {
                Passcode* passcode = dynamic_cast<Passcode*>(item.get());
                if (passcode) showNotification("SECRET CODE: " + passcode->getCode(), sf::Color::Yellow, 10.0f);
            } else {
                showNotification("Picked up: " + item->getName(), sf::Color::Cyan, 2.0f);
            }
        }
    }
}

void Game::checkPuzzleInteraction() {
    auto& puzzles = rooms[currentRoomID]->getPuzzles();
    for (auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            activatePuzzle(puzzle);
            if (currentRoomID == 4) showNotification("Enter code from Room 3 Secret Code!", sf::Color::Magenta, 4.0f);
            return;
        }
    }
}

void Game::checkWinCondition() {
    if (rooms[currentRoomID]->isExit()) {
        bool allPuzzlesSolved = true;
        for (auto& roomPair : rooms) {
            for (auto& puzzle : roomPair.second->getPuzzles()) {
                if (!puzzle->isSolvedStatus()) {
                    allPuzzlesSolved = false;
                    break;
                }
            }
            if (!allPuzzlesSolved) break;
        }
        if (allPuzzlesSolved) setGameOver(true);
        else showNotification("Solve ALL puzzles to escape!", sf::Color::Red, 2.0f);
    }
}

void Game::checkLoseCondition() {
    if (gameTimer->isExpired()) setGameOver(false);
}

void Game::setGameOver(bool victory) {
    currentState = victory ? GameState::VICTORY : GameState::GAME_OVER;
    gameTimer->stop();
}

void Game::pauseGame() {
    currentState = GameState::PAUSED;
    gameTimer->pause();
}

void Game::resumeGame() {
    currentState = GameState::PLAYING;
    gameTimer->resume();
}

void Game::resetGame() {
    currentState = GameState::MENU;
    gameTimer->reset();
}

void Game::showNotification(const std::string& message, const sf::Color& color, float duration) {
    currentNotification = message;
    notificationColor = color;
    notificationTimer = duration;
}