#pragma once

#include "ncengine/ecs/Entity.h"

#include <ranges>
#include <vector>

namespace nc
{
/** @brief */
struct Hierarchy
{
    auto IsRoot() const noexcept
    {
        return !parent.Valid();
    }

    Entity parent;
    std::vector<Entity> children;
};
} // namespace nc
