#include "NcAssetImpl.h"
#include "asset/AssetData.h"
#include "config/Config.h"
#include "manager/AudioClipAssetManager.h"
#include "manager/ConcaveColliderAssetManager.h"
#include "manager/CubeMapAssetManager.h"
#include "manager/HullColliderAssetManager.h"
#include "manager/MeshAssetManager.h"
#include "manager/NormalMapAssetManager.h"
#include "manager/TextureAssetManager.h"

namespace nc::asset
{
auto BuildAssetModule(const config::AssetSettings& assetSettings,
                      const config::MemorySettings& memorySettings) -> std::unique_ptr<NcAsset>
{
    return std::make_unique<NcAssetImpl>(assetSettings, memorySettings);
}

NcAssetImpl::NcAssetImpl(const config::AssetSettings& assetSettings,
                         const config::MemorySettings& memorySettings)
    : m_audioClipManager{std::make_unique<AudioClipAssetManager>(assetSettings.audioClipsPath)},
      m_concaveColliderManager{std::make_unique<ConcaveColliderAssetManager>(assetSettings.concaveCollidersPath)},
      m_cubeMapManager{std::make_unique<CubeMapAssetManager>(assetSettings.cubeMapsPath, memorySettings.maxTextures)},
      m_hullColliderManager{std::make_unique<HullColliderAssetManager>(assetSettings.hullCollidersPath)},
      m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)},
      m_normalMapManager{std::make_unique<NormalMapAssetManager>(assetSettings.normalMapsPath, memorySettings.maxTextures)},
      m_textureManager{std::make_unique<TextureAssetManager>(assetSettings.texturesPath, memorySettings.maxTextures)}
{
}

NcAssetImpl::~NcAssetImpl() = default;

auto NcAssetImpl::OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>&
{
    return m_cubeMapManager->OnUpdate();
}

auto NcAssetImpl::OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>&
{
    return m_textureManager->OnUpdate();
}

auto NcAssetImpl::OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&>&
{
    return m_meshManager->OnUpdate();
}
} // namespace nc::asset
