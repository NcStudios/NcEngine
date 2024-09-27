#pragma once

#include "ncengine/physics/Constraints.h"

#include <vector>

namespace nc::physics
{
struct DeferredConstraint
{
    Entity owner;
    Entity target;
    ConstraintInfo info;
};

struct DeferredPhysicsCreateState
{
    mutable std::vector<DeferredConstraint> constraints;
    size_t bodyBatchIndex = 0ull;
    size_t constraintBatchIndex = 0ull;
};
} // namespace nc::physics
