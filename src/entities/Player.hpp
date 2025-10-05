#pragma once
#include "raylib.h"
#include "world/Types.hpp"
#include <vector>
#include <map>
#include <string>

enum class Direction { DOWN, LEFT, RIGHT, UP };
enum class Action { IDLE, RUN, ATTACK1, ATTACK2 };

struct Animation {
    Texture2D texture;
    int frameWidth;
    int frameHeight;
    int frameCount;
    int currentFrame;
    float frameTime;
    float timer;
};

struct Player {
    Vector2 position;
    float speed;
    Direction direction;
    Action action;
    std::map<std::string, Animation> animations;
    Rectangle rect; // Rectangle visuel
    Rectangle hitboxCollision; // Rectangle pour les collisions
};

// Fonctions
void InitPlayer(Player& player, float startX, float startY);
void UpdatePlayer(Player& player, const std::vector<CollisionBox>& collisions);
void DrawPlayer(const Player& player);
void UnloadPlayer(Player& player);
