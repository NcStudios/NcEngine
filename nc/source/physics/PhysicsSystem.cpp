#include "PhysicsSystem.h"
#include "PhysicsConstants.h"
#include "collision/CollisionPhases.h"
#include "collision/CollisionNotification.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Solver.h"
#include "ecs/ColliderSystem.h"
#include "job/JobSystem.h"
#include "debug/Profiler.h"

namespace nc::physics
{
    #ifdef USE_VULKAN
    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #else
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
    #endif
        : m_clickableSystem{graphics},
          m_cache{},
          m_colliderSystem{colliderSystem},
          m_jobSystem{jobSystem}
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
        Clear(&m_cache);
    }

    void PhysicsSystem::DoPhysicsStep(float dt)
    {
        /** @todo Is it a problem that this is happening? */
        if(dt == 0.0f)
            return;

        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        #ifdef NC_DEBUG_RENDERING
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        #endif

        /** @todo Can use JobSystem here. Also, JobSystem seems to have a bug related
         *  to arguments. (Works if all args are captured in a lambda, but not if
         *  args are passed to func and bound in JobSystem.) */

        auto* registry = ActiveRegistry();
        auto bodies = registry->ViewAll<PhysicsBody>();

        UpdateWorldInertiaTensors(registry, bodies);
        ApplyGravity(bodies, dt);
        UpdateManifolds(registry, m_cache.manifolds);

        const auto* tree = m_colliderSystem->GetStaticTree();
        const auto* soa = m_colliderSystem->GetDynamicSoA();
        FetchEstimates(registry, m_colliderSystem->GetDynamicSoA(), m_cache.estimates);
        FindBroadPairs(m_cache.estimates, m_cache.broad.physics, m_cache.broad.trigger);
        FindBroadStaticPairs(m_cache.estimates, tree, m_cache.broad.staticPhysics, m_cache.broad.staticTrigger);
        FindNarrowPhysicsPairs(m_cache.broad.physics, m_cache.broad.staticPhysics, soa, m_cache.narrow.physics, m_cache.manifolds);
        FindNarrowTriggerPairs(m_cache.broad.trigger, m_cache.broad.staticTrigger, soa, m_cache.narrow.trigger);

        Constraints constraints;
        GenerateConstraints(registry, m_cache.manifolds, &constraints);
        ResolveConstraints(&constraints, dt);
        Integrate(registry, bodies, dt);

        NotifyCollisionEvents(registry, &m_cache);
        UpdatePreviousEvents(&m_cache);

        NC_PROFILE_END();
    }
} // namespace nc::physics