#pragma once

#include "module/Module.h"
#include "utility/Signal.h"

namespace nc
{
struct CubeMapBufferData;
struct MeshBufferData;
struct TextureBufferData;

namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config
}

namespace nc::asset
{
/** @brief Asset module interface. */
class NcAsset : public Module
{
    public:
        /** @brief Get the signal for CubeMap load and unload events. */
        virtual auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapBufferData&>& = 0;

        /** @brief Get the signal for Texture load and unload events. */
        virtual auto OnTextureUpdate() noexcept -> Signal<const TextureBufferData&>& = 0;

        /** @brief Get the signal for Mesh load and unload events. */
        virtual auto OnMeshUpdate() noexcept -> Signal<const MeshBufferData&>& = 0;
};

auto BuildAssetModule(const config::AssetSettings& assetSettings,
                      const config::MemorySettings& memorySettings) -> std::unique_ptr<NcAsset>;
} // namespace nc::asset
