/**
 * @file SceneSerialization.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/module/ModuleProvider.h"

#include <iosfwd>

namespace nc
{
/** @brief Initial value in a binary scene fragment blob. */
constexpr auto g_sceneFragmentMagicNumber = 0x3ff0e17b;

/** @brief Version number serialized with a scene fragment. */
constexpr auto g_currentSceneFragmentVersion = 3u;

/** @brief Header for a binary scene fragment blob. */
struct SceneFragmentHeader
{
    uint32_t magicNumber = g_sceneFragmentMagicNumber;
    uint32_t version = g_currentSceneFragmentVersion;
};

/** @brief A map of entities to their fragment ids. */
using EntityToFragmentIdMap = std::unordered_map<Entity, uint32_t, Entity::Hash>;

/** @brief A map of fragment ids to entities. */
using FragmentIdToEntityMap = std::unordered_map<uint32_t, Entity>;

/** @brief Context object passed to serialization functions. */
struct SerializationContext
{
    EntityToFragmentIdMap entityMap;
    ecs::Ecs ecs;
};

/** @brief Context object passed to deserialization functions. */
struct DeserializationContext
{
    FragmentIdToEntityMap entityMap;
    ecs::Ecs ecs;
};

/** @brief Save current game state to a binary stream. */
void SaveSceneFragment(std::ostream& stream,
                       ecs::Ecs ecs,
                       const asset::AssetMap& assets,
                       std::move_only_function<bool(Entity)> entityFilter = nullptr);

/** @brief Load game state from a binary stream. */
void LoadSceneFragment(std::istream& stream,
                       ecs::Ecs ecs,
                       ModuleProvider modules);
} // namespace nc
