#pragma once

#include <string>

#include "GameTypes.hpp"

class MapLoader {
public:
    static TMJMap LoadMap(const std::string& tmjPath);
    static const TileSet* FindTilesetForGID(const TMJMap& map, int gid);
};

