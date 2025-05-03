#pragma once

#include "asset/NcAsset.h"

#include <memory>

namespace nc
{
namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config

namespace asset
{
class AudioClipAssetManager;
class ConvexHullAssetManager;
class CubeMapAssetManager;
class FontAssetManager;
class IAssetServiceBase;
class MeshAssetManager;
class MeshColliderAssetManager;
class SkeletalAnimationAssetManager;
class TextureAssetManager;

class NcAssetImpl : public NcAsset
{
    public:
        NcAssetImpl(const config::AssetSettings& assetSettings,
                    const config::MemorySettings& memorySettings,
                    AssetMap defaults);
        ~NcAssetImpl() noexcept;

        void OnBeforeSceneLoad(const Scene& sceneToLoad) override;

        auto OnBoneUpdate() noexcept -> Signal<const BoneUpdateEventData&>& override;
        auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& override;
        auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&> & override;
        auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>& override;
        auto OnSkeletalAnimationUpdate() noexcept -> Signal<const SkeletalAnimationUpdateEventData&>& override;
        auto OnConvexHullUpdate() noexcept -> Signal<const ConvexHullUpdateEventData&>& override;
        auto OnMeshColliderUpdate() noexcept -> Signal<const MeshColliderUpdateEventData&>& override;
        auto OnFontUpdate() noexcept -> Signal<>& override;
        void LoadAssets(const AssetMap& assets) override;
        auto GetLoadedAssets() const noexcept -> AssetMap override;
        auto GetAssetPath(AssetType type, size_t id) const -> std::string_view override;

    private:
        std::unique_ptr<AudioClipAssetManager> m_audioClipManager;
        std::unique_ptr<MeshColliderAssetManager> m_meshColliderManager;
        std::unique_ptr<CubeMapAssetManager> m_cubeMapManager;
        std::unique_ptr<ConvexHullAssetManager> m_convexHullManager;
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<SkeletalAnimationAssetManager> m_skeletalAnimationManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
        std::unique_ptr<FontAssetManager> m_fontManager;
        AssetMap m_defaults;

        auto GetService(AssetType type) const -> const IAssetServiceBase&;
};
} // namespace asset
} // namespace nc
