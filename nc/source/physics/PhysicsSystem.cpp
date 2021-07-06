#include "PhysicsSystem.h"
#include "PhysicsConstants.h"
#include "dynamics/Solver.h"

namespace nc::physics
{
    void UpdateWorldInertiaTensors(registry_type* registry, std::span<PhysicsBody> bodies);
    void ApplyGravity(std::span<PhysicsBody> bodies, float dt);
    void Integrate(registry_type* registry, std::span<PhysicsBody> bodies, float dt);

    #ifdef USE_VULKAN
    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #else
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #endif
        : m_collisionSystem{colliderSystem, jobSystem},
          m_clickableSystem{graphics}
          #ifdef NC_DEBUG_RENDERING
          , m_debugRenderer{graphics}
          #endif
    {
    }

    #ifdef NC_DEBUG_RENDERING
    void PhysicsSystem::DebugRender()
    {
        m_debugRenderer.Render();
    }
    #endif

    void PhysicsSystem::ClearState()
    {
        m_clickableSystem.Clear();
        m_collisionSystem.ClearState();
    }

    void PhysicsSystem::DoPhysicsStep(float dt)
    {
        /** @todo Is it a problem that this is happening? */
        if(dt == 0.0f)
            return;

        #ifdef NC_DEBUG_RENDERING
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        #endif

        auto* registry = ActiveRegistry();
        auto bodies = registry->ViewAll<PhysicsBody>();

        UpdateWorldInertiaTensors(registry, bodies);
        ApplyGravity(bodies, dt);

        const auto& manifolds = m_collisionSystem.DoCollisionStep();
        Constraints constraints;
        GenerateConstraints(registry, manifolds, &constraints);

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

        Integrate(registry, bodies, dt);

        m_collisionSystem.NotifyCollisionEvents();
        m_collisionSystem.Cleanup();
    }

    void UpdateWorldInertiaTensors(registry_type* registry, std::span<PhysicsBody> bodies)
    {
        for(auto& body : bodies)
        {
            auto* transform = registry->Get<Transform>(body.GetParentEntity());
            /** @todo Can this be skipped for static bodies? */
            body.UpdateWorldInertia(transform);
        }
    }

    void ApplyGravity(std::span<PhysicsBody> bodies, float dt)
    {
        auto g = GravityAcceleration * dt;

        for(auto& body : bodies)
        {
            auto& properties = body.GetProperties();

            if(properties.useGravity)
            {
                properties.velocity += g;
            }
        }
    }

    void Integrate(registry_type* registry, std::span<PhysicsBody> bodies, float dt)
    {
        for(auto& body : bodies)
        {
            Entity entity = body.GetParentEntity();

            if(EntityUtils::IsStatic(entity))
                continue;
            
            auto& properties = body.GetProperties();
            auto* transform = registry->Get<Transform>(entity);

            properties.velocity = HadamardProduct(properties.linearFreedom, properties.velocity);
            properties.angularVelocity = HadamardProduct(properties.angularFreedom, properties.angularVelocity);

            transform->Translate(properties.velocity * dt);
            transform->Rotate(Quaternion::FromEulerAngles(properties.angularVelocity * dt));

            properties.velocity *= pow(1.0f - properties.drag, dt);
            properties.angularVelocity *= pow(1.0f - properties.angularDrag, dt);
        }
    }
} // namespace nc::physics