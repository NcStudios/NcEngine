#include "PhysicsSystem.h"

namespace nc::physics
{
    #ifdef USE_VULKAN
    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #else
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #endif
        : m_collisionSystem{colliderSystem, jobSystem},
          m_clickableSystem{graphics}
    {
    }

    void PhysicsSystem::ClearState()
    {
        m_clickableSystem.Clear();
        m_collisionSystem.ClearState();
    }

    void PhysicsSystem::DoPhysicsStep()
    {
        m_collisionSystem.DoCollisionStep();
    }

    #ifdef NC_EDITOR_ENABLED
    void PhysicsSystem::UpdateWidgets(graphics::FrameManager* frameManager)
    {
        m_collisionSystem.UpdateWidgets(frameManager);
    }
    #endif
} // namespace nc::physics