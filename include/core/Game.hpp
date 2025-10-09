#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GameTypes.hpp"

class Player;

class Game {
public:
    Game();
    ~Game();

    void Run(const std::string& mapPath);

private:
    void Initialize(const std::string& mapPath);
    void Update();
    void Render();
    void DrawDebugText();
    void Cleanup();

    static constexpr int WINDOW_WIDTH = 960;
    static constexpr int WINDOW_HEIGHT = 640;
    static constexpr int TARGET_FPS = 60;
    static constexpr const char* WINDOW_TITLE = "Raylib - TMJ Game Engine";

    TMJMap m_map;
    std::unique_ptr<Player> m_player;
    std::vector<Tile> m_backgroundTiles;
    std::vector<Tile> m_objectTiles;
    std::vector<PositionedCollision> m_collisions;
    bool m_debugMode = true;
};

