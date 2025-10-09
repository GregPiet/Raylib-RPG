#pragma once

#include <string>
#include <unordered_map>

#include <raylib.h>

class ResourceManager {
public:
    static ResourceManager& GetInstance();

    Texture2D& LoadTextureCached(const std::string& path);
    void UnloadAllTextures();
    static void Cleanup();

private:
    ResourceManager() = default;

    std::unordered_map<std::string, Texture2D> m_textureCache;
    static ResourceManager* s_instance;
};

