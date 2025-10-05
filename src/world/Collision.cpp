#include "Collision.hpp"
#include "pugixml.hpp"
#include <iostream>

std::vector<CollisionBox> LoadCollisionsFromTmx(const std::string& path) {
    std::vector<CollisionBox> boxes;
    pugi::xml_document doc;

    if (!doc.load_file(path.c_str())) {
        std::cerr << "❌ Erreur : impossible de charger " << path << "\n";
        return boxes;
    }

    pugi::xml_node map = doc.child("map");

    for (pugi::xml_node layer : map.children("objectgroup")) {
        std::string name = layer.attribute("name").as_string();
        if (name != "Collisions") continue;

        for (pugi::xml_node obj : layer.children("object")) {
            float x = obj.attribute("x").as_float();
            float y = obj.attribute("y").as_float();
            float w = obj.attribute("width").as_float(0);
            float h = obj.attribute("height").as_float(0);

            if (w <= 0 || h <= 0) continue;

            bool solid = false;
            for (pugi::xml_node prop : obj.child("properties").children("property")) {
                std::string propName = prop.attribute("name").as_string();
                if (propName == "solid" && prop.attribute("value").as_bool()) {
                    solid = true;
                    break;
                }
            }

            if (solid)
                boxes.push_back({ Rectangle{x, y, w, h}, true });
        }
    }

    std::cout << "✅ " << boxes.size() << " collisions chargées depuis " << path << "\n";
    return boxes;
}
