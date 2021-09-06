#include "PhysicsSystem.h"
#include "Physics.h"
#include "PhysicsConstants.h"
#include "collision/CollisionPhases.h"
#include "collision/CollisionNotification.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Solver.h"
#include "job/JobSystem.h"
#include "debug/Profiler.h"

namespace
{
    nc::physics::PhysicsSystem* g_physicsSystem = nullptr;
}

namespace nc::physics
{
    void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
    {
        g_physicsSystem->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
    }

    void RemoveJoint(Entity entityA, Entity entityB)
    {
        g_physicsSystem->RemoveJoint(entityA, entityB);
    }

    void RemoveAllJoints(Entity entity)
    {
        g_physicsSystem->RemoveAllJoints(entity);
    }

    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics, job::JobSystem* jobSystem)
        : m_cache{},
          m_joints{},
          m_clickableSystem{graphics},
          m_hullColliderManager{},
          m_jobSystem{jobSystem}
          #ifdef NC_DEBUG_RENDERING
          , m_debugRenderer{graphics}
          #endif
    {
        g_physicsSystem = this;
    }

    void PhysicsSystem::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
    {
        m_joints.emplace_back
        (
            entityA,
            entityB,
            nullptr,
            nullptr,
            DirectX::XMLoadVector3(&anchorA),
            DirectX::XMLoadVector3(&anchorB),
            DirectX::XMMATRIX{},
            DirectX::XMVECTOR{},
            DirectX::XMVECTOR{},
            DirectX::XMVECTOR{},
            DirectX::XMVECTOR{},
            bias,
            softness
        );
    }

    void PhysicsSystem::RemoveJoint(Entity entityA, Entity entityB)
    {
        for(auto& joint : m_joints)
        {
            if(joint.entityA == entityA && joint.entityB == entityB)
            {
                joint = m_joints.back();
                m_joints.pop_back();
                return;
            }
        }
    }

    void PhysicsSystem::RemoveAllJoints(Entity entity)
    {
        auto beg = m_joints.rbegin();
        auto end = m_joints.rend();
        for(auto cur = beg; cur != end; ++cur)
        {
            if(cur->entityA == entity || cur->entityB == entity)
            {
                *cur = m_joints.back();
                m_joints.pop_back();
            }
        }
    }

    #ifdef NC_DEBUG_RENDERING
    void PhysicsSystem::DebugRender()
    {
        m_debugRenderer.Render();
    }
    #endif

    void PhysicsSystem::ClearState()
    {
        m_joints.clear();
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
        const auto broadEvents = FindBroadPairs(estimates, m_cache.previousBroadPhysicsCount, m_cache.previousBroadTriggerCount);

        auto colliders = registry->ViewAll<Collider>();
        auto physicsResult = FindNarrowPhysicsPairs(colliders, matrices, broadEvents.physics);
        MergeNewContacts(physicsResult, m_cache.manifolds);
        auto triggerEvents = FindNarrowTriggerPairs(colliders, matrices, broadEvents.trigger);

        auto constraints = GenerateConstraints(registry, m_cache.manifolds);
        UpdateJoints(registry, m_joints, dt);
        ResolveConstraints(constraints, m_joints, dt);
        Integrate(registry, bodies, dt);

        NotifyCollisionEvents(registry, physicsResult.events, triggerEvents, &m_cache);

        m_cache.previousPhysics = std::move(physicsResult.events);
        m_cache.previousTrigger = std::move(triggerEvents);
        m_cache.previousBroadPhysicsCount = broadEvents.physics.size();
        m_cache.previousBroadTriggerCount = broadEvents.trigger.size();

        NC_PROFILE_END();
    }
} // namespace nc::physics