#pragma once

#include <vector>

#include "GameTypes.hpp"

namespace TileGenerator {
std::vector<Tile> GenerateTiles(const std::vector<TileLayer>& layers, const TMJMap& map);
}

