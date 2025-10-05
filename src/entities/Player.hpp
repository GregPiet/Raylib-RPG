#pragma once
#include "raylib.h"
#include "world/Types.hpp"
#include <vector>

struct Player {
    Rectangle rect;
    float speed;
};

void InitPlayer(Player& player, float startX, float startY);
void UpdatePlayer(Player& player, const std::vector<CollisionBox>& collisions);
void DrawPlayer(const Player& player);
