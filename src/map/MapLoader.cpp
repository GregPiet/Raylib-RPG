//==============================================================================
// MAP LOADER
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

class MapLoader {
private:
    // Parse layer nodes recursively
    static void ParseLayers(const json& layerNode, TMJMap& map, bool isBackground = false) {
        if (!layerNode.contains("type")) return;
        
        std::string type = layerNode["type"].get<std::string>();
        
        if (type == "group") {
            std::string groupName = layerNode.value("name", "");
            bool inBackground = (groupName == "Background" || isBackground);
            
            if (layerNode.contains("layers")) {
                for (auto& subLayer : layerNode["layers"]) {
                    ParseLayers(subLayer, map, inBackground);
                }
            }
        }
        else if (type == "tilelayer") {
            TileLayer layer;
            layer.width = layerNode["width"].get<int>();
            layer.height = layerNode["height"].get<int>();
            layer.data = layerNode["data"].get<std::vector<int>>();
            
            if (isBackground) {
                map.backgroundLayers.push_back(layer);
            } else {
                map.otherLayers.push_back(layer);
            }
        }
    }
    
    // Parse tileset data
    static void ParseTilesets(const json& data, TMJMap& map, const std::string& baseDir) {
        auto& resourceMgr = ResourceManager::GetInstance();
        
        for (auto& ts : data["tilesets"]) {
            TileSet tileset{};
            tileset.firstGid = ts["firstgid"].get<int>();
            tileset.tileWidth = ts["tilewidth"].get<int>();
            tileset.tileHeight = ts["tileheight"].get<int>();
            
            // Ensure columns is at least 1
            tileset.columns = 1;
            if (ts.contains("columns")) {
                int cols = 0;
                try {
                    cols = ts["columns"].get<int>();
                } catch (...) {
                    cols = 0;
                }
                if (cols > 0) tileset.columns = cols;
            }
            
            // Parse tile offset if present
            if (ts.contains("tileoffset")) {
                tileset.tileOffsetX = ts["tileoffset"].value("x", 0);
                tileset.tileOffsetY = ts["tileoffset"].value("y", 0);
            }
            
            // Determine if atlas or image collection
            if (ts.contains("image")) {
                tileset.isAtlas = true;
                std::string imageRel = ts["image"].get<std::string>();
                std::string imageFull = FileUtils::ResolvePath(baseDir, imageRel);
                Texture2D& tex = resourceMgr.LoadTextureCached(imageFull);
                tileset.atlas = &tex;
            } else {
                tileset.isAtlas = false;
                if (ts.contains("tiles")) {
                    for (auto& tileJson : ts["tiles"]) {
                        if (!tileJson.contains("id") || !tileJson.contains("image")) continue;
                        
                        int localId = tileJson["id"].get<int>();
                        std::string imgRel = tileJson["image"].get<std::string>();
                        std::string imgFull = FileUtils::ResolvePath(baseDir, imgRel);
                        Texture2D& tex = resourceMgr.LoadTextureCached(imgFull);
                        tileset.tileImages[localId] = &tex;
                    }
                }
            }
            
            // Parse collision shapes
            ParseTileCollisions(ts, tileset, map);
            
            map.tilesets.push_back(tileset);
        }
    }
    
    // Parse collision shapes for tiles
    static void ParseTileCollisions(const json& tilesetJson, const TileSet& tileset, TMJMap& map) {
        if (!tilesetJson.contains("tiles")) return;
        
        for (auto& tileJson : tilesetJson["tiles"]) {
            if (!tileJson.contains("id")) continue;
            
            int localId = tileJson["id"].get<int>();
            int globalId = tileset.firstGid + localId;
            
            if (!tileJson.contains("objectgroup")) continue;
            
            std::vector<CollisionShape> shapes;
            
            for (auto& obj : tileJson["objectgroup"]["objects"]) {
                CollisionShape shape{};
                
                if (obj.contains("polygon")) {
                    shape.type = ShapeType::Polygon;
                    for (auto& p : obj["polygon"]) {
                        shape.points.push_back({(float)p["x"], (float)p["y"]});
                    }
                }
                else if (obj.contains("polyline")) {
                    shape.type = ShapeType::Polyline;
                    for (auto& p : obj["polyline"]) {
                        shape.points.push_back({(float)p["x"], (float)p["y"]});
                    }
                }
                else if (obj.contains("ellipse") && obj["ellipse"].get<bool>()) {
                    shape.type = ShapeType::Ellipse;
                    shape.rect = {
                        (float)obj["x"], (float)obj["y"],
                        (float)obj["width"], (float)obj["height"]
                    };
                }
                else if (obj.contains("width") && obj.contains("height")) {
                    shape.type = ShapeType::Rectangle;
                    shape.rect = {
                        (float)obj["x"], (float)obj["y"],
                        (float)obj["width"], (float)obj["height"]
                    };
                }
                else {
                    shape.type = ShapeType::Unknown;
                }
                
                shapes.push_back(shape);
            }
            
            if (!shapes.empty()) {
                map.tileCollisions[globalId] = shapes;
            }
        }
    }
    
public:
    static TMJMap LoadMap(const std::string& tmjPath) {
        TMJMap map;
        
        std::ifstream file(tmjPath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open " << tmjPath << std::endl;
            return map;
        }
        
        json data;
        file >> data;
        
        const std::string baseDir = FileUtils::GetDirectoryName(tmjPath);
        
        // Parse map properties
        map.width = data["width"].get<int>();
        map.height = data["height"].get<int>();
        map.tileWidth = data["tilewidth"].get<int>();
        map.tileHeight = data["tileheight"].get<int>();
        
        // Parse tilesets
        ParseTilesets(data, map, baseDir);
        
        // Parse layers
        if (data.contains("layers")) {
            for (auto& layer : data["layers"]) {
                ParseLayers(layer, map);
            }
        }
        
        // Debug output
        size_t totalCollisions = 0;
        for (const auto& [gid, shapes] : map.tileCollisions) {
            totalCollisions += shapes.size();
        }
        
        std::cout << "Map loaded: " << map.width << "x" << map.height << std::endl;
        std::cout << "Tilesets: " << map.tilesets.size() << std::endl;
        std::cout << "Collision definitions: " << totalCollisions << std::endl;
        std::cout << "Background layers: " << map.backgroundLayers.size() << std::endl;
        std::cout << "Object layers: " << map.otherLayers.size() << std::endl;
        
        return map;
    }
    
    static const TileSet* FindTilesetForGID(const TMJMap& map, int gid) {
        const TileSet* result = nullptr;
        for (const auto& tileset : map.tilesets) {
            if (gid >= tileset.firstGid) {
                result = &tileset;
            } else {
                break;
            }
        }
        return result;
    }
};