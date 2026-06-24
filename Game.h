// Game.h
#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "PlayerData.h" 
#include"MinHeap.h"
#include"Inventory.h"
#include "AVLTree.h"

using namespace std;
using namespace sf;

class MainScreen {
private:
    RenderWindow& window;
    Font font;
    Font font2;
    Text xonix;
    Text playNowBtn;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture buttonTexture;
    Sprite playButtonSprite;

public:
    MainScreen(RenderWindow& win) : window(win) {

        font.loadFromFile("fonts/Rovelink.otf");
        font2.loadFromFile("fonts/Noxis.otf");

        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg"))
        {
            cout << "Error loading background image" << endl;
        }

        backgroundSprite.setTexture(backgroundTexture);

        xonix.setFont(font);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Load Button Texture
        if (!buttonTexture.loadFromFile("images/buttons.png"))
        {
            cout << "Error loading buttons.png" << endl;
        }
        playButtonSprite.setTexture(buttonTexture);
        playButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
        playButtonSprite.setPosition(1110, 495);
        playButtonSprite.setScale(1.2f, 1.2f);

        playNowBtn.setFont(font2);
        playNowBtn.setString("Play Now!");
        playNowBtn.setCharacterSize(55);

        FloatRect textBounds = playNowBtn.getLocalBounds();
        playNowBtn.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        playNowBtn.setPosition(
            playButtonSprite.getPosition().x + playButtonSprite.getLocalBounds().width / 2.0f + 30,
            playButtonSprite.getPosition().y + playButtonSprite.getLocalBounds().height / 2.0f + 12
        ); // Adjust Y offset as needed
    }

    bool display() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed &&
                    event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos(Mouse::getPosition(window));
                    if (playButtonSprite.getGlobalBounds().contains(mousePos)) {
                        return true; // Go to login screen
                    }
                }
            }

            // Hover effect (optional)
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (playButtonSprite.getGlobalBounds().contains(mousePos)) {
                // Optionally, change the appearance on hover
                playButtonSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Lighten button
            }
            else {
                playButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
            }

            window.clear(Color::Black);

            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(playButtonSprite);
            window.draw(playNowBtn);
            window.display();
        }
        return false;
    }
};

class LoginScreen {
private:
    RenderWindow& window;
    Font font;
    Font font2;
    Font font3;
    Text xonix;
    Text usernameLabel, passwordLabel;
    Text usernameInputText, passwordInputText;
    Text errorText;
    string usernameInput;
    string passwordInput;

    // Updated Login Button Components
    Text LoginBtn;
    Texture LoginButtonTexture;
    Sprite LoginButtonSprite;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    // Focus flags
    bool isUsernameActive;
    bool isPasswordActive;

    // Visual indicators for active fields
    RectangleShape usernameBox;
    RectangleShape passwordBox;

public:
    LoginScreen(RenderWindow& win) : window(win), isUsernameActive(false), isPasswordActive(false) {
        // Load Fonts
        if (!font.loadFromFile("fonts/Noxis.otf")) {
            cout << "Error loading Noxis.otf" << endl;
        }

        if (!font2.loadFromFile("fonts/Rovelink.otf")) {
            cout << "Error loading Rovelink.otf" << endl;
        }

        if (!font3.loadFromFile("fonts/Saphira.otf")) {
            cout << "Error loading joystix.ttf" << endl;
        }

        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Setup XONIX Text
        xonix.setFont(font2);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Setup Username Label
        usernameLabel.setFont(font);
        usernameLabel.setString("Username:");
        usernameLabel.setCharacterSize(40);
        usernameLabel.setFillColor(Color::White);
        usernameLabel.setPosition(window.getSize().x / 2.0f + 140, 447);

        // Setup Password Label
        passwordLabel.setFont(font);
        passwordLabel.setString("Password:");
        passwordLabel.setCharacterSize(40);
        passwordLabel.setFillColor(Color::White);
        passwordLabel.setPosition(window.getSize().x / 2.0f + 140, 581);

        // Setup Username Input Text
        usernameInputText.setFont(font3);
        usernameInputText.setCharacterSize(25);
        usernameInputText.setFillColor(Color::White);
        usernameInputText.setPosition(window.getSize().x / 2.0f + 150, 510);

        // Setup Password Input Text
        passwordInputText.setFont(font3);
        passwordInputText.setCharacterSize(30);
        passwordInputText.setFillColor(Color::White);
        passwordInputText.setPosition(window.getSize().x / 2.0f + 150, 645);

        // Setup Error Text
        errorText.setFont(font3);
        errorText.setCharacterSize(25);
        errorText.setFillColor(Color::Red);
        errorText.setPosition(window.getSize().x / 2.0f + 140, 700);
        errorText.setString("");

        // Setup input boxes
        usernameBox.setSize(Vector2f(400, 50));
        usernameBox.setPosition(window.getSize().x / 2.0f + 135, 500);
        usernameBox.setFillColor(Color::Transparent);
        usernameBox.setOutlineThickness(2);
        usernameBox.setOutlineColor(Color::White);

        passwordBox.setSize(Vector2f(400, 50));
        passwordBox.setPosition(window.getSize().x / 2.0f + 135, 630);
        passwordBox.setFillColor(Color::Transparent);
        passwordBox.setOutlineThickness(2);
        passwordBox.setOutlineColor(Color::White);

        // Load Login button texture
        if (!LoginButtonTexture.loadFromFile("images/buttons.png")) {
            cout << "Error loading buttons.png" << endl;
        }

        // Setup Login Button Sprite
        LoginButtonSprite.setTexture(LoginButtonTexture);
        LoginButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
        LoginButtonSprite.setPosition(1175, 750);
        LoginButtonSprite.setScale(0.8f, 0.8f);

        // Setup Login Button Text
        LoginBtn.setFont(font);
        LoginBtn.setString("Login");
        LoginBtn.setCharacterSize(40);
        LoginBtn.setFillColor(Color::White);

        FloatRect loginTextBounds = LoginBtn.getLocalBounds();
        LoginBtn.setOrigin(loginTextBounds.left + loginTextBounds.width / 2.0f,
            loginTextBounds.top + loginTextBounds.height / 2.0f);
        LoginBtn.setPosition(
            LoginButtonSprite.getPosition().x + LoginButtonSprite.getGlobalBounds().width / 2.0f,
            LoginButtonSprite.getPosition().y + LoginButtonSprite.getGlobalBounds().height / 2.0f
        );
    }

