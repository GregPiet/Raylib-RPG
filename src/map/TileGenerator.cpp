//==============================================================================
// TILE GENERATOR
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

class TileGenerator {
public:
    static std::vector<Tile> GenerateTiles(const std::vector<TileLayer>& layers, const TMJMap& map) {
        std::vector<Tile> tiles;
        tiles.reserve(layers.size() * 128);
        
        for (const auto& layer : layers) {
            for (int y = 0; y < layer.height; ++y) {
                for (int x = 0; x < layer.width; ++x) {
                    int tileId = layer.data[y * layer.width + x];
                    if (tileId == 0) continue;
                    
                    const TileSet* tileset = MapLoader::FindTilesetForGID(map, tileId);
                    if (!tileset) continue;
                    
                    int localId = tileId - tileset->firstGid;
                    Tile tile = CreateTile(tileset, localId, x, y, map);
                    
                    if (tile.tileset != nullptr) {
                        tiles.push_back(tile);
                    }
                }
            }
        }
        
        return tiles;
    }
    
private:
    static Tile CreateTile(const TileSet* tileset, int localId, int x, int y, const TMJMap& map) {
        Tile tile{};
        tile.tileset = tileset;
        tile.localId = localId;
        tile.destination = {(float)x * map.tileWidth, (float)y * map.tileHeight};
        tile.isImageCollection = !tileset->isAtlas;
        
        if (tileset->isAtlas) {
            // Spritesheet tile
            int columns = (tileset->columns > 0 ? tileset->columns : 1);
            int tx = (localId % columns) * tileset->tileWidth;
            int ty = (localId / columns) * tileset->tileHeight;
            
            tile.source = {
                (float)tx, (float)ty,
                (float)tileset->tileWidth, (float)tileset->tileHeight
            };
            
            // Apply tile offset
            tile.destination.x += tileset->tileOffsetX;
            tile.destination.y += tileset->tileOffsetY;
            tile.sortingY = tile.destination.y + tileset->tileHeight;
            tile.drawOffsetY = 0.0f;
        } else {
            // Image collection tile
            auto it = tileset->tileImages.find(localId);
            if (it == tileset->tileImages.end() || it->second == nullptr || it->second->id == 0) {
                tile.tileset = nullptr; // Mark as invalid
                return tile;
            }
            
            Texture2D* texture = it->second;
            tile.source = {0, 0, (float)texture->width, (float)texture->height};
            
            // Align to bottom like Tiled does
            float offsetY = (float)map.tileHeight - (float)texture->height;
            tile.destination.y += offsetY;
            
            // Apply tile offset
            tile.destination.x += tileset->tileOffsetX;
            tile.destination.y += tileset->tileOffsetY;
            tile.sortingY = tile.destination.y + (float)texture->height;
            tile.drawOffsetY = offsetY + (float)tileset->tileOffsetY;
        }
        
        return tile;
    }
};