#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"

#include <unordered_map>
#include <vector>

namespace nc
{
/** @brief Initial value in a binary SceneFragment blob. */
constexpr auto g_sceneFragmentMagicNumber = 0x3ff0e17b;

/** @brief Version number serialized with a SceneFragment. */
constexpr auto g_currentSceneFragmentVersion = 1u;

/** @brief A map of entities to their fragment ids. */
using EntityToFragmentIdMap = std::unordered_map<Entity, uint32_t, Entity::Hash>;

/** @brief A map of fragment ids to entities. */
using FragmentIdToEntityMap = std::unordered_map<uint32_t, Entity>;

/** @brief A pair containing an entity's local fragment id and EntityInfo. */
struct FragmentEntityInfo
{
    uint32_t fragmentId;
    EntityInfo info;
};

/** @brief Header for a serialized SceneFragment. */
struct SceneFragmentHeader
{
    uint32_t magicNumber = g_sceneFragmentMagicNumber;
    uint32_t version = g_currentSceneFragmentVersion;
};

/** @brief A collection of game state that may be overlayed on top of an existing scene.
 *         Intended for prefabs and serialized scene data.*/
struct SceneFragment
{
    asset::AssetMap assets;
    std::vector<FragmentEntityInfo> entities;
};

/** @brief Save current game state to a SceneFragment. */
auto SaveSceneFragment(ecs::Ecs ecs,
                       const asset::NcAsset& assetModule,
                       std::function<bool(Entity)> entityFilter = nullptr) -> SceneFragment;

/** @brief Load game state from a SceneFragment. */
void LoadSceneFragment(SceneFragment& fragment,
                       ecs::Ecs ecs,
                       asset::NcAsset& assetModule);
} // namespace nc
