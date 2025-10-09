#pragma once
#include <vector>
#include "TMJTypes.h"
#include "MapLoader.h"

class CollisionSystem {
public:
    static std::vector<PositionedCollision> GenerateCollisions(const TMJMap& map);
    static bool CheckPlayerCollision(const Rectangle& playerHitbox, const std::vector<PositionedCollision>& collisions);

private:
    static Vector2 CalculateCollisionPosition(int x, int y, const TileSet* tileset, int localId, const TMJMap& map);
    static bool CheckCollisionWithShape(const Rectangle& rect, const PositionedCollision& collision);
};
