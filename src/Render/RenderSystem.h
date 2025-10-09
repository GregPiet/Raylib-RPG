#pragma once
#include <vector>
#include <raylib.h>

#include "../Map/TMJTypes.h"
#include "../Player/Player.h"

//==============================================================================
// RENDER SYSTEM
//==============================================================================
class RenderSystem {
public:
    static void DrawTile(const Tile& tile);
    static void DrawTiles(const std::vector<Tile>& tiles);
    static void DrawTilesWithPlayer(std::vector<Tile>& tiles, const Player& player);
    static void DrawCollisionDebug(const std::vector<PositionedCollision>& collisions, Vector2 offset = {0, 0});

private:
    static void DrawCollisionShape(const PositionedCollision& collision, Vector2 offset);
};
