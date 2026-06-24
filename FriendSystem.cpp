#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <cctype>
#include "FriendSystem.h"
#include "HashTable.h"

using namespace sf;
using namespace std;

void displayFriendSystem(RenderWindow& window,
    FriendList& friendList,
    FriendRequestList& friendRequestList,
    int currentPlayerID,
    HashTable& hashTable)
{
    // Colors
    const Color hoverColor(173, 216, 230);
    const Color activeTabColor(Color::Cyan);
    const Color inactiveTabColor(Color::White);

    // Debug information
    std::cout << "Displaying friend system for player ID: " << currentPlayerID << endl;
    friendList.validate();
    friendRequestList.validate();
    cout << "\nDone validating friend system" << endl;

    // Load font
    Font font;
    if (!font.loadFromFile("fonts/joystix.otf")) {
        cout << "Error loading font for Friends Window" << endl;
        return;
    }

    // Load sprites for Accept, Reject, and Cross buttons
    Texture acceptTexture, rejectTexture, crossTexture;
    if (!acceptTexture.loadFromFile("images/tick.png")) {
        cout << "Error loading tick.png" << endl;
        return;
    }
    if (!rejectTexture.loadFromFile("images/cross.png")) {
        cout << "Error loading cross.png" << endl;
        return;
    }
    if (!crossTexture.loadFromFile("images/cross.png")) {
        cout << "Error loading delete_cross.png" << endl;
        return;
    }

    Sprite acceptSprite(acceptTexture);
    Sprite rejectSprite(rejectTexture);
    Sprite crossSprite(crossTexture);

    // Scale the sprites down
    acceptSprite.setScale(0.15f, 0.15f);
    rejectSprite.setScale(0.15f, 0.15f);
    crossSprite.setScale(0.15f, 0.15f);

    // Tab states
    bool friendsTabActive = true;
    bool requestsTabActive = false;
    bool searchTabActive = false;

    // Search variables
    string searchBuffer;
    bool playerFound = false;
    int foundPlayerID = -1;
    string foundPlayerName = "", searchErrorMsg = "";

    // UI components
    Text searchText, friendsTab, requestsTab, searchTabText, messageText, searchLabel;
    RectangleShape searchBox(Vector2f(100, 30));

    // Tab text setup
    auto setupTabText = [&](Text& tab, const string& label, int x, int y) {
        tab.setFont(font);
        tab.setString(label);
        tab.setCharacterSize(24);
        tab.setFillColor(inactiveTabColor);
        tab.setPosition(x, y);
        };

    setupTabText(friendsTab, "Friends", 50, 10);
    setupTabText(requestsTab, "Requests", 200, 10);
    setupTabText(searchTabText, "Search", 400, 10);

    // Message text
    messageText.setFont(font);
    messageText.setCharacterSize(20);
    messageText.setFillColor(Color::White);
    messageText.setPosition(50, 100);

    // Search label and input box
    searchLabel.setFont(font);
    searchLabel.setString("Search Player ID:");
    searchLabel.setCharacterSize(18);
    searchLabel.setFillColor(Color::White);
    searchLabel.setPosition(50, 60);

    searchBox.setPosition(330, 60);
    searchBox.setFillColor(Color::Black);
    searchBox.setOutlineThickness(2);
    searchBox.setOutlineColor(Color::White);

    searchText.setFont(font);
    searchText.setCharacterSize(18);
    searchText.setFillColor(Color::White);
    searchText.setPosition(335, 60);

    // Main event/draw loop
    while (window.isOpen()) {
        Event event;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            // --- Tab hover logic ---
            friendsTab.setFillColor(
                friendsTabActive ? activeTabColor :
                (friendsTab.getGlobalBounds().contains(mousePos) ? hoverColor : inactiveTabColor)
            );
            requestsTab.setFillColor(
                requestsTabActive ? activeTabColor :
                (requestsTab.getGlobalBounds().contains(mousePos) ? hoverColor : inactiveTabColor)
            );
            searchTabText.setFillColor(
                searchTabActive ? activeTabColor :
                (searchTabText.getGlobalBounds().contains(mousePos) ? hoverColor : inactiveTabColor)
            );

            // --- Mouse click logic ---
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                if (friendsTab.getGlobalBounds().contains(mousePos)) {
                    friendsTabActive = true;
                    requestsTabActive = false;
                    searchTabActive = false;
                    messageText.setString("");
                }
                else if (requestsTab.getGlobalBounds().contains(mousePos)) {
                    friendsTabActive = false;
                    requestsTabActive = true;
                    searchTabActive = false;
                    messageText.setString("");
                }
                else if (searchTabText.getGlobalBounds().contains(mousePos)) {
                    friendsTabActive = false;
                    requestsTabActive = false;
                    searchTabActive = true;
                    messageText.setString("");
                }
                else if (searchTabActive && playerFound) {
                    FloatRect addButtonRect(50.f, 180.f, 210.f, 30.f);
                    if (addButtonRect.contains(mousePos)) {
                        if (foundPlayerID == currentPlayerID)
                            searchErrorMsg = "Cannot add yourself!";
                        else if (friendList.isFriend(foundPlayerID))
                            searchErrorMsg = "Already your friend!";
                        else {
                            try {
                                FriendRequestList& receiversRequestList = hashTable.getFriendRequestListByID(foundPlayerID);
                                if (!receiversRequestList.hasRequest(currentPlayerID)) {
                                    receiversRequestList.addRequest(currentPlayerID);
                                    searchErrorMsg = "Request sent!";
                                    cout << "Friend request sent from ID: " << currentPlayerID << " to ID: " << foundPlayerID << endl;
                                }
                                else
                                    searchErrorMsg = "Request already sent!";
                            }
                            catch (...) {
                                searchErrorMsg = "User not found.";
                            }
                        }
                    }
                }
                // Requests tab accept/reject logic
                else if (requestsTabActive) {
                    int y = 125;
                    for (FriendRequestList::RequestNode* node = friendRequestList.getHead(); node; node = node->next) {
                        FloatRect acceptBounds(400.f, float(y), 24.f, 24.f); // Adjust sprite bounds
                        FloatRect rejectBounds(450.f, float(y), 24.f, 24.f); // Adjust sprite bounds

                        if (acceptBounds.contains(mousePos)) {
                            int senderID = node->senderID;
                            friendList.addFriend(senderID);
                            try {
                                hashTable.getFriendListByID(senderID).addFriend(currentPlayerID);
                            }
                            catch (...) {}
                            friendRequestList.removeRequest(senderID);
                            messageText.setString("Accepted request from " + hashTable.getPlayerNameByID(senderID));
                            cout << "Friend request accepted. Sender ID: " << senderID << ", Receiver ID: " << currentPlayerID << endl;
                            break;
                        }
                        if (rejectBounds.contains(mousePos)) {
                            int senderID = node->senderID;
                            friendRequestList.removeRequest(senderID);
                            messageText.setString("Rejected request from " + hashTable.getPlayerNameByID(senderID));
                            cout << "Friend request rejected. Sender ID: " << senderID << ", Receiver ID: " << currentPlayerID << endl;
                            break;
                        }
                        y += 48; // Adjust spacing for rows
                    }
                }
                else if (friendsTabActive) {
                    int y = 125;
                    for (FriendNode* node = friendList.getHead(); node; node = node->next) {
                        FloatRect crossBounds(400.f, float(y), 24.f, 24.f); // Adjust sprite bounds
                        if (crossBounds.contains(mousePos)) {
                            int friendID = node->friendID;
                            friendList.removeFriend(friendID); // Remove friend from list
                            hashTable.getFriendListByID(friendID).removeFriend(currentPlayerID); // Remove from their list too
                            messageText.setString("Removed friend with ID: " + to_string(friendID));
                            cout << "Friend removed. Friend ID: " << friendID << ", Player ID: " << currentPlayerID << endl;
                            break;
                        }
                        y += 48; // Adjust spacing for rows
                    }
                }
            }

            // --- Text input ---
            if (event.type == Event::TextEntered) {
                if (searchTabActive) {
                    if (event.text.unicode == '\b') { // Backspace
                        if (!searchBuffer.empty()) {
                            searchBuffer.pop_back();
                            searchText.setString(searchBuffer);
                        }
                    }
                    else if (event.text.unicode < 128 && isdigit(event.text.unicode)) { // Digits only
                        searchBuffer += static_cast<char>(event.text.unicode);
                        searchText.setString(searchBuffer);
                    }
                }
            }

            // --- Enter key logic ---
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (searchTabActive) {
                    playerFound = false;
                    searchErrorMsg = "";
                    if (searchBuffer.empty()) {
                        searchErrorMsg = "Please enter a Player ID";
                    }
                    else {
                        try {
                            int idToSearch = stoi(searchBuffer);
                            if (idToSearch < 0)
                                throw std::invalid_argument("Negative");
                            int foundIndex = hashTable.search(idToSearch);
                            if (foundIndex == -1)
                                searchErrorMsg = "No player found with this ID";
                            else {
                                foundPlayerID = idToSearch;
                                foundPlayerName = hashTable.getPlayerNameByID(idToSearch);
                                playerFound = true;
                            }
                        }
                        catch (...) {
                            searchErrorMsg = "Invalid input: only positive numbers allowed";
                        }
                    }
                    searchBuffer.clear();
                    searchText.setString("");
                }
            }
        } // End event loop

        // --- Drawing logic ---
        window.clear();
        window.draw(friendsTab);
        window.draw(requestsTab);
        window.draw(searchTabText);

        if (friendsTabActive) {
            string friendsList = "Friends:\n";
            int y = 125; // Starting vertical position for friends
            if (friendList.getHead() == nullptr) {
                friendsList += "No friends";
            }
            else {
                for (FriendNode* node = friendList.getHead(); node; node = node->next) {
                    friendsList += "- " + hashTable.getPlayerNameByID(node->friendID) + " (ID: " + to_string(node->friendID) + ")\n\n";

                    // Draw Cross sprite beside the friend
                    crossSprite.setPosition(400.f, float(y));
                    window.draw(crossSprite);

                    y += 48; // Adjust row spacing
                }
            }
            messageText.setString(friendsList);
            window.draw(messageText);
        }
        else if (requestsTabActive) {
            string reqList = "Requests:\n";
            int y = 125; // Starting vertical position for requests
            for (FriendRequestList::RequestNode* node = friendRequestList.getHead(); node; node = node->next) {
                string senderName = hashTable.getPlayerNameByID(node->senderID);
                reqList += "- " + senderName + " (ID: " + to_string(node->senderID) + ")\n\n";

                // Draw Accept and Reject sprites beside the request
                acceptSprite.setPosition(400.f, float(y));
                rejectSprite.setPosition(450.f, float(y));
                window.draw(acceptSprite);
                window.draw(rejectSprite);

                y += 48; // Adjust row spacing
            }
            messageText.setString(reqList);
            window.draw(messageText);
        }
        else if (searchTabActive) {
            window.draw(searchLabel);
            window.draw(searchBox);
            window.draw(searchText);
            if (!searchErrorMsg.empty()) {
                Text err;
                err.setFont(font);
                err.setString(searchErrorMsg);
                err.setCharacterSize(18);
                err.setFillColor(Color::Red);
                err.setPosition(50, 100);
                window.draw(err);
            }
            if (playerFound) {
                Text foundInfo;
                foundInfo.setFont(font);
                foundInfo.setString("Found: " + foundPlayerName + " (ID: " + to_string(foundPlayerID) + ")");
                foundInfo.setCharacterSize(18);
                foundInfo.setFillColor(Color::White);
                foundInfo.setPosition(50, 140);
                window.draw(foundInfo);

                RectangleShape addButton(Vector2f(170.f, 30.f));
                FloatRect addButtonRect(50.f, 180.f, 170.f, 30.f);
                bool addButtonHovered = addButtonRect.contains(mousePos);
                addButton.setFillColor(addButtonHovered ? hoverColor : Color::Green);
                addButton.setPosition(50.f, 180.f);
                window.draw(addButton);

                Text addButtonText;
                addButtonText.setFont(font);
                addButtonText.setString("Add Friend");
                addButtonText.setCharacterSize(18);
                addButtonText.setFillColor(Color::Black);
                addButtonText.setPosition(55.f, 185.f);
                window.draw(addButtonText);
            }
        }
        window.display();
    }
}