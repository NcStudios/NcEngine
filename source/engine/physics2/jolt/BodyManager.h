#pragma once

#include "BodyFactory.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/physics/RigidBody.h"
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
struct ComponentContext;
class ConstraintManager;
class ShapeFactory;

class BodyManager : public StableAddress
{
    public:
        static constexpr auto BodyMapSizeHint = 1000u;

        BodyManager(ecs::ComponentPool<Transform>& transformPool,
                    ecs::ComponentPool<RigidBody>& rigidBodyPool,
                    uint32_t maxEntities,
                    JPH::PhysicsSystem& physicsSystem,
                    ShapeFactory& shapeFactory,
                    ConstraintManager& constraintManager);

        ~BodyManager() noexcept;

        void AddBody(RigidBody& added);
        void RemoveBody(Entity entity);
        void Clear();

        auto BeginBatchAdd() -> size_t;
        void EndBatchAdd(size_t batchBegin);
        void DeferCleanup(bool value)
        {
            m_deferCleanup = value;
        }

    private:
        struct Connections;

        ecs::ComponentPool<Transform>* m_transformPool;
        sparse_map<JPH::BodyID> m_bodies;
        BodyFactory m_bodyFactory;
        ConstraintManager* m_constraintManager;
        std::unique_ptr<ComponentContext> m_ctx;
        std::unique_ptr<Connections> m_connections;
        bool m_deferInitialization = false;
        bool m_deferCleanup = false;
};
} // namespace nc::physics
