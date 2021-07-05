#include "PhysicsSystem.h"
#include "PhysicsConstants.h"
#include "dynamics/Solver.h"

namespace nc::physics
{
    void UpdateWorldInertiaTensors();

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

    void PhysicsSystem::DoPhysicsStep(float dt)
    {
        // how is this even happening?
        if(dt == 0.0f)
            return;

        UpdateWorldInertiaTensors();
        ApplyGravity(dt);

        const auto& manifolds = m_collisionSystem.DoCollisionStep();
        Constraints constraints;
        GenerateConstraints(manifolds, &constraints);

        for(size_t i = 0u; i < SolverIterations; ++i)
        {
            for(auto& c : constraints.contact)
            {
                ResolveConstraint(c, dt);
            }
        }

        for(const auto& c : constraints.basic)
        {
            ResolveConstraint(c);
        }

        Integrate(dt);

        m_collisionSystem.NotifyCollisionEvents();
        m_collisionSystem.Cleanup();
    }

    void UpdateWorldInertiaTensors()
    {
        auto* registry = ActiveRegistry();

        for(auto& body : registry->ViewAll<PhysicsBody>())
        {
            auto* transform = registry->Get<Transform>(body.GetParentEntity());
            /** @todo Can this be skipped for static bodies? */
            body.UpdateWorldInertia(transform);
        }
    }
} // namespace nc::physics