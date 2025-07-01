#include "ShapeKeyAnimationAssetManager.h"

#include "AssetUtilities.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <algorithm>

namespace nc::asset
{
ShapeKeyAnimationAssetManager::ShapeKeyAnimationAssetManager(const std::string& shapeKeyAnimationAssetDirectory, uint32_t maxShapeKeyAnimations)
    : m_assetDirectory{shapeKeyAnimationAssetDirectory},
      m_maxShapeKeyAnimationCount{maxShapeKeyAnimations},
      m_onUpdate{}
{
}

auto ShapeKeyAnimationAssetManager::Load(const std::string& path) -> bool
{
    if (m_table.size() + 1 >= m_maxShapeKeyAnimationCount)
    {
        throw NcError("Cannot exceed max shape key animations count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    m_table.emplace(path);
    const auto fullPath = m_assetDirectory + path;
    auto animation = ImportShapeKeyAnimation(fullPath);
    auto animationAsTextureWithId = asset::TextureWithId<float>{std::move(animation.animation), m_table.hash(path)};
    auto animationWithId = asset::ShapeKeyAnimationWithId{std::move(animationAsTextureWithId), animation.durationInSeconds, animation.numShapeKeys, m_table.size()-1};

    m_onUpdate.Emit(ShapeKeyAnimationUpdateEventData{
        UpdateAction::Load,
        std::span<const ShapeKeyAnimationWithId>{&animationWithId, 1},
    });
    return true;
}

auto ShapeKeyAnimationAssetManager::Load(std::span<const std::string> paths) -> bool
{
    if (m_table.size() + paths.size() >= m_maxShapeKeyAnimationCount)
    {
        throw NcError("Cannot exceed max shape key animations count.");
    }

    auto animations = std::vector<ShapeKeyAnimationWithId>{};
    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_table.emplace(path);
        const auto fullPath = m_assetDirectory + path;
        auto animation = ImportShapeKeyAnimation(fullPath);
        auto animationAsTextureWithId = asset::TextureWithId<float>{std::move(animation.animation), m_table.hash(path)};
        animations.emplace_back(std::move(animationAsTextureWithId), animation.durationInSeconds, animation.numShapeKeys, m_table.size()-1);
    }

    if (!animations.empty())
    {
        m_onUpdate.Emit(ShapeKeyAnimationUpdateEventData{
            UpdateAction::Load,
            std::span<const ShapeKeyAnimationWithId>{animations}
        });

        return true;
    }

    return false;
}

auto ShapeKeyAnimationAssetManager::Unload(const std::string& path) -> bool
{
    if (!m_table.erase(path))
        return false;

    m_onUpdate.Emit(ShapeKeyAnimationUpdateEventData{
        UpdateAction::Unload,
        {},
    });
    return true;
}

void ShapeKeyAnimationAssetManager::UnloadAll()
{
    m_table.clear();
    m_onUpdate.Emit(ShapeKeyAnimationUpdateEventData{
        UpdateAction::UnloadAll,
        {}
    });
}

auto ShapeKeyAnimationAssetManager::Acquire(const std::string& path) const -> ShapeKeyAnimationView
{
    NC_ASSERT(m_table.contains(path), fmt::format("ShapeKeyAnimation is not loaded: {}", path));
    return Acquire(m_table.hash(path));
}

auto ShapeKeyAnimationAssetManager::Acquire(AssetId id) const -> ShapeKeyAnimationView
{
    const auto index = m_table.index(id);
    NC_ASSERT(index != m_table.NullIndex, fmt::format("ShapeKeyAnimation is not loaded: {}", id));
    return ShapeKeyAnimationView{
        .id = id,
        .index = static_cast<uint32_t>(index)
    };
}

auto ShapeKeyAnimationAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys());
}
} // namespace nc::asset