    // Modified to accept the PlayerArray pointer so we do NOT re-load players each time
    bool display(PlayerArray* players) {
        // Clear previous inputs and error messages
        usernameInput.clear();
        passwordInput.clear();
        usernameInputText.setString("");
        passwordInputText.setString("");
        errorText.setString("");

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                    // Check if username box is clicked
                    if (usernameBox.getGlobalBounds().contains(mousePos)) {
                        isUsernameActive = true;
                        isPasswordActive = false;
                        usernameBox.setOutlineColor(Color::Yellow);
                        passwordBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                    // Check if password box is clicked
                    else if (passwordBox.getGlobalBounds().contains(mousePos)) {
                        isUsernameActive = false;
                        isPasswordActive = true;
                        passwordBox.setOutlineColor(Color::Yellow);
                        usernameBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                    // Check if Login button is clicked
                    else if (LoginButtonSprite.getGlobalBounds().contains(mousePos)) {
                        // Attempt login if all fields are filled
                        if (!usernameInput.empty() && !passwordInput.empty()) {
                            bool userFound = false;

                            // Use the existing PlayerArray instead of re-creating it
                            for (int i = 0; i < players->getSize(); ++i) {
                                const Player& player = players->getPlayers()[i];
                                if (player.getUsername() == usernameInput && player.getPassword() == passwordInput) {
                                    userFound = true;
                                    break;
                                }
                            }

                            if (userFound) {
                                cout << "Successfully logged in as " << usernameInput << endl;
                                return true;  // Let main know login succeeded
                            }
                            else {
                                errorText.setString("Login failed: incorrect username or password.");
                            }
                        }
                        else {
                            errorText.setString("Please enter both username and password.");
                        }
                    }
                    else {
                        // Clicked outside boxes / button
                        isUsernameActive = false;
                        isPasswordActive = false;
                        usernameBox.setOutlineColor(Color::White);
                        passwordBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                }

                if (event.type == Event::TextEntered) {
                    if (event.text.unicode == '\b') { // Backspace
                        if (isUsernameActive && !usernameInput.empty()) {
                            usernameInput.pop_back();
                            usernameInputText.setString(usernameInput);
                        }
                        if (isPasswordActive && !passwordInput.empty()) {
                            passwordInput.pop_back();
                            // Mask password input
                            passwordInputText.setString(string(passwordInput.size(), '*'));
                        }
                        errorText.setString(""); // Clear error message on backspace
                    }
                    else if (event.text.unicode == '\n' || event.text.unicode == 13) {
                        // Ignore Enter key
                        continue;
                    }
                    else if (event.text.unicode < 128) { // Printable ASCII
                        char enteredChar = static_cast<char>(event.text.unicode);
                        // Allow only alphanumerics and some special characters
                        bool isValidCharacter = isalnum(enteredChar) || string("!@#$%^&*").find(enteredChar) != string::npos;

                        if (isUsernameActive && isValidCharacter) {
                            if (usernameInput.length() >= 12) {
                                errorText.setString("Too many characters in Username (max 12).");
                            }
                            else {
                                usernameInput += enteredChar;
                                usernameInputText.setString(usernameInput);
                                errorText.setString(""); // Clear error message on valid input
                            }
                        }
                        if (isPasswordActive && isValidCharacter) {
                            if (passwordInput.length() >= 12) {
                                errorText.setString("Too many characters in Password (max 12).");
                            }
                            else {
                                passwordInput += enteredChar;
                                // Mask password
                                passwordInputText.setString(string(passwordInput.size(), '*'));
                                errorText.setString(""); // Clear error message on valid input
                            }
                        }
                    }
                }
            }

            // Hover effect for Login button
            Vector2f currentMousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (LoginButtonSprite.getGlobalBounds().contains(currentMousePos)) {
                LoginButtonSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered
            }
            else {
                LoginButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(usernameLabel);
            window.draw(passwordLabel);
            window.draw(usernameBox);
            window.draw(passwordBox);
            window.draw(usernameInputText);
            window.draw(passwordInputText);
            window.draw(errorText);
            window.draw(LoginButtonSprite);
            window.draw(LoginBtn);
            window.display();
        }
        return false; // If window closed or something else
    }

    string getUsername() const {
        return usernameInput;
    }

    string getPassword() const {
        return passwordInput;
    }
};

class StartMenu {
private:
    RenderWindow& window;
    Font& sfont1;
    Font& sfont2;
    Font& sfont3;
    Text xonix;
    Text loginBtn, signUpBtn, exitBtn;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture buttonTexture;
    Sprite loginButtonSprite, signUpButtonSprite, exitButtonSprite, profilePicSprite;


public:
    StartMenu(RenderWindow& win, Font& fnt, Font& fnt2, Font& fnt3)
        : window(win), sfont1(fnt), sfont2(fnt2), sfont3(fnt3) {

        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for StartMenu" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        xonix.setFont(sfont1);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        if (!buttonTexture.loadFromFile("images/buttons.png")) {
            cout << "Error loading buttons.png for StartMenu" << endl;
        }

        // Login button setup
        loginButtonSprite.setTexture(buttonTexture);
        loginButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
        loginButtonSprite.setPosition(1110, 395);
        //loginButtonSprite.setScale(1.2f, 1.2f);

        loginBtn.setFont(sfont2);
        loginBtn.setString("Login");
        loginBtn.setCharacterSize(40);
        loginBtn.setFillColor(Color::White);
        FloatRect loginTextBounds = loginBtn.getLocalBounds();
        loginBtn.setOrigin(loginTextBounds.left + loginTextBounds.width / 2.0f,
            loginTextBounds.top + loginTextBounds.height / 2.0f);
        loginBtn.setPosition(
            loginButtonSprite.getPosition().x + loginButtonSprite.getGlobalBounds().width / 2.0f,
            loginButtonSprite.getPosition().y + loginButtonSprite.getGlobalBounds().height / 2.0f
        );

        // Sign Up button setup
        signUpButtonSprite.setTexture(buttonTexture);
        signUpButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
        signUpButtonSprite.setPosition(1110, 535);
        //signUpButtonSprite.setScale(1.2f, 1.2f);

        signUpBtn.setFont(sfont2);
        signUpBtn.setString("Sign Up");
        signUpBtn.setCharacterSize(40);
        signUpBtn.setFillColor(Color::White);
        FloatRect signUpTextBounds = signUpBtn.getLocalBounds();
        signUpBtn.setOrigin(signUpTextBounds.left + signUpTextBounds.width / 2.0f,
            signUpTextBounds.top + signUpTextBounds.height / 2.0f);
        signUpBtn.setPosition(
            signUpButtonSprite.getPosition().x + signUpButtonSprite.getGlobalBounds().width / 2.0f,
            signUpButtonSprite.getPosition().y + signUpButtonSprite.getGlobalBounds().height / 2.0f
        );

        // Exit button setup
        exitButtonSprite.setTexture(buttonTexture);
        exitButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
        exitButtonSprite.setPosition(1110, 675);
        //exitButtonSprite.setScale(1.2f, 1.2f);

        exitBtn.setFont(sfont2);
        exitBtn.setString("Exit");
        exitBtn.setCharacterSize(40);
        exitBtn.setFillColor(Color::White);
        FloatRect exitTextBounds = exitBtn.getLocalBounds();
        exitBtn.setOrigin(exitTextBounds.left + exitTextBounds.width / 2.0f,
            exitTextBounds.top + exitTextBounds.height / 2.0f);
        exitBtn.setPosition(
            exitButtonSprite.getPosition().x + exitButtonSprite.getGlobalBounds().width / 2.0f,
            exitButtonSprite.getPosition().y + exitButtonSprite.getGlobalBounds().height / 2.0f
        );
    }

    // Display the start menu and return the user's choice
    int display() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    if (loginButtonSprite.getGlobalBounds().contains(mousePos)) {
                        return 1;
                    }
                    else if (signUpButtonSprite.getGlobalBounds().contains(mousePos)) {
                        return 2;
                    }
                    else if (exitButtonSprite.getGlobalBounds().contains(mousePos)) {
                        return -1;
                    }
                }
            }

            // Hover effect (optional)
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (loginButtonSprite.getGlobalBounds().contains(mousePos)) {
                loginButtonSprite.setTextureRect(IntRect(0, 130, 300, 110));
            }
            else {
                loginButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
            }

            if (signUpButtonSprite.getGlobalBounds().contains(mousePos)) {
                signUpButtonSprite.setTextureRect(IntRect(0, 130, 300, 110));
            }
            else {
                signUpButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
            }

            if (exitButtonSprite.getGlobalBounds().contains(mousePos)) {
                exitButtonSprite.setTextureRect(IntRect(0, 130, 300, 110));
            }
            else {
                exitButtonSprite.setTextureRect(IntRect(0, 0, 300, 110));
            }

            // Draw
            window.clear();
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(loginButtonSprite);
            window.draw(signUpButtonSprite);
            window.draw(exitButtonSprite);
            window.draw(loginBtn);
            window.draw(signUpBtn);
            window.draw(exitBtn);
            window.display();
        }

        return 0; // fallback in case window is closed
    }
};

class SignUpScreen {
private:
    RenderWindow& window;
    Font& font;
    Font& font2;
    Font& font3;
    Text xonix;
    Text usernameLabel, passwordLabel, nicknameLabel;
    Text usernameInputText, passwordInputText, nicknameInputText;
    Text errorText; // Added for displaying error messages
    string usernameInput;
    string passwordInput;
    string nicknameInput;
    Text signUpBtn;

    Texture backgroundTexture;
    Sprite backgroundSprite;
    Texture buttonTexture;
    Sprite SignUpButtonSprite;

    // Focus flags
    bool isUsernameActive;
    bool isPasswordActive;
    bool isNicknameActive;

