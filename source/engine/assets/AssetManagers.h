#pragma once

#include "utility/Signal.h"

#include <memory>

namespace nc
{
class AudioClipAssetManager;
class ConcaveColliderAssetManager;
class CubeMapAssetManager;
struct CubeMapBufferData;
class HullColliderAssetManager;
class MeshAssetManager;
struct MeshBufferData;
class TextureAssetManager;
struct TextureBufferData;

namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config

struct GpuAccessorSignals
{
    GpuAccessorSignals(Signal<const CubeMapBufferData&>* _onCubeMapUpdate,
                       Signal<const MeshBufferData&>* _onMeshUpdate,
                       Signal<const TextureBufferData&>* _onTextureUpdate) noexcept;

    Signal<const CubeMapBufferData&>* onCubeMapUpdate;
    Signal<const MeshBufferData&>* onMeshUpdate;
    Signal<const TextureBufferData&>* onTextureUpdate;
};

class AssetManagers
{
    public:
        AssetManagers(const config::AssetSettings& assetSettings, const config::MemorySettings& memorySettings);
        ~AssetManagers() noexcept;

        auto CreateGpuAccessorSignals() noexcept -> GpuAccessorSignals;

    private:
        std::unique_ptr<AudioClipAssetManager> m_audioClipManager;
        std::unique_ptr<ConcaveColliderAssetManager> m_concaveColliderManager;
        std::unique_ptr<CubeMapAssetManager> m_cubeMapManager;
        std::unique_ptr<HullColliderAssetManager> m_hullColliderManager;
        std::unique_ptr<MeshAssetManager> m_meshManager;
        std::unique_ptr<TextureAssetManager> m_textureManager;
};
} // namespace nc
