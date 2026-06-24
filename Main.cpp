#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include<sstream>
#include <time.h>
#include "Enemy.h"
#include "Global.h"
#include "Player.h"
#include "Text.h"
#include "Game.h" 
#include "PlayerData.h" 
#include "HashTable.h"
#include "FriendSystem.h" 
#include"GameLogic.h"
#include"PriorityQueue.h"
#include"Inventory.h"

using namespace std;
using namespace sf;


Text currentScoreText;
Text highScoreText;
Text powerupText;


bool initializeScoreTexts(Font& font) {
    // Initialize Current Score Text
    currentScoreText.setFont(font);
    currentScoreText.setCharacterSize(15);
    currentScoreText.setFillColor(Color::White);
    currentScoreText.setPosition(10, 10); // Top-left corner

    // Initialize High Score Text
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(15);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(10, 40); // Below Current Score

    // Initialize Powerup Text (positioned at bottom-left)
    powerupText.setFont(font);
    powerupText.setCharacterSize(15);
    powerupText.setFillColor(Color::White);
    powerupText.setPosition(10, 70);

    return true;
}

void animateMultiplayerMatchFound(sf::RenderWindow& window, Player* currentPlayer, Player* opponent, const sf::Font& font);
void showOpeningScreen(RenderWindow& window, Font& font, Font& creditsFont);
void displayGameOver(RenderWindow& window, const Font& font, int score);

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    int level = 1;
    int totalTiles = M * N;
    int filledTiles = 0;
    const int offsetX = (1920 - N * ts) / 2;
    const int offsetY = (1080 - M * ts) / 2;

    // Initialize the window in Fullscreen mode
    RenderWindow window(VideoMode(1920, 1080), "! XONIX !", Style::Fullscreen);
    window.setFramerateLimit(60);

    // Load fonts
    Font font2;
    if (!font2.loadFromFile("fonts/joystix.otf"))
    {
        cout << "Error loading joystix.otf" << endl;
        return -1;
    }
    Font font3;
    if (!font3.loadFromFile("fonts/Rovelink.otf"))
    {
        cout << "Error loading Rovelink.otf" << endl;
        return -1;
    }
    Font font4;
    if (!font4.loadFromFile("fonts/Noxis.otf"))
    {
        cout << "Error loading Noxis.otf" << endl;
        return -1;
    }
    Font font5;
    if (!font5.loadFromFile("fonts/Saphira.otf")) {
        cout << "Error loading Saphira.otf" << endl;
    }

    if (!initializeScoreTexts(font2)) {
        cout << "Error initializing score texts." << endl;
        return -1;
    }

    // Load textures with error checks
    Texture t1, t2, t3, t4;
    if (!t1.loadFromFile("images/tilesssss.png"))
    {
        cout << "Error loading tilesssss.png" << endl;
        return -1;
    }
    if (!t2.loadFromFile("images/gameover.png"))
    {
        cout << "Error loading gameover.png" << endl;
        return -1;
    }
    if (!t3.loadFromFile("images/enemy.png"))
    {
        cout << "Error loading enemy.png" << endl;
        return -1;
    }
    if (!t4.loadFromFile("images/youwin(2).png"))
    {
        cout << "Error loading youwin(2).png" << endl;
        return -1;
    }
    Sprite sTile(t1), sGameover(t2), sEnemy(t3), sYouWin(t4);
    sGameover.setPosition(100, 100);
    sYouWin.setPosition(140, 60);
    sEnemy.setOrigin(20, 20);

    if (!loadFontAndSetupText())
        return -1;

    Enemy* a = new Enemy[10]; 

    int enemyCount = 2;
    int Game = 1;
    int x = 0, y = 0, dx = 0, dy = 0;
    float timer = 0, delay = 0.07f;
    Clock clock;

    // Powerup variables
    bool powerupActive = false;
    float powerupTimer = 0.0f;
    const float powerupDuration = 3.0f;

    // Initialize PlayerData with filename and load players into a PlayerArray (primary code uses PlayerArray)
    PlayerData playerData("PlayerData.csv");
    PlayerArray* playerList = new PlayerArray(playerData.loadPlayers());

    // Current player pointer
    Player* currentPlayer = nullptr;

    int currentState = STATE_MAIN_SCREEN;

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
        cout << "Error loading background_menu2.jpg" << endl;
        return -1;
    }
    Sprite backgroundSprite(backgroundTexture);

    // Instantiate Screens and Menus (including additional screens from second file)
    GameLogic gameLogic(window, a, enemyCount, currentPlayer, playerData, playerList,
        level, Game, x, y, dx, dy, clock, timer, delay,
        currentScoreText, highScoreText, powerupText,
        levelCompleteText, levelText, levelLeft, levelTotal,
        sTile, sEnemy, sGameover,backgroundSprite,totalTiles,filledTiles);

    MainScreen mainScreen(window);
    LoginScreen loginScreen(window);
    MainMenu mainMenu(window, font3, font4);
    StartMenu startMenu(window, font3, font4, font5);
    SignUpScreen signUpScreen(window, font4, font3, font5);
    MatchSelectionScreen matchSelectionScreen(window, font3, font4); 
    PauseMenu pauseMenu(window);
    LeaderboardScreen leaderboardScreen(window);
    MultiplayerScreen multiplayerScreen(window, font3, font4); 
    Inventory inventory;
    InventoryScreen inventoryScreen(window, inventory, currentPlayer);

    HashTable hashTable;
    hashTable.setPlayerArray(playerList->getPlayers(), playerList->getSize());
    cout << "\nsize: " << playerList->getSize() << "\n";
    for (int i = 0; i < playerList->getSize(); ++i) {
        cout << "Adding: " << playerList->getPlayers()[i].getPlayerID() << " " << playerList->getPlayers()[i].getUsername() << endl;
        hashTable.insert(playerList->getPlayers()[i].getPlayerID(), i);
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    Music music;
    if (!music.openFromFile("5050.ogg")) {
        cerr << "Error: Could not load music file!" << endl;
        return -1;
    }

    // Set music properties
    music.setLoop(true);   // Loop the music
    music.setVolume(50);   // Set volume to 50%

    // Play the music
    music.play();

    bool hasLoadedSaveGame = false; // Flag to prevent repeated file checks
    showOpeningScreen(window, font3, font2);

    // ----------------------- MAIN GAME LOOP -----------------------
    while (window.isOpen())
    {
        switch (currentState)
        {
        case STATE_MAIN_SCREEN: {
            // Display opening screen and then proceed to start menu 
            if (mainScreen.display()) {
                currentState = STATE_START_MENU;
            }
            break;
        }
        case STATE_LOGIN: {
            // Display login screen; note: in this merged version, upon successful login we transition to match selection
            if (loginScreen.display(playerList)) {
                string username = loginScreen.getUsername();
                string password = loginScreen.getPassword();
                cout << "Attempting to log in as: " << username << endl;
                Player* foundPlayer = nullptr;
                for (int i = 0; i < playerList->getSize(); ++i) {
                    if (playerList->getPlayers()[i].getUsername() == username) {
                        if (playerList->getPlayers()[i].getPassword() == password) {
                            foundPlayer = &playerList->getPlayers()[i];
                            break;
                        }
                        else {
                            cout << "Incorrect password for username: " << username << endl;
                            break;
                        }
                    }
                }
                if (foundPlayer) {
                    currentPlayer = foundPlayer;
                    cout << "**Logged in as**: " << currentPlayer->getUsername() << endl;
                    // Transition to match selection instead of main menu so player can choose mode
                    currentState = STATE_MATCH_SELECTION;
                    gameLogic.setCurrentPlayer(currentPlayer);
                }
                else {
                    cout << "Login failed. Please try again or sign up." << endl;
                    currentState = STATE_LOGIN;
                }
            }
            break;
        }

        case STATE_SIGNUP: {
            if (signUpScreen.display(*playerList)) {
                string newUsername = signUpScreen.getUsername();
                string newPassword = signUpScreen.getPassword();
                string nickname = signUpScreen.getNickname();
                string profilePic = "default.png";

                bool exists = false;
                for (int i = 0; i < playerList->getSize(); ++i) {
                    if (playerList->getPlayers()[i].getUsername() == newUsername) {
                        exists = true;
                        break;
                    }
                }
                if (exists) {
                    cout << "Username already exists. Please choose a different username." << endl;
                    currentState = STATE_SIGNUP;
                }
                else {
                    int newPlayerID = 1;
                    for (int i = 0; i < playerList->getSize(); ++i) {
                        if (playerList->getPlayers()[i].getPlayerID() >= newPlayerID)
                            newPlayerID = playerList->getPlayers()[i].getPlayerID() + 1;
                    }
                    Player newPlayer(newPlayerID, newUsername, newPassword, nickname, profilePic);
                    playerList->addPlayer(newPlayer);
                    currentPlayer = &playerList->getPlayers()[playerList->getSize() - 1];
                    cout << "Signed up with Username: " << currentPlayer->getUsername() << endl;
                    if (!playerData.savePlayers(*playerList)) {
                        cout << "Error saving player data." << endl;
                    }
                    currentState = STATE_MATCH_SELECTION;
                }
            }
            break;
        }

        case STATE_START_MENU: {
            int startMenuSelection = startMenu.display();
            if (startMenuSelection == -1 || startMenuSelection == 3) {
                window.close();
                return 0;
            }
            else if (startMenuSelection == 1) {
                currentState = STATE_LOGIN;
            }
            else if (startMenuSelection == 2) {
                currentState = STATE_SIGNUP;
            }
            else if (startMenuSelection == 0) {
                // Directly start game in single-player mode if desired
                currentState = STATE_PLAYING_SINGLE;
            }
            break;
        }

        case STATE_MATCH_SELECTION: {
            // Display match selection screen to choose game mode
            int selection = matchSelectionScreen.display();
            if (selection == 1) {
                // Directly transition to Offline Multiplayer mode
                currentState = STATE_MAIN_MENU;
            }
            else if (selection == 2) {
                // Display multiplayer screen and decide between offline and matchmaking mode
                int multiSelection = multiplayerScreen.display();
                if (multiSelection == 1) {
                    // Choose Offline Multiplayer mode from the multiplayer screen
                    currentState = STATE_PLAYING_MULTI;
                }
                else if (multiSelection == 2) {
                    // Matchmaking mode selected
                    currentState = STATE_PLAYING_MATCH;
                }
                else if (multiSelection == -1) {
                    window.close();
                    return 0;
                }
            }
            else if (selection == -1) {
                window.close();
                return 0;
            }
            break;
        }

        case STATE_OFFLINE_MULTIPLAYER_SCREEN: {
            // Placeholder for offline multiplayer screen (can be expanded)
            currentState = STATE_MATCH_SELECTION;
            break;
        }

        case STATE_LEADERBOARD: {
            leaderboardScreen.display();
            currentState = STATE_MAIN_MENU;
            break;
        }

        case STATE_MAIN_MENU: {
            static int cachedThemeID = -1; // Cache the last loaded theme ID
            if (currentPlayer) {
                int themeID = currentPlayer->getSelectedThemeID();

                // Only load the texture if the theme has changed
                if (themeID != cachedThemeID) {
                    cachedThemeID = themeID; // Update cached theme ID
                    Theme* selectedTheme = inventory.getThemeByID(themeID);

                    if (selectedTheme && !selectedTheme->backgroundImage.empty()) {
                        cout << "Loading background for Theme ID: " << themeID << endl;
                        if (!backgroundTexture.loadFromFile(selectedTheme->backgroundImage)) {
                            cout << "Error loading background image for Theme ID: " << themeID << endl;
                            // Fallback to default background
                            if (!backgroundTexture.loadFromFile("images/default_background.jpg")) {
                                cout << "Error loading default background image." << endl;
                            }
                        }
                        else {
                            backgroundSprite.setTexture(backgroundTexture);
                            cout << "Background image updated to Theme ID: " << themeID << endl;
                        }
                    }
                    else {
                        cout << "Selected theme not found or background image is empty. Loading default background." << endl;
                        if (!backgroundTexture.loadFromFile("images/default_background.jpg")) {
                            cout << "Error loading default background image." << endl;
                        }
                        else {
                            backgroundSprite.setTexture(backgroundTexture);
                        }
                    }
                }
            }
            else {
                // Handle case when no player is logged in
                if (cachedThemeID != -1) { // Only load default background once
                    cachedThemeID = -1; // Reset the cached theme ID
                    cout << "No player logged in. Loading default background." << endl;
                    if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
                        cout << "Error loading default background image: background_menu2.jpg" << endl;
                    }
                    else {
                        backgroundSprite.setTexture(backgroundTexture);
                    }
                }
            }

            // Render the main menu with the updated background
            window.clear();
            window.draw(backgroundSprite); // Draw the updated background
            mainMenu.display(*currentPlayer);

            int mainMenuSelection = mainMenu.getSelectedOption();
            if (mainMenuSelection == -1 || mainMenuSelection == 3) {
                window.close();
                return 0;
            }
            else if (mainMenuSelection == 1) {
                // Leaderboard functionality
                currentState = STATE_LEADERBOARD;
            }
            else if (mainMenuSelection == 2) {
                currentState = STATE_INVENTORY;
            }
            else if (mainMenuSelection == 0) {
                // Start single-player game
                for (int i = 0; i < M; i++) {
                    for (int j = 0; j < N; j++) {
                        if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                            grid[i][j] = 1;
                        else
                            grid[i][j] = 0;
                    }
                }

                if (!initializeScoreTexts(font2)) {
                    cout << "Error initializing score texts." << endl;
                    return -1;
                }

                x = 10;
                y = 0;
                dx = 0;
                dy = 0;
                enemyCount = min(2 + level, 10);
                currentState = STATE_PLAYING_SINGLE;
                window.clear();

                Game = 1;
                level = 1;
                if (currentPlayer != nullptr) {
                    currentPlayer->resetScore();
                }
            }
            else if (mainMenuSelection == 4) { // Friends/Friend Requests
                RenderWindow friendsWindow(VideoMode(600, 500), "Friends & Requests", Style::Titlebar | Style::Close);
                displayFriendSystem(friendsWindow, currentPlayer->getFriendList(), currentPlayer->getFriendRequestList(), currentPlayer->getPlayerID(), hashTable);
                if (!playerData.savePlayers(*playerList)) {
                    cerr << "Failed to save player data after adding a friend." << endl;
                }
            }
            else if (mainMenuSelection == 5) { // Log Out
                currentState = STATE_LOGIN;
            }
            break;
        }

        case STATE_INVENTORY: {
            Event event;
            bool exitInventory = false;

            // Event handling for Inventory
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                }
                inventoryScreen.handleEvent(event, exitInventory);
            }

            if (exitInventory) {
                currentState = STATE_MAIN_MENU;
                break;
            }

            // Render Inventory Screen
            inventoryScreen.render();

            break;
        }

        case STATE_PLAYING_SINGLE: {
            if (!hasLoadedSaveGame) {
                hasLoadedSaveGame = true;

                if (currentPlayer != nullptr) {
                    string saveFilePath = getSaveFilePath(currentPlayer->getPlayerID());
                    if (fileExists(saveFilePath)) {
                        // Load saved game
                        GameState savedState;
                        if (SaveLoad::loadGame(saveFilePath, savedState, *currentPlayer)) {
                            level = savedState.level;
                            totalTiles = savedState.totalTiles;
                            filledTiles = savedState.filledTiles;
                            for (int i = 0; i < M; i++) {
                                for (int j = 0; j < N; j++) {
                                    grid[i][j] = savedState.gridState[i][j];
                                }
                            }
                            x = savedState.playerX;
                            y = savedState.playerY;
                            dx = savedState.dx;
                            dy = savedState.dy;
                            enemyCount = savedState.enemyCount;
                            for (int i = 0; i < enemyCount; i++) {
                                a[i].setX(savedState.enemies[i].getX());
                                a[i].setY(savedState.enemies[i].getY());
                            }
                            powerupActive = savedState.powerupActive;
                            powerupTimer = savedState.powerupTimer;
                            cout << "Loaded saved game for player: " << currentPlayer->getUsername() <<endl;
                        }
                        else {
                            cerr << "Failed to load the saved game. Starting a new game." << endl;
                        }
                    }
                    else {
                        cout << "No saved game found. Starting a new game." << endl;
                    }
                }
                else {
                    cout << "No player logged in. Starting a new game." << endl;
                }

                if (currentPlayer == nullptr || !fileExists(getSaveFilePath(currentPlayer->getPlayerID()))) {
                    for (int i = 0; i < M; i++) {
                        for (int j = 0; j < N; j++) {
                            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                                grid[i][j] = 1;
                            else
                                grid[i][j] = 0;
                        }
                    }
                    x = 10;
                    y = 0;
                    dx = 0;
                    dy = 0;
                    enemyCount = min(2 + level, 10);
                    if (currentPlayer != nullptr) {
                        currentPlayer->resetScore();
                    }
                }
            }

            // Start the game
            Game = 1;
            window.clear();
            gameLogic.run();

            if (Game == 2) {
                currentPlayer->addLoss();
                currentPlayer->finalizeGame();
                playerData.savePlayers(*playerList);
                currentState = STATE_GAMEOVER;
                hasLoadedSaveGame = false;
            }
            else if (Game == 3) {
                currentState = STATE_PAUSED;
            }
            break;
        }

        case STATE_GAMEOVER: {
            displayGameOver(window, font2, currentPlayer->getCurrentScore());
            currentPlayer->setCurrentScore(0);
            currentState = STATE_MAIN_MENU;
            break;
        }

        case STATE_PLAYING_MULTI: {

            Player dummyPlayer1(1, "Player1", "dummy", "Player 1", "dummy1.png");
            Player dummyPlayer2(2, "Player2", "dummy", "Player 2", "dummy2.png");
            window.clear();
            gameLogic.run2(&dummyPlayer1, &dummyPlayer2);

            if (Game == 2) {
                currentState = STATE_MATCH_SELECTION;
            }
            else if (Game == 3) {
                currentState = STATE_PAUSED;
            }
            break;
        }

        case STATE_PLAYING_MATCH: {
            MatchmakingQueue matchQueue;
            // iterate over all players in the player list.
            for (int i = 0; i < playerList->getSize(); i++) {
                Player* candidate = &playerList->getPlayers()[i];

                // Skip if the candidate is the current player.
                if (candidate->getUsername() == currentPlayer->getUsername())
                    continue;

                // Calculate difference as absolute difference between current player's score and candidate's currentScore.
                int diff = abs(candidate->getTotalPoints() - currentPlayer->getTotalPoints());
                matchQueue.insert(candidate, diff);
            }

            // Get the best match(sabse kam farq)
            Player* bestMatch = matchQueue.getBestMatch();
            //dummy
            if (bestMatch == nullptr) {
                cout << "No match found. Falling back to offline match." << endl;
                bestMatch = new Player(999, "Offline", "dummy", "Player 2", "offline.png");
            }

            animateMultiplayerMatchFound(window, currentPlayer, bestMatch, font2);

            // For debugging, print the chosen match's username and score.
            cout << "Matchmaking: current player " << currentPlayer->getUsername()
                << " (score " << currentPlayer->getCurrentScore() << ") vs. "
                << bestMatch->getUsername() << " (score " << bestMatch->getCurrentScore() << ")" << endl;

            // Call the game logic run method with the currentPlayer and bestMatch.
            window.clear();
            // run2 starts a match between two players.
            gameLogic.run2(currentPlayer, bestMatch);

            if (Game == 2) {
                // Transition state accordingly
                currentState = STATE_MATCH_SELECTION;
            }
            else if (Game == 3) {
                // Player wins
                currentState = STATE_PAUSED;
            }
            break;
        }

        case STATE_PAUSED: {
            pauseMenu.display();
            int pauseMenuSelection = pauseMenu.getSelectedOption();
            if (pauseMenuSelection == -1)
            {
                window.close();
                delete[] a;
                return 0;
            }
            else if (pauseMenuSelection == 0)
            {
                Game = 1;
                currentState = STATE_PLAYING_SINGLE;
                window.clear();
            }
            else if (pauseMenuSelection == 1)
            {
                for (int i = 1; i < M - 1; i++)
                    for (int j = 1; j < N - 1; j++)
                        grid[i][j] = 0;
                x = 10;
                y = 0;
                dx = 0;
                dy = 0;
                enemyCount = min(2 + level, 10);
                currentState = STATE_PLAYING_SINGLE;
                window.clear();

                Game = 1;
                level = 1;


                if (currentPlayer != nullptr) {
                    currentPlayer->resetScore();
                }

            }
            else if (pauseMenuSelection == 2)
            {
                currentState = STATE_MAIN_MENU;
            }
        }

        default:
            break;
        } // end switch
    } // end while window.isOpen()



    if (!playerData.savePlayers(*playerList)) {
        cout << "Error saving player data." << endl;
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\nfilled tiles: " << filledTiles;
    cout << "\ntotal tiles: " << totalTiles;

   
    return 0;
}

