#include "core/Game.hpp"

#include <string>

int main() {
    Game game;
    const std::string mapPath = "./assets/maps/map.tmj";
    game.Run(mapPath);
    return 0;
}

