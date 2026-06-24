#include <SFML/Graphics.hpp>
#include<iostream>
#include "Text.h"

using namespace sf;
using namespace std;        

Font font, font2;
Text levelText;
Text levelLeft;
Text levelTotal;
Text levelCompleteText;
Text scoreText;
Text errorShow;

bool loadFontAndSetupText()
{
    if (!font.loadFromFile("fonts/joystix.otf")) {
        cout << "Font loading failed!" << std::endl;
        return false;
    }
    if (!font2.loadFromFile("fonts/Saphira.otf")) {
        cout << "Font loading failed!" << std::endl;
        return false;
    }

    //level number
    levelText.setFont(font);
    levelText.setCharacterSize(30);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(920, 20);

    // % of coverage done
    levelLeft.setFont(font);
    levelLeft.setCharacterSize(20);
    levelLeft.setFillColor(sf::Color::White);
    levelLeft.setPosition(1760, 20);

    // % of coverage required
    levelTotal.setFont(font);
    levelTotal.setCharacterSize(20);
    levelTotal.setFillColor(sf::Color::White);
    levelTotal.setPosition(1800, 20);

    // level completed
    levelCompleteText.setFont(font);
    levelCompleteText.setCharacterSize(60);
    levelCompleteText.setFillColor(sf::Color::Red);
    levelCompleteText.setPosition(520, 500);

    // score text
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20, 20); // Adjusted position
    

    errorShow.setFont(font2);
    errorShow.setCharacterSize(24);
    errorShow.setFillColor(Color::Red);
    errorShow.setPosition(1920 / 2.0f + 140, 750);

    return true;
}
