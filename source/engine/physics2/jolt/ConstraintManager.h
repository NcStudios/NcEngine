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
    uint32_t ownerId = Entity::NullIndex; // Index of the Entity the constraint was added to
    uint32_t targetId = Entity::NullIndex; // Index of the other Entity targeted by the constraint (may be NullIndex if fixed to world)
};

// Per-Entity constraint information
struct EntityConstraints
{
    std::vector<uint32_t> ids; // ids of all constraints involving this entity (owned and targeted-by)
    std::vector<Constraint> constraints; // owned constraints
};

// Internal bodies referenced by a constraint
struct ConstraintBodies
{
    JPH::Body* body1 = nullptr;
    JPH::Body* body2 = nullptr;
};

class ConstraintManager
{
    public:
        static constexpr auto ConstraintMapSizeHint = 1000u;

        explicit ConstraintManager(JPH::PhysicsSystem& physicsSystem, uint32_t maxEntities)
            : m_physicsSystem{&physicsSystem},
              m_factory{physicsSystem},
              m_entityState{std::min(ConstraintMapSizeHint, maxEntities), maxEntities}
        {
            Constraint::s_manager = this;
        }

        auto AddConstraint(const ConstraintInfo& createInfo,
                           Entity owner,
                           JPH::Body& ownerBody,
                           Entity target,
                           JPH::Body& targetBody) -> Constraint&;

        auto AddConstraint(const ConstraintInfo& createInfo,
                           Entity owner,
                           JPH::Body& ownerBody) -> Constraint&;

        void EnableConstraint(Constraint& constraint, bool enabled);
        void UpdateConstraint(Constraint& constraint);
        void UpdateConstraintTarget(Constraint& constraint,
                                    Entity target,
                                    JPH::Body* targetBody);

        void RemoveConstraint(ConstraintId id);
        void RemoveConstraints(Entity toRemove);
        auto GetConstraintHandle(ConstraintId constraintId) -> JPH::Constraint*;
        auto GetConstraint(Entity owner, ConstraintId constraintId) -> Constraint&;
        auto GetConstraints(Entity owner) -> std::span<Constraint>;
        void Clear();

        auto BeginBatchAdd() -> size_t;
        void EndBatchAdd(size_t batchBegin);

    private:
        JPH::PhysicsSystem* m_physicsSystem;
        ConstraintFactory m_factory;
        std::vector<JPH::Constraint*> m_handles;
        std::vector<ConstraintPair> m_pairs;
        std::vector<uint32_t> m_freeIndices;
        sparse_map<EntityConstraints> m_entityState;
        bool m_isBatchInProgress = false;

        auto TrackConstraint(uint32_t entityId, ConstraintId constraintId) -> EntityConstraints&;
        void UntrackConstraint(uint32_t entityId, ConstraintId constraintId, bool isOwner);
        auto GetConstraintBodies(JPH::Constraint* handle) -> ConstraintBodies;
        void ReplaceInternalConstraint(const Constraint& replaceWith,
                                       JPH::Constraint* toReplace,
                                       JPH::Body* owner,
                                       JPH::Body* target);
};
} // namespace nc::physics
