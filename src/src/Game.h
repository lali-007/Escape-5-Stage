#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <map>
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "Item.h"

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    PUZZLE_ACTIVE,
    GAME_OVER,
    VICTORY
};

class Game {
private:
    // Window
    sf::RenderWindow window;
    sf::Clock clock;
    float deltaTime;
    
    // Game state
    GameState currentState;
    
    // Core components
    std::unique_ptr<Player> player;
    std::unique_ptr<Timer> gameTimer;
    std::unique_ptr<Inventory> inventory;
    
    // Rooms
    std::map<int, std::shared_ptr<Room>> rooms;
    int currentRoomID;
    
    // Active puzzle (when player interacts with one)
    std::shared_ptr<Puzzle> activePuzzle;
    
    // Assets (must be declared before Text objects that use them)
    sf::Font mainFont;
    sf::Font defaultFont; // Default font for initialization
    sf::Font notificationFont; // Font for notifications
    sf::Music backgroundMusic;
    
    // UI Elements (declared after fonts)
    sf::Text stateText; // Regular member, initialized in constructor
    sf::RectangleShape overlay; // Dark overlay for pause/puzzle screens
    
    // Notification system
    sf::Text notificationText;
    std::string currentNotification;
    float notificationTimer;
    sf::Color notificationColor;
    
public:
    // Constructor & Destructor
    Game();
    ~Game();
    
    // Game loop
    void run();
    
private:
    // Initialization
    void initialize();
    void loadAssets();
    void createRooms();
    void setupPuzzles();
    
    // Core loop functions
    void processEvents();
    void update();
    void render();
    
    // State-specific handlers
    void handleMenuInput(const sf::Event& event);
    void handlePlayingInput(const sf::Event& event);
    void handlePuzzleInput(const sf::Event& event);
    void handlePauseInput(const sf::Event& event);
    
    void updateMenu();
    void updatePlaying();
    void updatePuzzle();
    void updateGameOver();
    
    void renderMenu();
    void renderPlaying();
    void renderPuzzle();
    void renderGameOver();
    void renderVictory();
    
    // Game mechanics
    void changeRoom(int newRoomID);
    void activatePuzzle(std::shared_ptr<Puzzle> puzzle);
    void checkCollisions();
    void checkGuardDetection();
    void checkDoorInteraction();
    void checkItemPickup();
    void checkPuzzleInteraction();
    
    // Win/Lose conditions
    void checkWinCondition();
    void checkLoseCondition();
    void setGameOver(bool victory);
    
    // Utility
    void resetGame();
    void pauseGame();
    void resumeGame();
    void showNotification(const std::string& message, const sf::Color& color, float duration = 3.0f);
};

#endif // GAME_H