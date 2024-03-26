#include "SkeletalAnimationAssetManager.h"
#include "AssetUtilities.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <algorithm>

namespace nc::asset
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
    auto animation = ImportSkeletalAnimation(fullPath);
    m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
        std::span<const std::string>{m_table.keys()},
        std::span<const SkeletalAnimation>{&animation, 1},
        UpdateAction::Load
    });
    return true;
}

bool SkeletalAnimationAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    if (m_table.size() + paths.size() >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    auto animations = std::vector<SkeletalAnimation>{};
    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_table.emplace(path);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        animations.push_back(ImportSkeletalAnimation(fullPath));
    }

    if (!animations.empty())
    {
        m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
            std::span<const std::string>{m_table.keys()},
            std::span<const SkeletalAnimation>{animations},
            UpdateAction::Load
        });

        return true;
    }

    return false;
}

bool SkeletalAnimationAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (!m_table.erase(path))
        return false;

    m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
        std::span<const std::string>{&path, 1},
        {},
        UpdateAction::Unload
    });
    return true;
}

void SkeletalAnimationAssetManager::UnloadAll(asset_flags_type)
{
    m_table.clear();
}

auto SkeletalAnimationAssetManager::Acquire(const std::string& path, asset_flags_type) const -> SkeletalAnimationView
{
    const auto hash = m_table.hash(path);
    const auto index = m_table.index(hash);
    NC_ASSERT(index != m_table.NullIndex, fmt::format("Asset is not loaded: {}", path));
    return SkeletalAnimationView
    {
        .id = hash,
        .index = static_cast<uint32_t>(index)
    };
}

bool SkeletalAnimationAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_table.contains(path);
}

auto SkeletalAnimationAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys());
}

auto SkeletalAnimationAssetManager::OnUpdate() -> Signal<const SkeletalAnimationUpdateEventData&>&
{
    return m_onUpdate;
}
} // namespace nc::asset
