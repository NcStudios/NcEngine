#include "AssetManagers.h"
#include "AssetData.h"
#include "config/Config.h"
#include "manager/AudioClipAssetManager.h"
#include "manager/ConcaveColliderAssetManager.h"
#include "manager/CubeMapAssetManager.h"
#include "manager/HullColliderAssetManager.h"
#include "manager/MeshAssetManager.h"
#include "manager/TextureAssetManager.h"

namespace nc
{
AssetManagers::AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings)
    : m_audioClipManager{std::make_unique<AudioClipAssetManager>(assetSettings.audioClipsPath)},
      m_concaveColliderManager{std::make_unique<ConcaveColliderAssetManager>(assetSettings.concaveCollidersPath)},
      m_cubeMapManager{std::make_unique<CubeMapAssetManager>(assetSettings.cubeMapsPath, memorySettings.maxTextures)},
      m_hullColliderManager{std::make_unique<HullColliderAssetManager>(assetSettings.hullCollidersPath)},
      m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)},
      m_textureManager{std::make_unique<TextureAssetManager>(assetSettings.texturesPath, memorySettings.maxTextures)}
{
}

AssetManagers::~AssetManagers() = default;

GpuAccessorSignals AssetManagers::CreateGpuAccessorSignals() noexcept
{
    return GpuAccessorSignals
    (
        m_cubeMapManager->OnUpdate(),
        m_meshManager->OnUpdate(),
        m_textureManager->OnUpdate()
    );
}

GpuAccessorSignals::GpuAccessorSignals(nc::Signal<const CubeMapBufferData&>* _onCubeMapUpdate,
                                       nc::Signal<const MeshBufferData&>* _onMeshUpdate,
                                       nc::Signal<const TextureBufferData&>* _onTextureUpdate) noexcept
    : onCubeMapUpdate{_onCubeMapUpdate},
      onMeshUpdate{_onMeshUpdate},
      onTextureUpdate{_onTextureUpdate}
{
}
} // namespace nc