#pragma once

#include "asset/NcAsset.h"

#include <memory>

namespace nc
{
class AudioClipAssetManager;
class ConcaveColliderAssetManager;
class CubeMapAssetManager;
class HullColliderAssetManager;
class MeshAssetManager;
class NormalMapAssetManager;
class TextureAssetManager;

namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config
}

namespace nc::asset
{
class NcAssetImpl : public NcAsset
{
    public:
        NcAssetImpl(const config::AssetSettings& assetSettings,
                    const config::MemorySettings& memorySettings);
        ~NcAssetImpl() noexcept;

        auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& override;
        auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&> & override;
        auto OnNormalMapUpdate() noexcept -> Signal<const NormalMapUpdateEventData&> & override;
        auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>& override;

    private:
        std::unique_ptr<AudioClipAssetManager> m_audioClipManager;
        std::unique_ptr<ConcaveColliderAssetManager> m_concaveColliderManager;
        std::unique_ptr<CubeMapAssetManager> m_cubeMapManager;
        std::unique_ptr<HullColliderAssetManager> m_hullColliderManager;
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<NormalMapAssetManager> m_normalMapManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
};
} // namespace nc::asset