    // Visual indicators for active fields
    RectangleShape usernameBox;
    RectangleShape passwordBox;
    RectangleShape nicknameBox;

public:
    SignUpScreen(RenderWindow& win, Font& fnt, Font& fnt2, Font& fnt3)
        : window(win), font(fnt), font2(fnt2), font3(fnt3),
        isUsernameActive(false), isPasswordActive(false), isNicknameActive(false) {
        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for SignUpScreen" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Setup XONIX Text
        xonix.setFont(font2);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Setup Username Label
        usernameLabel.setFont(font);
        usernameLabel.setString("Username:");
        usernameLabel.setCharacterSize(40);
        usernameLabel.setFillColor(Color::White);
        usernameLabel.setPosition(window.getSize().x / 2.0f + 140, 395);

        // Setup Password Label
        passwordLabel.setFont(font);
        passwordLabel.setString("Password:");
        passwordLabel.setCharacterSize(40);
        passwordLabel.setFillColor(Color::White);
        passwordLabel.setPosition(window.getSize().x / 2.0f + 140, 510);

        // Setup Nickname Label
        nicknameLabel.setFont(font);
        nicknameLabel.setString("Nickname:");
        nicknameLabel.setCharacterSize(40);
        nicknameLabel.setFillColor(Color::White);
        nicknameLabel.setPosition(window.getSize().x / 2.0f + 140, 625);

        // Setup Username Input Text
        usernameInputText.setFont(font3);
        usernameInputText.setCharacterSize(25);
        usernameInputText.setFillColor(Color::White);
        usernameInputText.setPosition(window.getSize().x / 2.0f + 150, 460);

        // Setup Password Input Text
        passwordInputText.setFont(font3);
        passwordInputText.setCharacterSize(25);
        passwordInputText.setFillColor(Color::White);
        passwordInputText.setPosition(window.getSize().x / 2.0f + 150, 577);

        // Setup Nickname Input Text
        nicknameInputText.setFont(font3);
        nicknameInputText.setCharacterSize(25);
        nicknameInputText.setFillColor(Color::White);
        nicknameInputText.setPosition(window.getSize().x / 2.0f + 150, 687);

        // Setup Error Text
        errorText.setFont(font3);
        errorText.setCharacterSize(20);
        errorText.setFillColor(Color::Red);
        errorText.setPosition(window.getSize().x / 2.0f + 140, 750);
        errorText.setString("");

        // Setup input boxes
        usernameBox.setSize(Vector2f(400, 50));
        usernameBox.setPosition(window.getSize().x / 2.0f + 135, 447);
        usernameBox.setFillColor(Color::Transparent);
        usernameBox.setOutlineThickness(2);
        usernameBox.setOutlineColor(Color::White);

        passwordBox.setSize(Vector2f(400, 50));
        passwordBox.setPosition(window.getSize().x / 2.0f + 135, 565);
        passwordBox.setFillColor(Color::Transparent);
        passwordBox.setOutlineThickness(2);
        passwordBox.setOutlineColor(Color::White);

        nicknameBox.setSize(Vector2f(400, 50));
        nicknameBox.setPosition(window.getSize().x / 2.0f + 135, 675);
        nicknameBox.setFillColor(Color::Transparent);
        nicknameBox.setOutlineThickness(2);
        nicknameBox.setOutlineColor(Color::White);

        // Load button texture
        if (!buttonTexture.loadFromFile("images/buttons.png")) {
            cout << "Error loading buttons.png" << endl;
        }
        SignUpButtonSprite.setTexture(buttonTexture);
        SignUpButtonSprite.setTextureRect(IntRect(0, 0, 300, 200));
        SignUpButtonSprite.setPosition(1175, 850);
        SignUpButtonSprite.setScale(0.8f, 0.8f);

        // Setup Sign Up Button Text
        signUpBtn.setFont(font);
        signUpBtn.setString("Sign Up");
        signUpBtn.setCharacterSize(40);

        FloatRect textBounds = signUpBtn.getLocalBounds();
        signUpBtn.setOrigin(textBounds.left + textBounds.width / 2.0f + 30, textBounds.top + textBounds.height / 2.0f);
        signUpBtn.setPosition(
            SignUpButtonSprite.getPosition().x + SignUpButtonSprite.getGlobalBounds().width / 2.0f + 30,
            SignUpButtonSprite.getPosition().y + SignUpButtonSprite.getGlobalBounds().height / 2.0f - 35
        );
    }

