#pragma once
#include <string>

// Theme structure
struct Theme {
    int themeID;
    std::string name;
    std::string description;
    std::string preview; // Simulated color code or preview
    std::string backgroundImage; // <-- Add this line

    Theme(int id, const std::string& n, const std::string& desc, const std::string& prev, const std::string& bg)
        : themeID(id), name(n), description(desc), preview(prev), backgroundImage(bg) {
    }
};
// AVL Tree Node
struct AVLNode {
    Theme data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Theme& theme)
        : data(theme), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree Class
class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    void insert(const Theme& theme);
    Theme* searchByID(int id);
    Theme* searchByName(const std::string& name);
    void inOrderTraversal(void (*callback)(const Theme&));

private:
    AVLNode* root;

    // Helper functions
    AVLNode* insertNode(AVLNode* node, const Theme& theme);
    int getHeight(AVLNode* node);
    int getBalanceFactor(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    void destroyTree(AVLNode* node);
    Theme* searchByIDHelper(AVLNode* node, int id);
    Theme* searchByNameHelper(AVLNode* node, const std::string& name);
    void inOrderHelper(AVLNode* node, void (*callback)(const Theme&));
};
