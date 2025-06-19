#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include "Constants.h"
#include "Collision.h"
#include "Enemy.h"
#include "Player.h"
#include "Minimap.h"
#include "MapLoader.hpp"

int main() {
    // --- Window & view setup ---
    sf::RenderWindow window({WORLD_W / 2, WORLD_H / 2}, "Patrol + Chase Floorplan Modular");
    window.setFramerateLimit(60);
    sf::View view({0, 0, WORLD_W / 2.f, WORLD_H / 2.f});

    // --- Load map and extract walls ---
    MapLoader map("mainma.json", "ChatGPT Image Jun 17, 2025, 10_42_57 PM(1).png", 36, 36);
    if (!map.load()) return -1;
    const auto& collisionRects = map.getCollisionRects();

    // --- Create wall rectangles for collision detection (not drawing)
    std::vector<sf::RectangleShape> walls;
    for (const auto& rect : collisionRects) {
        sf::RectangleShape wall;
        wall.setPosition(rect.left, rect.top);
        wall.setSize({rect.width, rect.height});
        walls.push_back(wall);  // used for logic only, not rendering
    }

    // --- Build collision grid ---
    int cols = WORLD_W / TILE_SIZE, rows = WORLD_H / TILE_SIZE;
    std::vector<bool> grid(cols * rows, false);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            sf::FloatRect cell(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            for (const auto& w : walls) {
                if (cell.intersects(w.getGlobalBounds())) {
                    grid[y * cols + x] = true;
                    break;
                }
            }
        }
    }

    // --- Patrol circuit setup ---
    std::vector<Vec2f> circuit = {
        {950, 350}, {900, 160}, {800, 160}, {700, 160}, {600, 160},
        {600, 300}, {600, 400}, {700, 400}, {800, 400}, {900, 400}, {900, 300}
    };
    Enemy enemy(walls, grid, circuit);

    // --- Player setup ---
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("sprite.png")) return -1;
    Player player(playerTexture, walls);
    player.setPosition(WORLD_W / 2.f, WORLD_H / 2.f);  // spawn at map center

    // --- Zone & score setup ---
    sf::RectangleShape zone({200, 150});
    zone.setPosition(WORLD_W - 200, WORLD_H - 150);
    zone.setFillColor({0, 255, 0, 100});

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;
    sf::Text scoreText("Score: 0", font, 24);
    scoreText.setFillColor(sf::Color::White);
    int score = 0;

    // --- Minimap & clock ---
    Minimap minimap(WORLD_W, WORLD_H, font);
    sf::Clock clock;

    // --- Game loop ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            minimap.handleEvent(event, window);
        }

        float dt = clock.restart().asSeconds();
        player.handleInput(dt);
        player.update();

        bool inZone = zone.getGlobalBounds().intersects(player.getGlobalBounds());
        enemy.setChasing(inZone);
        enemy.update(player.getPosition(), dt);

        if (player.getGlobalBounds().intersects(enemy.shape().getGlobalBounds())) {
            ++score;
            scoreText.setString("Score: " + std::to_string(score));
            enemy.setChasing(false);
        }

        // --- Drawing ---
        window.clear();

        view.setCenter(player.getPosition()); // Follow player
        window.setView(view);

        map.draw(window);           // Draw tilemap
        window.draw(zone);          // Draw goal zone
        window.draw(enemy.shape()); // Draw enemy
        player.draw(window);        // Draw player

        // Draw score on screen
        sf::Vector2f topLeft = view.getCenter() - view.getSize() / 2.f;
        scoreText.setPosition(topLeft + Vec2f(10, 10));
        window.draw(scoreText);

        // Draw minimap (default view)
        window.setView(window.getDefaultView());
        minimap.draw(window, walls, player, enemy);

        window.display();
    }

    return 0;
}
