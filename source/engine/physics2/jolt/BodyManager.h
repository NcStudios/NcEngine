#pragma once

#include "BodyFactory.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/utility/SparseMap.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"

#include <memory>
#include <span>

namespace JPH
{
class PhysicsSystem;
} // namespace JPH

namespace nc::physics
{
class RigidBody;
struct ComponentContext;
class ConstraintManager;
class ShapeFactory;

class BodyManager : public StableAddress
{
    public:
        BodyManager(ecs::Ecs world,
                    uint32_t maxEntities,
                    JPH::PhysicsSystem& physicsSystem,
                    ShapeFactory& shapeFactory,
                    ConstraintManager& constraintManager);

        ~BodyManager() noexcept;

        void AddBody(RigidBody& added);
        void RemoveBody(Entity entity);
        void Clear();
        void DeferCleanup(bool value)
        {
            m_deferCleanup = value;
        }

    private:
        using set_t = ecs::detail::SparseSet<JPH::BodyID>;
        struct Connections;

        ecs::Ecs m_world;
        sparse_map<JPH::BodyID> m_bodies;
        BodyFactory m_bodyFactory;
        ConstraintManager* m_constraintManager;
        std::unique_ptr<ComponentContext> m_ctx;
        std::unique_ptr<Connections> m_connections;
        bool m_deferCleanup = false;
};
} // namespace nc::physics