    bool display(PlayerArray& playerArray) {
        usernameInput.clear();
        passwordInput.clear();
        nicknameInput.clear();
        usernameInputText.setString("");
        passwordInputText.setString("");
        nicknameInputText.setString("");
        errorText.setString("");

        while (window.isOpen()) {
            bool submitRequest = false;

            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                    if (usernameBox.getGlobalBounds().contains(mousePos)) {
                        isUsernameActive = true;
                        isPasswordActive = false;
                        isNicknameActive = false;
                        usernameBox.setOutlineColor(Color::Yellow);
                        passwordBox.setOutlineColor(Color::White);
                        nicknameBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                    else if (passwordBox.getGlobalBounds().contains(mousePos)) {
                        isUsernameActive = false;
                        isPasswordActive = true;
                        isNicknameActive = false;
                        passwordBox.setOutlineColor(Color::Yellow);
                        usernameBox.setOutlineColor(Color::White);
                        nicknameBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                    else if (nicknameBox.getGlobalBounds().contains(mousePos)) {
                        isUsernameActive = false;
                        isPasswordActive = false;
                        isNicknameActive = true;
                        nicknameBox.setOutlineColor(Color::Yellow);
                        usernameBox.setOutlineColor(Color::White);
                        passwordBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                    else if (SignUpButtonSprite.getGlobalBounds().contains(mousePos)) {
                        submitRequest = true;
                    }
                    else {
                        isUsernameActive = false;
                        isPasswordActive = false;
                        isNicknameActive = false;
                        usernameBox.setOutlineColor(Color::White);
                        passwordBox.setOutlineColor(Color::White);
                        nicknameBox.setOutlineColor(Color::White);
                        errorText.setString("");
                    }
                }

                if (event.type == Event::TextEntered) {
                    if (event.text.unicode == '\b') {
                        if (isUsernameActive && !usernameInput.empty()) {
                            usernameInput.pop_back();
                            usernameInputText.setString(usernameInput);
                        }
                        if (isPasswordActive && !passwordInput.empty()) {
                            passwordInput.pop_back();
                            passwordInputText.setString(string(passwordInput.size(), '*'));
                        }
                        if (isNicknameActive && !nicknameInput.empty()) {
                            nicknameInput.pop_back();
                            nicknameInputText.setString(nicknameInput);
                        }
                        errorText.setString(""); // Clear error message on backspace
                    }
                    else if (event.text.unicode < 128) {
                        char enteredChar = static_cast<char>(event.text.unicode);
                        if (isUsernameActive) {
                            if (usernameInput.length() >= 12) {
                                errorText.setString("Too many characters in Username (max 12).");
                            }
                            else if (isalnum(enteredChar) || enteredChar == '_') {
                                usernameInput += enteredChar;
                                usernameInputText.setString(usernameInput);
                                errorText.setString(""); // Clear error message on valid input
                            }
                        }
                        if (isPasswordActive) {
                            if (passwordInput.length() >= 12) {
                                errorText.setString("Too many characters in Password (max 12).");
                            }
                            else if (isalnum(enteredChar) || string("!@#$%^&*_").find(enteredChar) != string::npos) {
                                passwordInput += enteredChar;
                                passwordInputText.setString(string(passwordInput.size(), '*'));
                                errorText.setString(""); // Clear error message on valid input
                            }
                        }
                        if (isNicknameActive) {
                            if (nicknameInput.length() >= 12) {
                                errorText.setString("Too many characters in Nickname (max 12).");
                            }
                            else if (enteredChar != ' ') {
                                nicknameInput += enteredChar;
                                nicknameInputText.setString(nicknameInput);
                                errorText.setString(""); // Clear error message on valid input
                            }
                        }
                    }
                }
            }

            // If the user requested to submit (via Enter or button), do the checks
            if (submitRequest) {
                if (!usernameInput.empty() && !passwordInput.empty() && !nicknameInput.empty()) {
                    bool valid = true;
                    string errorMessage = "";

                    // Username validation
                    if (usernameInput.length() < 3) {
                        errorMessage += "Username must be at least 3 characters long.\n";
                        valid = false;
                    }
                    else {
                        for (char c : usernameInput) {
                            if (!isalnum(c) && c != '_') {
                                errorMessage += "Username can only contain alphanumeric characters and '_'.\n";
                                valid = false;
                                break;
                            }
                        }
                    }
                    // Username uniqueness check
                    for (int i = 0; i < playerArray.getSize(); ++i) {
                        if (playerArray.getPlayers()[i].getUsername() == usernameInput) {
                            errorMessage += "Username is already taken.\n";
                            valid = false;
                            break;
                        }
                    }

                    // Password validation
                    if (passwordInput.length() < 8) {
                        errorMessage += "Password must be at least 8 characters long.\n";
                        valid = false;
                    }
                    else {
                        bool hasNumber = false;
                        for (char c : passwordInput) {
                            if (isdigit(c)) {
                                hasNumber = true;
                                break;
                            }
                        }
                        if (!hasNumber) {
                            errorMessage += "Password must contain at least one number.\n";
                            valid = false;
                        }
                    }

                    // Nickname validation
                    if (nicknameInput.find(' ') != string::npos) {
                        errorMessage += "Nickname cannot contain spaces.\n";
                        valid = false;
                    }
                    // Nickname uniqueness check
                    for (int i = 0; i < playerArray.getSize(); ++i) {
                        if (playerArray.getPlayers()[i].getNickname() == nicknameInput) {
                            errorMessage += "Nickname is already taken.\n";
                            valid = false;
                            break;
                        }
                    }

                    if (valid) {
                        cout << "User " << usernameInput << " signed up successfully with nickname " << nicknameInput << endl;
                        return true; // Move to the next page
                    }
                    else {
                        errorText.setString(errorMessage);
                    }
                }
                else {
                    errorText.setString("Please fill in all fields.");
                }
            }

            // Handle hover effect for Sign Up button
            Vector2f currentMousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (SignUpButtonSprite.getGlobalBounds().contains(currentMousePos)) {
                SignUpButtonSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                SignUpButtonSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(usernameLabel);
            window.draw(passwordLabel);
            window.draw(nicknameLabel);
            window.draw(usernameBox);
            window.draw(passwordBox);
            window.draw(nicknameBox);
            window.draw(usernameInputText);
            window.draw(passwordInputText);
            window.draw(nicknameInputText);
            window.draw(errorText); // Draw error messages
            window.draw(SignUpButtonSprite);
            window.draw(signUpBtn);
            window.display();
        }
        return false;
    }

    string getUsername() const {
        return usernameInput;
    }

    string getPassword() const {
        return passwordInput;
    }

    string getNickname() const {
        return nicknameInput;
    }
};

class MainMenu
{
private:
    RenderWindow& window;
    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture menuOptionsTexture;       // Texture containing all main menu option images
    Sprite menuOptionSprites[4];      // Sprites for each main menu option

    // Separate IntRects for normal and hovered states for each main menu option
    IntRect menuOptionRectsNormal[4];
    IntRect menuOptionRectsHovered[4];

    Font font, font2, font3, font4;
    Text menuText[4];
    Text xonix;

    Texture profilePictureTexture;    // Texture for profile picture
    Sprite profilePictureSprite;      // Sprite for profile picture

    Text rankText;                    // Text displaying the player's rank
    Text playerNicknameText;          // Text displaying the player's nickname

    Text friendsText;                 // Text for "Friends" button
    Text logOutText;                  // Text for "Log Out" button

    int selectedOption;

public:
    MainMenu(RenderWindow& win, Font& fnt, Font& fnt2)
        : window(win), font(fnt), font2(fnt2) {

        if (!font3.loadFromFile("fonts/Course.ttf")) {
            cout << "Font loading failed!" << endl;
        }
        if (!font4.loadFromFile("fonts/joystix.otf")) {
            cout << "Font loading failed!" << endl;
        }
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for StartMenu" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        xonix.setFont(font);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Initialize text
        menuText[0].setFont(font2);
        menuText[0].setString("Start");
        menuText[0].setCharacterSize(40);
        menuText[0].setFillColor(Color::White);
        menuText[0].setPosition(window.getSize().x / 2.0f + 140 + 88, 385);

        menuText[1].setFont(font2);
        menuText[1].setString("Leaderboard");
        menuText[1].setCharacterSize(38);
        menuText[1].setFillColor(Color::White);
        menuText[1].setPosition(window.getSize().x / 2.0f + 140 + 25, 495);

        menuText[2].setFont(font2);
        menuText[2].setString("Inventory");
        menuText[2].setCharacterSize(40);
        menuText[2].setFillColor(Color::White);
        menuText[2].setPosition(window.getSize().x / 2.0f + 140 + 42, 605);

        menuText[3].setFont(font2);
        menuText[3].setString("Exit");
        menuText[3].setCharacterSize(40);
        menuText[3].setFillColor(Color::White);
        menuText[3].setPosition(window.getSize().x / 2.0f + 140 + 98, 715);

        // Load menu options texture
        if (!menuOptionsTexture.loadFromFile("images/buttons.png"))
        {
            cout << "Error loading menu options texture" << endl;
        }

        // Define IntRects for each main menu option's normal and hovered states
        for (int i = 0; i < 4; ++i)
        {
            menuOptionRectsNormal[i] = IntRect(0, 0, 300, 110);          // Normal state
            menuOptionRectsHovered[i] = IntRect(0, 130, 300, 110);       // Hovered state
            menuOptionSprites[i].setTexture(menuOptionsTexture);
            menuOptionSprites[i].setTextureRect(menuOptionRectsNormal[i]);
            menuOptionSprites[i].setPosition(window.getSize().x / 2.0f + 140, 355 + i * 110);
        }

        selectedOption = -1;
    }

    void loadProfilePicture(const string& profilePicPath)
    {
        if (!profilePictureTexture.loadFromFile(profilePicPath))
        {
            cout << "Error loading profile picture: " << profilePicPath << endl;
            if (!profilePictureTexture.loadFromFile("images/default.png")) // Fallback to default
            {
                cout << "Error loading default profile picture." << endl;
            }
        }
        profilePictureSprite.setTexture(profilePictureTexture);
        profilePictureSprite.setScale(0.10f, 0.10f); // Scale down the profile picture
        profilePictureSprite.setPosition(window.getSize().x - 150, 20); // Position it at the top-right corner
    }

    void display(Player& current)
    {
        // Load the profile picture for the current player
        string ppPath = "images/" + current.getUsername() + ".png";
        loadProfilePicture(ppPath);

        // Set rank
        rankText.setFont(font4);
        rankText.setString("Rank: ");
        rankText.setCharacterSize(24);
        rankText.setFillColor(Color::White);
        rankText.setPosition(window.getSize().x - 400, 70);

        // Set player's nickname
        playerNicknameText.setFont(font3);
        playerNicknameText.setString(current.getNickname());
        playerNicknameText.setCharacterSize(35);
        Color LightBlue(0, 255, 255);
        playerNicknameText.setFillColor(LightBlue);
        playerNicknameText.setPosition(window.getSize().x - 400, 15);

        // Set "Friends" text
        friendsText.setFont(font4);
        friendsText.setString("Friends");
        friendsText.setCharacterSize(18);
        friendsText.setFillColor(Color::White);
        friendsText.setPosition(window.getSize().x - 400, 115);

        // Set "Log Out" text
        logOutText.setFont(font4);
        logOutText.setString("Log Out");
        logOutText.setCharacterSize(18);
        logOutText.setFillColor(Color::White);
        logOutText.setPosition(window.getSize().x - 265, 115);

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    // Check if "Friends" or "Log Out" is clicked
                    if (friendsText.getGlobalBounds().contains(mousePos))
                    {
                        cout << "Friends clicked!" << endl;
                        selectedOption = 4;
                        return;
                    }
                    if (logOutText.getGlobalBounds().contains(mousePos))
                    {
                        cout << "Log Out clicked!" << endl;
                        selectedOption = 5;
                        return;
                    }

                    for (int i = 0; i < 4; ++i)
                    {
                        FloatRect bounds = menuOptionSprites[i].getGlobalBounds();
                        if (bounds.contains(mousePos))
                        {
                            selectedOption = i;
                            return; // Exit the menu loop
                        }
                    }
                }
            }

            // Highlight "Friends" and "Log Out" text if hovered
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (friendsText.getGlobalBounds().contains(mousePos))
                friendsText.setFillColor(LightBlue);
            else
                friendsText.setFillColor(Color::White);

            if (logOutText.getGlobalBounds().contains(mousePos))
                logOutText.setFillColor(LightBlue);
            else
                logOutText.setFillColor(Color::White);

            // Highlight main menu options if hovered
            for (int i = 0; i < 4; ++i)
            {
                FloatRect bounds = menuOptionSprites[i].getGlobalBounds();
                if (bounds.contains(mousePos))
                {
                    menuOptionSprites[i].setTextureRect(menuOptionRectsHovered[i]);
                }
                else
                {
                    menuOptionSprites[i].setTextureRect(menuOptionRectsNormal[i]);
                }
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(xonix);

            for (int i = 0; i < 4; ++i)
            {
                window.draw(menuOptionSprites[i]);
                window.draw(menuText[i]);
            }

            // Draw the profile picture, rank, nickname, and buttons
            window.draw(profilePictureSprite);
            window.draw(rankText);
            window.draw(playerNicknameText);
            window.draw(friendsText);
            window.draw(logOutText);

            window.display();
        }

        return; // Ensure function returns even if window is closed
    }

