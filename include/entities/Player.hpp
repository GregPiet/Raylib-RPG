#pragma once

#include <map>
#include <string>
#include <vector>

#include "GameTypes.hpp"
#include "map/CollisionSystem.hpp"

class Player {
public:
    Player(float startX, float startY);

    void Update(const std::vector<PositionedCollision>& collisions);
    void Draw() const;
    void DrawDebug() const;
    float GetSortingY() const;

private:
    std::string GetAnimationKey(PlayerAction action, PlayerDirection direction) const;
    void LoadAnimations();
    void UpdateHitbox();

    Vector2 m_position{};
    float m_speed = 0.0f;
    PlayerAction m_currentAction = PlayerAction::Idle;
    PlayerDirection m_currentDirection = PlayerDirection::Down;
    Rectangle m_hitbox{0, 0, 0, 0};
    std::map<std::string, Animation> m_animations;
};

