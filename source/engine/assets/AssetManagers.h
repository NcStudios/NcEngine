#pragma once

#include "asset/Assets.h"
#include "assets/AssetData.h"
#include "assets/CubeMapAssetManager.h"
#include "assets/MeshAssetManager.h"
#include "assets/TextureAssetManager.h"
#include "config/Config.h"
#include "utility/Signal.h"

#include <memory>

namespace nc
{
struct GpuAccessorSignals
{
    GpuAccessorSignals(Signal<const CubeMapBufferData&>* _onCubeMapUpdate,
                       Signal<const MeshBufferData&>* _onMeshUpdate,
                       Signal<const TextureBufferData&>* _onTextureUpdate
                      ) noexcept;
    Signal<const CubeMapBufferData&>* onCubeMapUpdate;
    Signal<const MeshBufferData&>* onMeshUpdate;
    Signal<const TextureBufferData&>* onTextureUpdate;
};

class AssetManagers
{
    public:
        AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings);
        GpuAccessorSignals CreateGpuAccessorSignals() noexcept;

    private:
        std::unique_ptr<CubeMapAssetManager> m_cubeMapManager;
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
};
} // namespace nc