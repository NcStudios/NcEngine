#pragma once

#include "asset/Assets.h"
#include "assets/AssetData.h"
#include "assets/MeshAssetManager.h"
#include "assets/TextureAssetManager.h"
#include "config/Config.h"
#include "utility/Signal.h"

#include <memory>

namespace nc
{
struct GpuAccessorSignals
{
    GpuAccessorSignals(Signal<const MeshBufferData&>* _onMeshUpdate,
                       Signal<const TextureBufferData&>* _onTextureUpdate
                      ) noexcept;
    Signal<const MeshBufferData&>* onMeshUpdate;
    Signal<const TextureBufferData&>* onTextureUpdate;
};

class AssetManagers
{
    public:
        AssetManagers(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings);
        GpuAccessorSignals CreateGpuAccessorSignals() noexcept;

    private:
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
};
}