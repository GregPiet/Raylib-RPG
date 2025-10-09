#include "MapLoader.h"

//==============================================================================
// PARSE LAYERS
//==============================================================================
void MapLoader::ParseLayers(const json& layerNode, TMJMap& map, bool isBackground) {
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

//==============================================================================
// PARSE TILESETS
//==============================================================================
void MapLoader::ParseTilesets(const json& data, TMJMap& map, const std::string& baseDir) {
    auto& resourceMgr = ResourceManager::GetInstance();

    for (auto& ts : data["tilesets"]) {
        TileSet tileset{};
        tileset.firstGid = ts["firstgid"].get<int>();
        tileset.tileWidth = ts["tilewidth"].get<int>();
        tileset.tileHeight = ts["tileheight"].get<int>();

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

        ParseTileCollisions(ts, tileset, map);
        map.tilesets.push_back(tileset);
    }
}

//==============================================================================
// PARSE COLLISIONS
//==============================================================================
void MapLoader::ParseTileCollisions(const json& tilesetJson, const TileSet& tileset, TMJMap& map) {
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

//==============================================================================
// LOAD MAP
//==============================================================================
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
        for (auto& layer : data["layers"]) {
            ParseLayers(layer, map);
        }
    }

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

//==============================================================================
// FIND TILESET FOR GID
//==============================================================================
const TileSet* MapLoader::FindTilesetForGID(const TMJMap& map, int gid) {
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
