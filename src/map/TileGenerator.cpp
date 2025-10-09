#include "map/TileGenerator.hpp"

#include "map/MapLoader.hpp"

namespace TileGenerator {

namespace {
Tile CreateTile(const TileSet* tileset, int localId, int x, int y, const TMJMap& map) {
    Tile tile{};
    tile.tileset = tileset;
    tile.localId = localId;
    tile.destination = {static_cast<float>(x * map.tileWidth), static_cast<float>(y * map.tileHeight)};
    tile.isImageCollection = !tileset->isAtlas;

    if (tileset->isAtlas) {
        int columns = (tileset->columns > 0 ? tileset->columns : 1);
        int tx = (localId % columns) * tileset->tileWidth;
        int ty = (localId / columns) * tileset->tileHeight;

        tile.source = {
            static_cast<float>(tx),
            static_cast<float>(ty),
            static_cast<float>(tileset->tileWidth),
            static_cast<float>(tileset->tileHeight)
        };

        tile.destination.x += tileset->tileOffsetX;
        tile.destination.y += tileset->tileOffsetY;
        tile.sortingY = tile.destination.y + tileset->tileHeight;
        tile.drawOffsetY = 0.0f;
    } else {
        auto it = tileset->tileImages.find(localId);
        if (it == tileset->tileImages.end() || it->second == nullptr || it->second->id == 0) {
            tile.tileset = nullptr;
            return tile;
        }

        Texture2D* texture = it->second;
        tile.source = {0, 0, static_cast<float>(texture->width), static_cast<float>(texture->height)};

        float offsetY = static_cast<float>(map.tileHeight) - static_cast<float>(texture->height);
        tile.destination.y += offsetY;

        tile.destination.x += tileset->tileOffsetX;
        tile.destination.y += tileset->tileOffsetY;
        tile.sortingY = tile.destination.y + static_cast<float>(texture->height);
        tile.drawOffsetY = offsetY + static_cast<float>(tileset->tileOffsetY);
    }

    return tile;
}
}  // namespace

std::vector<Tile> GenerateTiles(const std::vector<TileLayer>& layers, const TMJMap& map) {
    std::vector<Tile> tiles;
    tiles.reserve(layers.size() * 128);

    for (const auto& layer : layers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int tileId = layer.data[y * layer.width + x];
                if (tileId == 0) {
                    continue;
                }

                const TileSet* tileset = MapLoader::FindTilesetForGID(map, tileId);
                if (!tileset) {
                    continue;
                }

                int localId = tileId - tileset->firstGid;
                Tile tile = CreateTile(tileset, localId, x, y, map);

                if (tile.tileset != nullptr) {
                    tiles.push_back(tile);
                }
            }
        }
    }

    return tiles;
}

}  // namespace TileGenerator

