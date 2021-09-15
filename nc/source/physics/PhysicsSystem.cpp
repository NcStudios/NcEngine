#include "PhysicsSystem.h"
#include "Physics.h"
#include "PhysicsConstants.h"
#include "collision/CollisionPhases.h"
#include "collision/CollisionNotification.h"
#include "config/Config.h"
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

    PhysicsSystem::PhysicsSystem(registry_type* registry, graphics::Graphics* graphics, job::JobSystem* jobSystem)
        : m_cache{},
          m_joints{},
          m_bspTree{registry},
          m_clickableSystem{graphics},
          m_jobSystem{jobSystem},
          m_tasks{}
          #ifdef NC_DEBUG_RENDERING
          , m_debugRenderer{graphics}
          #endif
    {
        g_physicsSystem = this;
        m_cache.fixedTimeStep = config::GetPhysicsSettings().fixedUpdateInterval;
        //BuildTaskGraph(registry);
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
        m_bspTree.Clear();
        m_clickableSystem.Clear();
        m_cache.previousPhysics.clear();
        m_cache.previousTrigger.clear();
        m_cache.manifolds.clear();
    }

    void PhysicsSystem::BuildTaskGraph(registry_type* registry)
    {
        auto& cache = m_cache;


        // @todo should maybe have task data type thing instead of just cache

        auto updateInertiaTask = m_tasks.emplace([registry]
        {
            UpdateWorldInertiaTensors(registry);
        });

        auto applyGravityTask = m_tasks.emplace(
            [registry,
             dt = cache.fixedTimeStep]
        {
            ApplyGravity(registry, dt);
        });

        auto updateManifoldsTask = m_tasks.emplace(
            [registry,
             &manifolds = cache.manifolds]
        {
            UpdateManifolds(registry, manifolds);
        });

        auto fetchEstimatesTask = m_tasks.emplace(
            [registry,
             &initData = cache.initData]
        {
            FetchEstimates(registry, &initData);
        });

        auto broadPhaseTask = m_tasks.emplace(
            [&estimates = std::as_const(cache.initData.estimates),
             &physicsCount = std::as_const(cache.previousBroadPhysicsCount),
             &triggerCount = std::as_const(cache.previousBroadTriggerCount),
             &broadResults = cache.broadResults]
        {
            FindBroadPairs(estimates, physicsCount, triggerCount, &broadResults);
        });

        auto narrowPhasePhysicsTask = m_tasks.emplace(
            [registry,
             &matrices = std::as_const(cache.initData.matrices),
             &broadPhysicsResults = std::as_const(cache.broadResults.physics),
             &physicsResults = cache.physicsResults]
        {
            FindNarrowPhysicsPairs(registry, matrices, broadPhysicsResults, &physicsResults);
        });

        auto narrowPhaseTriggerTask = m_tasks.emplace(
            [registry,
             &matrices = std::as_const(cache.initData.matrices),
             &broadTriggerResults = std::as_const(cache.broadResults.trigger),
             &currentTrigger = cache.currentTrigger]
        {
            FindNarrowTriggerPairs(registry, matrices, broadTriggerResults, &currentTrigger);
        });

        auto staticPhaseTask = m_tasks.emplace(
            [&bspTree = m_bspTree,
             &initData = std::as_const(cache.initData),
             &staticResults = cache.staticResults]
        {
            bspTree.CheckCollisions(initData.matrices, initData.estimates, &staticResults);
        });

        auto mergeContactsTask = m_tasks.emplace(
            [&physicsResults = std::as_const(cache.physicsResults),
             &staticResults = std::as_const(cache.staticResults),
             &manifolds = cache.manifolds]
        {
            MergeNewContacts(physicsResults, manifolds);
            MergeNewContacts(staticResults, manifolds);
        });

        auto generateConstraintsTask = m_tasks.emplace(
            [registry,
             &manifolds = std::as_const(cache.manifolds),
             &constraints = cache.constraints]
        {
            GenerateConstraints(registry, manifolds, &constraints);
        });

        auto updateJointsTask = m_tasks.emplace(
            [registry,
             &joints = m_joints,
             dt = cache.fixedTimeStep]
        {
            UpdateJoints(registry, joints, dt);
        });

        auto resolveConstraintsTask = m_tasks.emplace(
            [&constraints = cache.constraints,
             &joints = m_joints,
             dt = cache.fixedTimeStep]
        {
            ResolveConstraints(constraints, joints, dt);
        });


        auto cacheImpulsesTask = m_tasks.emplace(
            [&constraints = std::as_const(cache.constraints.contact),
            &manifolds = cache.manifolds]
        {
            if constexpr(EnableContactWarmstarting)
                CacheImpulses(constraints, manifolds);
        });

        auto integrateTask = m_tasks.emplace(
            [registry,
             dt = cache.fixedTimeStep]
        {
            Integrate(registry, dt);
        });

        auto notifyEventsTask = m_tasks.emplace(
            [registry,
             &physicsEvents = std::as_const(cache.physicsResults.events),
             &currentTrigger = std::as_const(cache.currentTrigger),
             &cache]
        {
            NotifyCollisionEvents(registry, physicsEvents, currentTrigger, &cache);
        });

        auto updateCacheTask = m_tasks.emplace([&cache]
        {
            UpdateCache(&cache);
        });

        broadPhaseTask.succeed(fetchEstimatesTask);
        broadPhaseTask.precede(narrowPhasePhysicsTask, narrowPhaseTriggerTask);
        staticPhaseTask.succeed(fetchEstimatesTask);
        mergeContactsTask.succeed(updateManifoldsTask, narrowPhasePhysicsTask, staticPhaseTask);
        generateConstraintsTask.succeed(mergeContactsTask, updateInertiaTask, applyGravityTask);
        updateJointsTask.succeed(applyGravityTask, updateInertiaTask);
        resolveConstraintsTask.succeed(generateConstraintsTask, updateJointsTask);
        cacheImpulsesTask.succeed(resolveConstraintsTask);
        integrateTask.succeed(resolveConstraintsTask);
        notifyEventsTask.succeed(integrateTask, narrowPhaseTriggerTask, cacheImpulsesTask);
        updateCacheTask.succeed(notifyEventsTask);

        // For generating visual task flow
        #if 1
        m_tasks.name("Physics Step");
        updateInertiaTask.name("Update Inertia");
        applyGravityTask.name("Apply Gravity");
        updateManifoldsTask.name("Update Manifolds");
        fetchEstimatesTask.name("Fetch Estimates");
        broadPhaseTask.name("Broad Phase");
        narrowPhasePhysicsTask.name("Narrow Phase - Physics");
        narrowPhaseTriggerTask.name("Narrow Phase - Trigger");
        staticPhaseTask.name("Static Phase (Bsp Tree)");
        mergeContactsTask.name("Merge Contacts");
        generateConstraintsTask.name("Generate Constraints");
        updateJointsTask.name("Update Joints");
        resolveConstraintsTask.name("Resolve Constraints");
        cacheImpulsesTask.name("Cache Impulses");
        integrateTask.name("Integrate");
        notifyEventsTask.name("Notify Events");
        updateCacheTask.name("Update Cache");
        m_tasks.dump(std::cout);
        #endif
    }

    void PhysicsSystem::DoPhysicsStep(float dt, tf::Executor& taskExecutor)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        #ifdef NC_DEBUG_RENDERING
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        #endif

        //auto result = taskExecutor.run(m_tasks);
        //result.wait();

        auto* registry = ActiveRegistry();

        UpdateWorldInertiaTensors(registry);
        ApplyGravity(registry, dt);
        UpdateManifolds(registry, m_cache.manifolds);

        FetchEstimates(registry, &m_cache.initData);

        FindBroadPairs(m_cache.initData.estimates, m_cache.previousBroadPhysicsCount, m_cache.previousBroadTriggerCount, &m_cache.broadResults);

        FindNarrowPhysicsPairs(registry, m_cache.initData.matrices, m_cache.broadResults.physics, &m_cache.physicsResults);

        FindNarrowTriggerPairs(registry, m_cache.initData.matrices, m_cache.broadResults.trigger, &m_cache.currentTrigger);

        m_bspTree.CheckCollisions(m_cache.initData.matrices, m_cache.initData.estimates, &m_cache.staticResults);

        MergeNewContacts(m_cache.physicsResults, m_cache.manifolds);
        MergeNewContacts(m_cache.staticResults, m_cache.manifolds);

        GenerateConstraints(registry, m_cache.manifolds, &m_cache.constraints);
        UpdateJoints(registry, m_joints, dt);
        ResolveConstraints(m_cache.constraints, m_joints, dt);

        if constexpr(EnableContactWarmstarting)
            CacheImpulses(m_cache.constraints.contact, m_cache.manifolds);

        Integrate(registry, dt);
        NotifyCollisionEvents(registry, m_cache.physicsResults.events, m_cache.currentTrigger, &m_cache);
        UpdateCache(&m_cache);

        NC_PROFILE_END();
    }
} // namespace nc::physics