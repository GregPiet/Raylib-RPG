#pragma once
#include <vector>
#include "TMJTypes.h"
#include "MapLoader.h"

class TileGenerator {
public:
    static std::vector<Tile> GenerateTiles(const std::vector<TileLayer>& layers, const TMJMap& map);

private:
    static Tile CreateTile(const TileSet* tileset, int localId, int x, int y, const TMJMap& map);
};
