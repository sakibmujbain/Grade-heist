#include "MapLoader.hpp"
#include "tileson/include/tileson.hpp"
#include <iostream>

MapLoader::MapLoader(const std::string& mapPath, const std::string& tilesetPath, int tileWidth, int tileHeight)
    : mapFile(mapPath), tilesetFile(tilesetPath), tileW(tileWidth), tileH(tileHeight) {}

bool MapLoader::load() {
    tson::Tileson parser;
    std::unique_ptr<tson::Map> map = parser.parse(fs::path(mapFile));

    if (map->getStatus() != tson::ParseStatus::OK) {
        std::cerr << "Tileson error: Failed to parse map" << std::endl;
        return false;
    }

    if (!tilesetTexture.loadFromFile(tilesetFile)) {
        std::cerr << "Failed to load tileset: " << tilesetFile << std::endl;
        return false;
    }

    auto* layer = map->getLayer("Tile Layer 1"); // Adjust if your layer is named differently
    if (!layer || layer->getType() != tson::LayerType::TileLayer) {
        std::cerr << "Invalid or missing tile layer" << std::endl;
        return false;
    }

    const int columns = map->getTilesets()[0].getColumns();
    const int firstGid = map->getTilesets()[0].getFirstgid();

    auto& tiles = layer->getTileObjects();
    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(tiles.size() * 4);

    int idx = 0;
    for (auto& tileObj : tiles) {
        tson::TileObject& tile = tileObj.second;  // remove const

        if (tile.getTile() == nullptr)
            continue;

        int gid = tile.getTile()->getId() + firstGid;
        int x = tile.getPosition().x;
        int y = tile.getPosition().y;

        int tu = (gid - firstGid) % columns;
        int tv = (gid - firstGid) / columns;

        sf::Vertex* quad = &vertices[idx * 4];

        quad[0].position = sf::Vector2f(x, y);
        quad[1].position = sf::Vector2f(x + tileW, y);
        quad[2].position = sf::Vector2f(x + tileW, y + tileH);
        quad[3].position = sf::Vector2f(x, y + tileH);

        quad[0].texCoords = sf::Vector2f(tu * tileW, tv * tileH);
        quad[1].texCoords = sf::Vector2f((tu + 1) * tileW, tv * tileH);
        quad[2].texCoords = sf::Vector2f((tu + 1) * tileW, (tv + 1) * tileH);
        quad[3].texCoords = sf::Vector2f(tu * tileW, (tv + 1) * tileH);

        // Correct way to access the "c" boolean property
        if (tile.getTile()->getProperties().getValue<bool>("c") == true) {

            collisionRects.emplace_back(x, y, tileW, tileH);
        }

        ++idx;
    }

    return true;
}

void MapLoader::draw(sf::RenderWindow& window) {
    sf::RenderStates states;
    states.texture = &tilesetTexture;
    window.draw(vertices, states);
}

const std::vector<sf::FloatRect>& MapLoader::getCollisionRects() const {
    return collisionRects;
}
