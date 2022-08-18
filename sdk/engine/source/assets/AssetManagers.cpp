#include "assets/AssetManagers.h"

namespace nc
{
AssetManagers::AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings)
    : m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)},
      m_textureManager{std::make_unique<TextureAssetManager>(assetSettings.texturesPath, memorySettings.maxTextures)}
{
}

GpuAccessorSignals AssetManagers::CreateGpuAccessorSignals() noexcept
{
    return GpuAccessorSignals
    (
        m_meshManager->OnUpdate(),
        m_textureManager->OnUpdate()
    );
}

GpuAccessorSignals::GpuAccessorSignals(nc::Signal<const MeshBufferData&>* _onMeshUpdate,
                                       nc::Signal<const TextureBufferData&>* _onTextureUpdate) noexcept
    : onMeshUpdate{_onMeshUpdate},
      onTextureUpdate{_onTextureUpdate}
{
}
} // namespace nc