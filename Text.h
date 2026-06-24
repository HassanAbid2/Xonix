#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

//what extern does is it tells the compiler
//"this variable is declared here, but defined elsewhere"

extern Font font;
extern Text levelText;
extern Text levelLeft;
extern Text levelTotal;
extern Text levelCompleteText;
extern Text levelCompleteText;
extern Text scoreText;
extern Text errorShow;
bool loadFontAndSetupText();