#include "render/RenderSystem.hpp"

#include <raylib.h>
#include <raymath.h>

#include "entities/Player.hpp"

namespace RenderSystem {

void DrawTile(const Tile& tile) {
    if (!tile.tileset) {
        return;
    }

    if (tile.isImageCollection) {
        auto it = tile.tileset->tileImages.find(tile.localId);
        if (it == tile.tileset->tileImages.end() || !it->second || it->second->id == 0) {
            return;
        }
        DrawTextureRec(*it->second, tile.source, tile.destination, WHITE);
    } else {
        if (!tile.tileset->atlas || tile.tileset->atlas->id == 0) {
            return;
        }
        DrawTextureRec(*tile.tileset->atlas, tile.source, tile.destination, WHITE);
    }
}

void DrawTiles(const std::vector<Tile>& tiles) {
    for (const auto& tile : tiles) {
        DrawTile(tile);
    }
}

void DrawTilesWithPlayer(const std::vector<Tile>& tiles, const Player& player) {
    bool playerDrawn = false;
    float playerY = player.GetSortingY();

    for (const auto& tile : tiles) {
        if (!playerDrawn && playerY < tile.sortingY) {
            player.Draw();
            playerDrawn = true;
        }
        DrawTile(tile);
    }

    if (!playerDrawn) {
        player.Draw();
    }
}

void DrawCollisionDebug(const std::vector<PositionedCollision>& collisions, Vector2 offset) {
    for (const auto& collision : collisions) {
        const auto& shape = collision.shape;
        Vector2 shapeOffset = {
            collision.position.x + offset.x,
            collision.position.y + offset.y
        };

        switch (shape.type) {
            case ShapeType::Rectangle:
                DrawRectangleLines(
                    static_cast<int>(shape.rect.x + shapeOffset.x),
                    static_cast<int>(shape.rect.y + shapeOffset.y),
                    static_cast<int>(shape.rect.width),
                    static_cast<int>(shape.rect.height),
                    RED
                );
                break;

            case ShapeType::Ellipse: {
                float rx = shape.rect.width / 2.0f;
                float ry = shape.rect.height / 2.0f;
                DrawEllipseLines(
                    static_cast<int>(shape.rect.x + shapeOffset.x + rx),
                    static_cast<int>(shape.rect.y + shapeOffset.y + ry),
                    static_cast<int>(rx),
                    static_cast<int>(ry),
                    ORANGE
                );
                break;
            }

            case ShapeType::Polygon:
                if (shape.points.size() > 1) {
                    for (size_t i = 0; i < shape.points.size(); ++i) {
                        Vector2 a = Vector2Add(shape.points[i], shapeOffset);
                        Vector2 b = Vector2Add(shape.points[(i + 1) % shape.points.size()], shapeOffset);
                        DrawLineV(a, b, BLUE);
                    }
                }
                break;

            case ShapeType::Polyline:
                if (shape.points.size() > 1) {
                    for (size_t i = 0; i < shape.points.size() - 1; ++i) {
                        Vector2 a = Vector2Add(shape.points[i], shapeOffset);
                        Vector2 b = Vector2Add(shape.points[i + 1], shapeOffset);
                        DrawLineV(a, b, PURPLE);
                    }
                }
                break;

            default:
                break;
        }
    }
}

}  // namespace RenderSystem

