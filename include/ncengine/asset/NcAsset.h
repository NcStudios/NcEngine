/**
 * @file NcAsset.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/module/Module.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetType.h"

namespace nc
{
namespace config
{
struct AssetSettings;
struct MemorySettings;
} // namespace config

namespace asset
{
struct BoneUpdateEventData;
struct CubeMapUpdateEventData;
struct MeshUpdateEventData;
struct SkeletalAnimationUpdateEventData;
struct TextureUpdateEventData;

/** @brief A map of AssetType to a list of asset paths of that type. */
using AssetMap = std::unordered_map<asset::AssetType, std::vector<std::string>>;

/** @brief Asset module interface. */
class NcAsset : public Module
{
    public:
        /** @brief Get the signal for BonesData load and unload events. */
        virtual auto OnBoneUpdate() noexcept -> Signal<const BoneUpdateEventData&>& = 0;

        /** @brief Get the signal for CubeMap load and unload events. */
        virtual auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& = 0;

        /** @brief Get the signal for Mesh load and unload events. */
        virtual auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&>& = 0;

        /** @brief Get the signal for SkeletalAnimation load and unload events. */
        virtual auto OnSkeletalAnimationUpdate() noexcept -> Signal<const SkeletalAnimationUpdateEventData&>& = 0;

        /** @brief Get the signal for Texture load and unload events. */
        virtual auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>& = 0;

        /** @brief Get the signal for Font load and unload events. */
        virtual auto OnFontUpdate() noexcept -> Signal<>& = 0;

        /** @brief Load assets from an AssetMap. */
        virtual void LoadAssets(const AssetMap& assets) = 0;

        /** @brief Get the names of all loaded assets as an AssetMap. */
        virtual auto GetLoadedAssets() const noexcept -> AssetMap = 0;
};

/**
 * @brief Build an NcAsset instance.
 * @param assetSettings Settings controlling asset search locations.
 * @param memorySettings Settings controlling memory limits.
 * @param defaults A collection of assets to be available by default.
 * @return An NcAsset instance.
 */
auto BuildAssetModule(const config::AssetSettings& assetSettings,
                      const config::MemorySettings& memorySettings,
                      AssetMap defaults) -> std::unique_ptr<NcAsset>;
} // namespace asset
} // namespace nc
