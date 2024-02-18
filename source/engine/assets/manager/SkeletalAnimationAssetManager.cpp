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
      m_onUpdate{}
{
}

bool SkeletalAnimationAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (m_table.size() + 1 >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    m_table.emplace(path);
    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    auto animation = asset::ImportSkeletalAnimation(fullPath);
    m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
        std::span<const std::string>{m_table.keys()},
        std::span<const asset::SkeletalAnimation>{&animation, 1},
        asset::UpdateAction::Load
    });
    return true;
}

bool SkeletalAnimationAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    if (m_table.size() + paths.size() >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    auto animations = std::vector<asset::SkeletalAnimation>{};
    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_table.emplace(path);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        animations.push_back(asset::ImportSkeletalAnimation(fullPath));
    }

    if (!animations.empty())
    {
        m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
            std::span<const std::string>{m_table.keys()},
            std::span<const asset::SkeletalAnimation>{animations},
            asset::UpdateAction::Load
        });

        return true;
    }

    return false;
}

bool SkeletalAnimationAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (!m_table.erase(path))
        return false;

    m_onUpdate.Emit(asset::SkeletalAnimationUpdateEventData{
        std::span<const std::string>{&path, 1},
        {},
        asset::UpdateAction::Unload
    });
    return true;
}

void SkeletalAnimationAssetManager::UnloadAll(asset_flags_type)
{
    m_table.clear();
}

auto SkeletalAnimationAssetManager::Acquire(const std::string& path, asset_flags_type) const -> SkeletalAnimationView
{
    const auto index = m_table.index(path);
    NC_ASSERT(index != StringTable::NullIndex, fmt::format("Asset is not loaded: {}", path));
    return SkeletalAnimationView{.index = static_cast<uint32_t>(index)};
}

bool SkeletalAnimationAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_table.contains(path);
}

auto SkeletalAnimationAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys());
}

auto SkeletalAnimationAssetManager::OnUpdate() -> Signal<const asset::SkeletalAnimationUpdateEventData&>&
{
    return m_onUpdate;
}
} // namespace nc
