#include "map/MapLoader.hpp"

#include <fstream>
#include <iostream>

#include "core/FileUtils.hpp"
#include "core/ResourceManager.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace {

void ParseTileCollisions(const json& tilesetJson, const TileSet& tileset, TMJMap& map) {
    if (!tilesetJson.contains("tiles")) {
        return;
    }

    for (const auto& tileJson : tilesetJson["tiles"]) {
        if (!tileJson.contains("id")) {
            continue;
        }

        int localId = tileJson["id"].get<int>();
        int globalId = tileset.firstGid + localId;

        if (!tileJson.contains("objectgroup")) {
            continue;
        }

        std::vector<CollisionShape> shapes;

        for (const auto& obj : tileJson["objectgroup"]["objects"]) {
            CollisionShape shape{};

            if (obj.contains("polygon")) {
                shape.type = ShapeType::Polygon;
                for (const auto& point : obj["polygon"]) {
                    shape.points.push_back({static_cast<float>(point["x"]), static_cast<float>(point["y"])});
                }
            } else if (obj.contains("polyline")) {
                shape.type = ShapeType::Polyline;
                for (const auto& point : obj["polyline"]) {
                    shape.points.push_back({static_cast<float>(point["x"]), static_cast<float>(point["y"])});
                }
            } else if (obj.contains("ellipse") && obj["ellipse"].get<bool>()) {
                shape.type = ShapeType::Ellipse;
                shape.rect = {
                    static_cast<float>(obj["x"]),
                    static_cast<float>(obj["y"]),
                    static_cast<float>(obj["width"]),
                    static_cast<float>(obj["height"])
                };
            } else if (obj.contains("width") && obj.contains("height")) {
                shape.type = ShapeType::Rectangle;
                shape.rect = {
                    static_cast<float>(obj["x"]),
                    static_cast<float>(obj["y"]),
                    static_cast<float>(obj["width"]),
                    static_cast<float>(obj["height"])
                };
            } else {
                shape.type = ShapeType::Unknown;
            }

            shapes.push_back(shape);
        }

        if (!shapes.empty()) {
            map.tileCollisions[globalId] = shapes;
        }
    }
}

void ParseTilesets(const json& data, TMJMap& map, const std::string& baseDir) {
    auto& resourceMgr = ResourceManager::GetInstance();

    for (const auto& ts : data["tilesets"]) {
        TileSet tileset{};
        tileset.firstGid = ts["firstgid"].get<int>();
        tileset.tileWidth = ts["tilewidth"].get<int>();
        tileset.tileHeight = ts["tileheight"].get<int>();
        tileset.columns = ts.value("columns", 1);

        if (ts.contains("tileoffset")) {
            tileset.tileOffsetX = ts["tileoffset"].value("x", 0);
            tileset.tileOffsetY = ts["tileoffset"].value("y", 0);
        }

        if (ts.contains("image")) {
            tileset.isAtlas = true;
            std::string imageRel = ts["image"].get<std::string>();
            std::string imageFull = FileUtils::ResolvePath(baseDir, imageRel);
            Texture2D& tex = resourceMgr.LoadTextureCached(imageFull);
            tileset.atlas = &tex;
        } else {
            tileset.isAtlas = false;
            if (ts.contains("tiles")) {
                for (const auto& tileJson : ts["tiles"]) {
                    if (!tileJson.contains("id") || !tileJson.contains("image")) {
                        continue;
                    }

                    int localId = tileJson["id"].get<int>();
                    std::string imgRel = tileJson["image"].get<std::string>();
                    std::string imgFull = FileUtils::ResolvePath(baseDir, imgRel);
                    Texture2D& tex = resourceMgr.LoadTextureCached(imgFull);
                    tileset.tileImages[localId] = &tex;
                }
            }
        }

        ParseTileCollisions(ts, tileset, map);
        map.tilesets.push_back(tileset);
    }
}

void ParseLayers(const json& layerNode, TMJMap& map, bool isBackground = false) {
    if (!layerNode.contains("type")) {
        return;
    }

    std::string type = layerNode["type"].get<std::string>();

    if (type == "group") {
        std::string groupName = layerNode.value("name", "");
        bool inBackground = (groupName == "Background" || isBackground);

        if (layerNode.contains("layers")) {
            for (const auto& subLayer : layerNode["layers"]) {
                ParseLayers(subLayer, map, inBackground);
            }
        }
    } else if (type == "tilelayer") {
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

}  // namespace

TMJMap MapLoader::LoadMap(const std::string& tmjPath) {
    TMJMap map;

    std::ifstream file(tmjPath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open " << tmjPath << std::endl;
        return map;
    }

    json data;
    file >> data;

    const std::string baseDir = FileUtils::GetDirectoryName(tmjPath);

    map.width = data["width"].get<int>();
    map.height = data["height"].get<int>();
    map.tileWidth = data["tilewidth"].get<int>();
    map.tileHeight = data["tileheight"].get<int>();

    ParseTilesets(data, map, baseDir);

    if (data.contains("layers")) {
        for (const auto& layer : data["layers"]) {
            ParseLayers(layer, map);
        }
    }

    return map;
}

const TileSet* MapLoader::FindTilesetForGID(const TMJMap& map, int gid) {
    for (auto it = map.tilesets.rbegin(); it != map.tilesets.rend(); ++it) {
        if (gid >= it->firstGid) {
            return &(*it);
        }
    }
    return nullptr;
}

