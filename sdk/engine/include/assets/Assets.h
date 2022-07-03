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

    struct Assets
    {
        Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
        ~Assets() noexcept; 

        std::unique_ptr<MeshAssetManager> meshManager;
    };
}