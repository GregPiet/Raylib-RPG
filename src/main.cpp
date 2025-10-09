#pragma once
#include "./include/GameTypes.hpp"

#include <memory>
#include <vector>
#include "./map/MapLoader.cpp"
#include "./map/TileGenerator.cpp"
#include "./map/CollisionSystem.cpp"
#include "./render/RenderSystem.cpp"
#include "./entities/Player.cpp"
#include "./core/ResourceManager.cpp"
#include "./core/FileUtils.cpp"
#include "core/Game.hpp"


#include <raylib.h>
#include <raymath.h>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include "./json.hpp"

//==============================================================================
// MAIN ENTRY POINT
//==============================================================================
int main() {
    // Create and run game
    Game game;
    
    // IMPORTANT: Set the actual path to your TMJ file here
    const std::string mapPath = "./assets/maps/map.tmj";
    
    game.Run(mapPath);
    
    return 0;
}