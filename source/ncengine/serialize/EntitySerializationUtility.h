#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/serialize/SceneSerialization.h"

#include <functional>
#include <span>
#include <unordered_map>
#include <vector>

namespace nc
{
// A pair containing an entity's local fragment id and EntityInfo.
struct FragmentEntityInfo
{
    uint32_t fragmentId;
    EntityInfo info;
};

// Build a list of all entities to include in a scene fragment. All parents are guaranteed to precede their children.
auto BuildFragmentEntityList(std::span<const Entity> in,
                             std::move_only_function<bool(Entity)>& filter,
                             ecs::ExplicitEcs<Hierarchy> ecs) -> std::vector<Entity>;

// Build a map of Entity to fragment id.
auto BuildEntityToFragmentIdMap(std::span<const Entity> entities) -> EntityToFragmentIdMap;

// Sets an Entity's index to its fragment id. This invalidates the entity for use in the registry.
void RemapEntity(Entity& entity, const EntityToFragmentIdMap& map);

// Sets a deserialized Entity (containing a fragment id) to its actual value in the registry.
void RemapEntity(Entity& entity, const FragmentIdToEntityMap& map);

// Build a list of pairs of entity fragment id and reconstructed EntityInfo.
auto BuildFragmentEntityInfos(std::span<const Entity> entities,
                              ecs::ExplicitEcs<Hierarchy, Transform, Tag> ecs,
                              const EntityToFragmentIdMap& entityMap) -> std::vector<FragmentEntityInfo>;
} // namespace nc
