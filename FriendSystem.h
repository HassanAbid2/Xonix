#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>
#include <string>
extern class HashTable;
using namespace sf;
using namespace std;

struct FriendNode {
    int friendID;
    FriendNode* next;
    FriendNode(int id) : friendID(id), next(nullptr) {}
};

class FriendList {
private:
    FriendNode* head;
public:
    FriendList() : head(nullptr) {}
    ~FriendList() { clear(); }

    FriendList(const FriendList& other) : head(nullptr) {
        FriendNode* current = other.head;
        while (current) {
            addFriend(current->friendID); // Reuse existing addFriend method
            current = current->next;
        }
    }

    FriendList& operator=(const FriendList& other) {
        if (this != &other) {
            clear(); // Ensure no memory leaks
            FriendNode* current = other.head;
            while (current) {
                addFriend(current->friendID); // Reuse existing addFriend method
                current = current->next;
            }
        }
        return *this;
    }

    void addFriend(int id) {
        if (isFriend(id)) return;

        // Validate the current list structure
        FriendNode* current = head;
        while (current) {
            if (!current->next) break; // Ensure proper termination
            current = current->next;
        }

        FriendNode* newNode = new FriendNode(id);
        newNode->next = head; // Insert at the front
        head = newNode;
    }

    bool isFriend(int id) const {
        FriendNode* curr = head;
        while (curr) {
            if (curr->friendID == id) return true;
            curr = curr->next;
        }
        return false;
    }

    void removeFriend(int id) {
        FriendNode* curr = head, * prev = nullptr;
        while (curr) {
            if (curr->friendID == id) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    void clear() {
        while (head) {
            FriendNode* tmp = head;
            head = head->next;
            cout << "Deleting friend with ID: " << tmp->friendID << endl;
            delete tmp;
        }
        head = nullptr; // Ensure head is null
    }

    void displayFriends() const {
        FriendNode* curr = head;
        if (!curr) {
            cout << "No friends\n";
            return;
        }
        while (curr) {
            cout << "Friend ID: " << curr->friendID << std::endl;
            curr = curr->next;
        }
    }

    void validate() const {
        FriendNode* current = head;
        int count = 0;
        while (current) {
            if (!current->next && count > 0) {
                cout << "List properly terminated after " << count << " nodes." << endl;
                return;
            }
            current = current->next;
            count++;
        }
        cout << "List is empty or improperly terminated!" << endl;
    }

    FriendNode* getHead() const { return head; }
};

class FriendRequestList {
public:
    struct RequestNode {
        int senderID;
        RequestNode* next;
        RequestNode(int id) : senderID(id), next(nullptr) {}
    };
private:
    RequestNode* head;
public:
    FriendRequestList() : head(nullptr) {}

    FriendRequestList(const FriendRequestList& other) : head(nullptr) {
		RequestNode* current = other.head;
		while (current) {
			addRequest(current->senderID); // Reuse existing addRequest method
			current = current->next;
		}
	}

    FriendRequestList& operator=(const FriendRequestList& other) {
		if (this != &other) {
			clear(); // Ensure no memory leaks
			RequestNode* current = other.head;
			while (current) {
				addRequest(current->senderID); // Reuse existing addRequest method
				current = current->next;
			}
		}
		return *this;
	}

    ~FriendRequestList() { clear(); }


    void addRequest(int senderID) {
    if (hasRequest(senderID)) return;

    // Validate the current list structure
    RequestNode* current = head;
    while (current) {
        if (!current->next) break; // Ensure proper termination
        current = current->next;
    }

    RequestNode* newReq = new RequestNode(senderID);
    newReq->next = head; // Insert at the front
    head = newReq;
}

    bool hasRequest(int senderID) const {
        RequestNode* curr = head;
        while (curr) {
            if (curr->senderID == senderID) return true;
            curr = curr->next;
        }
        return false;
    }

    void removeRequest(int senderID) {
        RequestNode* curr = head, * prev = nullptr;
        while (curr) {
            if (curr->senderID == senderID) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    void clear() {
        while (head) {
            RequestNode* tmp = head;
            head = head->next;
            cout << "Deleting friend request from: " << tmp->senderID << endl;
            delete tmp;
        }
        head = nullptr; // Ensure head is null
    }

    void validate() const {
        RequestNode* current = head;
        int count = 0;

        while (current) {
            cout << "Request from ID: " << current->senderID << endl;
            count++;
            if (!current->next) {
                cout << "List properly terminated after " << count << " nodes." << endl;
                return;
            }
            current = current->next;
        }

        cout << "List is empty or improperly terminated!" << endl;
    }

    void displayRequests() const {
        RequestNode* curr = head;
        if (!curr) {
            cout << "No friend requests\n";
            return;
        }
        while (curr) {
           cout << "Friend request from: " << curr->senderID << std::endl;
            curr = curr->next;
        }
    }

    RequestNode* getHead() const { return head; }
};
void displayFriendSystem(sf::RenderWindow& window,
    FriendList& friendList,
    FriendRequestList& friendRequestList,
    int currentPlayerID,
    HashTable& hashTable);