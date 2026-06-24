#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "Player.h"

using namespace std;

class PlayerArray
{
private:
    Player* players; 
    int size;          
    int capacity;     

    void resize() {
        capacity *= 2; 
        Player* newPlayers = new Player[capacity];
        for (int i = 0; i < size; ++i) {
            newPlayers[i] = Player(players[i]);
        }
        delete[] players;
        players = newPlayers;
    }

public:
    PlayerArray(int initialCapacity = 10)
        : size(0), capacity(initialCapacity)
    {
        players = new Player[capacity];
    }

    ~PlayerArray()
    {
        delete[] players;
    }

    void addPlayer(const Player& player)
    {
        if (size >= capacity)
        {
            resize();
        }
        cout << size << " Added  " << player.getNickname() << endl;
        players[size++] = player;
    }

    // Find a player by username
    Player* findPlayerByUsername(const string& uname) const
    {
        for (int i = 0; i < size; ++i)
        {
            if (players[i].getUsername() == uname)
            {
                return &players[i];
            }
        }
        return nullptr;
    }

    int getSize() const
    {
        return size;
    }

    Player* getPlayers() const
    {
        return players;
    }
};

// Class to handle saving and loading Player data
class PlayerData
{
private:
    string filename;

public:
    PlayerData(const string& file) : filename(file) {}

    bool savePlayers(const PlayerArray& playerArray) const
    {
        ofstream outFile(filename, ios::trunc);
        if (!outFile.is_open())
        {
            cerr << "Failed to open file for writing: " << filename << endl;
            return false;
        }

        // Write header
        outFile << "PlayerID,Username,Password,Nickname,ProfilePic,TotalPoints,Wins,Losses,PowerUpCount,HighScore,Friends,FriendRequests\n";

        for (int i = 0; i < playerArray.getSize(); ++i)
        {
            const Player& player = playerArray.getPlayers()[i];

            // Serialize friends and friend requests
            string friendsSerialized = serializeFriends(player.getFriendList());
            string requestsSerialized = serializeFriendRequests(player.getFriendRequestList());

            outFile << player.getPlayerID() << ','
                << player.getUsername() << ','
                << player.getPassword() << ','
                << player.getNickname() << ','
                << player.getProfilePic() << ','
                << player.getTotalPoints() << ','
                << player.getWins() << ','
                << player.getLosses() << ','
                << player.getPowerUpCount() << ','
                << player.getHighScore() << ','
                << '"' << friendsSerialized << '"' << ','
                << '"' << requestsSerialized << '"' << '\n';

            cout << "Saving: " << friendsSerialized << "\n";
        }
        
        outFile.close();
        return true;
    }

    PlayerArray loadPlayers() const
    {
        PlayerArray playerArray;
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "Failed to open file for reading: " << filename << endl;
            return playerArray;
        }

        string line;
        int lineNumber = 0;

        // Read and skip the header line
        if (getline(inFile, line))
        {
            ++lineNumber;
        }
        else
        {
            cerr << "Empty file or failed to read header from file: " << filename << endl;
            inFile.close();
            return playerArray;
        }

        // Read and parse data lines
        while (getline(inFile, line))
        {
            ++lineNumber;
            if (line.empty())
            {
                cerr << "Empty line at line " << lineNumber << endl;
                continue; // Skip empty lines
            }

            size_t pos = 0;
            string token;
            int id, totalPoints, wins, losses, powerUpCount, highScore;
            string username, password, nickname, profilePic, friendsSerialized, requestsSerialized;

            try
            {
                // Parse PlayerID
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing PlayerID");
                id = stoi(token);

                // Parse Username
                username = getNextToken(line, pos);
                if (username.empty()) throw invalid_argument("Missing Username");

                // Parse Password
                password = getNextToken(line, pos);
                if (password.empty()) throw invalid_argument("Missing Password");

                // Parse Nickname
                nickname = getNextToken(line, pos);
                if (nickname.empty()) throw invalid_argument("Missing Nickname");

                // Parse ProfilePic
                profilePic = getNextToken(line, pos);
                if (profilePic.empty()) throw invalid_argument("Missing ProfilePic");

                // Parse TotalPoints
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing TotalPoints");
                totalPoints = stoi(token);

                // Parse Wins
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing Wins");
                wins = stoi(token);

                // Parse Losses
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing Losses");
                losses = stoi(token);

                // Parse PowerUpCount
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing PowerUpCount");
                powerUpCount = stoi(token);

                // Parse HighScore
                token = getNextToken(line, pos);
                if (token.empty()) throw invalid_argument("Missing HighScore");
                highScore = stoi(token);

                // Parse Friends
                friendsSerialized = getNextToken(line, pos);

                // Parse FriendRequests
                requestsSerialized = getNextToken(line, pos);

                // Create Player object
                Player player(id, username, password, nickname, profilePic);

                // Set attributes
                player.setTotalPoints(totalPoints);
                player.setWins(wins);
                player.setLosses(losses);
                player.setPowerUpCount(powerUpCount);
                player.setHighScore(highScore);

                // Deserialize friends and friend requests
                deserializeFriends(friendsSerialized, player.getFriendList());
                deserializeFriendRequests(requestsSerialized, player.getFriendRequestList());

                playerArray.addPlayer(player);
            }
            catch (const invalid_argument& e)
            {
                cerr << "Invalid data at line " << lineNumber << ": " << e.what()
                    << " | Line Content: " << line << endl;
                continue;
            }
        }

