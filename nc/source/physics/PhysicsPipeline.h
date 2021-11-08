#pragma once

#include "PhysicsConstants.h"
#include "PhysicsPipelineTypes.h"
#include "collision/BspTree.h"
#include "collision/narrow_phase/ManifoldCache.h"
#include "collision/narrow_phase/NarrowPhase.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Joint.h"
#include "dynamics/Solver.h"
#include "ecs/Registry.h"
#include "config/Config.h"
#include "task/Task.h"

namespace nc::physics
{
    template<class Stages>
    class PhysicsPipeline
    {
        using multithreaded = Stages::multithreaded;
        using proxy = Stages::proxy;
        using proxy_cache = Stages::proxy_cache;
        using broad_phase = Stages::broad_phase;
        using concave_phase = Stages::concave_phase;

        static_assert(Proxy<proxy>);
        static_assert(ProxyCache<proxy_cache>);
        static_assert(BroadPhase<broad_phase>);
        static_assert(ConcavePhase<concave_phase>);

        public:
            PhysicsPipeline(Registry* registry, float fixedTimeStep);

            void Step(tf::Executor& executor);
            void Clear();
            auto GetJointSystem() -> JointSystem* { return &m_jointSystem; }

        private:
            TaskGraph m_tasks;
            proxy_cache m_proxyCache;
            broad_phase m_broadPhase;
            NarrowPhase m_narrowPhase;
            concave_phase m_concavePhase;
            JointSystem m_jointSystem;
            Solver m_solver;
            Registry* m_registry;
            float m_fixedTimeStep;

            void BuildTaskGraph(Registry* registry);
    };

    template<class Stages>
    PhysicsPipeline<Stages>::PhysicsPipeline(Registry* registry, float fixedTimeStep)
        : m_tasks{},
          m_proxyCache{registry},
          m_broadPhase{},
          m_narrowPhase{registry},
          m_concavePhase{registry},
          m_jointSystem{registry},
          m_solver{registry},
          m_registry{registry},
          m_fixedTimeStep{fixedTimeStep}
    {
        if constexpr(multithreaded::value)
            BuildTaskGraph(registry);
    }

    template<class Stages>
    void PhysicsPipeline<Stages>::Step(tf::Executor& executor)
    {
        if constexpr(multithreaded::value)
        {
            m_tasks.Run(executor);
        }
        else
        {
            for(auto& group : m_registry->ViewAll<AutoComponentGroup>())
                group.SendFixedUpdate();

            UpdateWorldInertiaTensors(m_registry);
            ApplyGravity(m_registry, m_fixedTimeStep);

            m_proxyCache.Update();
            m_broadPhase.Update(&m_proxyCache);
            m_broadPhase.FindPairs();

            m_narrowPhase.UpdateManifolds();
            m_concavePhase.template FindPairs<proxy>(m_proxyCache.GetProxies());
            m_narrowPhase.FindPhysicsPairs<proxy>(m_broadPhase.GetPhysicsPairs());
            m_narrowPhase.FindTriggerPairs<proxy>(m_broadPhase.GetTriggerPairs());
            m_narrowPhase.MergeContacts(m_concavePhase.GetPairs());

            m_jointSystem.UpdateJoints(m_fixedTimeStep);
            m_solver.GenerateConstraints(m_narrowPhase.GetManifolds());
            m_solver.ResolveConstraints(m_jointSystem.GetJoints(), m_fixedTimeStep);

            if constexpr(EnableContactWarmstarting)
                m_narrowPhase.CacheImpulses(m_solver.GetContactConstraints());

            Integrate(m_registry, m_fixedTimeStep);
            m_narrowPhase.NotifyEvents();
        }
    }

    template<class Stages>
    void PhysicsPipeline<Stages>::Clear()
    {
        m_broadPhase.Clear();
        m_narrowPhase.Clear();
        m_concavePhase.Clear();
        m_jointSystem.Clear();
    }

