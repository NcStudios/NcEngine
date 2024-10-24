#include "NcAssetImpl.h"
#include "asset/AssetData.h"
#include "config/Config.h"
#include "manager/AudioClipAssetManager.h"
#include "manager/ConcaveColliderAssetManager.h"
#include "manager/CubeMapAssetManager.h"
#include "manager/FontAssetManager.h"
#include "manager/HullColliderAssetManager.h"
#include "manager/MeshAssetManager.h"
#include "manager/SkeletalAnimationAssetManager.h"
#include "manager/TextureAssetManager.h"

#include <array>
#include <ranges>

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
      m_cubeMapManager{std::make_unique<CubeMapAssetManager>(assetSettings.cubeMapsPath, memorySettings.maxCubeMaps)},
      m_hullColliderManager{std::make_unique<HullColliderAssetManager>(assetSettings.hullCollidersPath)},
      m_meshManager{std::make_unique<MeshAssetManager>(assetSettings.meshesPath)},
      m_skeletalAnimationManager{std::make_unique<SkeletalAnimationAssetManager>(assetSettings.skeletalAnimationsPath, memorySettings.maxSkeletalAnimations)},
      m_textureManager{std::make_unique<TextureAssetManager>(assetSettings.texturesPath, memorySettings.maxTextures)},
      m_fontManager{std::make_unique<FontAssetManager>(assetSettings.fontsPath)},
      m_defaults{std::move(defaults)}
{
}

NcAssetImpl::~NcAssetImpl() noexcept = default;

void NcAssetImpl::OnBeforeSceneLoad()
{
    LoadAssets(m_defaults);
}

auto NcAssetImpl::OnBoneUpdate() noexcept -> Signal<const BoneUpdateEventData&>&
{
    return m_meshManager->OnBoneUpdate();
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
    return m_meshManager->OnMeshUpdate();
}

auto NcAssetImpl::OnSkeletalAnimationUpdate() noexcept -> Signal<const SkeletalAnimationUpdateEventData&>&
{
    return m_skeletalAnimationManager->OnUpdate();
}

auto NcAssetImpl::OnFontUpdate() noexcept -> Signal<>&
{
    return m_fontManager->OnUpdate();
}

void NcAssetImpl::LoadAssets(const AssetMap& assets)
{
    if (assets.contains(asset::AssetType::AudioClip))
        m_audioClipManager->Load(assets.at(asset::AssetType::AudioClip), false);
    if (assets.contains(asset::AssetType::ConcaveCollider))
        m_concaveColliderManager->Load(assets.at(asset::AssetType::ConcaveCollider), false);
    if (assets.contains(asset::AssetType::CubeMap))
        m_cubeMapManager->Load(assets.at(asset::AssetType::CubeMap), false);
    if (assets.contains(asset::AssetType::HullCollider))
        m_hullColliderManager->Load(assets.at(asset::AssetType::HullCollider), false);
    if (assets.contains(asset::AssetType::Mesh))
        m_meshManager->Load(assets.at(asset::AssetType::Mesh), false);
    if (assets.contains(asset::AssetType::SkeletalAnimation))
        m_skeletalAnimationManager->Load(assets.at(asset::AssetType::SkeletalAnimation), false);
    if (assets.contains(asset::AssetType::Texture))
        m_textureManager->Load(assets.at(asset::AssetType::Texture), false);
}

auto NcAssetImpl::GetLoadedAssets() const noexcept -> AssetMap
{
    const auto managers = std::array<IAssetServiceBase*, 7>
    {
        m_audioClipManager.get(),
        m_concaveColliderManager.get(),
        m_cubeMapManager.get(),
        m_hullColliderManager.get(),
        m_meshManager.get(),
        m_skeletalAnimationManager.get(),
        m_textureManager.get()
    };

    auto out = AssetMap{};
    std::ranges::for_each(managers, [&out](auto manager) mutable
    {
        auto assets = manager->GetAllLoaded()
            | std::views::transform([](auto&& name){ return std::string{name}; });

        out.emplace(manager->GetAssetType(), std::vector<std::string>{assets.begin(), assets.end()});
    });

    return out;
}
} // namespace nc::asset
