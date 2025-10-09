//==============================================================================
// PLAYER CLASS
//==============================================================================
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
    
    std::string GetAnimationKey(PlayerAction action, PlayerDirection direction) const {
        return std::to_string(static_cast<int>(action)) + "_" + 
               std::to_string(static_cast<int>(direction));
    }
    
    void LoadAnimations() {
        auto& resourceMgr = ResourceManager::GetInstance();
        
        std::map<PlayerAction, std::string> basePaths = {
            {PlayerAction::Idle, "assets/player/IDLE/idle_"},
            {PlayerAction::Run, "assets/player/RUN/run_"},
            {PlayerAction::Attack1, "assets/player/ATTACK 1/attack1_"},
            {PlayerAction::Attack2, "assets/player/ATTACK 2/attack2_"}
        };
        
        std::map<PlayerDirection, std::string> directionSuffixes = {
            {PlayerDirection::Down, "down.png"},
            {PlayerDirection::Left, "left.png"},
            {PlayerDirection::Right, "right.png"},
            {PlayerDirection::Up, "up.png"}
        };
        
        for (const auto& [action, basePath] : basePaths) {
            for (const auto& [direction, suffix] : directionSuffixes) {
                std::string key = GetAnimationKey(action, direction);
                std::string path = basePath + suffix;
                
                Texture2D& texture = resourceMgr.LoadTextureCached(path);
                
                Animation anim{};
                anim.texture = &texture;
                anim.frameCount = SPRITE_FRAME_COUNT;
                anim.frameWidth = (texture.id != 0) ? texture.width / SPRITE_FRAME_COUNT : 0;
                anim.frameHeight = (texture.id != 0) ? texture.height : 0;
                anim.currentFrame = 0;
                anim.frameTime = SPRITE_FRAME_TIME;
                anim.timer = 0.0f;
                
                m_animations[key] = anim;
            }
        }
    }
    
    void UpdateHitbox() {
        m_hitbox.x = m_position.x + HITBOX_OFFSET_X;
        m_hitbox.y = m_position.y + HITBOX_OFFSET_Y;
    }
    
public:
    Player(float startX, float startY) {
        m_position = {startX, startY};
        m_speed = MOVEMENT_SPEED;
        m_currentDirection = PlayerDirection::Down;
        m_currentAction = PlayerAction::Idle;
        m_hitbox = {
            startX + HITBOX_OFFSET_X, 
            startY + HITBOX_OFFSET_Y, 
            HITBOX_WIDTH, 
            HITBOX_HEIGHT
        };
        
        LoadAnimations();
    }
    
    void Update(const std::vector<PositionedCollision>& collisions) {
        // Handle input and movement
        Vector2 movementVector = {0, 0};
        PlayerAction newAction = PlayerAction::Idle;
        
        if (IsKeyDown(KEY_RIGHT)) {
            movementVector.x += 1;
            m_currentDirection = PlayerDirection::Right;
            newAction = PlayerAction::Run;
        }
        if (IsKeyDown(KEY_LEFT)) {
            movementVector.x -= 1;
            m_currentDirection = PlayerDirection::Left;
            newAction = PlayerAction::Run;
        }
        if (IsKeyDown(KEY_UP)) {
            movementVector.y -= 1;
            m_currentDirection = PlayerDirection::Up;
            newAction = PlayerAction::Run;
        }
        if (IsKeyDown(KEY_DOWN)) {
            movementVector.y += 1;
            m_currentDirection = PlayerDirection::Down;
            newAction = PlayerAction::Run;
        }
        
        if (IsKeyPressed(KEY_SPACE)) {
            newAction = PlayerAction::Attack1;
        }
        
        m_currentAction = newAction;
        
        // Normalize movement
        if (Vector2Length(movementVector) > 0) {
            movementVector = Vector2Normalize(movementVector);
        }
        
        // Apply movement with collision detection
        Vector2 oldPosition = m_position;
        m_position.x += movementVector.x * m_speed * GetFrameTime();
        m_position.y += movementVector.y * m_speed * GetFrameTime();
        
        UpdateHitbox();
        
        if (CollisionSystem::CheckPlayerCollision(m_hitbox, collisions)) {
            m_position = oldPosition;
            UpdateHitbox();
        }
        
        // Update animation
        std::string key = GetAnimationKey(m_currentAction, m_currentDirection);
        Animation& anim = m_animations[key];
        
        anim.timer += GetFrameTime();
        if (anim.timer >= anim.frameTime) {
            anim.timer = 0;
            anim.currentFrame = (anim.currentFrame + 1) % (anim.frameCount > 0 ? anim.frameCount : 1);
        }
    }
    
    void Draw() const {
        std::string key = GetAnimationKey(m_currentAction, m_currentDirection);
        const Animation& anim = m_animations.at(key);
        
        if (!anim.texture || anim.texture->id == 0) return;
        
        Rectangle sourceRect = {
            (float)(anim.currentFrame * anim.frameWidth), 0,
            (float)anim.frameWidth, (float)anim.frameHeight
        };
        
        Rectangle destRect = {
            m_position.x, m_position.y,
            (float)anim.frameWidth * 2, (float)anim.frameHeight * 2
        };
        
        Vector2 origin = {
            (float)anim.frameWidth / 2, 
            (float)anim.frameHeight / 2
        };
        
        DrawTexturePro(*anim.texture, sourceRect, destRect, origin, 0.0f, WHITE);
    }
    
    void DrawDebug() const {
        DrawRectangleLinesEx(m_hitbox, 1, RED);
    }
    
    float GetSortingY() const {
        return m_position.y + 80.0f; // Adjust based on sprite
    }
};
