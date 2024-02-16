/**
 * @file Hierarchy.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Entity.h"

#include <ranges>
#include <vector>

namespace nc
{
/**
 * @brief Component managing an Entity's scene graph relationships.
 * @note Ecs::SetParent() offers a simpler method for updating Hierarchies compared to direct component modification.
 * 
 * A Hierarchy component is automatically attached to an Entity on creation, with its initial parent value taken from
 * the EntityInfo struct. Root Entities have their parent set to Entity::Null(). A Hierarchy may be directly modified,
 * but care must be taken to ensure related Hierarchy objects are updated accordingly. Ecs::SetParent() automatically
 * handles this and should be preferred over direct modifications.
*/
struct Hierarchy
{
    Entity parent;
    std::vector<Entity> children;
};
} // namespace nc
