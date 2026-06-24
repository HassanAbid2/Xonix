#pragma once
#include "Global.h"
#include "FriendSystem.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;

class Player {
private:
    int playerID;
    int totalPoints;    // Accumulates points over all games
    int highScore;      // Highest score achieved in a single game
    int currentScore;   // Points in the current game
    int wins;
    int losses;
    string username;
    string password;
    string nickname;
    string profilePic;

    FriendList friendsList;        // Linked list for friends
    FriendRequestList friendRequests; // Linked list for friend requests

    int rewardCounter;
    int powerUpCount;
    int nextPowerUpThreshold;

    // Position and Sprite (from the first Player class)
    float x;
    float y;
    sf::Sprite sprite;

    // Status flags (from the first Player class)
    bool constructing;
    bool eliminated;

    int selectedThemeID = 1;

public:
    // Constructor
    Player(int id = 0, const string& uname = "", const string& pwd = "", const string& nname = "", const string& pPic = "")
        : playerID(id), username(uname), password(pwd), nickname(nname), profilePic(pPic),
        totalPoints(0), highScore(0), currentScore(0), wins(0), losses(0), rewardCounter(0),
        powerUpCount(0), nextPowerUpThreshold(50),
        x(0.0f), y(0.0f), constructing(false), eliminated(false)
    {
        if (!isValidUsername(username) || !isValidPassword(password)) {
            username = "default_user";
            password = "password";
            currentScore = 0;
        }

        cout << "Player object created: " << username << endl;
    }

    // Copy constructor
    Player(const Player& other)
        : playerID(other.playerID), totalPoints(other.totalPoints), highScore(other.highScore),
        currentScore(other.currentScore), wins(other.wins), losses(other.losses),
        username(other.username), password(other.password), nickname(other.nickname),
        profilePic(other.profilePic), rewardCounter(other.rewardCounter),
        powerUpCount(other.powerUpCount), nextPowerUpThreshold(other.nextPowerUpThreshold),
        x(other.x), y(other.y), sprite(other.sprite),
        constructing(other.constructing), eliminated(other.eliminated),
        friendsList(other.friendsList), friendRequests(other.friendRequests) {}

    // Destructor
    ~Player() {
        cout << "Player object destroyed: " << username << endl;
    }

    // Getters
    int getPlayerID() const { return playerID; }
    const string& getUsername() const { return username; }
    const string& getPassword() const { return password; }
    const string& getNickname() const { return nickname; }
    const string& getProfilePic() const { return profilePic; }
    int getTotalPoints() const { return totalPoints; }
    int getHighScore() const { return highScore; }
    int getCurrentScore() const { return currentScore; }
    int getWins() const { return wins; }
    int getLosses() const { return losses; }
    int getPowerUpCount() const { return powerUpCount; }
    float getX() const { return x; }
    float getY() const { return y; }
    sf::Sprite& getSprite() { return sprite; }
    bool isConstructing() const { return constructing; }
    bool isEliminated() const { return eliminated; }
    void setSelectedThemeID(int id) { selectedThemeID = id; }
    int getSelectedThemeID() const { return selectedThemeID; }
    void setPlayerID(int id) { playerID = id; }
    void setUsername(const std::string& uname) { username = uname; }

    // Overloaded getters for FriendList and FriendRequestList
    FriendList& getFriendList() { return friendsList; }
    const FriendList& getFriendList() const { return friendsList; }

    FriendRequestList& getFriendRequestList() { return friendRequests; }
    const FriendRequestList& getFriendRequestList() const { return friendRequests; }

    // Setters
    bool setPassword(const string& newPassword) {
        if (isValidPassword(newPassword)) {
            password = newPassword;
            return true;
        }
        return false;
    }

    void setNickname(const string& newNickname) { nickname = newNickname; }
    void setProfilePic(const string& newProfilePic) { profilePic = newProfilePic; }
    void setTotalPoints(int points) { totalPoints = points; }
    void setHighScore(int score) {
        if (score > highScore) {
            highScore = score;
        }
    }
    void setWins(int w) { wins = w; }
    void setLosses(int l) { losses = l; }
    void setPowerUpCount(int count) { powerUpCount = count; }
    void setCurrentScore(int score) { currentScore = score; }
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }

    // Methods to update player stats
    void addWin() { wins += 1; }
    void addLoss() { losses += 1; eliminated = true; }
    void addPoints(int points) { totalPoints += points; }
    void addCurrentScore(int points) { currentScore += points; }

    void tilesCaptured(int tilesInRow) {
        int pointsEarned = tilesInRow;
        if (tilesInRow >= 10) {
            pointsEarned *= 2;
            rewardCounter += 1;
        }

        if (rewardCounter >= 3 && tilesInRow >= 5) {
            pointsEarned *= 2;
        }

        if (rewardCounter >= 5 && tilesInRow >= 5) {
            pointsEarned *= 2;
        }

        addCurrentScore(pointsEarned);

        if (currentScore >= nextPowerUpThreshold) {
            powerUpCount += 1;
            nextPowerUpThreshold += 30;
        }

        cout << "Tiles captured: " << tilesInRow << ", Points earned: " << pointsEarned
            << ", Current score: " << currentScore << endl;
    }

    void finalizeGame() {
        addPoints(currentScore);

        if (currentScore > highScore) {
            highScore = currentScore;
            cout << "New high score for " << username << ": " << highScore << endl;
        }

        //currentScore = 0;
        constructing = false;
        eliminated = false;
    }

    bool usePowerUp() {
        if (powerUpCount > 0) {
            powerUpCount--;
            cout << "Power-up used. Remaining: " << powerUpCount << endl;
            return true;
        }
        return false;
    }

    void move(int dx, int dy) {
        // Update logical positions
        x += dx;
        y += dy;

        // Update sprite position
        sprite.move(static_cast<float>(dx), static_cast<float>(dy));
    }

    // Friend-related methods
    void addFriend(int friendID) { friendsList.addFriend(friendID); }
    void removeFriend(int friendID) { friendsList.removeFriend(friendID); }
    bool isFriend(int friendID) const { return friendsList.isFriend(friendID); }
    void displayFriends() const { friendsList.displayFriends(); }

    // Friend Request-related methods
    void addFriendRequest(int senderID) { friendRequests.addRequest(senderID); }
    void removeFriendRequest(int senderID) { friendRequests.removeRequest(senderID); }
    bool hasFriendRequest(int senderID) const { return friendRequests.hasRequest(senderID); }
    void displayFriendRequests() const { friendRequests.displayRequests(); }

    // Validation methods
    bool isValidUsername(const string& uname) const {
        for (char ch : uname) {
            if (ch == ' ') {
                return false;
            }
        }
        return true;
    }

    bool isValidPassword(const string& pwd) const {
        return pwd.length() >= 8;
    }

    void addPowerUp() { powerUpCount += 1; }

    void resetScore(){
		currentScore = 0;
		nextPowerUpThreshold = 50;
		rewardCounter = 0;
	}

    void reset() {
        currentScore = 0;
        wins = 0;
        losses = 0;
        powerUpCount = 0;
        nextPowerUpThreshold = 50;
        rewardCounter = 0;
        constructing = false;
        eliminated = false;
    }
};