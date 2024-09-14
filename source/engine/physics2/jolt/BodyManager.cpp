#include "BodyManager.h"
#include "Conversion.h"
#include "ComponentContext.h"
#include "ConstraintManager.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/utility/Signal.h"

#include "Jolt/Physics/PhysicsSystem.h"

namespace nc::physics
{
struct BodyManager::Connections
{
    static auto Connect(BodyManager* manager, ecs::ComponentPool<RigidBody>& pool)
    {
        return std::make_unique<Connections>(
            pool.OnAdd().Connect(manager, &BodyManager::AddBody)
            // pool.OnRemove().Connect(manager, &BodyManager::RemoveBody)
        );
    }

    Connection<RigidBody&> addRigidBodyConnection;
    // Connection<Entity> removeRigidBodyConnection;
};

BodyManager::BodyManager(ecs::Ecs world,
                         uint32_t maxEntities,
                         JPH::PhysicsSystem& physicsSystem,
                         ShapeFactory& shapeFactory,
                         ConstraintManager& constraintManager)
    : m_world{world},
      m_bodies{1000, maxEntities}, // todo figure this out
      m_bodyFactory{physicsSystem.GetBodyInterfaceNoLock(), shapeFactory},
      m_constraintManager{&constraintManager},
      m_ctx{std::make_unique<ComponentContext>(
          physicsSystem.GetBodyInterfaceNoLock(),
          shapeFactory,
          constraintManager,
          *this
      )},
      m_connections{Connections::Connect(this, world.GetPool<RigidBody>())}
{
}

BodyManager::~BodyManager() noexcept = default;

void BodyManager::AddBody(RigidBody& added)
{
    auto& transform = m_world.Get<Transform>(added.GetEntity());
    auto [handle, adjustedScale, wasScaleAdjusted] = m_bodyFactory.MakeBody(added, transform.TransformationMatrix());
    if (wasScaleAdjusted)
    {
        transform.SetScale(adjustedScale);
    }

    m_ctx->interface.AddBody(handle->GetID(), JPH::EActivation::Activate);
    added.SetContext(handle, m_ctx.get());
    m_bodies.emplace(added.GetEntity().Index(), handle->GetID());
}

void BodyManager::RemoveBody(Entity toRemove)
{
    if (m_deferCleanup)
    {
        return;
    }

    const auto bodyId = m_bodies.at(toRemove.Index());
    m_bodies.erase(toRemove.Index());
    m_constraintManager->RemoveConstraints(toRemove);
    m_ctx->interface.RemoveBody(bodyId);
    m_ctx->interface.DestroyBody(bodyId);
}

void BodyManager::Clear()
{
    auto ids = m_bodies.values();
    const auto size = static_cast<int>(ids.size());
    if (size == 0)
    {
        return; // Jolt asserts size != 0 instead of just returning...
    }

    m_ctx->interface.RemoveBodies(ids.data(), size);
    m_ctx->interface.DestroyBodies(ids.data(), size);
    m_bodies.clear();
}
} // namespace nc::physics
