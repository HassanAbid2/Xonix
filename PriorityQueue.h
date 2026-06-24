#pragma once
#include <cstdlib>
#include "Player.h"

// Structure used to store references to candidates along with 
// the absolute difference in score compared to the current player's score.
struct MatchPlayer {
    Player* player;
    int diff; // lower diff means closer in score.
    MatchPlayer* next;
};

class MatchmakingQueue {
private:
    MatchPlayer* front;
public:
    MatchmakingQueue() : front(nullptr) {}

    // Insert candidate in ascending order of diff.
    void insert(Player* player, int diff) {
        MatchPlayer* newMP = new MatchPlayer();
        newMP->player = player;
        newMP->diff = diff;
        newMP->next = nullptr;

        if (front == nullptr || diff < front->diff) {
            newMP->next = front;
            front = newMP;
        }
        else {
            MatchPlayer* current = front;
            while (current->next != nullptr && current->next->diff <= diff) {
                current = current->next;
            }
            newMP->next = current->next;
            current->next = newMP;
        }
    }

    bool isEmpty() {
        return front == nullptr;
    }

    // Get the candidate with the smallest score difference.
    // (This does not remove it from the queue.)
    Player* getBestMatch() {
        if (front == nullptr)
            return nullptr;
        return front->player;
    }

    ~MatchmakingQueue() {
        while (front != nullptr) {
            MatchPlayer* temp = front;
            front = front->next;
            delete temp;
        }
    }
};