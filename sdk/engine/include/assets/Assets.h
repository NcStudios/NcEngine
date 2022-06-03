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

    namespace graphics { class AssetsSink; }

    struct Assets
    {
        Assets(graphics::AssetsSink* assetsSink, const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
        ~Assets() noexcept; 

        std::unique_ptr<MeshAssetManager> meshManager;
    };
}