void displayGameOver(RenderWindow& window, const Font& font, int score) {
    // Create text objects for the "Game Over" message
    Text gameOverText("Game Over", font, 100);
    gameOverText.setFillColor(sf::Color::Cyan);
    FloatRect gameOverBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverBounds.left + gameOverBounds.width / 2, gameOverBounds.top + gameOverBounds.height / 2);
    gameOverText.setPosition(window.getSize().x / 2, window.getSize().y / 3);

    // Create text for the player's score
    Text scoreText("Your Score: " + to_string(score), font, 50);
    scoreText.setFillColor(sf::Color::White);
    FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2, scoreBounds.top + scoreBounds.height / 2);
    scoreText.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    // Create text for returning to the main menu
    Text returnText("Returning to Main Menu...", font, 30);
    returnText.setFillColor(Color::Yellow);
    FloatRect returnBounds = returnText.getLocalBounds();
    returnText.setOrigin(returnBounds.left + returnBounds.width / 2, returnBounds.top + returnBounds.height / 2);
    returnText.setPosition(window.getSize().x / 2, window.getSize().y * 2 / 3);

    // Use a clock to control the duration of the display
    Clock clock;

    while (clock.getElapsedTime().asSeconds() < 3.0f) { // Display for 3 seconds
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return;
            }
        }

        // Clear and draw the texts
        window.clear(Color::Black);  // Black background
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(returnText);
        window.display();
    }
}

