#pragma once

#include "assets/AssetUtilities.h"
#include "config/Config.h"
#include "utility/Signal.h"
#include <memory>

namespace nc
{
    /** @todo Implement once POC is complete
    class ConcaveColliderAssetManager;
    class ConvexHullAssetManager;
    class AudioClipAssetManager;
    class TextureAssetManager;
    class CubeMapAssetManager;
    */

    class MeshAssetManager;

    struct GpuAccessorChannels
    {
        nc::Signal<const MeshAsset&> onMeshAdd;
    };

    class Assets
    {
        public:
            Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
            ~Assets() noexcept; 
            
            GpuAccessorChannels* GpuAccessorChannels();

        private:
            nc::GpuAccessorChannels m_gpuAccessorChannels;
            std::unique_ptr<MeshAssetManager> m_meshManager;
    };
}