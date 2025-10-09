#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <./json.hpp>

#include "../Core/ResourceManager.h"
#include "../Core/FileUtils.h"
#include "TMJTypes.h"

using json = nlohmann::json;

//==============================================================================
// MAP LOADER
//==============================================================================
class MapLoader {
private:
    static void ParseLayers(const json& layerNode, TMJMap& map, bool isBackground = false);
    static void ParseTilesets(const json& data, TMJMap& map, const std::string& baseDir);
    static void ParseTileCollisions(const json& tilesetJson, const TileSet& tileset, TMJMap& map);

public:
    static TMJMap LoadMap(const std::string& tmjPath);
    static const TileSet* FindTilesetForGID(const TMJMap& map, int gid);
};
