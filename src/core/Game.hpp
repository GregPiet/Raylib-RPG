#pragma once
#include "world/Map.hpp"
#include "entities/Player.hpp"

class Game {
public:
    void Init();
    void Update();
    void Draw();
    void Unload();
    bool ShouldClose() const;

private:
    Map map;
    Player player;
};
