#ifndef MAPLOADER_HPP
#define MAPLOADER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

class MapLoader {
public:
    MapLoader(const std::string& mapPath, const std::string& tilesetPath, int tileWidth, int tileHeight);

    bool load();
    void draw(sf::RenderWindow& window);
    const std::vector<sf::FloatRect>& getCollisionRects() const;

private:
    std::string mapFile, tilesetFile;
    int tileW, tileH;

    sf::Texture tilesetTexture;
    sf::VertexArray vertices;
    std::vector<sf::FloatRect> collisionRects;
};

#endif
