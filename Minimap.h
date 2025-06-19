#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Enemy.h"

class Minimap {
public:
    // Constructor signature must match exactly what you declare here:
    Minimap(float worldWidth, float worldHeight, const sf::Font& font);

    // Handle mouse clicks in pixel-space
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);

    // Draw the button & popup (enemy passed non-const so we can query shape())
    void draw(sf::RenderWindow& window,
              const std::vector<sf::RectangleShape>& walls,
              const Player& player,
              Enemy& enemy);

private:
    bool show = false;
    sf::RectangleShape button;
    sf::RectangleShape popup;
    sf::RectangleShape closeBtn;
    sf::Text          closeX;
    float scaleX, scaleY;
};

#endif // MINIMAP_H

