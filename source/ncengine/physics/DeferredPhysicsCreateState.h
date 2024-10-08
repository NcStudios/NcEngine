#pragma once

#include "ncengine/physics/Constraints.h"

#include <limits>
#include <vector>

namespace nc::physics
{
// A deserialized constraint that has not yet been added to the simulation
struct DeferredConstraint
{
    Entity owner;
    Entity target;
    ConstraintInfo info;
};

// State for in progress batch/deserialization operations on bodies and constraints
struct DeferredPhysicsCreateState
{
    static constexpr auto NullBatch = std::numeric_limits<size_t>::max();

    std::vector<DeferredConstraint> constraints; // constraints waiting to be added to the simulation
    size_t bodyBatchIndex = NullBatch;           // index of the first body of a batch add operation
    size_t constraintBatchIndex = NullBatch;     // index of the first constraint of a batch add operation
};
} // namespace nc::physics
