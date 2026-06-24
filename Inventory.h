#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "AVLTree.h"

class Inventory {
public:
    Inventory();
    ~Inventory();
    void loadPredefinedThemes();
    void displayAllThemes(sf::RenderWindow& window, sf::Font& font);
    bool savePlayerTheme(int playerID, int themeID);
    bool loadPlayerTheme(int playerID, int& themeID);
	Theme* getThemeByID(int id);
    Theme* getThemeByName(const std::string& name);

private:
    AVLTree themeTree;
    static void displayTheme(const Theme& theme);
};
