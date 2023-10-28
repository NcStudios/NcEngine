#include "NcAssetImpl.h"
#include "asset/Assets.h"
#include "asset/AssetData.h"
#include "config/Config.h"
#include "manager/AudioClipAssetManager.h"
#include "manager/ConcaveColliderAssetManager.h"
#include "manager/CubeMapAssetManager.h"
#include "manager/HullColliderAssetManager.h"
#include "manager/MeshAssetManager.h"
#include "manager/TextureAssetManager.h"

namespace nc::asset
{
auto BuildAssetModule(const config::AssetSettings& assetSettings,
                      const config::MemorySettings& memorySettings,
                      AssetMap defaults) -> std::unique_ptr<NcAsset>
{
    return std::make_unique<NcAssetImpl>(assetSettings, memorySettings, std::move(defaults));
}

NcAssetImpl::NcAssetImpl(const config::AssetSettings& assetSettings,
                         const config::MemorySettings& memorySettings,
                         AssetMap defaults)
    : m_audioClipManager{std::make_unique<AudioClipAssetManager>(assetSettings.audioClipsPath)},
      m_concaveColliderManager{std::make_unique<ConcaveColliderAssetManager>(assetSettings.concaveCollidersPath)},
      m_cubeMapManager{std::make_unique<CubeMapAssetManager>(assetSettings.cubeMapsPath, memorySettings.maxTextures)},
      m_hullColliderManager{std::make_unique<HullColliderAssetManager>(assetSettings.hullCollidersPath)},
      m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)},
      m_textureManager{std::make_unique<TextureAssetManager>(assetSettings.texturesPath, memorySettings.maxTextures)},
      m_defaults{std::move(defaults)}
{
}

NcAssetImpl::~NcAssetImpl() noexcept = default;

void NcAssetImpl::OnBeforeSceneLoad()
{
    if (m_defaults.contains(asset::AssetType::AudioClip))
        m_audioClipManager->Load(m_defaults.at(asset::AssetType::AudioClip), false);
    if (m_defaults.contains(asset::AssetType::ConcaveCollider))
        m_concaveColliderManager->Load(m_defaults.at(asset::AssetType::ConcaveCollider), false);
    if (m_defaults.contains(asset::AssetType::CubeMap))
        m_cubeMapManager->Load(m_defaults.at(asset::AssetType::CubeMap), false);
    if (m_defaults.contains(asset::AssetType::HullCollider))
        m_hullColliderManager->Load(m_defaults.at(asset::AssetType::HullCollider), false);
    if (m_defaults.contains(asset::AssetType::Mesh))
        m_meshManager->Load(m_defaults.at(asset::AssetType::Mesh), false);
    if (m_defaults.contains(asset::AssetType::Texture))
        m_textureManager->Load(m_defaults.at(asset::AssetType::Texture), false);
}

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
