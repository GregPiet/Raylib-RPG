#pragma once
#include <raylib.h>
#include <vector>
#include <map>
#include <string>

//==============================================================================
// ENUMERATIONS
//==============================================================================
enum class ShapeType {
    Rectangle,
    Ellipse,
    Polygon,
    Polyline,
    Unknown
};

enum class PlayerAction {
    Idle,
    Run,
    Attack1,
    Attack2
};

enum class PlayerDirection {
    Down,
    Left,
    Right,
    Up
};

//==============================================================================
// DATA STRUCTURES
//==============================================================================

// Collision shape definition
struct CollisionShape {
    ShapeType type = ShapeType::Unknown;
    std::vector<Vector2> points;
    Rectangle rect{0, 0, 0, 0};
};

// Positioned collision in world space
struct PositionedCollision {
    CollisionShape shape;
    Vector2 position;
};

// Tileset data structure
struct TileSet {
    int firstGid = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    int columns = 1;
    bool isAtlas = true;

    int tileOffsetX = 0;
    int tileOffsetY = 0;

    Texture2D* atlas = nullptr;
    std::map<int, Texture2D*> tileImages;
};

// Tile layer data
struct TileLayer {
    std::vector<int> data;
    int width = 0;
    int height = 0;
};

// Individual tile for rendering
struct Tile {
    Rectangle source{0, 0, 0, 0};
    Vector2 destination{0, 0};
    const TileSet* tileset = nullptr;
    float sortingY = 0.0f;
    int localId = 0;
    bool isImageCollection = false;
    float drawOffsetY = 0.0f;
};

// Animation data
struct Animation {
    Texture2D* texture = nullptr;
    int frameCount = 0;
    int frameWidth = 0;
    int frameHeight = 0;
    int currentFrame = 0;
    float frameTime = 0.1f;
    float timer = 0.0f;
};

// TMJ Map structure
struct TMJMap {
    int width = 0;
    int height = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    std::vector<TileSet> tilesets;
    std::vector<TileLayer> backgroundLayers;
    std::vector<TileLayer> otherLayers;
    std::map<int, std::vector<CollisionShape>> tileCollisions;
};