void animateMultiplayerMatchFound(RenderWindow& window, Player* currentPlayer, Player* opponent, const Font& font) {
    // Create text objects.
    sf::Text title("Match Found!", font, 50);
    sf::Text currentPlayerText("", font, 30);
    sf::Text opponentText("", font, 30);
    sf::Text vsText("VS", font, 40);

    // Set initial colors with 0 alpha.
    title.setFillColor(sf::Color::Cyan);           // Yellow
    currentPlayerText.setFillColor(sf::Color(255, 255, 255, 0)); // White
    opponentText.setFillColor(sf::Color(255, 255, 255, 0));      // White
    vsText.setFillColor(sf::Color(255, 255, 255, 0));            // White

    // Position title at top center.
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2, titleBounds.top + titleBounds.height / 2);
    title.setPosition(window.getSize().x / 2, window.getSize().y * 0.2);

    // Set up current player's info.
    stringstream cpStream;
    cpStream << currentPlayer->getNickname()
        << "\nTotal Points: " << currentPlayer->getTotalPoints();
    currentPlayerText.setString(cpStream.str());
    // Position current player on left side (centered vertically).
    FloatRect cpBounds = currentPlayerText.getLocalBounds();
    currentPlayerText.setPosition(window.getSize().x * 0.2 - cpBounds.width / 2, window.getSize().y / 2 - cpBounds.height / 2);

    // Set up opponent's info.
    stringstream opStream;
    opStream << opponent->getNickname()
        << "\nTotal Points: " << opponent->getTotalPoints();
    opponentText.setString(opStream.str());
    // Position opponent on right side (centered vertically).
    FloatRect opBounds = opponentText.getLocalBounds();
    opponentText.setPosition(window.getSize().x * 0.8 - opBounds.width / 2, window.getSize().y / 2 - opBounds.height / 2);

    // Setup the VS text in the center.
    FloatRect vsBounds = vsText.getLocalBounds();
    vsText.setOrigin(vsBounds.left + vsBounds.width / 2, vsBounds.top + vsBounds.height / 2);
    vsText.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    // Animation timing parameters.
    const float durationFadeIn = 2.0f;   // seconds for fade in
    const float durationHold = 1.0f;   // seconds hold full opacity
    const float durationFadeOut = 2.0f;   // seconds for fade out

    Clock clock;
    bool animationDone = false;
    while (!animationDone) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return;
            }
        }

        float timeElapsed = clock.getElapsedTime().asSeconds();
        float alpha = 0.f;

        if (timeElapsed < durationFadeIn) {
            // Fade in: alpha increases from 0 to 255.
            alpha = (timeElapsed / durationFadeIn) * 255;
        }
        else if (timeElapsed < durationFadeIn + durationHold) {
            // Hold phase: alpha remains at maximum.
            alpha = 255;
        }
        else if (timeElapsed < durationFadeIn + durationHold + durationFadeOut) {
            // Fade out: alpha decreases from 255 to 0.
            float fadeOutTime = timeElapsed - durationFadeIn - durationHold;
            alpha = 255 - (fadeOutTime / durationFadeOut) * 255;
        }
        else {
            animationDone = true;
            break;
        }

        // Update the color alpha values.
        title.setFillColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(alpha)));
        currentPlayerText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        opponentText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        vsText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));

        window.clear();
        // Optionally, draw a background that matches your opening screen's theme here.
        window.draw(title);
        window.draw(currentPlayerText);
        window.draw(opponentText);
        window.draw(vsText);
        window.display();
    }
}

