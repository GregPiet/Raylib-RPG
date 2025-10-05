#pragma once
#include "Types.hpp"
#include <string>
#include <vector>

std::vector<CollisionBox> LoadCollisionsFromTmx(const std::string& path);
