#include "assets/AssetManagers.h"

namespace nc
{
AssetManagers::AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings)
    : m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)}
{
}

GpuAccessorSignals AssetManagers::CreateGpuAccessorSignals() noexcept
{
    return GpuAccessorSignals(m_meshManager->OnMeshAdd());
}

GpuAccessorSignals::GpuAccessorSignals(nc::Signal<const MeshBufferData&>* _onMeshAdd) noexcept
    : onMeshAdd{_onMeshAdd}
{
}
}