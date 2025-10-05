#include "Map.hpp"
#include "Collision.hpp"
#include <iostream>

Map LoadMap(const std::string& bgPath, const std::string& fgPath, const std::string& tmxPath) {
    Map map;

    Image bgImage = LoadImage(bgPath.c_str());
    if (bgImage.data == nullptr) {
        std::cerr << "❌ Erreur : impossible de charger " << bgPath << "\n";
        return map;
    }

    map.background = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage);

    map.foreground = LoadTexture(fgPath.c_str());
    map.collisions = LoadCollisionsFromTmx(tmxPath);

    return map;
}

void DrawMap(const Map& map) {
    DrawTexture(map.background, 0, 0, WHITE);
}

void UnloadMap(Map& map) {
    UnloadTexture(map.background);
    UnloadTexture(map.foreground);
}
