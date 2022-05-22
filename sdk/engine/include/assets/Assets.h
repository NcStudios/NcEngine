#pragma once

#include "config/Config.h"
#include <memory>

namespace nc
{
    class ConcaveColliderAssetManager;
    class ConvexHullAssetManager;
    class AudioClipAssetManager;
    class MeshAssetManager;
    class TextureAssetManager;
    class CubeMapAssetManager;

    namespace graphics { class Graphics; }

    struct Assets
    {
        Assets(graphics::Graphics* graphics, const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
        ~Assets() noexcept; 

        std::unique_ptr<ConcaveColliderAssetManager> concaveManager;
        std::unique_ptr<ConvexHullAssetManager> hullManager;
        std::unique_ptr<AudioClipAssetManager> audioClipManager;
        std::unique_ptr<MeshAssetManager> meshManager;
        std::unique_ptr<TextureAssetManager> textureManager;
        std::unique_ptr<CubeMapAssetManager> cubeMapAssetManager;
    };
}