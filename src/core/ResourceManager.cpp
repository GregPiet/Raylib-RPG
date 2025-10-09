#include "core/ResourceManager.hpp"

ResourceManager* ResourceManager::s_instance = nullptr;

ResourceManager& ResourceManager::GetInstance() {
    if (!s_instance) {
        s_instance = new ResourceManager();
    }
    return *s_instance;
}

Texture2D& ResourceManager::LoadTextureCached(const std::string& path) {
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        return it->second;
    }

    Texture2D texture{};
    if (!path.empty()) {
        texture = LoadTexture(path.c_str());
    }

    m_textureCache[path] = texture;
    return m_textureCache[path];
}

void ResourceManager::UnloadAllTextures() {
    for (auto& [path, texture] : m_textureCache) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    m_textureCache.clear();
}

void ResourceManager::Cleanup() {
    if (s_instance) {
        s_instance->UnloadAllTextures();
        delete s_instance;
        s_instance = nullptr;
    }
}

