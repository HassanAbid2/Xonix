#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstdio>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "Enemy.h"
#include "Global.h"
#include "Player.h"
#include "PlayerData.h"
#include "Game.h"
#include "SaveLoad.h"
#include "GameState.h"

const int offsetX = 180;
const int offsetY = 100;

// ========== SIMPLE HELPERS ==========

// Returns: "SavedGames/Game_State_<playerID>.csv"
inline std::string getSaveFilePath(int playerID) {
    char buf[128];
    snprintf(buf, sizeof(buf), "SavedGames/Game_State_%d.csv", playerID);
    return std::string(buf);
}

// Checks if file exists
inline bool fileExists(const std::string& filename) {
    FILE* file = nullptr;
    fopen_s(&file, filename.c_str(), "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Ensures SavedGames directory exists
inline void ensureSaveDirExists() {
#ifdef _WIN32
    _mkdir("SavedGames");
#else
    mkdir("SavedGames", 0777);
#endif
}


// Iterative flood fill (replaces recursive drop to avoid stack overflow on large empty grids)
inline void drop(int startY, int startX)
{
    if (startY < 0 || startY >= M || startX < 0 || startX >= N) return;
    if (grid[startY][startX] != 0) return;

    // Simple stack using two arrays
    int stackY[M * N], stackX[M * N];
    int top = 0;
    stackY[top] = startY;
    stackX[top] = startX;
    top++;
    grid[startY][startX] = -1;

    const int dy4[] = { -1, 1, 0, 0 };
    const int dx4[] = { 0, 0, -1, 1 };

    while (top > 0) {
        top--;
        int y = stackY[top];
        int x = stackX[top];
        for (int d = 0; d < 4; d++) {
            int ny = y + dy4[d];
            int nx = x + dx4[d];
            if (ny >= 0 && ny < M && nx >= 0 && nx < N && grid[ny][nx] == 0) {
                grid[ny][nx] = -1;
                stackY[top] = ny;
                stackX[top] = nx;
                top++;
            }
        }
    }
}

inline void drop2(int startY, int startX) {
    if (startY < 0 || startY >= M || startX < 0 || startX >= N) return;
    if (grid[startY][startX] != 0) return;

    int stackY[M * N], stackX[M * N];
    int top = 0;
    stackY[top] = startY;
    stackX[top] = startX;
    top++;
    grid[startY][startX] = -2;

    const int dy4[] = { -1, 1, 0, 0 };
    const int dx4[] = { 0, 0, -1, 1 };

    while (top > 0) {
        top--;
        int y = stackY[top];
        int x = stackX[top];
        for (int d = 0; d < 4; d++) {
            int ny = y + dy4[d];
            int nx = x + dx4[d];
            if (ny >= 0 && ny < M && nx >= 0 && nx < N && grid[ny][nx] == 0) {
                grid[ny][nx] = -2;
                stackY[top] = ny;
                stackX[top] = nx;
                top++;
            }
        }
    }
}

class GameLogic {
public:
    GameLogic(sf::RenderWindow& window,
        Enemy* enemies,
        int& enemyCount,
        Player* currentPlayer,
        PlayerData& playerData,
        PlayerArray* playerList,
        int& level,
        int& gameState, 
        int& x,
        int& y,
        int& dx,
        int& dy,
        sf::Clock& clock,
        float& timer,
        float delay,
        sf::Text& currentScoreText,
        sf::Text& highScoreText,
        sf::Text& powerupText,
        sf::Text& levelCompleteText,
        sf::Text& levelText,
        sf::Text& levelLeft,
        sf::Text& levelTotal,
        sf::Sprite& tileSprite,  
        sf::Sprite& enemySprite, 
        sf::Sprite& gameoverSprite,
        sf::Sprite& backgroundSprite, 
        int m_totalTiles,
        int m_filledTiles
    )
        : m_window(window), m_enemies(enemies), m_enemyCount(enemyCount), m_currentPlayer(currentPlayer),
        m_playerData(playerData), m_playerList(playerList), m_level(level), m_gameState(gameState),
        m_x(x), m_y(y), m_dx(dx), m_dy(dy), m_clock(clock), m_timer(timer), m_delay(delay),
        m_currentScoreText(currentScoreText), m_highScoreText(highScoreText), m_powerupText(powerupText),
        m_levelCompleteText(levelCompleteText), m_levelText(levelText), m_levelLeft(levelLeft), m_levelTotal(levelTotal),
        m_tileSprite(tileSprite), m_enemySprite(enemySprite),m_gameoverSprite(gameoverSprite),m_backgroungSprite(backgroundSprite),
        m_totalTiles(m_totalTiles), m_filledTiles(m_filledTiles)
    {
    }

    bool loadIfExistsForCurrentPlayer() {
        if (!m_currentPlayer) return false;
        std::string savePath = getSaveFilePath(m_currentPlayer->getPlayerID());
        if (!fileExists(savePath)) return false;
        GameState gs;
        if (SaveLoad::loadGame(savePath, gs, *m_currentPlayer)) {
            return true;
        }
        else {
            std::cout << "Failed to load savegame for current player." << std::endl;
            return false;
        }
    }

    void run() {
        int tilesInRow = 0;
        sf::Event e;

        static bool powerupActive = false;
        static float powerupTimer = 0.0f;
        const float powerupDuration = 3.0f;

        // Process SFML events.
        while (m_window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                m_window.close();
                return;
            }

            if (e.type == sf::Event::KeyPressed &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                e.key.code == sf::Keyboard::S)
            {
                if (m_currentPlayer) {
                    ensureSaveDirExists();
                    GameState gs;
                    // Fill gs with state as before...
                    gs.level = m_level;
                    gs.totalTiles = m_totalTiles;
                    gs.filledTiles = m_filledTiles;
                    for (int i = 0; i < M; ++i)
                        for (int j = 0; j < N; ++j)
                            gs.gridState[i][j] = grid[i][j];
                    gs.playerX = m_x;
                    gs.playerY = m_y;
                    gs.dx = m_dx;
                    gs.dy = m_dy;
                    gs.enemyCount = m_enemyCount;
                    for (int i = 0; i < m_enemyCount; ++i)
                        gs.enemies[i] = m_enemies[i];
                    // If you want to save powerupActive, etc., add here

                    std::string savePath = getSaveFilePath(m_currentPlayer->getPlayerID());
                    if (SaveLoad::saveGame(savePath, gs, *m_currentPlayer)) {
                        cout << "Game saved!" << std::endl;
                    }
                    else {
                        cout << "Failed to save game." << std::endl;
                    }
                }
            }

            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    m_gameState = 3;  // STATE_PAUSED
                }
                else if (e.key.code == sf::Keyboard::L) {
                    // Leaderboard functionality to be handled externally.
                }
                else if (e.key.code == sf::Keyboard::R) {
                    if (m_currentPlayer != nullptr){
                        m_currentPlayer->finalizeGame();
                        m_currentPlayer->resetScore();
                    }
                    for (int i = 1; i < M - 1; i++)
                        for (int j = 1; j < N - 1; j++)
                            grid[i][j] = 0;
                    m_x = 10;
                    m_y = 0;
                    m_dx = m_dy = 0;
                    m_enemyCount = std::min(2 + m_level, 10);
                    m_gameState = 1;  // STATE_PLAYING_SINGLE
                }
                else if (e.key.code == sf::Keyboard::P) {
                    // Powerup activation using the 'P' key
                    if (m_currentPlayer != nullptr && m_currentPlayer->getPowerUpCount() > 0 && !powerupActive) {
                        if (m_currentPlayer->usePowerUp()) {
                            powerupActive = true;
                            powerupTimer = 0.0f;
                            m_powerupText.setString("Your Powerups: " + std::to_string(m_currentPlayer->getPowerUpCount()));
                            // Freeze enemies
                            for (int i = 0; i < m_enemyCount; i++)
                                m_enemies[i].setSpeed(0.0f);
                            std::cout << "Powerup activated! Enemies are frozen for 3 seconds." << std::endl;
                        }
                    }
                    else if (m_currentPlayer != nullptr && m_currentPlayer->getPowerUpCount() <= 0) {
                        std::cout << "No powerups available!" << std::endl;
                    }
                    else if (powerupActive) {
                        std::cout << "Powerup is already active!" << std::endl;
                    }
                    else {
                        std::cout << "No player is currently logged in!" << std::endl;
                    }
                }
            }
        }

        // Handle continuous key presses.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { m_dx = -1; m_dy = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { m_dx = 1; m_dy = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { m_dx = 0; m_dy = -1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { m_dx = 0; m_dy = 1; }

        

        if (m_gameState != 1) {
            return;  // Exit if not in a playable state
        }
        // Movement update.
        float time = m_clock.getElapsedTime().asSeconds();
        m_clock.restart();
        m_timer += time;
        
        // Update powerup timer (if active)
        if (powerupActive) {
            powerupTimer += time;
            float remainingTime = powerupDuration - powerupTimer;
            if (remainingTime > 0)
                m_powerupText.setString("Freezing Enemies: " + std::to_string(static_cast<int>(remainingTime)) + "s");
            else {
                powerupActive = false;
                // Reset enemy speed
                for (int i = 0; i < m_enemyCount; i++) {
                    m_enemies[i].resetSpeed();
                }
                if (m_currentPlayer != nullptr)
                    m_powerupText.setString("Your Powerups: " + std::to_string(m_currentPlayer->getPowerUpCount()));
                std::cout << "Powerup effect ended. Enemies are unfrozen." << std::endl;
            }
        }
        else {
            if (m_currentPlayer != nullptr)
                m_powerupText.setString("Your Powerups: " + std::to_string(m_currentPlayer->getPowerUpCount()));
        }

        if (m_timer > m_delay) {
            m_x += m_dx;
            m_y += m_dy;
            if (m_x < 0) m_x = 0;
            if (m_x > N - 1) m_x = N - 1;
            if (m_y < 0) m_y = 0;
            if (m_y > M - 1) m_y = M - 1;
            if (grid[m_y][m_x] == 2)
                m_gameState = 2;  // STATE_GAMEOVER
            if (grid[m_y][m_x] == 0)
                grid[m_y][m_x] = 2;
            m_timer = 0;
        }



        // Update enemies.
        for (int i = 0; i < m_enemyCount; i++)
            m_enemies[i].move();

        // Process collisions and tile capturing.
        // Only trigger capture if the player was moving (has a trail to close).
        if (grid[m_y][m_x] == 1 && (m_dx != 0 || m_dy != 0)) {
            m_dx = m_dy = 0;
            for (int i = 0; i < m_enemyCount; i++)
                drop(m_enemies[i].getY() / ts, m_enemies[i].getX() / ts);
            tilesInRow = 0;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == -1)
                        grid[i][j] = 0;
                    else if (grid[i][j] == 0 || grid[i][j] == 2) {
                        grid[i][j] = 1;
                        tilesInRow++;
                    }
                }
            }
            if (tilesInRow > 0 && m_currentPlayer != nullptr) {
                m_currentPlayer->tilesCaptured(tilesInRow);
                m_currentScoreText.setString("Current Score: " + std::to_string(m_currentPlayer->getCurrentScore()));
                if (m_currentPlayer->getHighScore() < m_currentPlayer->getCurrentScore()) {
                    m_currentPlayer->setHighScore(m_currentPlayer->getCurrentScore());
                    m_highScoreText.setString("High Score: " + std::to_string(m_currentPlayer->getHighScore()));
                }
                m_playerData.savePlayers(*m_playerList);
            }
        }
        for (int i = 0; i < m_enemyCount; i++) {
            if (grid[m_enemies[i].getY() / ts][m_enemies[i].getX() / ts] == 2) {
                m_gameState = 2;
            }
        }

        // Calculate filled tiles and level threshold.
        int count = 0;
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                if (grid[i][j] == 1)
                    count++;
        int filledTiles = count;
        float totalTiles = static_cast<float>(M * N);
        float levelThreshold = 0.45f + 0.1f * (m_level * 0.5f);

        if (filledTiles >= (totalTiles * levelThreshold)) {
            m_levelCompleteText.setString("Level " + std::to_string(m_level) + " Complete!");

            // Play level animation.
            sf::Clock animClock;
            float animDuration = 3.0f; // Animation lasts for 3 seconds.
            int currentRow = M - 2;
            float rowDelay = 0.03f;
            sf::Clock rowTimer;
            while (currentRow >= 1) {
                if (rowTimer.getElapsedTime().asSeconds() >= rowDelay) {
                    rowTimer.restart();
                    m_window.clear();
                    for (int i = 0; i < M; i++) {
                        for (int j = 0; j < N; j++) {
                            // Draw border always.
                            if (i == 0 || i == M - 1 || j == 0 || j == N - 1)
                                m_tileSprite.setTextureRect(sf::IntRect(0, 36, ts, ts));
                            else if (i >= currentRow) {
                                // Draw the tile if captured or marked.
                                if (grid[i][j] == 1 || grid[i][j] == -1)
                                    m_tileSprite.setTextureRect(sf::IntRect(0, 36, ts, ts));
                            } else
                                continue;
                            m_tileSprite.setPosition(offsetX + (j * ts), offsetY + (i * ts));
                            m_window.draw(m_tileSprite);
                        }
                    }
                    m_window.draw(m_levelCompleteText);
                    m_window.display();
                    currentRow--;
                }
                if (animClock.getElapsedTime().asSeconds() >= animDuration)
                    break;
            }
            
            sf::sleep(sf::seconds(1));
            m_level++;
            m_window.clear();
            m_levelCompleteText.setPosition(820, 500);
            m_levelCompleteText.setString("Level "+to_string(m_level));
            m_window.draw(m_levelCompleteText);
            m_window.display();
            
            sf::sleep(sf::seconds(2));
            for (int i = 1; i < M - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    grid[i][j] = 0;
            m_x = 10;
            m_y = 0;
            m_dx = m_dy = 0;
            m_enemyCount = std::min(2 + m_level, 10);
            m_gameState = 1;
            if (m_currentPlayer != nullptr) {
                m_currentPlayer->tilesCaptured(tilesInRow);
                m_currentPlayer->addWin();
                m_currentScoreText.setString("Current Score: " + std::to_string(m_currentPlayer->getCurrentScore()));
                if (m_currentPlayer->getHighScore() < m_currentPlayer->getCurrentScore()) {
                    m_currentPlayer->setHighScore(m_currentPlayer->getCurrentScore());
                    m_highScoreText.setString("High Score: " + std::to_string(m_currentPlayer->getHighScore()));
                }
                m_playerData.savePlayers(*m_playerList);
            }
        }
        if (m_currentPlayer != nullptr) {
            m_currentScoreText.setString("Current Score: " + std::to_string(m_currentPlayer->getCurrentScore()));
            m_highScoreText.setString("High Score: " + std::to_string(m_currentPlayer->getHighScore()));
        }

        // ---------- Drawing Section ----------
        m_window.clear();
        m_backgroungSprite.setColor(sf::Color(255, 255, 255, 85)); // Reset background color
        m_window.draw(m_backgroungSprite);
        // Draw the grid.
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0)
                    continue;
                if (grid[i][j] == 1)
                    m_tileSprite.setTextureRect(sf::IntRect(36, 0, ts, ts));
                if (grid[i][j] == 2)
                    m_tileSprite.setTextureRect(sf::IntRect(0, 0, ts, ts));
                m_tileSprite.setPosition(offsetX + (j * ts), offsetY + (i * ts));
                m_window.draw(m_tileSprite);
            }
        }
        // Draw the player's trail.
        m_tileSprite.setTextureRect(sf::IntRect(0, 18, ts, ts));
        m_tileSprite.setPosition(offsetX + (m_x * ts), offsetY + (m_y * ts));
        m_window.draw(m_tileSprite);
        // Draw the enemies.
        m_enemySprite.rotate(10);
        for (int i = 0; i < m_enemyCount; i++) {
            m_enemySprite.setPosition(offsetX + m_enemies[i].getX(), offsetY + m_enemies[i].getY());
            m_window.draw(m_enemySprite);
        }

        // Draw additional UI texts.
        m_levelText.setString("Level: " + std::to_string(m_level));
        m_window.draw(m_levelText);
        m_levelLeft.setString(std::to_string(static_cast<int>(((filledTiles) / (float)totalTiles) * 100)) + "%");
        m_window.draw(m_levelLeft);
        m_levelTotal.setString(" (" + std::to_string(static_cast<int>(levelThreshold * 100))+")");
        m_window.draw(m_levelTotal);


        

        m_window.draw(m_currentScoreText);
        m_window.draw(m_highScoreText);
        m_window.draw(m_powerupText);
        
        m_window.display();
    }

    // Helper function to check if there is an empty cell adjacent to both Player1's trail and home base.
    bool canFloodFillPlayer1() {
        for (int i = 1; i < M - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                if (grid[i][j] == 0) {
                    bool adjacentTrail = (grid[i - 1][j] == 2 || grid[i + 1][j] == 2 || grid[i][j - 1] == 2 || grid[i][j + 1] == 2);
                    bool adjacentCaptured = (grid[i - 1][j] == 1 || grid[i + 1][j] == 1 || grid[i][j - 1] == 1 || grid[i][j + 1] == 1);
                    if (adjacentTrail && adjacentCaptured)
                        return true;
                }
            }
        }
        return false;
    }

    // Helper function to check if there is an empty cell adjacent to both Player2's trail and home base.
    bool canFloodFillPlayer2() {
        for (int i = 1; i < M - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                if (grid[i][j] == 0) {
                    bool adjacentTrail = (grid[i - 1][j] == 4 || grid[i + 1][j] == 4 || grid[i][j - 1] == 4 || grid[i][j + 1] == 4);
                    bool adjacentCaptured = (grid[i - 1][j] == 3 || grid[i + 1][j] == 3 || grid[i][j - 1] == 3 || grid[i][j + 1] == 3);
                    if (adjacentTrail && adjacentCaptured)
                        return true;
                }
            }
        }
        return false;
    }

    void run2(Player* player, Player* player2) {
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (i == 0) {
                    if (j < N / 2)
                        grid[i][j] = 1;
                    else
                        grid[i][j] = 3;
                }
                else if (i == M - 1) {
                    if (j < N / 2)
                        grid[i][j] = 1;
                    else
                        grid[i][j] = 3;
                }
                else {
                    if (j == 0)
                        grid[i][j] = 1;
                    else if (j == N - 1)
                        grid[i][j] = 3;
                    else
                        grid[i][j] = 0;  
                }
            }
        }

        int windowWidth = m_window.getSize().x;
        int windowHeight = m_window.getSize().y;
        int gridWidth = N * ts;
        int gridHeight = M * ts;
        int offsetX_local = (windowWidth - gridWidth) / 2;
        int offsetY_local = (windowHeight - gridHeight) / 2;

        // ----------------------
        // Player 1 variables.
        // ----------------------
        int x = 10, y = 0;  // Player1's starting grid coordinates.
        int dx = 0, dy = 0;
        bool Game = true;   // Flag indicating if Player1 is alive.

        // ----------------------
        // Player 2 variables.
        // ----------------------
        int x2 = N - 11, y2 = M - 1;  // Player2's starting grid coordinates.
        int dx2 = 0, dy2 = 0;
        bool Game2 = true;           // Flag indicating if Player2 is alive.

        // Shared movement update timer.
        float timer = 0, delay = 0.07f;

        // Variable for flood fill.
        int tilesInRow = 0;

        // Game timer.
        const float gameDuration = 60.0f;
        float countdown = gameDuration;
        sf::Clock clock;  // For per-frame delta timing

        // ---------------------------
        // Power-up variables
        // ---------------------------
        // For Player1 (WASD controls) power up: use X key.
        static bool powerupActiveP1 = false;
        static float powerupTimerP1 = 0.0f;
        // For Player2 (Arrow keys) power up: use M key.
        static bool powerupActiveP2 = false;
        static float powerupTimerP2 = 0.0f;
        const float powerupDuration = 3.0f;

        while (m_window.isOpen()) {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;
            countdown -= time;

            // Process events.
            sf::Event e;
            while (m_window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    m_window.close();
                if (e.type == sf::Event::KeyPressed) {
                    if (e.key.code == sf::Keyboard::R) {
                        // Clear inner grid and reset players' positions.
                        for (int i = 1; i < M - 1; i++) {
                            for (int j = 1; j < N - 1; j++) {
                                grid[i][j] = 0;
                            }
                        }
                        x = 10; y = 0;
                        x2 = N - 11; y2 = M - 1;
                        Game = true;
                        Game2 = true;
                    }
                    if (e.key.code == sf::Keyboard::Escape) {
                        // Clear inner grid and reset players' positions, then mark both as dead.
                        for (int i = 1; i < M - 1; i++) {
                            for (int j = 1; j < N - 1; j++) {
                                grid[i][j] = 0;
                            }
                        }
                        x = 10; y = 0;
                        x2 = N - 11; y2 = M - 1;
                        Game = false;
                        Game2 = false;
                        m_gameState = 2;
                    }
                    // Power-up activation for Player1 (WASD): use key X.
                    if (e.key.code == sf::Keyboard::X) {
                        if (player->getPowerUpCount() > 0 && !powerupActiveP1) {
                            if (player->usePowerUp()) {
                                powerupActiveP1 = true;
                                powerupTimerP1 = 0.0f;
                                // Freeze enemies for both players.
                                for (int i = 0; i < m_enemyCount; i++)
                                    m_enemies[i].setSpeed(0.0f);
                                std::cout << "Player1 Powerup activated! Enemies and Player2 frozen for "
                                    << powerupDuration << " seconds." << std::endl;
                            }
                        }
                        else if (player->getPowerUpCount() <= 0)
                            std::cout << "Player1: No powerups available!" << std::endl;
                        else if (powerupActiveP1)
                            std::cout << "Player1: Powerup is already active!" << std::endl;
                    }
                    // Power-up activation for Player2 (Arrow keys): use key M.
                    if (e.key.code == sf::Keyboard::M) {
                        if (player2->getPowerUpCount() > 0 && !powerupActiveP2) {
                            if (player2->usePowerUp()) {
                                powerupActiveP2 = true;
                                powerupTimerP2 = 0.0f;
                                // Freeze enemies for both players.
                                for (int i = 0; i < m_enemyCount; i++)
                                    m_enemies[i].setSpeed(0.0f);
                                std::cout << "Player2 Powerup activated! Enemies and Player1 frozen for "
                                    << powerupDuration << " seconds." << std::endl;
                            }
                        }
                        else if (player2->getPowerUpCount() <= 0)
                            std::cout << "Player2: No powerups available!" << std::endl;
                        else if (powerupActiveP2)
                            std::cout << "Player2: Powerup is already active!" << std::endl;
                    }
                }
            }

            // Player1 uses WASD keys.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { dx = -1; dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { dx = 1;  dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { dx = 0;  dy = -1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { dx = 0;  dy = 1; }
            // Player2 uses arrow keys.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { dx2 = -1; dy2 = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { dx2 = 1;  dy2 = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { dx2 = 0;  dy2 = -1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { dx2 = 0;  dy2 = 1; }

            // Continue only if at least one player is alive.
            if (!Game && !Game2)
                return;

            // Power-up timer update for Player1.
            if (powerupActiveP1) {
                powerupTimerP1 += time;
                if (powerupTimerP1 >= powerupDuration) {
                    powerupActiveP1 = false;
                    // Unfreeze enemies if no power-up is active for Player2.
                    if (!powerupActiveP2) {
                        for (int i = 0; i < m_enemyCount; i++)
                            m_enemies[i].resetSpeed();
                    }
                    m_powerupText.setString("Powerups: " + std::to_string(player->getPowerUpCount()));
                    std::cout << "Player1 Powerup effect ended. Enemies unfrozen." << std::endl;
                }
                else {
                    m_powerupText.setString("Freezing Enemies: " +
                        std::to_string(static_cast<int>(powerupDuration - powerupTimerP1)) + "s");
                }
            }
            else {
                m_powerupText.setString("Powerups: " + std::to_string(player->getPowerUpCount()));
            }
            // Power-up timer update for Player2.
            if (powerupActiveP2) {
                powerupTimerP2 += time;
                if (powerupTimerP2 >= powerupDuration) {
                    powerupActiveP2 = false;
                    if (!powerupActiveP1) {
                        for (int i = 0; i < m_enemyCount; i++)
                            m_enemies[i].resetSpeed();
                    }
                    std::cout << "Player2 Powerup effect ended. Enemies unfrozen." << std::endl;
                }
            }

            // Movement update: prevent movement if the opponent's power-up is active.
            if (timer > delay) {
                // Update Player1 only if alive and not frozen by Player2�s powerup.
                if (Game && !powerupActiveP2) {
                    x += dx;
                    y += dy;
                    if (x < 0) x = 0;
                    if (x > N - 1) x = N - 1;
                    if (y < 0) y = 0;
                    if (y > M - 1) y = M - 1;
                    if (grid[y][x] == 2 || grid[y][x] == 3)
                        Game = false;
                    else if (grid[y][x] == 0)
                        grid[y][x] = 2;
                }
                // Update Player2 only if alive and not frozen by Player1�s powerup.
                if (Game2 && !powerupActiveP1) {
                    x2 += dx2;
                    y2 += dy2;
                    if (x2 < 0) x2 = 0;
                    if (x2 > N - 1) x2 = N - 1;
                    if (y2 < 0) y2 = 0;
                    if (y2 > M - 1) y2 = M - 1;
                    if (grid[y2][x2] == 4 || grid[y2][x2] == 1)
                        Game2 = false;
                    else if (grid[y2][x2] == 0)
                        grid[y2][x2] = 4;
                }
                timer = 0;
            }

            // Update enemy movements.
            for (int i = 0; i < m_enemyCount; i++)
                m_enemies[i].move2();

            // Enemy collision detection.
            for (int i = 0; i < m_enemyCount; i++) {
                int cell = grid[m_enemies[i].getY() / ts][m_enemies[i].getX() / ts];
                if (cell == 2)
                    Game = false;
                if (cell == 4)
                    Game2 = false;
            }

            // Flood fill for Player1.
            if (Game && grid[y][x] == 1 && canFloodFillPlayer1()) {
                dx = dy = 0;
                for (int i = 0; i < m_enemyCount; i++)
                    drop(m_enemies[i].getY() / ts, m_enemies[i].getX() / ts);
                tilesInRow = 0;
                for (int i = 0; i < M; i++) {
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1)
                            grid[i][j] = 0;
                        else if (grid[i][j] == 0 || grid[i][j] == 2) {
                            grid[i][j] = 1;
                            tilesInRow++;
                        }
                    }
                }
                // Score counting logic for Player1.
                if (tilesInRow > 0) {
                    player->tilesCaptured(tilesInRow);
                    // Update score UI for Player1.
                    m_currentScoreText.setString("P1 Score: " + std::to_string(player->getCurrentScore()));
                    if (player->getHighScore() < player->getCurrentScore()) {
                        player->setHighScore(player->getCurrentScore());
                        m_highScoreText.setString("P1 High Score: " + std::to_string(player->getHighScore()));
                    }
                    // If the player's nickname is not generic, save their data.
                    if (player->getNickname() != "Player 1")
                        m_playerData.savePlayers(*m_playerList);
                }
            }
            // Flood fill for Player2.
            if (Game2 && grid[y2][x2] == 3 && canFloodFillPlayer2()) {
                dx2 = dy2 = 0;
                for (int i = 0; i < m_enemyCount; i++)
                    drop2(m_enemies[i].getY() / ts, m_enemies[i].getX() / ts);
                tilesInRow = 0;
                for (int i = 0; i < M; i++) {
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -2)
                            grid[i][j] = 0;
                        else if (grid[i][j] == 0 || grid[i][j] == 4) {
                            grid[i][j] = 3;
                            tilesInRow++;
                        }
                    }
                }
                // Score counting logic for Player2.
                if (tilesInRow > 0) {
                    player2->tilesCaptured(tilesInRow);
                    // If the player's nickname is not generic, save their data.
                    if (player2->getNickname() != "Player 2")
                        m_playerData.savePlayers(*m_playerList);
                }
            }

            // ---------- Drawing Section ----------
            m_window.clear();

            // Draw grid.
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0)
                        continue;
                    if (grid[i][j] == 1)
                        m_tileSprite.setTextureRect(sf::IntRect(36, 0, ts, ts));
                    else if (grid[i][j] == 2)
                        m_tileSprite.setTextureRect(sf::IntRect(54, 0, ts, ts));
                    else if (grid[i][j] == 3)
                        m_tileSprite.setTextureRect(sf::IntRect(0, 36, ts, ts));
                    else if (grid[i][j] == 4)
                        m_tileSprite.setTextureRect(sf::IntRect(18, 36, ts, ts));
                    m_tileSprite.setPosition(offsetX_local + (j * ts), offsetY_local + (i * ts));
                    m_window.draw(m_tileSprite);
                }
            }

            // Draw player sprites.
            m_tileSprite.setTextureRect(sf::IntRect(0, 18, ts, ts));
            m_tileSprite.setPosition(offsetX_local + (x * ts), offsetY_local + (y * ts));
            m_window.draw(m_tileSprite);
            m_tileSprite.setTextureRect(sf::IntRect(80, 18, ts, ts));
            m_tileSprite.setPosition(offsetX_local + (x2 * ts), offsetY_local + (y2 * ts));
            m_window.draw(m_tileSprite);

            // Draw enemies.
            m_enemySprite.rotate(10);
            for (int i = 0; i < m_enemyCount; i++) {
                m_enemySprite.setPosition(offsetX_local + m_enemies[i].x, offsetY_local + m_enemies[i].y);
                m_window.draw(m_enemySprite);
            }

            // Prepare UI texts for both players.
            std::string player1Info;
            std::string player2Info;
            if (!Game)
                player1Info = player->getNickname() + " | DEAD";
            else
                player1Info = player->getNickname() + "\nScore: " + std::to_string(player->getCurrentScore()) +
                "\nHigh Score: " + std::to_string(player->getHighScore()) +
                "\nPowerUps: " + std::to_string(player->getPowerUpCount());
            if (!Game2)
                player2Info = player2->getNickname() + " | DEAD";
            else
                player2Info = player2->getNickname() + "\nScore: " + std::to_string(player2->getCurrentScore()) +
                "\nHigh Score: " + std::to_string(player2->getHighScore()) +
                "\nPowerUps: " + std::to_string(player2->getPowerUpCount());

            sf::Text p1InfoText = m_currentScoreText;
            p1InfoText.setString(player1Info);
            p1InfoText.setPosition(20, 15);
            m_window.draw(p1InfoText);

            sf::Text p2InfoText = m_currentScoreText;
            p2InfoText.setString(player2Info);
            // Changed the position as per your changes.
            p2InfoText.setPosition(windowWidth - 220, 15);
            m_window.draw(p2InfoText);

            m_powerupText.setPosition((windowWidth / 2) - 50, offsetY_local - 20);
            m_powerupText.setString("Time: " + std::to_string(static_cast<int>(countdown)));
            m_window.draw(m_powerupText);

            m_window.display();

            // Termination: game ends if both players are dead or timer runs out.
            if (((!Game) && (!Game2)) || (countdown <= 0)) {
                m_gameState = 2; // Game Over

                std::string winner;
                if (!Game && !Game2) { // Both players are dead: compare scores.
                    if (player->getCurrentScore() > player2->getCurrentScore())
                        winner = "Winner: " + std::string(player->getNickname());
                    else if (player->getCurrentScore() < player2->getCurrentScore())
                        winner = "Winner: " + std::string(player2->getNickname());
                    else
                        winner = "Tie!";
                }
                else if (countdown <= 0) { // Timer ended, check alive status.
                    if (Game && !Game2)
                        winner = "Winner: " + std::string(player->getNickname());
                    else if (!Game && Game2)
                        winner = "Winner: " + std::string(player2->getNickname());
                    else { // Both alive: compare scores.
                        if (player->getCurrentScore() > player2->getCurrentScore())
                            winner = "Winner: " + std::string(player->getNickname());
                        else if (player->getCurrentScore() < player2->getCurrentScore())
                            winner = "Winner: " + std::string(player2->getNickname());
                        else
                            winner = "Tie!";
                    }
                }
                else { // Early termination due to collision.
                    if (!Game && Game2)
                        winner = "Winner: " + std::string(player2->getNickname());
                    else if (!Game2 && Game)
                        winner = "Winner: " + std::string(player->getNickname());
                    else {
                        if (player->getCurrentScore() > player2->getCurrentScore())
                            winner = "Winner: " + std::string(player->getNickname());
                        else if (player->getCurrentScore() < player2->getCurrentScore())
                            winner = "Winner: " + std::string(player2->getNickname());
                        else
                            winner = "Tie!";
                    }
                }

                sf::Text winnerText = m_currentScoreText;
                winnerText.setString(winner);
                int textX = (windowWidth - static_cast<int>(winnerText.getGlobalBounds().width)) / 2;
                int textY = (windowHeight - static_cast<int>(winnerText.getGlobalBounds().height)) / 2;
                winnerText.setPosition(textX, textY);
                m_window.clear();
                m_window.draw(winnerText);
                m_window.display();
                sf::sleep(sf::seconds(3));
                break;
            }
        } // End main game loop

        std::cout << "Game Over!" << std::endl;
    }

    void setCurrentPlayer(Player* player) {
        m_currentPlayer = player;
    }

private:
    sf::RenderWindow& m_window;
    Enemy* m_enemies;
    int& m_enemyCount; 
    Player* m_currentPlayer;
    PlayerData& m_playerData;
    PlayerArray* m_playerList;
    int& m_level;
    int& m_gameState;
    int& m_x;
    int& m_y;
    int& m_dx;
    int& m_dy;
    sf::Clock& m_clock;
    float& m_timer;
    float m_delay;
    sf::Text& m_currentScoreText;
    sf::Text& m_highScoreText;
    sf::Text& m_powerupText;
    sf::Text& m_levelCompleteText;
    sf::Text& m_levelText;
    sf::Text& m_levelLeft;
    sf::Text& m_levelTotal;
    sf::Sprite& m_tileSprite;
    sf::Sprite& m_enemySprite;
    sf::Sprite& m_gameoverSprite;
    sf::Sprite& m_backgroungSprite;
    int m_totalTiles;
    int m_filledTiles;

};