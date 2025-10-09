#pragma once

#include <vector>

#include "GameTypes.hpp"

class Player;

namespace RenderSystem {
void DrawTile(const Tile& tile);
void DrawTiles(const std::vector<Tile>& tiles);
void DrawTilesWithPlayer(const std::vector<Tile>& tiles, const Player& player);
void DrawCollisionDebug(const std::vector<PositionedCollision>& collisions, Vector2 offset = {0, 0});
}

