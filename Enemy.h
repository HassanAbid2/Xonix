#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Global.h"

class Enemy
{
public:
    int x, y;
    int dx, dy;
    float speed;
    float baseSpeed;
    sf::Sprite sprite; // Sprite for the enemy
    sf::Texture texture; // Texture for the enemy

    Enemy()
    {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
        baseSpeed = 4.0f;
        speed = baseSpeed;

        // Load a texture and set it to the sprite
        if (!texture.loadFromFile("images/enemy.png"))
        {
            std::cerr << "Failed to load enemy texture!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
    }

    // Existing move method that only handles collisions with Player1 captured walls (value 1)
    void move()
    {
        x += static_cast<int>(dx * speed / baseSpeed);
        if (grid[y / ts][x / ts] == 1) {
            dx = -dx;
            x += static_cast<int>(dx * speed / baseSpeed);
        }
        y += static_cast<int>(dy * speed / baseSpeed);
        if (grid[y / ts][x / ts] == 1) {
            dy = -dy;
            y += static_cast<int>(dy * speed / baseSpeed);
        }
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
    }

    // New move2 method which handles collisions with both Player1 and Player2 walls (values 1 and 3)
    void move2()
    {
        // Move horizontally
        x += static_cast<int>(dx * speed / baseSpeed);
        // Check if the new cell is a wall (value 1 or 3) and bounce if so
        if (grid[y / ts][x / ts] == 1 || grid[y / ts][x / ts] == 3) {
            dx = -dx;
            x += static_cast<int>(dx * speed / baseSpeed);
        }

        // Move vertically
        y += static_cast<int>(dy * speed / baseSpeed);
        // Check for collision with walls (value 1 or 3) and bounce vertically if needed
        if (grid[y / ts][x / ts] == 1 || grid[y / ts][x / ts] == 3) {
            dy = -dy;
            y += static_cast<int>(dy * speed / baseSpeed);
        }

        // Update sprite position
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
    }

    void setSpeed(float newSpeed)
    {
        speed = newSpeed;
    }

    void resetSpeed()
    {
        speed = baseSpeed;
    }

    // Getters for x, y, and sprite
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    const sf::Sprite& getSprite() const { return sprite; }
};