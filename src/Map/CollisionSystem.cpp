#include "CollisionSystem.h"

std::vector<PositionedCollision> CollisionSystem::GenerateCollisions(const TMJMap& map) {
    std::vector<PositionedCollision> collisions;

    std::vector<TileLayer> allLayers = map.backgroundLayers;
    allLayers.insert(allLayers.end(), map.otherLayers.begin(), map.otherLayers.end());

    collisions.reserve(map.width * map.height);

    for (const auto& layer : allLayers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int gid = layer.data[y * layer.width + x];
                if (gid == 0) continue;

                auto it = map.tileCollisions.find(gid);
                if (it == map.tileCollisions.end()) continue;

                const TileSet* tileset = MapLoader::FindTilesetForGID(map, gid);
                if (!tileset) continue;

                int localId = gid - tileset->firstGid;
                Vector2 position = CalculateCollisionPosition(x, y, tileset, localId, map);

                for (const auto& shape : it->second) {
                    collisions.push_back({shape, position});
                }
            }
        }
    }

    return collisions;
}

bool CollisionSystem::CheckPlayerCollision(const Rectangle& playerHitbox, const std::vector<PositionedCollision>& collisions) {
    for (const auto& collision : collisions) {
        if (CheckCollisionWithShape(playerHitbox, collision)) {
            return true;
        }
    }
    return false;
}

Vector2 CollisionSystem::CalculateCollisionPosition(int x, int y, const TileSet* tileset, int localId, const TMJMap& map) {
    Vector2 position = {(float)(x * map.tileWidth), (float)(y * map.tileHeight)};

    float offsetY = 0.0f;
    if (!tileset->isAtlas) {
        auto it = tileset->tileImages.find(localId);
        if (it != tileset->tileImages.end() && it->second && it->second->id != 0) {
            offsetY = (float)map.tileHeight - (float)it->second->height;
        }
    }

    position.x += tileset->tileOffsetX;
    position.y += tileset->tileOffsetY + offsetY;

    return position;
}

bool CollisionSystem::CheckCollisionWithShape(const Rectangle& rect, const PositionedCollision& collision) {
    switch (collision.shape.type) {
        case ShapeType::Rectangle: {
            Rectangle shapeRect = {
                collision.shape.rect.x + collision.position.x,
                collision.shape.rect.y + collision.position.y,
                collision.shape.rect.width,
                collision.shape.rect.height
            };
            return CheckCollisionRecs(rect, shapeRect);
        }

        case ShapeType::Ellipse: {
            Vector2 center = {
                collision.shape.rect.x + collision.position.x + collision.shape.rect.width / 2.0f,
                collision.shape.rect.y + collision.position.y + collision.shape.rect.height / 2.0f
            };
            float rx = collision.shape.rect.width / 2.0f;
            float ry = collision.shape.rect.height / 2.0f;
            float radius = (rx + ry) / 2.0f; // Approximation
            return CheckCollisionCircleRec(center, radius, rect);
        }

        default:
            return false;
    }
}
