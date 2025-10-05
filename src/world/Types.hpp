#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Structure de collision
struct CollisionBox {
    Rectangle rect;
    bool solid;
};

// Structure de la carte
struct Map {
    Texture2D background;
    Texture2D foreground;
    std::vector<CollisionBox> collisions;
};
