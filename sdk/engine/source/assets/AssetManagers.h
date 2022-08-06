#pragma once

#include "asset/Assets.h"
#include "assets/AssetData.h"
#include "assets/MeshAssetManager.h"
#include "config/Config.h"
#include "utility/Signal.h"

#include <memory>

namespace nc
{
struct GpuAccessorSignals
{
    GpuAccessorSignals(Signal<const MeshBufferData&>* _onMeshAdd) noexcept;
    Signal<const MeshBufferData&>* onMeshAdd;
};

class AssetManagers
{
    public:
        AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings);
        GpuAccessorSignals CreateGpuAccessorSignals() noexcept;

    private:
        std::unique_ptr<MeshAssetManager> m_meshManager;
};
}