    int getSelectedOption()
    {
        return selectedOption;
    }
};

class PauseMenu
{
    RenderWindow& window;
    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture pauseOptionsTexture;      // Texture containing all pause menu option images
    Sprite pauseOptionSprites[3];     // Sprites for each pause menu option

    // Separate IntRects for normal and hovered states for each pause menu option
    IntRect pauseOptionRectsNormal[3];
    IntRect pauseOptionRectsHovered[3];

    Font font, font2;
    Text menuText[3];
    Text xonix;

    int selectedOption;

public:
    PauseMenu(RenderWindow& win) : window(win), selectedOption(-1)
    {
        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg"))
        {
            cout << "Error loading background image" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Load fonts
        if (!font.loadFromFile("fonts/Noxis.otf")) {
            cout << "Font loading failed!" << std::endl;
        }
        if (!font2.loadFromFile("fonts/Rovelink.otf")) {
            cout << "Font loading failed!" << std::endl;
        }

        xonix.setFont(font2);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Initialize text
        menuText[0].setFont(font);
        menuText[0].setString("Resume");
        menuText[0].setCharacterSize(40);
        menuText[0].setFillColor(Color::White);
        menuText[0].setPosition(window.getSize().x / 2.0f + 140 + 78, 405);

        menuText[1].setFont(font);
        menuText[1].setString("Reset");
        menuText[1].setCharacterSize(38);
        menuText[1].setFillColor(Color::White);
        menuText[1].setPosition(window.getSize().x / 2.0f + 140 + 90, 515);

        menuText[2].setFont(font);
        menuText[2].setString("Quit");
        menuText[2].setCharacterSize(40);
        menuText[2].setFillColor(Color::White);
        menuText[2].setPosition(window.getSize().x / 2.0f + 140 + 105, 625);

        // Load pause options texture
        if (!pauseOptionsTexture.loadFromFile("images/buttons.png"))
        {
            cout << "Error loading pause options texture" << endl;
        }

        // Define IntRects for each pause menu option's normal and hovered states
        // Menu Option 0: Resume
        pauseOptionRectsNormal[0] = IntRect(0, 0, 300, 110);          // Resume_Normal
        pauseOptionRectsHovered[0] = IntRect(0, 130, 300, 110);       // Resume_Hovered
        pauseOptionSprites[0].setTexture(pauseOptionsTexture);
        pauseOptionSprites[0].setTextureRect(pauseOptionRectsNormal[0]);
        pauseOptionSprites[0].setPosition(window.getSize().x / 2.0f + 140, 375);

        // Menu Option 1: Controls
        pauseOptionRectsNormal[1] = IntRect(0, 0, 300, 110);         // Controls_Normal
        pauseOptionRectsHovered[1] = IntRect(0, 130, 300, 110);     // Controls_Hovered
        pauseOptionSprites[1].setTexture(pauseOptionsTexture);
        pauseOptionSprites[1].setTextureRect(pauseOptionRectsNormal[1]);
        pauseOptionSprites[1].setPosition(window.getSize().x / 2.0f + 140, 485);

        // Menu Option 2: Quit
        pauseOptionRectsNormal[2] = IntRect(0, 0, 300, 110);         // Quit_Normal
        pauseOptionRectsHovered[2] = IntRect(0, 130, 300, 110);     // Quit_Hovered
        pauseOptionSprites[2].setTexture(pauseOptionsTexture);
        pauseOptionSprites[2].setTextureRect(pauseOptionRectsNormal[2]);
        pauseOptionSprites[2].setPosition(window.getSize().x / 2.0f + 140, 595);
    }

    void display()
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == Mouse::Left)
                    {
                        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                        for (int i = 0; i < 3; ++i)
                        {
                            FloatRect bounds = pauseOptionSprites[i].getGlobalBounds();
                            if (bounds.contains(mousePos))
                            {
                                selectedOption = i;
                                return; // Exit the menu loop
                            }
                        }
                    }
                }
            }

            // Highlight pause menu option if mouse is over it
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            for (int i = 0; i < 3; ++i)
            {
                FloatRect bounds = pauseOptionSprites[i].getGlobalBounds();
                if (bounds.contains(mousePos))
                {
                    pauseOptionSprites[i].setTextureRect(pauseOptionRectsHovered[i]);
                }
                else
                {
                    pauseOptionSprites[i].setTextureRect(pauseOptionRectsNormal[i]);
                }
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(xonix);

            for (int i = 0; i < 3; ++i)
            {
                window.draw(pauseOptionSprites[i]);
                window.draw(menuText[i]);
            }

            window.display();
        }
    }

    int getSelectedOption()
    {
        return selectedOption;
    }
};

class LeaderboardScreen {
private:
    RenderWindow& window;
    Font font;    // Primary font
    Font font2;   // Secondary font
    Text xonix;
    Text leaderboardTitle;
    Text tableHeaders[3];       // Rank, Nickname, High Score
    Text tableEntries[10][3];   // Top 10 entries

    // Background components
    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    LeaderboardScreen(RenderWindow& win) : window(win) {
        // Load fonts
        if (!font.loadFromFile("fonts/Rovelink.otf")) {
            cout << "Error loading Rovelink.otf" << endl;
        }
        if (!font2.loadFromFile("fonts/Noxis.otf")) {
            cout << "Error loading Noxis.otf" << endl;
        }

        // Load and set background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for LeaderboardScreen" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Setup XONIX Text
        xonix.setFont(font);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475); // Updated Y position to match MainScreen
        xonix.setRotation(355);

        // Setup Leaderboard Title
        leaderboardTitle.setFont(font2);
        leaderboardTitle.setString("Leaderboard");
        leaderboardTitle.setCharacterSize(50);
        leaderboardTitle.setFillColor(Color::White);
        leaderboardTitle.setPosition(1200, 200); // Moved to the right

        // Setup table headers
        tableHeaders[0].setFont(font2);
        tableHeaders[0].setString("Rank");
        tableHeaders[0].setCharacterSize(30);
        tableHeaders[0].setFillColor(Color::Yellow);
        tableHeaders[0].setPosition(1050, 300);

        tableHeaders[1].setFont(font2);
        tableHeaders[1].setString("Nickname");
        tableHeaders[1].setCharacterSize(30);
        tableHeaders[1].setFillColor(Color::Yellow);
        tableHeaders[1].setPosition(1250, 300);

        tableHeaders[2].setFont(font2);
        tableHeaders[2].setString("High Score");
        tableHeaders[2].setCharacterSize(30);
        tableHeaders[2].setFillColor(Color::Yellow);
        tableHeaders[2].setPosition(1550, 300);
    }

    void display() {
        // Load players into a dynamic array from PlayerData
        PlayerData playerData("PlayerData.csv");
        PlayerArray players = playerData.loadPlayers();

        // Build the leaderboard using MinHeap and the dynamic array
        MinHeap heap;
        for (int i = 0; i < players.getSize(); i++) {
            heap.insert(players.getPlayers()[i].getNickname().c_str(), players.getPlayers()[i].getHighScore());
        }

        PlayerEntry topEntries[10];
        int count = 0;
        heap.getTopEntries(topEntries, count);

        // Populate table entries
        for (int i = 0; i < count; ++i) {
            // Rank
            tableEntries[i][0].setFont(font2);
            tableEntries[i][0].setString(to_string(i + 1));
            tableEntries[i][0].setCharacterSize(25);
            tableEntries[i][0].setFillColor(Color::White);
            tableEntries[i][0].setPosition(1050, 340 + 40 * i);

            // Nickname
            tableEntries[i][1].setFont(font2);
            tableEntries[i][1].setString(topEntries[i].nickname);
            tableEntries[i][1].setCharacterSize(25);
            tableEntries[i][1].setFillColor(Color::White);
            tableEntries[i][1].setPosition(1250, 340 + 40 * i);

            // Score
            tableEntries[i][2].setFont(font2);
            tableEntries[i][2].setString(to_string(topEntries[i].score));
            tableEntries[i][2].setCharacterSize(25);
            tableEntries[i][2].setFillColor(Color::White);
            tableEntries[i][2].setPosition(1550, 340 + 40 * i);
        }

        // Main loop for the leaderboard screen
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    // Return to main menu
                    return;
                }
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(leaderboardTitle);

            for (int i = 0; i < 3; ++i)
                window.draw(tableHeaders[i]);

            for (int i = 0; i < count; ++i)
                for (int j = 0; j < 3; ++j)
                    window.draw(tableEntries[i][j]);

            window.display();
        }
    }
};