        inFile.close();
        return playerArray;
    }

private:
    // Helper to get the next token from a CSV line
    string getNextToken(const string& line, size_t& pos) const
    {
        if (pos >= line.length()) return "";
        size_t nextPos = line.find(',', pos);
        string token = line.substr(pos, nextPos - pos);
        cout << "Extracted token: " << token << endl; // Debug: Show each token
        pos = (nextPos == string::npos) ? line.length() : nextPos + 1;
        return token;
    }

    // Serialize a FriendList to a semicolon-separated string
    string serializeFriends(const FriendList& friends) const
    {
        string result;
        FriendNode* current = friends.getHead();
        cout << "Serializing friends: ";
        while (current != nullptr)
        {
            cout << current->friendID << " ";
            result += to_string(current->friendID);
            if (current->next != nullptr)
            {
                result += ';';
            }
            current = current->next;
        }
        cout << endl;
        return result;
    }

    // Serialize a FriendRequestList to a semicolon-separated string
    string serializeFriendRequests(const FriendRequestList& requests) const
    {
        string result;
        FriendRequestList::RequestNode* current = requests.getHead();
        while (current != nullptr)
        {
            result += to_string(current->senderID);
            if (current->next != nullptr)
            {
                result += ';';
            }
            current = current->next;
        }
        return result;
    }

    // Deserialize a semicolon-separated string into a FriendList
    void deserializeFriends(const string& data, FriendList& friends)const {
        if (data.empty() || data == "\"\"") return;

        size_t start = 0, end = data.find(';');
        while (start != string::npos) {
            string token = data.substr(start, end - start);
            removeQuotesAndTrim(token); // Ensure quotes are removed
            if (!token.empty()) {
                int friendID = stoi(token);
                if (!friends.isFriend(friendID)) {
                    friends.addFriend(friendID);
                }
            }
            start = (end == string::npos) ? string::npos : end + 1;
            end = data.find(';', start);
        }
    }

    // Deserialize a semicolon-separated string into a FriendRequestList
    void deserializeFriendRequests(const string& data, FriendRequestList& requests) const
    {
        if (data.empty() || data == "\"\"") return;

        size_t start = 0;
        size_t end = data.find(';');

        while (start != string::npos)
        {
            string token = data.substr(start, end - start);
            removeQuotesAndTrim(token);
            if (!token.empty())
            {
                try
                {
                    int senderID = stoi(token);
                    requests.addRequest(senderID);
                }
                catch (...)
                {
                    cerr << "Invalid friend request ID: " << token << endl;
                }
            }
            start = (end == string::npos) ? string::npos : end + 1;
            end = data.find(';', start);
        }
    }

    // Helper to remove quotes and trim whitespace
    void removeQuotesAndTrim(string& str) const
    {
        cout << "Before removing quotes and trimming: " << str << endl; // Debug
        if (!str.empty() && str.front() == '\"') str.erase(0, 1);
        if (!str.empty() && str.back() == '\"') str.pop_back();

        size_t start = 0;
        while (start < str.length() && (str[start] == ' ' || str[start] == '\t'))
        {
            ++start;
        }
        size_t end = str.length();
        while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t'))
        {
            --end;
        }
        str = str.substr(start, end - start);
        cout << "After removing quotes and trimming: " << str << endl; // Debug
    }
};