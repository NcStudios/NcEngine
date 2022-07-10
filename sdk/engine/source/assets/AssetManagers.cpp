#include "assets/AssetManagers.h"

namespace nc
{
AssetManagers::AssetManagers(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
    : m_meshManager{std::make_unique<MeshAssetManager>(projectSettings.meshesPath)}
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