class MatchSelectionScreen {
private:
    RenderWindow& window;
    Font& font;
    Font& font2;
    Text xonix;
    Text singlePlayerBtn, multiPlayerBtn, exitBtn;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture buttonTexture;
    Sprite singlePlayerSprite, multiPlayerSprite, exitSprite;

public:
    MatchSelectionScreen(RenderWindow& win, Font& fnt, Font& fnt2)
        : window(win), font(fnt), font2(fnt2) {

        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for MatchSelectionScreen" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Setup XONIX Text
        xonix.setFont(font);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);


        // Load button texture
        if (!buttonTexture.loadFromFile("images/buttons.png")) {
            cout << "Error loading buttons.png for MatchSelectionScreen" << endl;
        }

        // Single Player Button Setup
        singlePlayerSprite.setTexture(buttonTexture);
        singlePlayerSprite.setTextureRect(IntRect(0, 0, 300, 110));
        singlePlayerSprite.setPosition(1110, 395);
        singlePlayerSprite.setScale(1.0f, 1.0f);

        singlePlayerBtn.setFont(font2);
        singlePlayerBtn.setString("Single Player");
        singlePlayerBtn.setCharacterSize(35);
        singlePlayerBtn.setFillColor(Color::White);
        FloatRect spBounds = singlePlayerBtn.getLocalBounds();
        singlePlayerBtn.setOrigin(spBounds.left + spBounds.width / 2.0f, spBounds.top + spBounds.height / 2.0f);
        singlePlayerBtn.setPosition(
            singlePlayerSprite.getPosition().x + singlePlayerSprite.getGlobalBounds().width / 2.0f,
            singlePlayerSprite.getPosition().y + singlePlayerSprite.getGlobalBounds().height / 2.0f
        );

        // Multiplayer Button Setup
        multiPlayerSprite.setTexture(buttonTexture);
        multiPlayerSprite.setTextureRect(IntRect(0, 0, 300, 110));
        multiPlayerSprite.setPosition(1110, 535);
        multiPlayerSprite.setScale(1.0f, 1.0f);

        multiPlayerBtn.setFont(font2);
        multiPlayerBtn.setString("Multiplayer");
        multiPlayerBtn.setCharacterSize(35);
        multiPlayerBtn.setFillColor(Color::White);
        FloatRect mpBounds = multiPlayerBtn.getLocalBounds();
        multiPlayerBtn.setOrigin(mpBounds.left + mpBounds.width / 2.0f, mpBounds.top + mpBounds.height / 2.0f);
        multiPlayerBtn.setPosition(
            multiPlayerSprite.getPosition().x + multiPlayerSprite.getGlobalBounds().width / 2.0f,
            multiPlayerSprite.getPosition().y + multiPlayerSprite.getGlobalBounds().height / 2.0f
        );

        // Exit Button Setup
        exitSprite.setTexture(buttonTexture);
        exitSprite.setTextureRect(IntRect(0, 0, 300, 110));
        exitSprite.setPosition(1110, 675);
        exitSprite.setScale(1.0f, 1.0f);

        exitBtn.setFont(font2);
        exitBtn.setString("Exit");
        exitBtn.setCharacterSize(45);
        exitBtn.setFillColor(Color::White);
        FloatRect exitBounds = exitBtn.getLocalBounds();
        exitBtn.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
        exitBtn.setPosition(
            exitSprite.getPosition().x + exitSprite.getGlobalBounds().width / 2.0f,
            exitSprite.getPosition().y + exitSprite.getGlobalBounds().height / 2.0f
        );
    }

    // Display the match selection screen and return the user's choice
    // Returns:
    // 1 - Single Player
    // 2 - Multiplayer
    // -1 - Exit
    int display() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    if (singlePlayerSprite.getGlobalBounds().contains(mousePos)) {
                        return 1; // Single Player selected
                    }
                    if (multiPlayerSprite.getGlobalBounds().contains(mousePos)) {
                        return 2; // Multiplayer selected
                    }
                    if (exitSprite.getGlobalBounds().contains(mousePos)) {
                        window.close(); // Exit the application
                        return -1;
                    }
                }
            }

            // Hover effects
            Vector2f currentMousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            if (singlePlayerSprite.getGlobalBounds().contains(currentMousePos)) {
                singlePlayerSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                singlePlayerSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            if (multiPlayerSprite.getGlobalBounds().contains(currentMousePos)) {
                multiPlayerSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                multiPlayerSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            if (exitSprite.getGlobalBounds().contains(currentMousePos)) {
                exitSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                exitSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            // Draw everything
            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(singlePlayerSprite);
            window.draw(singlePlayerBtn);
            window.draw(multiPlayerSprite);
            window.draw(multiPlayerBtn);
            window.draw(exitSprite);
            window.draw(exitBtn);
            window.display();
        }
        return -1; // Default return if window is closed
    }
};

class MultiplayerScreen {
private:
    RenderWindow& window;
    Font& font;
    Font& font2;
    Text xonix;
    Text offlineBtn, matchmakingBtn, exitBtn;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture buttonTexture;
    Sprite offlineSprite, matchmakingSprite, exitSprite;

public:
    MultiplayerScreen(RenderWindow& win, Font& fnt, Font& fnt2)
        : window(win), font(fnt), font2(fnt2) {
        // Load background texture
        if (!backgroundTexture.loadFromFile("images/background_menu2.jpg")) {
            cout << "Error loading background image for MultiplayerScreen" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Setup XONIX Text for Multiplayer Screen (consistent with MatchSelectionScreen)
        xonix.setFont(font);
        xonix.setString("XONIX");
        xonix.setCharacterSize(170);
        xonix.setFillColor(Color::White);
        xonix.setPosition(380, 475);
        xonix.setRotation(355);

        // Load button texture (same as MatchSelectionScreen)
        if (!buttonTexture.loadFromFile("images/buttons.png")) {
            cout << "Error loading buttons.png for MultiplayerScreen" << endl;
        }

        // Offline Button Setup - consistent positioning with MatchSelectionScreen
        offlineSprite.setTexture(buttonTexture);
        offlineSprite.setTextureRect(IntRect(0, 0, 300, 110));
        offlineSprite.setPosition(1110, 395);
        offlineSprite.setScale(1.0f, 1.0f);

        offlineBtn.setFont(font2);
        offlineBtn.setString("Offline");
        offlineBtn.setCharacterSize(35);
        offlineBtn.setFillColor(Color::White);
        FloatRect offBounds = offlineBtn.getLocalBounds();
        offlineBtn.setOrigin(offBounds.left + offBounds.width / 2.0f, offBounds.top + offBounds.height / 2.0f);
        offlineBtn.setPosition(
            offlineSprite.getPosition().x + offlineSprite.getGlobalBounds().width / 2.0f,
            offlineSprite.getPosition().y + offlineSprite.getGlobalBounds().height / 2.0f
        );

        // Matchmaking Button Setup - consistent with MatchSelectionScreen
        matchmakingSprite.setTexture(buttonTexture);
        matchmakingSprite.setTextureRect(IntRect(0, 0, 300, 110));
        matchmakingSprite.setPosition(1110, 535);
        matchmakingSprite.setScale(1.0f, 1.0f);

        matchmakingBtn.setFont(font2);
        matchmakingBtn.setString("Matchmaking");
        matchmakingBtn.setCharacterSize(35);
        matchmakingBtn.setFillColor(Color::White);
        FloatRect mmBounds = matchmakingBtn.getLocalBounds();
        matchmakingBtn.setOrigin(mmBounds.left + mmBounds.width / 2.0f, mmBounds.top + mmBounds.height / 2.0f);
        matchmakingBtn.setPosition(
            matchmakingSprite.getPosition().x + matchmakingSprite.getGlobalBounds().width / 2.0f,
            matchmakingSprite.getPosition().y + matchmakingSprite.getGlobalBounds().height / 2.0f
        );

        // Exit Button Setup - consistent with MatchSelectionScreen
        exitSprite.setTexture(buttonTexture);
        exitSprite.setTextureRect(IntRect(0, 0, 300, 110));
        exitSprite.setPosition(1110, 675);
        exitSprite.setScale(1.0f, 1.0f);

        exitBtn.setFont(font2);
        exitBtn.setString("Exit");
        exitBtn.setCharacterSize(45);
        exitBtn.setFillColor(Color::White);
        FloatRect exitBounds = exitBtn.getLocalBounds();
        exitBtn.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
        exitBtn.setPosition(
            exitSprite.getPosition().x + exitSprite.getGlobalBounds().width / 2.0f,
            exitSprite.getPosition().y + exitSprite.getGlobalBounds().height / 2.0f
        );
    }

    // Display the multiplayer selection screen and return the user's choice
    // Returns:
    // 1 - Offline
    // 2 - Matchmaking
    // -1 - Exit
    int display() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    if (offlineSprite.getGlobalBounds().contains(mousePos)) {
                        return 1; // Offline selected
                    }
                    if (matchmakingSprite.getGlobalBounds().contains(mousePos)) {
                        return 2; // Matchmaking selected
                    }
                    if (exitSprite.getGlobalBounds().contains(mousePos)) {
                        window.close(); // Exit the application
                        return -1;
                    }
                }
            }

            // Hover effects
            Vector2f currentMousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            if (offlineSprite.getGlobalBounds().contains(currentMousePos)) {
                offlineSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                offlineSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            if (matchmakingSprite.getGlobalBounds().contains(currentMousePos)) {
                matchmakingSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                matchmakingSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            if (exitSprite.getGlobalBounds().contains(currentMousePos)) {
                exitSprite.setTextureRect(IntRect(0, 130, 300, 110)); // Hovered state
            }
            else {
                exitSprite.setTextureRect(IntRect(0, 0, 300, 110)); // Normal state
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(xonix);
            window.draw(offlineSprite);
            window.draw(offlineBtn);
            window.draw(matchmakingSprite);
            window.draw(matchmakingBtn);
            window.draw(exitSprite);
            window.draw(exitBtn);
            window.display();
        }
        return -1; // Default return if window is closed
    }
};

