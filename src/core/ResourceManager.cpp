//==============================================================================
// RESOURCE MANAGER (Singleton Pattern)
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

class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> m_textureCache;
    static ResourceManager* s_instance;
    
    ResourceManager() = default;
    
public:
    static ResourceManager& GetInstance() {
        if (!s_instance) {
            s_instance = new ResourceManager();
        }
        return *s_instance;
    }
    
    Texture2D& LoadTextureCached(const std::string& path) {
        auto it = m_textureCache.find(path);
        if (it != m_textureCache.end()) {
            return it->second;
        }
        
        Texture2D tex{};
        if (!path.empty()) {
            tex = LoadTexture(path.c_str());
        }
        m_textureCache[path] = tex;
        return m_textureCache[path];
    }
    
    void UnloadAllTextures() {
        for (auto& [path, texture] : m_textureCache) {
            if (texture.id != 0) {
                UnloadTexture(texture);
            }
        }
        m_textureCache.clear();
    }
    
    static void Cleanup() {
        if (s_instance) {
            s_instance->UnloadAllTextures();
            delete s_instance;
            s_instance = nullptr;
        }
    }
};
