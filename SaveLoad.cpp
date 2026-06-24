#include "SaveLoad.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Helper function to serialize enemies (saves x, y, dx, dy)
std::string serializeEnemies(const Enemy enemies[], int count) {
    std::ostringstream oss;
    for (int i = 0; i < count; ++i) {
        oss << enemies[i].getX() << "," << enemies[i].getY()
            << "," << enemies[i].dx << "," << enemies[i].dy;
        if (i < count - 1)
            oss << ";";
    }
    return oss.str();
}

// Helper function to deserialize enemies (restores x, y, dx, dy)
bool deserializeEnemies(const std::string& data, Enemy enemies[], int& count) {
    std::istringstream iss(data);
    std::string token;
    count = 0;
    while (std::getline(iss, token, ';')) {
        std::istringstream enemyStream(token);
        std::string xStr, yStr, dxStr, dyStr;
        if (!std::getline(enemyStream, xStr, ',') || !std::getline(enemyStream, yStr, ',')) {
            return false;
        }
        // dx/dy are optional for backwards compatibility with old save files
        bool hasDxDy = std::getline(enemyStream, dxStr, ',') && std::getline(enemyStream, dyStr, ',');
        try {
            int x = std::stoi(xStr);
            int y = std::stoi(yStr);
            enemies[count].setX(x);
            enemies[count].setY(y);
            if (hasDxDy && !dxStr.empty() && !dyStr.empty()) {
                enemies[count].dx = std::stoi(dxStr);
                enemies[count].dy = std::stoi(dyStr);
                // Ensure at least 1 unit of movement in some direction
                if (enemies[count].dx == 0 && enemies[count].dy == 0)
                    enemies[count].dx = 4;
            }
            ++count;
            if (count >= 10) break; // Prevent exceeding array size
        }
        catch (...) {
            return false;
        }
    }
    return true;
}

bool SaveLoad::saveGame(const std::string& filename, const GameState& gameState, const Player& player) {
    std::ofstream outFile(filename, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
        return false;
    }

    // Serialize GameState
    outFile << "Level:" << gameState.level << "\n";
    outFile << "TotalTiles:" << gameState.totalTiles << "\n";
    outFile << "FilledTiles:" << gameState.filledTiles << "\n";

    // Serialize grid
    outFile << "Grid:\n";
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            outFile << gameState.gridState[i][j];
            if (j < N - 1)
                outFile << ",";
        }
        outFile << "\n";
    }

    // Serialize player
    outFile << "PlayerX:" << gameState.playerX << "\n";
    outFile << "PlayerY:" << gameState.playerY << "\n";
    outFile << "DX:" << gameState.dx << "\n";
    outFile << "DY:" << gameState.dy << "\n";

    // Serialize enemies
    outFile << "EnemyCount:" << gameState.enemyCount << "\n";
    outFile << "Enemies:" << serializeEnemies(gameState.enemies, gameState.enemyCount) << "\n";

    // Serialize power-up
    outFile << "PowerupActive:" << gameState.powerupActive << "\n";
    outFile << "PowerupTimer:" << gameState.powerupTimer << "\n";

    // Serialize Player data
    outFile << "PlayerData:\n";
    outFile << "PlayerID:" << player.getPlayerID() << "\n";
    outFile << "Username:" << player.getUsername() << "\n";
    outFile << "Nickname:" << player.getNickname() << "\n";
    outFile << "PlayerScore:" << player.getCurrentScore() << "\n"; // Save current score
    outFile << "HighScore:" << player.getHighScore() << "\n";     // Save high score

    outFile.close();
    return true;
}

bool SaveLoad::loadGame(const std::string& filename, GameState& gameState, Player& player) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        size_t delimiterPos = line.find(':');
        if (delimiterPos == std::string::npos) continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        if (key == "Level") {
            gameState.level = std::stoi(value);
        }
        else if (key == "TotalTiles") {
            gameState.totalTiles = std::stoi(value);
        }
        else if (key == "FilledTiles") {
            gameState.filledTiles = std::stoi(value);
        }
        else if (key == "Grid") {
            // Read the grid
            for (int i = 0; i < M; ++i) {
                if (!std::getline(inFile, line)) {
                    std::cerr << "Unexpected end of file while reading grid." << std::endl;
                    return false;
                }
                std::istringstream gridStream(line);
                std::string cell;
                for (int j = 0; j < N; ++j) {
                    if (!std::getline(gridStream, cell, ',')) {
                        std::cerr << "Insufficient grid data." << std::endl;
                        return false;
                    }
                    gameState.gridState[i][j] = std::stoi(cell);
                }
            }
        }
        else if (key == "PlayerX") {
            gameState.playerX = std::stoi(value);
        }
        else if (key == "PlayerY") {
            gameState.playerY = std::stoi(value);
        }
        else if (key == "DX") {
            gameState.dx = std::stoi(value);
        }
        else if (key == "DY") {
            gameState.dy = std::stoi(value);
        }
        else if (key == "EnemyCount") {
            gameState.enemyCount = std::stoi(value);
        }
        else if (key == "Enemies") {
            if (!deserializeEnemies(value, gameState.enemies, gameState.enemyCount)) {
                std::cerr << "Failed to deserialize enemies." << std::endl;
                return false;
            }
        }
        else if (key == "PowerupActive") {
            gameState.powerupActive = (value == "1");
        }
        else if (key == "PowerupTimer") {
            gameState.powerupTimer = std::stof(value);
        }
        else if (key == "PlayerData") {
            // Read Player data
            while (std::getline(inFile, line) && !line.empty()) {
                size_t pos = line.find(':');
                if (pos == std::string::npos) continue;
                std::string pKey = line.substr(0, pos);
                std::string pValue = line.substr(pos + 1);

                if (pKey == "PlayerID") {
                    player.setPlayerID(std::stoi(pValue));
                }
                else if (pKey == "Username") {
                    player.setUsername(pValue);
                }
                else if (pKey == "Nickname") {
                    player.setNickname(pValue);
                }
                else if (pKey == "PlayerScore") {
                    player.setCurrentScore(std::stoi(pValue)); // Restore current score
                }
                else if (pKey == "HighScore") {
                    player.setHighScore(std::stoi(pValue));   // Restore high score
                }
            }
        }
    }

    inFile.close();
    return true;
}