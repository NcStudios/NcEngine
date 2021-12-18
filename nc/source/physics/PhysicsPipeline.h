#pragma once

#include "PhysicsConstants.h"
#include "PhysicsPipelineTypes.h"
#include "collision/BspTree.h"
#include "collision/narrow_phase/ManifoldCache.h"
#include "collision/narrow_phase/NarrowPhase.h"
#include "config/Config.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Joint.h"
#include "dynamics/Solver.h"
#include "ecs/Registry.h"
#include "optick/optick.h"
#include "task/TaskGraph.h"

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
        OPTICK_CATEGORY("PhysicsPipeline::Step", Optick::Category::Physics);
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
            m_concavePhase.template FindPairs<proxy>(m_proxyCache.Proxies());
            m_narrowPhase.FindPhysicsPairs<proxy>(m_broadPhase.PhysicsPairs());
            m_narrowPhase.FindTriggerPairs<proxy>(m_broadPhase.TriggerPairs());
            m_narrowPhase.MergeContacts(m_concavePhase.Pairs());

            m_jointSystem.UpdateJoints(m_fixedTimeStep);
            m_solver.GenerateConstraints(m_narrowPhase.Manifolds());
            m_solver.ResolveConstraints(m_jointSystem.Joints(), m_fixedTimeStep);

            if constexpr(EnableContactWarmstarting)
                m_narrowPhase.CacheImpulses(m_solver.ContactConstraints());

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
            OPTICK_CATEGORY("SendFixedUpdate", Optick::Category::Physics);
            for(auto& group : registry->ViewAll<AutoComponentGroup>())
                group.SendFixedUpdate();
        });

        auto updateInertiaTask = m_tasks.AddGuardedTask([registry]
        {
            OPTICK_CATEGORY("UpdateWorldInertiaTensors", Optick::Category::Physics);
            UpdateWorldInertiaTensors(registry);
        });

        auto applyGravityTask = m_tasks.AddGuardedTask(
            [registry,
             dt = m_fixedTimeStep]
        {
            OPTICK_CATEGORY("ApplyGravity", Optick::Category::Physics);
            ApplyGravity(registry, dt);
        });

        auto updateManifoldsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase]
        {
            OPTICK_CATEGORY("NarrowPhase::UpdateManifolds", Optick::Category::Physics);
            narrow.UpdateManifolds();
        });

        auto updateProxyCacheTask = m_tasks.AddGuardedTask([proxyCache = &m_proxyCache]
        {
            OPTICK_CATEGORY("ProxyCache::Update", Optick::Category::Physics);
            proxyCache->Update();
        });

        auto broadPhaseTask = m_tasks.AddGuardedTask(
            [broad = &m_broadPhase,
             cache = &m_proxyCache]
        {
            OPTICK_CATEGORY("BroadPhase", Optick::Category::Physics);
            broad->Update(cache);
            broad->FindPairs();
        });

        auto narrowPhasePhysicsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &broad = std::as_const(m_broadPhase)]
        {
            OPTICK_CATEGORY("NarrowPhase::FindPhysicsPairs1", Optick::Category::Physics);
            narrow.FindPhysicsPairs<proxy>(broad.PhysicsPairs());
        });

        auto narrowPhaseTriggerTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &broad = std::as_const(m_broadPhase)]
        {
            OPTICK_CATEGORY("NarrowPhase::FindTriggerPairs", Optick::Category::Physics);
            narrow.FindTriggerPairs<proxy>(broad.TriggerPairs());
        });

        auto concavePhaseTask = m_tasks.AddGuardedTask(
            [&concave = m_concavePhase,
             &proxyCache = m_proxyCache]
        {
            OPTICK_CATEGORY("Concave::FindPairs", Optick::Category::Physics);
            concave.template FindPairs<proxy>(proxyCache.Proxies());
        });

        auto mergeContactsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase,
             &concave = std::as_const(m_concavePhase)]
        {
            OPTICK_CATEGORY("NarrowPhase::MergeContacts", Optick::Category::Physics);
            narrow.MergeContacts(concave.Pairs());
        });

        auto generateConstraintsTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &narrow = std::as_const(m_narrowPhase)]
        {
            OPTICK_CATEGORY("Solver::GenerateConstraints", Optick::Category::Physics);
            solver.GenerateConstraints(narrow.Manifolds());
        });

        auto updateJointsTask = m_tasks.AddGuardedTask(
            [&jointSystem = m_jointSystem,
             dt = m_fixedTimeStep]
        {
            OPTICK_CATEGORY("JointSystem::UpdateJoints", Optick::Category::Physics);
            jointSystem.UpdateJoints(dt);
        });

        auto resolveConstraintsTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &jointSystem = m_jointSystem,
             dt = m_fixedTimeStep]
        {
            OPTICK_CATEGORY("Solver::ResolveConstraints", Optick::Category::Physics);
            solver.ResolveConstraints(jointSystem.Joints(), dt);
        });

        auto cacheImpulsesTask = m_tasks.AddGuardedTask(
            [&solver = m_solver,
             &narrow = m_narrowPhase]
        {
            OPTICK_CATEGORY("NarrowPhase::CacheImpulses", Optick::Category::Physics);
            if constexpr(EnableContactWarmstarting)
                narrow.CacheImpulses(solver.ContactConstraints());
        });

        auto integrateTask = m_tasks.AddGuardedTask(
            [registry,
             dt = m_fixedTimeStep]
        {
            OPTICK_CATEGORY("Integrate", Optick::Category::Physics);
            Integrate(registry, dt);
        });

        auto notifyEventsTask = m_tasks.AddGuardedTask(
            [&narrow = m_narrowPhase]
        {
            OPTICK_CATEGORY("NarrowPhase::NotifyEvents", Optick::Category::Physics);
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