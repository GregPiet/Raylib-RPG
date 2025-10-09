#include "Player.h"
#include <raymath.h>

//==============================================================================
// INTERNALS
//==============================================================================
std::string Player::GetAnimationKey(PlayerAction action, PlayerDirection direction) const {
    return std::to_string(static_cast<int>(action)) + "_" + 
           std::to_string(static_cast<int>(direction));
}

//------------------------------------------------------------------------------
void Player::LoadAnimations() {
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

//------------------------------------------------------------------------------
void Player::UpdateHitbox() {
    m_hitbox.x = m_position.x + HITBOX_OFFSET_X;
    m_hitbox.y = m_position.y + HITBOX_OFFSET_Y;
}

//==============================================================================
// CONSTRUCTOR
//==============================================================================
Player::Player(float startX, float startY) {
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

//==============================================================================
// UPDATE
//==============================================================================
void Player::Update(const std::vector<PositionedCollision>& collisions) {
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

    // Normalisation du vecteur
    if (Vector2Length(movementVector) > 0) {
        movementVector = Vector2Normalize(movementVector);
    }

    // Mouvement + collisions
    Vector2 oldPosition = m_position;
    m_position.x += movementVector.x * m_speed * GetFrameTime();
    m_position.y += movementVector.y * m_speed * GetFrameTime();

    UpdateHitbox();

    if (CollisionSystem::CheckPlayerCollision(m_hitbox, collisions)) {
        m_position = oldPosition;
        UpdateHitbox();
    }

    // Animation
    std::string key = GetAnimationKey(m_currentAction, m_currentDirection);
    Animation& anim = m_animations[key];

    anim.timer += GetFrameTime();
    if (anim.timer >= anim.frameTime) {
        anim.timer = 0;
        anim.currentFrame = (anim.currentFrame + 1) % (anim.frameCount > 0 ? anim.frameCount : 1);
    }
}

//==============================================================================
// DRAW
//==============================================================================
void Player::Draw() const {
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

//------------------------------------------------------------------------------
void Player::DrawDebug() const {
    DrawRectangleLinesEx(m_hitbox, 1, RED);
}

//------------------------------------------------------------------------------
float Player::GetSortingY() const {
    return m_position.y + 80.0f;
}
