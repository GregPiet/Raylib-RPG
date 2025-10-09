// Game.hpp
#pragma once
#include "./include/GameTypes.hpp"

#include <memory>
#include <vector>
#include "./map/MapLoader.cpp"
#include "./map/TileGenerator.cpp"
#include "./map/CollisionSystem.cpp"
#include "./render/RenderSystem.cpp"
#include "./entities/Player.cpp"
#include "./core/ResourceManager.cpp"
#include "./core/FileUtils.cpp"
#include "core/Game.hpp"


#include <raylib.h>
#include <raymath.h>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include "./json.hpp"

using json = nlohmann::json;

ResourceManager* ResourceManager::s_instance = nullptr;




//==============================================================================
// GAME CLASS (Main Game Logic)
//==============================================================================
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
    
    void Initialize(const std::string& mapPath) {
        // Load map
        m_map = MapLoader::LoadMap(mapPath);
        
        // Initialize player
        m_player = std::make_unique<Player>(200.0f, 300.0f);
        
        // Generate tiles
        m_backgroundTiles = TileGenerator::GenerateTiles(m_map.backgroundLayers, m_map);
        m_objectTiles = TileGenerator::GenerateTiles(m_map.otherLayers, m_map);
        
        // Sort object tiles by Y for depth sorting
        std::sort(m_objectTiles.begin(), m_objectTiles.end(),
            [](const Tile& a, const Tile& b) {
                return a.sortingY < b.sortingY;
            }
        );
        
        // Generate collision data
        m_collisions = CollisionSystem::GenerateCollisions(m_map);
    }
    
    void Update() {
        // Toggle debug mode
        if (IsKeyPressed(KEY_F1)) {
            m_debugMode = !m_debugMode;
        }
        
        // Update player
        m_player->Update(m_collisions);
    }
    
    void Render() {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw background tiles
        RenderSystem::DrawTiles(m_backgroundTiles);
        
        // Draw objects and player with depth sorting
        RenderSystem::DrawTilesWithPlayer(m_objectTiles, *m_player);
        
        // Draw debug information
        if (m_debugMode) {
            RenderSystem::DrawCollisionDebug(m_collisions);
            m_player->DrawDebug();
            DrawDebugText();
        }
        
        EndDrawing();
    }
    
    void DrawDebugText() {
        DrawText("Debug Mode (F1 to toggle)", 10, 10, 16, DARKGRAY);
        DrawText("Rect=Red | Ellipse=Orange | Poly=Blue | Polyline=Purple", 10, 30, 16, DARKGRAY);
        DrawText("Use Arrow Keys to move, Space to attack", 10, 50, 16, DARKGRAY);
        DrawFPS(10, 70);
    }
    
public:
    Game() = default;
    ~Game() = default;
    
    void Run(const std::string& mapPath) {
        // Initialize window
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
        SetTargetFPS(TARGET_FPS);
        
        // Initialize game
        Initialize(mapPath);
        
        // Game loop
        while (!WindowShouldClose()) {
            Update();
            Render();
        }
        
        // Cleanup
        Cleanup();
    }
    
    void Cleanup() {
        ResourceManager::Cleanup();
        CloseWindow();
    }
};
