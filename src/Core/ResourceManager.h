#pragma once
#include <raylib.h>
#include <unordered_map>
#include <string>

// Singleton de gestion des ressources (textures)
class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> m_textureCache;
    static ResourceManager* s_instance;

    ResourceManager() = default;

public:
    static ResourceManager& GetInstance();

    Texture2D& LoadTextureCached(const std::string& path);

    void UnloadAllTextures();

    static void Cleanup();
};
