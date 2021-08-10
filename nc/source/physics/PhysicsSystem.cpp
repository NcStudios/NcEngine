#include "PhysicsSystem.h"
#include "PhysicsConstants.h"
#include "collision/CollisionPhases.h"
#include "collision/CollisionNotification.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Solver.h"
#include "job/JobSystem.h"
#include "debug/Profiler.h"

namespace nc::physics
{
    #ifdef USE_VULKAN
    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics, job::JobSystem* jobSystem)
    #else
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics, job::JobSystem* jobSystem)
    #endif
        : m_cache{},
          m_clickableSystem{graphics},
          m_hullColliderManager{},
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
        m_cache.previousPhysics.clear();
        m_cache.previousTrigger.clear();
        m_cache.manifolds.clear();
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

        const auto [matrices, estimates] = FetchEstimates(registry);
        const auto broadEvents = FindBroadPairs(estimates);

        auto colliders = registry->ViewAll<Collider>();

        auto physicsResult = FindNarrowPhysicsPairs(colliders, matrices, broadEvents.physics);
        MergeNewContacts(physicsResult, m_cache.manifolds);
        auto triggerEvents = FindNarrowTriggerPairs(colliders, matrices, broadEvents.trigger);

        auto constraints = GenerateConstraints(registry, m_cache.manifolds);
        ResolveConstraints(constraints, dt);
        Integrate(registry, bodies, dt);

        NotifyCollisionEvents(registry, physicsResult.events, triggerEvents, &m_cache);

        m_cache.previousPhysics = std::move(physicsResult.events);
        m_cache.previousTrigger = std::move(triggerEvents);

        NC_PROFILE_END();
    }
} // namespace nc::physics