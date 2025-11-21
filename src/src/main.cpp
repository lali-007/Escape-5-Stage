/*
 * Museum Escape - Main Entry Point
 * CS/CE 224/272 - Fall 2025
 * Team: Hamza Sami & Mohammad Yousuf Lali
 */

#include <iostream>
#include "Game.h"

int main() {
    try {
        // Create game instance
        Game game;
        
        // Run the game loop
        game.run();
        
    } catch (const std::exception& e) {
        // Catch any errors and display them
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}