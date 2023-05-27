#pragma once

#include "module/Module.h"
#include "utility/Signal.h"

namespace nc
{
namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config

namespace asset
{
struct CubeMapUpdateEventData;
struct MeshUpdateEventData;
struct NormalMapUpdateEventData;
struct TextureUpdateEventData;

/** @brief Asset module interface. */
class NcAsset : public Module
{
    public:
        /** @brief Get the signal for CubeMap load and unload events. */
        virtual auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& = 0;

        /** @brief Get the signal for Mesh load and unload events. */
        virtual auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&>& = 0;

        /** @brief Get the signal for NormalMap load and unload events. */
        virtual auto OnNormalMapUpdate() noexcept -> Signal<const NormalMapUpdateEventData&> & = 0;

        /** @brief Get the signal for Texture load and unload events. */
        virtual auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&> & = 0;
};

auto BuildAssetModule(const config::AssetSettings& assetSettings,
                      const config::MemorySettings& memorySettings) -> std::unique_ptr<NcAsset>;
} // namespace asset
} // namespace nc
