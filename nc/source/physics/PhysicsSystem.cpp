#include "PhysicsSystem.h"
#include "Physics.h"
#include "PhysicsConstants.h"
#include "collision/CollisionPhases.h"
#include "collision/CollisionNotification.h"
#include "config/Config.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Solver.h"
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

    PhysicsSystem::PhysicsSystem(registry_type* registry, graphics::Graphics* graphics)
        : m_cache{},
          m_joints{},
          m_bspTree{registry},
          m_clickableSystem{graphics},
          m_tasks{}
          #ifdef NC_DEBUG_RENDERING
          , m_debugRenderer{graphics}
          #endif
    {
        g_physicsSystem = this;
        m_cache.fixedTimeStep = config::GetPhysicsSettings().fixedUpdateInterval;
        BuildTaskGraph(registry);
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

        auto fixedUpdateTask = m_tasks.AddGuardedTask([registry]
        {
            for(auto& group : registry->ViewAll<AutoComponentGroup>())
                group.SendFixedUpdate();
        });

        auto updateInertiaTask = m_tasks.AddGuardedTask([registry]
        {
            UpdateWorldInertiaTensors(registry);
        });

        auto applyGravityTask = m_tasks.AddGuardedTask([registry, dt = cache.fixedTimeStep]
        {
            ApplyGravity(registry, dt);
        });

        auto updateManifoldsTask = m_tasks.AddGuardedTask([registry, &manifolds = cache.manifolds]
        {
            UpdateManifolds(registry, manifolds);
        });

        auto fetchEstimatesTask = m_tasks.AddGuardedTask([registry, &initData = cache.initData]
        {
            FetchEstimates(registry, &initData);
        });

        auto broadPhaseTask = m_tasks.AddGuardedTask(
            [&estimates = std::as_const(cache.initData.estimates),
             &physicsCount = std::as_const(cache.previousBroadPhysicsCount),
             &triggerCount = std::as_const(cache.previousBroadTriggerCount),
             &broadResults = cache.broadResults]
        {
            FindBroadPairs(estimates, physicsCount, triggerCount, &broadResults);
        });

        auto narrowPhasePhysicsTask = m_tasks.AddGuardedTask(
            [registry,
             &matrices = std::as_const(cache.initData.matrices),
             &broadPhysicsResults = std::as_const(cache.broadResults.physics),
             &physicsResults = cache.physicsResults]
        {
            FindNarrowPhysicsPairs(registry, matrices, broadPhysicsResults, &physicsResults);
        });

        auto narrowPhaseTriggerTask = m_tasks.AddGuardedTask(
            [registry,
             &matrices = std::as_const(cache.initData.matrices),
             &broadTriggerResults = std::as_const(cache.broadResults.trigger),
             &currentTrigger = cache.currentTrigger]
        {
            FindNarrowTriggerPairs(registry, matrices, broadTriggerResults, &currentTrigger);
        });

        auto staticPhaseTask = m_tasks.AddGuardedTask(
            [&bspTree = m_bspTree,
             &initData = std::as_const(cache.initData),
             &staticResults = cache.staticResults]
        {
            bspTree.CheckCollisions(initData.matrices, initData.estimates, &staticResults);
        });

        auto mergeContactsTask = m_tasks.AddGuardedTask(
            [&physicsResults = std::as_const(cache.physicsResults),
             &staticResults = std::as_const(cache.staticResults),
             &manifolds = cache.manifolds]
        {
            MergeNewContacts(physicsResults, manifolds);
            MergeNewContacts(staticResults, manifolds);
        });

        auto generateConstraintsTask = m_tasks.AddGuardedTask(
            [registry,
             &manifolds = std::as_const(cache.manifolds),
             &constraints = cache.constraints]
        {
            GenerateConstraints(registry, manifolds, &constraints);
        });

        auto updateJointsTask = m_tasks.AddGuardedTask(
            [registry,
             &joints = m_joints,
             dt = cache.fixedTimeStep]
        {
            UpdateJoints(registry, joints, dt);
        });

        auto resolveConstraintsTask = m_tasks.AddGuardedTask(
            [&constraints = cache.constraints,
             &joints = m_joints,
             dt = cache.fixedTimeStep]
        {
            ResolveConstraints(constraints, joints, dt);
        });

        auto cacheImpulsesTask = m_tasks.AddGuardedTask(
            [&constraints = std::as_const(cache.constraints.contact),
            &manifolds = cache.manifolds]
        {
            if constexpr(EnableContactWarmstarting)
                CacheImpulses(constraints, manifolds);
        });

        auto integrateTask = m_tasks.AddGuardedTask([registry, dt = cache.fixedTimeStep]
        {
            Integrate(registry, dt);
        });

        auto notifyEventsTask = m_tasks.AddGuardedTask(
            [registry,
             &physicsEvents = std::as_const(cache.physicsResults.events),
             &currentTrigger = std::as_const(cache.currentTrigger),
             &cache]
        {
            NotifyCollisionEvents(registry, physicsEvents, currentTrigger, &cache);
        });

        auto updateCacheTask = m_tasks.AddGuardedTask([&cache]
        {
            UpdateCache(&cache);
        });

        fixedUpdateTask.precede(updateInertiaTask, applyGravityTask, updateManifoldsTask, fetchEstimatesTask);
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

        #if NC_OUTPUT_TASKFLOW
        m_tasks.GetTaskFlow().name("Physics Step");
        fixedUpdateTask.name("Component - FixedUpdate");
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
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }

    void PhysicsSystem::DoPhysicsStep(tf::Executor& taskExecutor)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        #ifdef NC_DEBUG_RENDERING
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        #endif

        m_tasks.Run(taskExecutor);

        NC_PROFILE_END();
    }
} // namespace nc::physics