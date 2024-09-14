#pragma once

#include "ConstraintFactory.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/utility/SparseMap.h"

#include <span>
#include <vector>

namespace JPH
{
class Constraint;
class PhysicsSystem;
} // namespace JPH

namespace nc::physics
{
// Pair of entity indices associated with a constraint
struct ConstraintPair
{
    uint32_t owningId = UINT32_MAX; // Index of the Entity the constraint was added to
    uint32_t referencedId = UINT32_MAX; // Index of the other Entity referenced by the constraint (may be Null if fixed to world)
};

// Per-Entity constraint information
struct EntityConstraints
{
    std::vector<uint32_t> ids; // ids of all constraints involving this entity (owned and referenced-by)
    std::vector<ConstraintView> views; // original ConstraintInfos for owned constraints
};

class ConstraintManager
{
    public:
        static constexpr auto ConstraintMapSizeHint = 1000u;

        explicit ConstraintManager(JPH::PhysicsSystem& physicsSystem, uint32_t maxEntities)
            : m_physicsSystem{&physicsSystem},
              m_factory{physicsSystem},
              m_entityState{ConstraintMapSizeHint, maxEntities}
        {
        }

        auto AddConstraint(const ConstraintInfo& createInfo,
                           Entity owner,
                           JPH::Body* ownerBody,
                           Entity referenced,
                           JPH::Body* referencedBody) -> ConstraintId;

        void RemoveConstraint(ConstraintId id);
        void RemoveConstraints(Entity toRemove);
        auto GetConstraints(Entity owner) const -> std::span<const ConstraintView>;
        void Clear();

    private:
        JPH::PhysicsSystem* m_physicsSystem;
        ConstraintFactory m_factory;
        std::vector<JPH::Constraint*> m_handles;
        std::vector<ConstraintPair> m_pairs;
        std::vector<uint32_t> m_freeIndices;
        sparse_map<EntityConstraints> m_entityState;
};
} // namespace nc::physics
