#pragma once

#include "PhysicsPipelineBuilder.h"

#include "optick.h"

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
        PhysicsPipeline(Registry* registry, float fixedTimeStep, Signal<>& rebuildStatics);

        void Clear();
        auto GetJointSystem() -> JointSystem* { return &m_jointSystem; }
        auto BuildTaskGraph(task::ExceptionContext& context) -> std::unique_ptr<tf::Taskflow>;

    private:
        proxy_cache m_proxyCache;
        broad_phase m_broadPhase;
        NarrowPhase m_narrowPhase;
        concave_phase m_concavePhase;
        JointSystem m_jointSystem;
        Solver m_solver;
        Registry* m_registry;
        float m_fixedTimeStep;
};

template<class Stages>
PhysicsPipeline<Stages>::PhysicsPipeline(Registry* registry, float fixedTimeStep, Signal<>& rebuildStatics)
    : m_proxyCache{registry},
      m_broadPhase{},
      m_narrowPhase{registry},
      m_concavePhase{registry, rebuildStatics},
      m_jointSystem{registry},
      m_solver{registry},
      m_registry{registry},
      m_fixedTimeStep{fixedTimeStep}
{
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
auto PhysicsPipeline<Stages>::BuildTaskGraph(task::ExceptionContext& context) -> std::unique_ptr<tf::Taskflow>
{
    auto registry = m_registry;
    auto tasks = std::make_unique<tf::Taskflow>();
    auto builder = PipelineBuilder{*tasks};

    auto fixedUpdate = builder.Add(
        "Run FixedLogic",
        BuildFixedUpdateTask(context, registry)
    );

    auto updateInertia = builder.Add(
        "Update World Inertia Tensors",
        BuildUpdateInertiaTask(context, registry),
        {fixedUpdate}
    );

    auto applyGravity = builder.Add(
        "Apply Gravity",
        BuildApplyGravityTask(context, registry, m_fixedTimeStep),
        {fixedUpdate}
    );

    auto updateManifolds = builder.Add(
        "Update Contact Manifolds",
        BuildUpdateManifoldsTask(context, m_narrowPhase),
        {fixedUpdate}
    );

    auto updateProxyCache = builder.Add(
        "Proxy Cache - Update",
        BuildUpdateProxyCacheTask(context, m_proxyCache),
        {fixedUpdate}
    );

    auto broadPhase = builder.Add(
        "Broad Detection",
        BuildBroadPhaseDetectionTask(context, m_broadPhase, m_proxyCache),
        {updateProxyCache}
    );

    auto narrowPhasePhysics = builder.Add(
        "Narrow Detection - Convex Contacts",
        BuildNarrowPhasePhysicsTask(context, m_narrowPhase, m_broadPhase),
        {broadPhase, updateManifolds}
    );

    auto narrowPhaseTrigger = builder.Add(
        "Narrow Detection - Triggers",
        BuildNarrowPhaseTriggerTask(context, m_narrowPhase, m_broadPhase),
        {broadPhase}
    );

    auto concavePhase = builder.Add(
        "Narrow Detection - Concave Contacts",
        BuildConcavePhaseTask(context, m_concavePhase, m_proxyCache),
        {updateProxyCache}
    );

    auto mergeContacts = builder.Add(
        "Merge Contacts",
        BuildMergeContactsTask(context, m_narrowPhase, m_concavePhase),
        {narrowPhasePhysics, concavePhase}
    );

    auto generateFreedomConstraints = builder.Add(
        "Solver - Generate Freedom Constraints",
        BuildGenerateFreedomConstraintsTask(context, m_solver, m_fixedTimeStep),
        {updateInertia, applyGravity}
    );

    auto generateContactConstraints = builder.Add(
        "Solver - Generate Contact Constraints",
        BuildGenerateContactConstraintsTask(context, m_solver, m_narrowPhase),
        {updateInertia, applyGravity, mergeContacts}
    );

    auto updateJoints = builder.Add(
        "Joint System - Update",
        BuildUpdateJointsTask(context, m_jointSystem, m_fixedTimeStep),
        {applyGravity, updateInertia}
    );

    auto resolveConstraints = builder.Add(
        "Solver - Resolve Constraints",
        BuildResolveConstraintsTask(context, m_solver, m_jointSystem, m_fixedTimeStep),
        {generateFreedomConstraints, generateContactConstraints, updateJoints}
    );

    auto cacheImpulses = builder.Add(
        "Cache Impulses",
        BuildCacheImpulsesTask(context, m_solver, m_narrowPhase),
        {resolveConstraints}
    );

    auto integrate = builder.Add(
        "Integrate",
        BuildIntegrateTask(context, registry, m_fixedTimeStep),
        {resolveConstraints}
    );

    builder.Add(
        "Notify Events",
        BuildNotifyEventsTask(context, m_narrowPhase),
        {narrowPhaseTrigger, cacheImpulses, integrate}
    );

    return tasks;
}
} // namespace nc::physics
