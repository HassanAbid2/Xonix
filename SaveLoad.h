// File: DSA_Project/SaveLoad.h
#pragma once
#include "GameState.h"
#include "Player.h"
#include <string>

class SaveLoad {
public:
    // Saves the current game state to a file
    static bool saveGame(const std::string& filename, const GameState& gameState, const Player& player);

    // Loads the game state from a file
    static bool loadGame(const std::string& filename, GameState& gameState, Player& player);
};
