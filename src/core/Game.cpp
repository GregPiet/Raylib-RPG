#include "core/Game.hpp"
#include "raylib.h"

void Game::Init() {
    
    InitWindow(1280, 720, "RPG Game");
    SetTargetFPS(60);

    // Chargement de la carte et du joueur
    map = LoadMap("assets/maps/map_background.png",
                  "assets/maps/map_foreground.png",
                  "assets/maps/map.tmx");

    // Ajuste dynamiquement la taille de la fenêtre en fonction des dimensions de la carte.
    if (map.background.id != 0) {
        SetWindowSize(map.background.width, map.background.height);
    }
    InitPlayer(player, 100, 100);
}


void Game::Update() {
    UpdatePlayer(player, map.collisions);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawMap(map);
    DrawPlayer(player);
    DrawTexture(map.foreground, 0, 0, WHITE);

    EndDrawing();
}

void Game::Unload() {
    UnloadMap(map);
    CloseWindow();
}

bool Game::ShouldClose() const {
    return WindowShouldClose();
}