    template<class Stages>
    void PhysicsPipeline<Stages>::BuildTaskGraph(Registry* registry)
    {
        auto fixedUpdateTask = m_tasks.AddGuardedTask([registry]
        {
            for(auto& group : registry->ViewAll<AutoComponentGroup>())
                group.SendFixedUpdate();
        });

        auto updateInertiaTask = m_tasks.AddGuardedTask([registry]
        {
            UpdateWorldInertiaTensors(registry);
        });

        auto applyGravityTask = m_tasks.AddGuardedTask(
            [registry,
             dt = m_fixedTimeStep]
        {
            ApplyGravity(registry, dt);
        });

        auto updateManifoldsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase]
        {
            narrow.UpdateManifolds();
        });

        auto updateProxyCacheTask = m_tasks.AddGuardedTask([proxyCache = &m_proxyCache]
        {
            proxyCache->Update();
        });

        auto broadPhaseTask = m_tasks.AddGuardedTask(
            [proxyCache = &m_proxyCache,
             broadPhase = &m_broadPhase]
        {
            broadPhase->Update(proxyCache);
            broadPhase->FindPairs();
        });

        auto narrowPhasePhysicsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &broad = std::as_const(m_broadPhase)]
        {
            narrow.FindPhysicsPairs<proxy>(broad.GetPhysicsPairs());
        });

        auto narrowPhaseTriggerTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &broad = std::as_const(m_broadPhase)]
        {
            narrow.FindTriggerPairs<proxy>(broad.GetTriggerPairs());
        });

        auto concavePhaseTask = m_tasks.AddGuardedTask(
            [&concave = m_concavePhase,
             &proxyCache = m_proxyCache]
        {
            concave.template FindPairs<proxy>(proxyCache.GetProxies());
        });

        auto mergeContactsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &concave = std::as_const(m_concavePhase)]
        {
            narrow.MergeContacts(concave.GetPairs());
        });

        auto generateConstraintsTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &narrow = std::as_const(m_narrowPhase)]
        {
            solver.GenerateConstraints(narrow.GetManifolds());
        });

        auto updateJointsTask = m_tasks.AddGuardedTask(
            [&jointSystem = m_jointSystem,
             dt = m_fixedTimeStep]
        {
            jointSystem.UpdateJoints(dt);
        });

        auto resolveConstraintsTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &jointSystem = m_jointSystem,
             dt = m_fixedTimeStep]
        {
            solver.ResolveConstraints(jointSystem.GetJoints(), dt);
        });

        auto cacheImpulsesTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &narrow = m_narrowPhase]
        {
            if constexpr(EnableContactWarmstarting)
                narrow.CacheImpulses(solver.GetContactConstraints());
        });

        auto integrateTask = m_tasks.AddGuardedTask(
            [registry,
             dt = m_fixedTimeStep]
        {
            Integrate(registry, dt);
        });

        auto notifyEventsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase]
        {
            narrow.NotifyEvents();
        });

        fixedUpdateTask.precede(updateInertiaTask, applyGravityTask, updateManifoldsTask, updateProxyCacheTask);
        broadPhaseTask.succeed(updateProxyCacheTask);
        broadPhaseTask.precede(narrowPhasePhysicsTask, narrowPhaseTriggerTask);
        narrowPhasePhysicsTask.succeed(updateManifoldsTask);
        concavePhaseTask.succeed(updateProxyCacheTask);
        mergeContactsTask.succeed(narrowPhasePhysicsTask, concavePhaseTask);
        generateConstraintsTask.succeed(mergeContactsTask, updateInertiaTask, applyGravityTask);
        updateJointsTask.succeed(applyGravityTask, updateInertiaTask);
        resolveConstraintsTask.succeed(generateConstraintsTask, updateJointsTask);
        cacheImpulsesTask.succeed(resolveConstraintsTask);
        integrateTask.succeed(resolveConstraintsTask);
        notifyEventsTask.succeed(integrateTask, narrowPhaseTriggerTask, cacheImpulsesTask);

        /** Task graph visual output */
        #if 0
        m_tasks.GetTaskFlow().name("Physics Step");
        fixedUpdateTask.name("Component FixedUpdate Logic");
        updateInertiaTask.name("Update Inertia Tensors");
        applyGravityTask.name("Apply Gravity");
        updateManifoldsTask.name("Narrow Phase - Update Manifolds");
        updateProxyCacheTask.name("Proxy Cache - Update");
        broadPhaseTask.name("Broad Phase - Update & Find Pairs");
        narrowPhasePhysicsTask.name("Narrow Phase - Detect Contacts");
        narrowPhaseTriggerTask.name("Narrow Phase - Detect Triggers");
        concavePhaseTask.name("Concave Phase - Detect Contacts");
        mergeContactsTask.name("Narrow Phase - Merge Contacts");
        generateConstraintsTask.name("Solver - Generate Constraints");
        updateJointsTask.name("Joints - Update");
        resolveConstraintsTask.name("Solver - Resolve Constraints");
        cacheImpulsesTask.name("Narrow Phase - Cache Impulses");
        integrateTask.name("Integrate");
        notifyEventsTask.name("Narrow Phase - Notify Events");
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }
}