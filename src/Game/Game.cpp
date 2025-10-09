#include "Game.h"

//==============================================================================
// INITIALISATION
//==============================================================================
void Game::Initialize(const std::string& mapPath) {
    // Charger la carte
    m_map = MapLoader::LoadMap(mapPath);

    // Initialiser le joueur
    m_player = std::make_unique<Player>(200.0f, 300.0f);

    // Générer les tuiles
    m_backgroundTiles = TileGenerator::GenerateTiles(m_map.backgroundLayers, m_map);
    m_objectTiles = TileGenerator::GenerateTiles(m_map.otherLayers, m_map);

    // Trier les tuiles par profondeur (Y)
    std::sort(m_objectTiles.begin(), m_objectTiles.end(),
        [](const Tile& a, const Tile& b) {
            return a.sortingY < b.sortingY;
        }
    );

    // Générer les collisions
    m_collisions = CollisionSystem::GenerateCollisions(m_map);
}

//==============================================================================
// MISE À JOUR
//==============================================================================
void Game::Update() {
    // Basculer le mode debug
    if (IsKeyPressed(KEY_F1)) {
        m_debugMode = !m_debugMode;
    }

    // Mettre à jour le joueur
    m_player->Update(m_collisions);
}

//==============================================================================
// RENDU
//==============================================================================
void Game::Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Dessiner les tuiles d’arrière-plan
    RenderSystem::DrawTiles(m_backgroundTiles);

    // Dessiner les objets et le joueur avec tri Y
    RenderSystem::DrawTilesWithPlayer(m_objectTiles, *m_player);

    // Mode debug
    if (m_debugMode) {
        RenderSystem::DrawCollisionDebug(m_collisions);
        m_player->DrawDebug();
        DrawDebugText();
    }

    EndDrawing();
}

//==============================================================================
// TEXTE DEBUG
//==============================================================================
void Game::DrawDebugText() {
    DrawText("Debug Mode (F1 to toggle)", 10, 10, 16, DARKGRAY);
    DrawText("Rect=Red | Ellipse=Orange | Poly=Blue | Polyline=Purple", 10, 30, 16, DARKGRAY);
    DrawText("Use Arrow Keys to move, Space to attack", 10, 50, 16, DARKGRAY);
    DrawFPS(10, 70);
}

//==============================================================================
// LANCEMENT DU JEU
//==============================================================================
void Game::Run(const std::string& mapPath) {
    // Initialisation de la fenêtre
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    // Initialiser le jeu
    Initialize(mapPath);

    // Boucle principale
    while (!WindowShouldClose()) {
        Update();
        Render();
    }

    // Nettoyage
    Cleanup();
}

//==============================================================================
// LIBÉRATION DES RESSOURCES
//==============================================================================
void Game::Cleanup() {
    ResourceManager::Cleanup();
    CloseWindow();
}