class InventoryScreen {
private:
    sf::RenderWindow& window;
    Inventory& inventory;
    sf::Font font, font2;
    sf::Text inventoryHeading;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture buttonTexture;
    sf::Sprite themeButtonSprites[10];
    sf::Text themeButtonTexts[10];

    // Description box
    sf::Text descriptionLabel;
    sf::RectangleShape descriptionBox;
    sf::Text descriptionText;

    // Search by ID
    sf::Text searchIDLabel;
    sf::RectangleShape searchIDBox;
    sf::Text searchIDInputText;
    std::string searchIDInput;

    // Search by Name
    sf::Text searchNameLabel;
    sf::RectangleShape searchNameBox;
    sf::Text searchNameInputText;
    std::string searchNameInput;

    // Preview box and heading
    sf::RectangleShape previewBox;
    sf::Text previewHeading;
    sf::Texture previewTexture;
    sf::Sprite previewSprite;

    int hoveredThemeIndex = -1;
    int searchIDHighlight = -1;
    int searchNameHighlight = -1;

    Player*& currentPlayer; // Add this line

    // Flag to track if a preview image should be displayed
    bool hasPreview = false;

public:
    InventoryScreen(sf::RenderWindow& win, Inventory& inv, Player*& player)
        : window(win), inventory(inv), currentPlayer(player) {
        font.loadFromFile("fonts/Noxis.otf");
        font2.loadFromFile("fonts/Saphira.otf");

        // Main heading
        inventoryHeading.setFont(font);
        inventoryHeading.setString("INVENTORY");
        inventoryHeading.setCharacterSize(80);
        inventoryHeading.setFillColor(sf::Color::White);
        inventoryHeading.setPosition(730.f, 40.f);

        backgroundTexture.loadFromFile("images/background_menu2.jpg");
        backgroundSprite.setTexture(backgroundTexture);

        buttonTexture.loadFromFile("images/buttons.png");

        // Theme buttons
        float startX = 120.f;
        float startY = 260.f;
        float buttonSpacingX = 340.f;
        float buttonSpacingY = 150.f;
        int themesPerRow = 5;

        for (int i = 0; i < 10; ++i) {
            themeButtonSprites[i].setTexture(buttonTexture);
            themeButtonSprites[i].setTextureRect(sf::IntRect(0, 0, 300, 110));
            float x = startX + (i % themesPerRow) * buttonSpacingX;
            float y = startY + (i / themesPerRow) * buttonSpacingY;
            themeButtonSprites[i].setPosition(x, y);

            themeButtonTexts[i].setFont(font2);
            themeButtonTexts[i].setCharacterSize(35);
            themeButtonTexts[i].setFillColor(sf::Color::White);
            if (Theme* theme = inventory.getThemeByID(i + 1)) {
                themeButtonTexts[i].setString(std::to_string(theme->themeID) + ". " + theme->name);
            }
            else {
                themeButtonTexts[i].setString("Unknown Theme");
            }
            sf::FloatRect textBounds = themeButtonTexts[i].getLocalBounds();
            themeButtonTexts[i].setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            themeButtonTexts[i].setPosition(x + 150, y + 55);
        }

        // Description box
        float descBoxX = 120.f;
        float descBoxY = startY + 2 * buttonSpacingY + 40.f - 20.f; // Move up by 15 pixels
        float descBoxWidth = 900.f;
        float descBoxHeight = 100.f;

        descriptionLabel.setFont(font);
        descriptionLabel.setString("Description:");
        descriptionLabel.setCharacterSize(40);
        descriptionLabel.setFillColor(sf::Color::White);
        descriptionLabel.setPosition(descBoxX, descBoxY);

        descriptionBox.setSize(sf::Vector2f(descBoxWidth, descBoxHeight));
        descriptionBox.setPosition(descBoxX, descBoxY + 45.f);
        descriptionBox.setFillColor(sf::Color(30, 30, 30, 200));
        descriptionBox.setOutlineThickness(2);
        descriptionBox.setOutlineColor(sf::Color::White);

        descriptionText.setFont(font2);
        descriptionText.setCharacterSize(32);
        descriptionText.setFillColor(sf::Color::White);
        descriptionText.setPosition(descBoxX + 10.f, descBoxY + 45.f + (descBoxHeight - 32) / 2.f);

        // Search boxes (each half the width of description box, minus gap)
        float gap = 20.f;
        float searchBoxWidth = (descBoxWidth - gap) / 2.f;
        float searchBoxHeight = 70.f;
        // Move search boxes 20px further down
        float searchBoxY = descBoxY + descBoxHeight + 45.f + 10.f + 20.f;

        // Search by ID (left half)
        searchIDLabel.setFont(font);
        searchIDLabel.setString("Search by ID:");
        searchIDLabel.setCharacterSize(35);
        searchIDLabel.setFillColor(sf::Color::White);
        searchIDLabel.setPosition(descBoxX, searchBoxY);

        searchIDBox.setSize(sf::Vector2f(searchBoxWidth, searchBoxHeight));
        searchIDBox.setPosition(descBoxX, searchBoxY + 45.f);
        searchIDBox.setFillColor(sf::Color(30, 30, 30, 200));
        searchIDBox.setOutlineThickness(2);
        searchIDBox.setOutlineColor(sf::Color::White);

        searchIDInputText.setFont(font2);
        searchIDInputText.setCharacterSize(32);
        searchIDInputText.setFillColor(sf::Color::White);
        searchIDInputText.setPosition(descBoxX + 10.f, searchBoxY + 45.f + (searchBoxHeight - 32) / 2.f);

        // Search by Name (right half, with gap)
        searchNameLabel.setFont(font);
        searchNameLabel.setString("Search by Name:");
        searchNameLabel.setCharacterSize(30);
        searchNameLabel.setFillColor(sf::Color::White);
        searchNameLabel.setPosition(descBoxX + searchBoxWidth + gap, searchBoxY);

        searchNameBox.setSize(sf::Vector2f(searchBoxWidth, searchBoxHeight));
        searchNameBox.setPosition(descBoxX + searchBoxWidth + gap, searchBoxY + 45.f);
        searchNameBox.setFillColor(sf::Color(30, 30, 30, 200));
        searchNameBox.setOutlineThickness(2);
        searchNameBox.setOutlineColor(sf::Color::White);

        searchNameInputText.setFont(font2);
        searchNameInputText.setCharacterSize(30);
        searchNameInputText.setFillColor(sf::Color::White);
        searchNameInputText.setPosition(descBoxX + searchBoxWidth + gap + 10.f, searchBoxY + 45.f + (searchBoxHeight - 32) / 2.f);

        // --- Preview box alignment calculations ---
        // Outermost theme button (rightmost in top row)
        float themeRightX = startX + (themesPerRow - 1) * buttonSpacingX + 300.f; // 300 is button width

        // Keep the original left/top position
        float previewBoxX = descBoxX + descBoxWidth + 40.f;
        float previewBoxY = descBoxY + 45.f;

        // Scale width and height based on window size (e.g., 38% width, 42% height)
        sf::Vector2u winSize = window.getSize();
        float previewBoxWidth = winSize.x * 0.38f;
        float previewBoxHeight = winSize.y * 0.42f;

        // Optionally, make sure the box doesn't overflow the right/bottom edge
        if (previewBoxX + previewBoxWidth > winSize.x)
            previewBoxWidth = winSize.x - previewBoxX - 20.f;
        if (previewBoxY + previewBoxHeight > winSize.y)
            previewBoxHeight = winSize.y - previewBoxY - 20.f;

        // Reduce size and adjust position
        previewBoxX += 5.f;
        previewBoxY += 10.f;
        previewBoxWidth -= 10.f;
        previewBoxHeight -= 20.f;

        previewBox.setSize(sf::Vector2f(previewBoxWidth, previewBoxHeight));
        previewBox.setPosition(previewBoxX, previewBoxY);
        previewBox.setFillColor(sf::Color(20, 20, 20, 220));
        previewBox.setOutlineThickness(2);
        previewBox.setOutlineColor(sf::Color::White);

        // Preview heading: align Y with previewBox
        previewHeading.setFont(font);
        previewHeading.setString("Preview");
        previewHeading.setCharacterSize(40);
        previewHeading.setFillColor(sf::Color::White);
        previewHeading.setPosition(previewBoxX, previewBoxY - 50.f);

        previewSprite.setPosition(previewBoxX + 10.f, previewBoxY + 10.f);

    }