void showOpeningScreen(RenderWindow& window, Font& font, Font& creditsFont)
{
    Text title("XONIX", font, 170);
    Text credits(" By Talha\nand Hassan", creditsFont, 40);
    Color neonBlue(0, 170, 255); // Darker cyan-like neon

    title.setFillColor(Color::Transparent);
    credits.setFillColor(Color::Transparent);
    title.setOutlineColor(Color::White);
    credits.setOutlineColor(Color::White);
    title.setOutlineThickness(0.4f);
    credits.setOutlineThickness(0.7f);

    FloatRect titleBounds = title.getLocalBounds();
    FloatRect creditsBounds = credits.getLocalBounds();
    Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    title.setPosition(380, 355 + 120);
    title.setRotation(355);
    credits.setPosition(380 + 730, 355 + 160);

    Clock clock;
    float opacity = 0;
    while (opacity < 255)
    {
        float dt = clock.restart().asSeconds();
        opacity += 100 * dt;
        if (opacity > 255) opacity = 255;
        Color fadeColor(neonBlue.r, neonBlue.g, neonBlue.b, static_cast<Uint8>(opacity));
        title.setFillColor(fadeColor);
        window.clear(Color::Black);
        window.draw(title);
        window.display();
    }
    sleep(seconds(0.5f));
    opacity = 0;
    while (opacity < 255)
    {
        float dt = clock.restart().asSeconds();
        opacity += 80 * dt;
        if (opacity > 255) opacity = 255;
        Color fadeColor(neonBlue.r, neonBlue.g, neonBlue.b, static_cast<Uint8>(opacity));
        credits.setFillColor(fadeColor);
        window.clear(Color::Black);
        window.draw(title);
        window.draw(credits);
        window.display();
    }
    opacity = 255;
    while (opacity > 0)
    {
        float dt = clock.restart().asSeconds();
        opacity -= 80 * dt;
        if (opacity < 0) opacity = 0;
        Color fadeColor(neonBlue.r, neonBlue.g, neonBlue.b, static_cast<Uint8>(opacity));
        title.setFillColor(fadeColor);
        credits.setFillColor(fadeColor);
        window.clear(Color::Black);
        window.draw(title);
        window.draw(credits);
        window.display();
    }
}