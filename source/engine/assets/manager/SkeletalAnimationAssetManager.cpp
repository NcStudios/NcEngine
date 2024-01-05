#include "SkeletalAnimationAssetManager.h"
#include "AssetUtilities.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <algorithm>

namespace nc
{
SkeletalAnimationAssetManager::SkeletalAnimationAssetManager(const std::string& skeletalAnimationAssetDirectory, uint32_t maxSkeletalAnimations)
    : m_assetDirectory{skeletalAnimationAssetDirectory},
      m_maxSkeletalAnimationCount{maxSkeletalAnimations},
      m_assetIds{},
      m_skeletalAnimations{},
      m_onUpdate{}
{
}

bool SkeletalAnimationAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (m_assetIds.size() + 1 >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_assetIds.emplace_back(path);
    m_skeletalAnimations.emplace_back(asset::ImportSkeletalAnimation(fullPath));

    m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
        std::span<const std::string>{m_assetIds},
        std::span<const asset::SkeletalAnimation>{m_skeletalAnimations},
        asset::UpdateAction::Load
    });
    return true;
}

bool SkeletalAnimationAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    if (m_assetIds.size() + paths.size() >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    bool anyLoaded = false;

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_assetIds.emplace_back(path);
        m_skeletalAnimations.emplace_back(asset::ImportSkeletalAnimation(fullPath));
        anyLoaded = true;
    }

    if (anyLoaded)
    {
        m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
            std::span<const std::string>{m_assetIds},
            std::span<const asset::SkeletalAnimation>{m_skeletalAnimations},
            asset::UpdateAction::Load
        });
    }
    return anyLoaded;
}

bool SkeletalAnimationAssetManager::Unload(const std::string& path, asset_flags_type)
{
    const auto pos = std::ranges::find(m_assetIds, path);
    if (pos == std::ranges::cend(m_assetIds))
    {
        return false;
    }

    const auto offset = std::ranges::distance(std::ranges::cbegin(m_assetIds), pos);
    m_assetIds.erase(pos);
    m_skeletalAnimations.erase(std::ranges::cbegin(m_skeletalAnimations) + offset);

    m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
        std::span<const std::string>{m_assetIds},
        std::span<const asset::SkeletalAnimation>{m_skeletalAnimations},
        asset::UpdateAction::Unload
    });
    return true;
}

void SkeletalAnimationAssetManager::UnloadAll(asset_flags_type)
{
    m_skeletalAnimations.clear();
    m_assetIds.clear();
}

auto SkeletalAnimationAssetManager::Acquire(const std::string& path, asset_flags_type) const -> SkeletalAnimationView
{
    auto pos = std::find(m_assetIds.begin(), m_assetIds.end(), path);
    if (pos == m_assetIds.end())
    {
        throw NcError("Asset is not loaded: " + path);
    }

    return SkeletalAnimationView
    {
        .index = static_cast<uint32_t>(pos - m_assetIds.begin())
    };
}

bool SkeletalAnimationAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return std::find(m_assetIds.begin(), m_assetIds.end(), path) != m_assetIds.end();
}

auto SkeletalAnimationAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_assetIds);
}

auto SkeletalAnimationAssetManager::OnUpdate() -> Signal<const asset::SkeletalAnimationUpdateEventData&>&
{
    return m_onUpdate;
}
} // namespace nc
