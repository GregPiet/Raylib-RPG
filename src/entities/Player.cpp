#include "entities/Player.hpp"
#include "raymath.h"
#include <iostream>

static std::string Key(Action action, Direction dir) {
    return std::to_string((int)action) + "_" + std::to_string((int)dir);
}

void InitPlayer(Player& player, float startX, float startY) {
    player.position = { startX, startY };
    player.speed = 100.0f;
    player.direction = Direction::DOWN;
    player.action = Action::IDLE;
    player.rect = { startX + 20 + 38, startY + 34 + 23, 22, 8 };
    player.hitboxCollision = {startX +20+38, startY+34+23, 22, 8};

    
    const int FRAME_COUNT = 8;
    const float FRAME_TIME = 0.1f;

    std::map<Action, std::string> folders = {
        {Action::IDLE, "IDLE"},
        {Action::RUN, "RUN"},
        {Action::ATTACK1, "ATTACK 1"},
        {Action::ATTACK2, "ATTACK 2"}
    };

    std::map<Direction, std::string> dirNames = {
        {Direction::DOWN, "down"},
        {Direction::LEFT, "left"},
        {Direction::RIGHT, "right"},
        {Direction::UP, "up"}
    };

    for (auto& [act, folder] : folders) {
        for (auto& [dir, dname] : dirNames) {
            std::string key = Key(act, dir);
            std::string path;

            switch (act) {
                case Action::IDLE: path = "assets/player/IDLE/idle_" + dname + ".png"; break;
                case Action::RUN: path = "assets/player/RUN/run_" + dname + ".png"; break;
                case Action::ATTACK1: path = "assets/player/ATTACK 1/attack1_" + dname + ".png"; break;
                case Action::ATTACK2: path = "assets/player/ATTACK 2/attack2_" + dname + ".png"; break;
            }

            Animation anim;
            anim.texture = LoadTexture(path.c_str());
            anim.frameCount = FRAME_COUNT;
            anim.frameWidth = anim.texture.width / FRAME_COUNT;
            anim.frameHeight = anim.texture.height;
            anim.currentFrame = 0;
            anim.frameTime = FRAME_TIME;
            anim.timer = 0.0f;

            player.animations[key] = anim;
        }
    }
}

void UpdatePlayer(Player& player, const std::vector<CollisionBox>& collisions) {
    Vector2 move = {0, 0};
    Action newAction = Action::IDLE;

    if (IsKeyDown(KEY_RIGHT)) { move.x += 1; player.direction = Direction::RIGHT; newAction = Action::RUN; }
    if (IsKeyDown(KEY_LEFT))  { move.x -= 1; player.direction = Direction::LEFT;  newAction = Action::RUN; }
    if (IsKeyDown(KEY_UP))    { move.y -= 1; player.direction = Direction::UP;    newAction = Action::RUN; }
    if (IsKeyDown(KEY_DOWN))  { move.y += 1; player.direction = Direction::DOWN;  newAction = Action::RUN; }

    if (IsKeyPressed(KEY_SPACE)) newAction = Action::ATTACK1;

    player.action = newAction;
    std::cerr<<"Postion : "<< player.position.x<<","<<player.position.y<<"\n";
    std::cerr<<"Hitbox : "<< player.hitboxCollision.x<<","<<player.hitboxCollision.y<<"\n";

    

    if (Vector2Length(move) > 0) move = Vector2Normalize(move);

    Vector2 oldPos = player.position;
    player.position.x += move.x * player.speed * GetFrameTime();
    player.position.y += move.y * player.speed * GetFrameTime();
    player.rect.x = player.position.x;
    player.rect.y = player.position.y;

    player.hitboxCollision.x = player.position.x +38;
    player.hitboxCollision.y = player.position.y+68 ;

    // Gestion collisions
    for (const auto& col : collisions) {

        if (CheckCollisionRecs(player.hitboxCollision, col.rect)) {
            player.position = oldPos;
            player.rect.x = oldPos.x;
            player.rect.y = oldPos.y;
            break;
        }
    }

    // Animation
    std::string key = Key(player.action, player.direction);
    Animation& anim = player.animations[key];
    anim.timer += GetFrameTime();
    if (anim.timer >= anim.frameTime) {
        anim.timer = 0;
        anim.currentFrame++;
        if (anim.currentFrame >= anim.frameCount) anim.currentFrame = 0;
    }
}

void DrawPlayer(const Player& player) {
    std::string key = std::to_string((int)player.action) + "_" + std::to_string((int)player.direction);
    const Animation& anim = player.animations.at(key);

    Rectangle src = {
        (float)(anim.currentFrame * anim.frameWidth),
        0,
        (float)anim.frameWidth,
        (float)anim.frameHeight
    };

    Rectangle dest = {
        player.position.x,
        player.position.y,
        (float)anim.frameWidth * 2,  // agrandit pour être visible
        (float)anim.frameHeight * 2
    };

    Vector2 origin = { (float)anim.frameWidth / 2, (float)anim.frameHeight / 2 };
    DrawTexturePro(anim.texture, src, dest, origin, 0.0f, WHITE);

    DrawRectangleLinesEx(player.hitboxCollision, 1, RED); // afficher la hitboxCollision
}


void UnloadPlayer(Player& player) {
    for (auto& [_, anim] : player.animations)
        UnloadTexture(anim.texture);
}
