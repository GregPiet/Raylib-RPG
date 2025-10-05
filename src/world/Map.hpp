#pragma once
#include "Types.hpp"
#include <string>

Map LoadMap(const std::string& bgPath, const std::string& fgPath, const std::string& tmxPath);
void DrawMap(const Map& map);
void UnloadMap(Map& map);
