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
        GpuAccessorChannels(nc::Signal<const MeshAsset&>* _onMeshAdd);

        nc::Signal<const MeshAsset&>* onMeshAdd;
    };

    class Assets
    {
        public:
            Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
            ~Assets() noexcept; 
            
            GpuAccessorChannels* GpuAccessorChannels();

        private:
            std::unique_ptr<MeshAssetManager> m_meshManager;
            nc::GpuAccessorChannels m_gpuAccessorChannels;
    };
}