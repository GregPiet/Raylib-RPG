#pragma once
#include <raylib.h>
#include <map>
#include <string>
#include <vector>
#include <cmath>

#include "../Core/ResourceManager.h"
#include "../Map/TMJTypes.h"
#include "../Map/CollisionSystem.h"

//==============================================================================
// PLAYER CLASS
//==============================================================================
class Player {
private:
    static constexpr int SPRITE_FRAME_COUNT = 8;
    static constexpr float SPRITE_FRAME_TIME = 0.1f;
    static constexpr float MOVEMENT_SPEED = 100.0f;
    static constexpr float HITBOX_OFFSET_X = 38.0f;
    static constexpr float HITBOX_OFFSET_Y = 68.0f;
    static constexpr float HITBOX_WIDTH = 22.0f;
    static constexpr float HITBOX_HEIGHT = 8.0f;

    Vector2 m_position;
    float m_speed;
    PlayerAction m_currentAction;
    PlayerDirection m_currentDirection;
    Rectangle m_hitbox;
    std::map<std::string, Animation> m_animations;

    std::string GetAnimationKey(PlayerAction action, PlayerDirection direction) const;
    void LoadAnimations();
    void UpdateHitbox();

public:
    Player(float startX, float startY);

    void Update(const std::vector<PositionedCollision>& collisions);
    void Draw() const;
    void DrawDebug() const;
    float GetSortingY() const;
};
