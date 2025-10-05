#include "entities/Player.hpp"

void InitPlayer(Player& player, float startX, float startY) {
    player.rect = { startX, startY, 32, 32 };
    player.speed = 3.0f;
}

void UpdatePlayer(Player& player, const std::vector<CollisionBox>& collisions) {
    Vector2 oldPos = {player.rect.x, player.rect.y};

    if (IsKeyDown(KEY_RIGHT)) player.rect.x += player.speed;
    if (IsKeyDown(KEY_LEFT))  player.rect.x -= player.speed;
    if (IsKeyDown(KEY_DOWN))  player.rect.y += player.speed;
    if (IsKeyDown(KEY_UP))    player.rect.y -= player.speed;

    for (const auto &col : collisions) {
        if (CheckCollisionRecs(player.rect, col.rect)) {
            player.rect.x = oldPos.x;
            player.rect.y = oldPos.y;
            break;
        }
    }
}

void DrawPlayer(const Player& player) {
    DrawRectangleRec(player.rect, BLACK);
}