    void handleEvent(const sf::Event& event, bool& exitInventory) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            exitInventory = true;
        }
        if (event.type == sf::Event::TextEntered) {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

            bool idActive = searchIDBox.getGlobalBounds().contains(mousePos);
            bool nameActive = searchNameBox.getGlobalBounds().contains(mousePos);

            if (idActive) {
                if (event.text.unicode == '\b' && !searchIDInput.empty()) {
                    searchIDInput.pop_back();
                }
                else if (event.text.unicode < 128 && isdigit(static_cast<char>(event.text.unicode))) {
                    searchIDInput += static_cast<char>(event.text.unicode);
                }
                searchIDInputText.setString(searchIDInput);
            }
            else if (nameActive) {
                if (event.text.unicode == '\b' && !searchNameInput.empty()) {
                    searchNameInput.pop_back();
                }
                else if (event.text.unicode < 128 && isalnum(static_cast<char>(event.text.unicode))) {
                    searchNameInput += static_cast<char>(event.text.unicode);
                }
                searchNameInputText.setString(searchNameInput);
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            for (int i = 0; i < 10; ++i) {
                if (themeButtonSprites[i].getGlobalBounds().contains(mousePos)) {
                    if (currentPlayer) {
                        currentPlayer->setSelectedThemeID(i + 1);
                    }
                    exitInventory = true;
                }
            }
        }
    }

    void render() {
        // Hover effect
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
        hoveredThemeIndex = -1;

        // --- Search by ID highlight ---
        searchIDHighlight = -1;
        if (!searchIDInput.empty()) {
            int id = 0;
            bool valid = true;
            for (int i = 0; i < searchIDInput.size(); ++i) {
                if (searchIDInput[i] < '0' || searchIDInput[i] > '9') {
                    valid = false;
                    break;
                }
                id = id * 10 + (searchIDInput[i] - '0');
            }
            if (valid && id >= 1 && id <= 10) {
                searchIDHighlight = id - 1; // 0-based index
            }
        }

        // --- Search by Name highlight ---
        searchNameHighlight = -1;
        if (!searchNameInput.empty()) {
            for (int i = 0; i < 10; ++i) {
                Theme* theme = inventory.getThemeByID(i + 1);
                if (theme) {
                    // Case-insensitive substring search
                    std::string themeName = theme->name;
                    std::string search = searchNameInput;
                    // Convert both to lower case
                    for (int j = 0; j < themeName.size(); ++j)
                        if (themeName[j] >= 'A' && themeName[j] <= 'Z') themeName[j] += 'a' - 'A';
                    for (int j = 0; j < search.size(); ++j)
                        if (search[j] >= 'A' && search[j] <= 'Z') search[j] += 'a' - 'A';
                    if (themeName.find(search) != std::string::npos) {
                        searchNameHighlight = i;
                        break;
                    }
                }
            }
        }

        mousePixel = sf::Mouse::getPosition(window);
        mousePos = window.mapPixelToCoords(mousePixel);

        // Determine hovered theme index
        hoveredThemeIndex = -1;
        for (int i = 0; i < 10; ++i) {
            if (themeButtonSprites[i].getGlobalBounds().contains(mousePos)) {
                hoveredThemeIndex = i;
                break;
            }
        }

        // Clear all highlights
        for (int i = 0; i < 10; ++i) {
            themeButtonSprites[i].setTextureRect(sf::IntRect(0, 0, 300, 110));
        }

        // Determine which theme to highlight and show description for
        int highlightIndex = -1;
        if (hoveredThemeIndex != -1) {
            highlightIndex = hoveredThemeIndex;
        }
        else if (searchIDHighlight != -1) {
            highlightIndex = searchIDHighlight;
        }
        else if (searchNameHighlight != -1) {
            highlightIndex = searchNameHighlight;
        }

        // Apply highlight if any
        if (highlightIndex != -1) {
            themeButtonSprites[highlightIndex].setTextureRect(sf::IntRect(0, 130, 300, 110));
        }

        // Set description
        if (highlightIndex != -1) {
            Theme* theme = inventory.getThemeByID(highlightIndex + 1);
            if (theme) {
                descriptionText.setString(theme->description);
            }
        }
        else {
            descriptionText.setString("Hover over a theme to see its description.");
        }




        // Preview logic
        hasPreview = false;
        std::string previewPath = "";
        int previewIndex = hoveredThemeIndex;
        if (previewIndex == -1 && searchIDHighlight != -1) previewIndex = searchIDHighlight;
        if (previewIndex == -1 && searchNameHighlight != -1) previewIndex = searchNameHighlight;

        if (previewIndex != -1) {
            if (Theme* theme = inventory.getThemeByID(previewIndex + 1)) {
                previewPath = theme->backgroundImage;
            }
        }

        if (!previewPath.empty() && previewTexture.loadFromFile(previewPath)) {
            previewSprite.setTexture(previewTexture, true);
            float scaleX = (previewBox.getSize().x - 20.f) / previewTexture.getSize().x;
            float scaleY = (previewBox.getSize().y - 20.f) / previewTexture.getSize().y;
            float scale = (scaleX < scaleY) ? scaleX : scaleY; // No std::min
            previewSprite.setScale(scale, scale);
            previewSprite.setPosition(
                previewBox.getPosition().x + (previewBox.getSize().x - previewTexture.getSize().x * scale) / 2.f,
                previewBox.getPosition().y + (previewBox.getSize().y - previewTexture.getSize().y * scale) / 2.f
            );
            hasPreview = true;
        }

        // Rendering process
        window.clear();
        window.draw(backgroundSprite);
        window.draw(inventoryHeading);
        for (int i = 0; i < 10; ++i) {
            window.draw(themeButtonSprites[i]);
            window.draw(themeButtonTexts[i]);
        }
        window.draw(descriptionLabel);
        window.draw(descriptionBox);
        window.draw(descriptionText);
        window.draw(searchIDLabel);
        window.draw(searchIDBox);
        window.draw(searchIDInputText);
        window.draw(searchNameLabel);
        window.draw(searchNameBox);
        window.draw(searchNameInputText);
        window.draw(previewHeading);
        window.draw(previewBox);
        if (hasPreview) window.draw(previewSprite);
        window.display();
    }
};