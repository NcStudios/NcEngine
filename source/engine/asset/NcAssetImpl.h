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
class ConcaveColliderAssetManager;
class CubeMapAssetManager;
class FontAssetManager;
class HullColliderAssetManager;
class MeshAssetManager;
class SkeletalAnimationAssetManager;
class TextureAssetManager;

class NcAssetImpl : public NcAsset
{
    public:
        NcAssetImpl(const config::AssetSettings& assetSettings,
                    const config::MemorySettings& memorySettings,
                    AssetMap defaults);
        ~NcAssetImpl() noexcept;

        void OnBeforeSceneLoad() override;

        auto OnBoneUpdate() noexcept -> Signal<const BoneUpdateEventData&>& override;
        auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& override;
        auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&> & override;
        auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>& override;
        auto OnSkeletalAnimationUpdate() noexcept -> Signal<const SkeletalAnimationUpdateEventData&>& override;
        auto OnFontUpdate() noexcept -> Signal<>& override;
        void LoadAssets(const AssetMap& assets) override;
        auto GetLoadedAssets() const noexcept -> AssetMap override;
        void SetLoader(const PhysicsAssetLoader& loader) override;
        void ConvertToConvexHull(std::string_view mesh) override;

    private:
        std::unique_ptr<AudioClipAssetManager> m_audioClipManager;
        std::unique_ptr<ConcaveColliderAssetManager> m_concaveColliderManager;
        std::unique_ptr<CubeMapAssetManager> m_cubeMapManager;
        std::unique_ptr<HullColliderAssetManager> m_hullColliderManager;
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<SkeletalAnimationAssetManager> m_skeletalAnimationManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
        std::unique_ptr<FontAssetManager> m_fontManager;
        AssetMap m_defaults;
        const PhysicsAssetLoader* m_physicsAssetLoader = nullptr;
};
} // namespace asset
} // namespace nc
