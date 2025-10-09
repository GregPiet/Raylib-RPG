#include "core/Game.hpp"

#include <algorithm>

#include <raylib.h>

#include "core/ResourceManager.hpp"
#include "entities/Player.hpp"
#include "map/CollisionSystem.hpp"
#include "map/MapLoader.hpp"
#include "map/TileGenerator.hpp"
#include "render/RenderSystem.hpp"

Game::Game() = default;
Game::~Game() = default;

void Game::Initialize(const std::string& mapPath) {
    m_map = MapLoader::LoadMap(mapPath);

    m_player = std::make_unique<Player>(200.0f, 300.0f);

    m_backgroundTiles = TileGenerator::GenerateTiles(m_map.backgroundLayers, m_map);
    m_objectTiles = TileGenerator::GenerateTiles(m_map.otherLayers, m_map);

    std::sort(m_objectTiles.begin(), m_objectTiles.end(),
              [](const Tile& a, const Tile& b) { return a.sortingY < b.sortingY; });

    m_collisions = CollisionSystem::GenerateCollisions(m_map);
}

void Game::Update() {
    if (IsKeyPressed(KEY_F1)) {
        m_debugMode = !m_debugMode;
    }

    m_player->Update(m_collisions);
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    RenderSystem::DrawTiles(m_backgroundTiles);
    RenderSystem::DrawTilesWithPlayer(m_objectTiles, *m_player);

    if (m_debugMode) {
        RenderSystem::DrawCollisionDebug(m_collisions);
        m_player->DrawDebug();
        DrawDebugText();
    }

    EndDrawing();
}

void Game::DrawDebugText() {
    DrawText("Debug Mode (F1 to toggle)", 10, 10, 16, DARKGRAY);
    DrawText("Rect=Red | Ellipse=Orange | Poly=Blue | Polyline=Purple", 10, 30, 16, DARKGRAY);
    DrawText("Use Arrow Keys to move, Space to attack", 10, 50, 16, DARKGRAY);
    DrawFPS(10, 70);
}

void Game::Cleanup() {
    ResourceManager::Cleanup();
    CloseWindow();
}

void Game::Run(const std::string& mapPath) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    Initialize(mapPath);

    while (!WindowShouldClose()) {
        Update();
        Render();
    }

    Cleanup();
}

