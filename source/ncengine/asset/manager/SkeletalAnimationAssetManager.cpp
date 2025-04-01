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

auto SkeletalAnimationAssetManager::Load(const std::string& path) -> bool
{
    auto previousTableSize = m_table.size();
    if (m_table.size() + 1 >= m_maxSkeletalAnimationCount)
    {
        throw NcError("Cannot exceed max skeletal animations count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    m_table.emplace(path);
    const auto fullPath = m_assetDirectory + path;
    auto animation = ImportSkeletalAnimation(fullPath);
    m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
        std::span<const std::string>{m_table.keys().begin() + previousTableSize, m_table.keys().end()},
        std::span<const SkeletalAnimation>{&animation, 1},
        UpdateAction::Load
    });
    return true;
}

auto SkeletalAnimationAssetManager::Load(std::span<const std::string> paths) -> bool
{
    auto previousTableSize = m_table.size();
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
        const auto fullPath = m_assetDirectory + path;
        animations.push_back(ImportSkeletalAnimation(fullPath));
    }

    if (!animations.empty())
    {
        m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
            std::span<const std::string>{m_table.keys().begin() + previousTableSize, m_table.keys().end()},
            std::span<const SkeletalAnimation>{animations},
            UpdateAction::Load
        });

        return true;
    }

    return false;
}

auto SkeletalAnimationAssetManager::Unload(const std::string& path) -> bool
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

void SkeletalAnimationAssetManager::UnloadAll()
{
    m_table.clear();
    m_onUpdate.Emit(SkeletalAnimationUpdateEventData{
        {},
        {},
        UpdateAction::UnloadAll
    });
}

auto SkeletalAnimationAssetManager::Acquire(const std::string& path) const -> SkeletalAnimationView
{
    NC_ASSERT(m_table.contains(path), fmt::format("SkeletalAnimation is not loaded: {}", path));
    return Acquire(m_table.hash(path));
}

auto SkeletalAnimationAssetManager::Acquire(AssetId id) const -> SkeletalAnimationView
{
    const auto index = m_table.index(id);
    NC_ASSERT(index != m_table.NullIndex, fmt::format("SkeletalAnimation is not loaded: {}", id));
    return SkeletalAnimationView{
        .id = id,
        .index = static_cast<uint32_t>(index)
    };
}

auto SkeletalAnimationAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys());
}
} // namespace nc::asset
