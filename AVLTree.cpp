#include "AVLTree.h"
#include <iostream>

// Constructor
AVLTree::AVLTree() : root(nullptr) {}

// Destructor
AVLTree::~AVLTree() {
    destroyTree(root);
}

void AVLTree::destroyTree(AVLNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

int AVLTree::getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));

    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

    return y;
}

AVLNode* AVLTree::insertNode(AVLNode* node, const Theme& theme) {
    if (!node)
        return new AVLNode(theme);

    if (theme.themeID < node->data.themeID)
        node->left = insertNode(node->left, theme);
    else if (theme.themeID > node->data.themeID)
        node->right = insertNode(node->right, theme);
    else
        return node; // Duplicate IDs not allowed

    // Update height
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

    // Get balance factor
    int balance = getBalanceFactor(node);

    // Balance the tree
    // Left Left Case
    if (balance > 1 && theme.themeID < node->left->data.themeID)
        return rotateRight(node);

    // Right Right Case
    if (balance < -1 && theme.themeID > node->right->data.themeID)
        return rotateLeft(node);

    // Left Right Case
    if (balance > 1 && theme.themeID > node->left->data.themeID) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && theme.themeID < node->right->data.themeID) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

void AVLTree::insert(const Theme& theme) {
    root = insertNode(root, theme);
}

Theme* AVLTree::searchByIDHelper(AVLNode* node, int id) {
    if (!node)
        return nullptr;
    if (id == node->data.themeID)
        return &(node->data);
    else if (id < node->data.themeID)
        return searchByIDHelper(node->left, id);
    else
        return searchByIDHelper(node->right, id);
}

Theme* AVLTree::searchByID(int id) {
    return searchByIDHelper(root, id);
}

Theme* AVLTree::searchByNameHelper(AVLNode* node, const std::string& name) {
    if (!node)
        return nullptr;
    if (name == node->data.name)
        return &(node->data);
    Theme* found = searchByNameHelper(node->left, name);
    if (found)
        return found;
    return searchByNameHelper(node->right, name);
}

Theme* AVLTree::searchByName(const std::string& name) {
    return searchByNameHelper(root, name);
}

void AVLTree::inOrderHelper(AVLNode* node, void (*callback)(const Theme&)) {
    if (node) {
        inOrderHelper(node->left, callback);
        callback(node->data);
        inOrderHelper(node->right, callback);
    }
}

void AVLTree::inOrderTraversal(void (*callback)(const Theme&)) {
    inOrderHelper(root, callback);
}
