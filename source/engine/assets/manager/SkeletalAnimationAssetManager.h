#pragma once

#include "assets/AssetService.h"
#include "ncasset/AssetsFwd.h"
#include "utility/Signal.h"

#include <vector>

namespace nc
{
namespace asset
{
struct SkeletalAnimationUpdateEventData;
} // namespace asset

class SkeletalAnimationAssetManager : public IAssetService<SkeletalAnimationView, std::string>
{
    public:
        explicit SkeletalAnimationAssetManager(const std::string& skeletalAnimationAssetDirectory, uint32_t maxSkeletalAnimations);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> SkeletalAnimationView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto OnUpdate() -> Signal<const asset::SkeletalAnimationUpdateEventData&>&;

    private:
        std::string m_assetDirectory;
        uint32_t m_maxSkeletalAnimationCount;
        std::vector<std::string> m_assetIds;
        std::vector<asset::SkeletalAnimation> m_skeletalAnimations;
        Signal<const asset::SkeletalAnimationUpdateEventData&> m_onUpdate;
};
} // namespace nc