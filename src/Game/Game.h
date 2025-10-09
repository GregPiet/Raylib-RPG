#pragma once
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <raylib.h>

#include "../Map/TMJTypes.h"
#include "../Map/MapLoader.h"
#include "../Map/TileGenerator.h"
#include "../Map/CollisionSystem.h"
#include "../Render/RenderSystem.h"
#include "../Player/Player.h"
#include "../Core/ResourceManager.h"

// Classe principale du jeu (boucle, initialisation, rendu)
class Game {
private:
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

    void Initialize(const std::string& mapPath);
    void Update();
    void Render();
    void DrawDebugText();

public:
    Game() = default;
    ~Game() = default;

    void Run(const std::string& mapPath);
    void Cleanup();
};
