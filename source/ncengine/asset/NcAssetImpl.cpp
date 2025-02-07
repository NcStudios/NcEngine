#include "NcAssetImpl.h"
#include "asset/AssetData.h"
#include "config/Config.h"
#include "manager/AudioClipAssetManager.h"
#include "manager/ConvexHullAssetManager.h"
#include "manager/CubeMapAssetManager.h"
#include "manager/FontAssetManager.h"
#include "manager/MeshAssetManager.h"
#include "manager/MeshColliderAssetManager.h"
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
      m_meshColliderManager{std::make_unique<MeshColliderAssetManager>(assetSettings.meshCollidersPath)},
      m_cubeMapManager{std::make_unique<CubeMapAssetManager>(assetSettings.cubeMapsPath, memorySettings.maxCubeMaps)},
      m_convexHullManager{std::make_unique<ConvexHullAssetManager>(assetSettings.convexHullsPath)},
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

auto NcAssetImpl::OnConvexHullUpdate() noexcept -> Signal<const ConvexHullUpdateEventData&>&
{
    return m_convexHullManager->OnUpdate();
}

auto NcAssetImpl::OnMeshColliderUpdate() noexcept -> Signal<const MeshColliderUpdateEventData&>&
{
    return m_meshColliderManager->OnUpdate();
}

auto NcAssetImpl::OnFontUpdate() noexcept -> Signal<>&
{
    return m_fontManager->OnUpdate();
}

void NcAssetImpl::LoadAssets(const AssetMap& assets)
{
    if (assets.contains(asset::AssetType::AudioClip))
        m_audioClipManager->Load(assets.at(asset::AssetType::AudioClip), false);
    if (assets.contains(asset::AssetType::ConvexHull))
        m_convexHullManager->Load(assets.at(asset::AssetType::ConvexHull), false);
    if (assets.contains(asset::AssetType::CubeMap))
        m_cubeMapManager->Load(assets.at(asset::AssetType::CubeMap), false);
    if (assets.contains(asset::AssetType::Mesh))
        m_meshManager->Load(assets.at(asset::AssetType::Mesh), false);
    if (assets.contains(asset::AssetType::MeshCollider))
        m_meshColliderManager->Load(assets.at(asset::AssetType::MeshCollider), false);
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
        m_meshColliderManager.get(),
        m_cubeMapManager.get(),
        m_convexHullManager.get(),
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

auto NcAssetImpl::GetAssetPath(AssetType type, size_t id) const -> std::string_view
{
    return GetService(type).GetPath(id);
}

auto NcAssetImpl::GetService(AssetType type) const -> const IAssetServiceBase&
{
    switch (type)
    {
        case AssetType::AudioClip:         return *m_audioClipManager;
        case AssetType::ConvexHull:        return *m_convexHullManager;
        case AssetType::CubeMap:           return *m_cubeMapManager;
        case AssetType::Mesh:              return *m_meshManager;
        case AssetType::MeshCollider:      return *m_meshColliderManager;
        case AssetType::Shader:            throw NcError{"Not Implemented"};
        case AssetType::SkeletalAnimation: return *m_skeletalAnimationManager;
        case AssetType::Texture:           return *m_textureManager;
        case AssetType::Font:              return *m_fontManager;
    }

    NC_ASSERT(false, "Unexpected code path");
    std::unreachable();
}
} // namespace nc::asset
