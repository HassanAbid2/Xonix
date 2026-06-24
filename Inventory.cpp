// Inventory.cpp

#include "Inventory.h"
#include <fstream>
#include <sstream>

// Constructor
Inventory::Inventory() {
    loadPredefinedThemes();
}

// Destructor
Inventory::~Inventory() {}

// Load predefined themes into AVL Tree
void Inventory::loadPredefinedThemes() {
    std::ifstream infile("Themes.csv");
    if (!infile.is_open()) {
        std::cerr << "Error opening Themes.csv" << std::endl;
        return;
    }

    std::string line;
    // Skip header line if present
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string idStr, name, description, preview, backgroundImage;

        // Read all five columns (including new backgroundImage)
        if (std::getline(ss, idStr, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, description, ',') &&
            std::getline(ss, preview, ',') &&
            std::getline(ss, backgroundImage)) {

            try {
                int id = std::stoi(idStr);
                Theme theme(id, name, description, preview, backgroundImage);
                themeTree.insert(theme);
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid theme ID: " << idStr << std::endl;
            }
        }
    }

    infile.close();
}

// Define the DisplayContext structure
struct DisplayContext {
    sf::RenderWindow* window;
    sf::Font* font;
    float xPos;
    float yPos;
};

// Static context pointer
static DisplayContext* g_displayContext = nullptr;

// Static callback function
static void DisplayThemeCallback(const Theme& theme) {
    if (g_displayContext) {
        // Display theme details in the console
        std::cout << "ID: " << theme.themeID << "\n"
            << "Name: " << theme.name << "\n"
            << "Description: " << theme.description << "\n"
            << "Preview: " << theme.preview << "\n"
            << "Background: " << theme.backgroundImage << "\n\n";

        // Create and configure the SFML text
        sf::Text themeText(theme.name, *(g_displayContext->font), 20);
        themeText.setFillColor(sf::Color::White);
        themeText.setPosition(g_displayContext->xPos, g_displayContext->yPos += 30.f); // Increment Y position
        g_displayContext->window->draw(themeText);
    }
}

// Display all themes on Inventory screen
void Inventory::displayAllThemes(sf::RenderWindow& window, sf::Font& font) {
    float yPos = 150.f; // Start below the title
    float xPos = 50.f;

    DisplayContext context = { &window, &font, xPos, yPos };
    g_displayContext = &context;

    themeTree.inOrderTraversal(DisplayThemeCallback);

    g_displayContext = nullptr;
}

//getThemeByID
Theme* Inventory::getThemeByID(int id) {
    return themeTree.searchByID(id);
}
