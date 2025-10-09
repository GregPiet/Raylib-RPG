#pragma once

#include <vector>

#include "GameTypes.hpp"

namespace CollisionSystem {
std::vector<PositionedCollision> GenerateCollisions(const TMJMap& map);
bool CheckPlayerCollision(const Rectangle& playerHitbox, const std::vector<PositionedCollision>& collisions);
}

