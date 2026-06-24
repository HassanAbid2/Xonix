#pragma once
#include "Global.h"
#include "Player.h"
#include "Enemy.h"

struct GameState {
    int level;
    int totalTiles;
    int filledTiles;
    int gridState[M][N]; // Represents the game grid
    int playerX; // Player's X position
    int playerY; // Player's Y position
    int dx;      // Player's movement in X direction
    int dy;      // Player's movement in Y direction
    int enemyCount;
    Enemy enemies[10]; // Assuming a maximum of 10 enemies
    bool powerupActive;
    float powerupTimer;

    // Constructor to initialize default values
    GameState()
        : level(1), totalTiles(M* N), filledTiles(0),
        playerX(10), playerY(0), dx(0), dy(0),
        enemyCount(0), powerupActive(false), powerupTimer(0.0f) {
        // Initialize gridState with default values (e.g., borders set to 1, inner cells set to 0)
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
                    gridState[i][j] = 1; // Border
                }
                else {
                    gridState[i][j] = 0; // Empty cell
                }
            }
        }
    }
};
