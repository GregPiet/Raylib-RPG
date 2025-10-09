//==============================================================================
// RENDER SYSTEM
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
class RenderSystem {
public:
    static void DrawTile(const Tile& tile) {
        if (!tile.tileset) return;
        
        if (tile.isImageCollection) {
            auto it = tile.tileset->tileImages.find(tile.localId);
            if (it == tile.tileset->tileImages.end() || !it->second || it->second->id == 0) {
                return;
            }
            DrawTextureRec(*it->second, tile.source, tile.destination, WHITE);
        } else {
            if (!tile.tileset->atlas || tile.tileset->atlas->id == 0) return;
            DrawTextureRec(*tile.tileset->atlas, tile.source, tile.destination, WHITE);
        }
    }
    
    static void DrawTiles(const std::vector<Tile>& tiles) {
        for (const auto& tile : tiles) {
            DrawTile(tile);
        }
    }
    
    static void DrawTilesWithPlayer(std::vector<Tile>& tiles, const Player& player) {
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
    
    static void DrawCollisionDebug(const std::vector<PositionedCollision>& collisions, Vector2 offset = {0, 0}) {
        for (const auto& collision : collisions) {
            DrawCollisionShape(collision, offset);
        }
    }
    
private:
    static void DrawCollisionShape(const PositionedCollision& collision, Vector2 offset) {
        const auto& shape = collision.shape;
        Vector2 shapeOffset = {
            collision.position.x + offset.x,
            collision.position.y + offset.y
        };
        
        switch (shape.type) {
            case ShapeType::Rectangle:
                DrawRectangleLines(
                    (int)(shape.rect.x + shapeOffset.x),
                    (int)(shape.rect.y + shapeOffset.y),
                    (int)shape.rect.width,
                    (int)shape.rect.height,
                    RED
                );
                break;
                
            case ShapeType::Ellipse: {
                float rx = shape.rect.width / 2.0f;
                float ry = shape.rect.height / 2.0f;
                DrawEllipseLines(
                    (int)(shape.rect.x + shapeOffset.x + rx),
                    (int)(shape.rect.y + shapeOffset.y + ry),
                    (int)rx, (int)ry,
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
};