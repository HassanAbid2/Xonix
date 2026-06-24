#pragma once
#include <iostream>
#include <string>
#include "Player.h"

class HashTable {
private:
    static const int TABLE_SIZE = 100;

    struct Entry {
        int key;       // Player ID
        int value;     // Index in Player array
        bool isOccupied; // Indicates if the slot is occupied
        bool isDeleted;  // Indicates if the slot has been logically deleted
        Entry() : key(-1), value(-1), isOccupied(false), isDeleted(false) {}
    };

    Entry table[TABLE_SIZE];
    Player* playerArray;
    int playerArraySize;

    int hashFunction(int key) const { return key % TABLE_SIZE; }

    int probeNext(int index) const {
        return (index + 1) % TABLE_SIZE; // Linear probing
    }

public:
    HashTable() : playerArray(nullptr), playerArraySize(0) {}

    void setPlayerArray(Player* arr, int sz) {
        playerArray = arr;
        playerArraySize = sz;
    }

    void insert(int key, int value) {
        int index = hashFunction(key);
        int originalIndex = index;
        bool inserted = false;

        // Linear probing to find an empty or deleted slot
        while (!inserted) {
            if (!table[index].isOccupied || table[index].isDeleted) {
                table[index].key = key;
                table[index].value = value;
                table[index].isOccupied = true;
                table[index].isDeleted = false;
                inserted = true;
                std::cout << "Inserted key " << key << " with value " << value << " at index " << index << std::endl;
            } else if (table[index].key == key) {
                // Update the value if the key already exists
                table[index].value = value;
                inserted = true;
                std::cout << "Updated key " << key << " with value " << value << " at index " << index << std::endl;
            } else {
                index = probeNext(index);
                if (index == originalIndex) {
                    std::cerr << "HashTable is full, cannot insert key " << key << std::endl;
                    return;
                }
            }
        }
    }

    void remove(int key) {
        int index = hashFunction(key);
        int originalIndex = index;

        // Linear probing to find the key
        while (table[index].isOccupied) {
            if (table[index].key == key && !table[index].isDeleted) {
                table[index].isDeleted = true; // Mark as logically deleted
                std::cout << "Removed key " << key << " from index " << index << std::endl;
                return;
            }
            index = probeNext(index);
            if (index == originalIndex) break; // Return to the original index
        }
        std::cerr << "Key " << key << " not found for removal" << std::endl;
    }

    int search(int key) const {
        int index = hashFunction(key);
        int originalIndex = index;

        // Linear probing to find the key
        while (table[index].isOccupied) {
            if (table[index].key == key && !table[index].isDeleted) {
                return table[index].value;
            }
            index = probeNext(index);
            if (index == originalIndex) break; // Return to the original index
        }
        return -1; // Key not found
    }

    void clear() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            table[i].isOccupied = false;
            table[i].isDeleted = false;
        }
        std::cout << "Cleared the hash table" << std::endl;
    }

    // --------- FRIEND SYSTEM METHODS -----------

    std::string getPlayerNameByID(int playerID) const {
        int idx = search(playerID);
        if (idx != -1 && playerArray && idx < playerArraySize) {
            const Player& p = playerArray[idx];
            return p.getUsername();
        }
        return "";
    }

    FriendList& getFriendListByID(int playerID) const {
        int idx = search(playerID);
        if (idx == -1 || !playerArray || idx >= playerArraySize) {
            throw std::out_of_range("Invalid player ID or corrupted PlayerArray.");
        }
        return playerArray[idx].getFriendList();
    }

    FriendRequestList& getFriendRequestListByID(int playerID) const {
        int idx = search(playerID);
        if (idx != -1 && playerArray && idx < playerArraySize) {
            return playerArray[idx].getFriendRequestList();
        }
        throw std::out_of_range("Player ID not found for FriendRequestList");
    }

    Player* getPlayerByID(int playerID) const {
        int idx = search(playerID);
        if (idx != -1 && playerArray && idx < playerArraySize) {
            return &playerArray[idx];
        }
        return nullptr;
    }

    Player* getPlayerByName(const std::string& username) const {
        if (!playerArray) return nullptr;
        for (int i = 0; i < playerArraySize; ++i) {
            if (playerArray[i].getUsername() == username) {
                return &playerArray[i];
            }
        }
        return nullptr;
    }

    void display() const {
        std::cout << "HashTable Contents:\n";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            if (table[i].isOccupied && !table[i].isDeleted) {
                std::cout << "Index " << i << ": [ID: " << table[i].key
                          << ", Value: " << table[i].value
                          << ", Username: " << getPlayerNameByID(table[i].key)
                          << "]" << std::endl;
            }
        }
    }
};