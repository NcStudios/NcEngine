#pragma once

#include "PhysicsConstants.h"
#include "PhysicsPipelineTypes.h"
#include "collision/narrow_phase/NarrowPhase.h"
#include "dynamics/Dynamics.h"
#include "dynamics/Joint.h"
#include "dynamics/Solver.h"
#include "ecs/Logic.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "task/TaskGraph.h"

#include "optick.h"

namespace nc::physics
{
class PipelineBuilder
{
    public:
        PipelineBuilder(tf::Taskflow& tasks)
            : m_tasks{tasks}
        {
        }

        template<class F>
        auto Add(std::string_view name, F&& func, std::initializer_list<tf::Task> dependencies = {})
        {
            auto task = m_tasks.emplace(std::forward<F>(func)).name(name.data());
            for (auto dependency : dependencies)
            {
                task.succeed(dependency);
            }

            return task;
        }

    private:
        tf::Taskflow& m_tasks;
};

inline auto BuildFixedUpdateTask(task::ExceptionContext& ctx, Registry* registry)
{
    return task::Guard(ctx, [registry]
    {
        OPTICK_CATEGORY("SendFixedUpdate", Optick::Category::Physics);
        for(auto& fixedLogic : View<FixedLogic>{registry})
            fixedLogic.Run(registry);
    });
}

inline auto BuildUpdateInertiaTask(task::ExceptionContext& ctx, Registry* registry)
{
    return task::Guard(ctx, [registry]
    {
        OPTICK_CATEGORY("UpdateWorldInertiaTensors", Optick::Category::Physics);
        UpdateWorldInertiaTensors(registry);
    });
}

inline auto BuildApplyGravityTask(task::ExceptionContext& ctx, Registry* registry, float fixedTimeStep)
{
    return task::Guard(ctx, [registry, fixedTimeStep]
    {
        OPTICK_CATEGORY("ApplyGravity", Optick::Category::Physics);
        ApplyGravity(registry, fixedTimeStep);
    });
}

inline auto BuildUpdateManifoldsTask(task::ExceptionContext& ctx, NarrowPhase& phase)
{
    return task::Guard(ctx, [&phase]
    {
        OPTICK_CATEGORY("NarrowPhase::UpdateManifolds", Optick::Category::Physics);
        phase.UpdateManifolds();
    });
}

template<ProxyCache Cache>
auto BuildUpdateProxyCacheTask(task::ExceptionContext& ctx, Cache& cache)
{
    return task::Guard(ctx, [&cache]
    {
        OPTICK_CATEGORY("ProxyCache::Update", Optick::Category::Physics);
        cache.Update();
    });
}

template<BroadPhase Broad, ProxyCache Cache>
auto BuildBroadPhaseDetectionTask(task::ExceptionContext& ctx, Broad& phase, Cache& cache)
{
    return task::Guard(ctx, [&phase, &cache]
    {
        OPTICK_CATEGORY("BroadPhase", Optick::Category::Physics);
        phase.Update(&cache);
        phase.FindPairs();
    });
}

template<BroadPhase Broad>
auto BuildNarrowPhasePhysicsTask(task::ExceptionContext& ctx, NarrowPhase& narrow, Broad& broad)
{
    return task::Guard(ctx, [&narrow, &broad]
    {
        OPTICK_CATEGORY("NarrowPhase::FindPhysicsPairs1", Optick::Category::Physics);
        using proxy = typename Broad::proxy_type;
        narrow.FindPhysicsPairs<proxy>(broad.PhysicsPairs());
    });
}

template<BroadPhase Broad>
auto BuildNarrowPhaseTriggerTask(task::ExceptionContext& ctx, NarrowPhase& narrow, const Broad& broad)
{
    return task::Guard(ctx, [&narrow, &broad]
    {
        OPTICK_CATEGORY("NarrowPhase::FindTriggerPairs", Optick::Category::Physics);
        using proxy = typename Broad::proxy_type;
        narrow.FindTriggerPairs<proxy>(broad.TriggerPairs());
    });
}

template<ConcavePhase Concave, ProxyCache Cache>
auto BuildConcavePhaseTask(task::ExceptionContext& ctx, Concave& concave, Cache& cache)
{
    return task::Guard(ctx, [&concave, &cache]
    {
        OPTICK_CATEGORY("Concave::FindPairs", Optick::Category::Physics);
        using proxy = typename Cache::proxy_type;
        concave.template FindPairs<proxy>(cache.Proxies());
    });
}

template<ConcavePhase Concave>
auto BuildMergeContactsTask(task::ExceptionContext& ctx, NarrowPhase& narrow, Concave& concave)
{
    return task::Guard(ctx, [&narrow, &concave]
    {
        OPTICK_CATEGORY("NarrowPhase::MergeContacts", Optick::Category::Physics);
        narrow.MergeContacts(concave.Pairs());
    });
}

inline auto BuildGenerateFreedomConstraintsTask(task::ExceptionContext& ctx, Solver& solver, float fixedTimeStep)
{
    return task::Guard(ctx, [&solver, fixedTimeStep]
    {
        OPTICK_CATEGORY("Solver::GenerateFreedomConstraints", Optick::Category::Physics);
        solver.GenerateFreedomConstraints(fixedTimeStep);
    });
}

inline auto BuildGenerateContactConstraintsTask(task::ExceptionContext& ctx, Solver& solver, NarrowPhase& narrow)
{
    return task::Guard(ctx, [&solver, &narrow]
    {
        OPTICK_CATEGORY("Solver::GenerateContactConstraints", Optick::Category::Physics);
        solver.GenerateContactConstraints(narrow.Manifolds());
    });
}

inline auto BuildUpdateJointsTask(task::ExceptionContext& ctx, JointSystem& joints, float fixedTimeStep)
{
    return task::Guard(ctx, [&joints, fixedTimeStep]
    {
        OPTICK_CATEGORY("JointSystem::UpdateJoints", Optick::Category::Physics);
        joints.UpdateJoints(fixedTimeStep);
    });
}

inline auto BuildResolveConstraintsTask(task::ExceptionContext& ctx, Solver& solver, JointSystem& joints, float fixedTimeStep)
{
    return task::Guard(ctx, [&solver, &joints, fixedTimeStep]
    {
        OPTICK_CATEGORY("Solver::ResolveConstraints", Optick::Category::Physics);
        solver.ResolveConstraints(joints.Joints(), fixedTimeStep);
    });
}

inline auto BuildCacheImpulsesTask(task::ExceptionContext& ctx, Solver& solver, NarrowPhase& narrow)
{
    return task::Guard(ctx, [&solver, &narrow]
    {
        OPTICK_CATEGORY("NarrowPhase::CacheImpulses", Optick::Category::Physics);
        if constexpr(EnableContactWarmstarting)
            narrow.CacheImpulses(solver.ContactConstraints());
    });
}

inline auto BuildIntegrateTask(task::ExceptionContext& ctx, Registry* registry, float fixedTimeStep)
{
    return task::Guard(ctx, [registry, fixedTimeStep]
    {
        OPTICK_CATEGORY("Integrate", Optick::Category::Physics);
        Integrate(registry, fixedTimeStep);
    });
}

inline auto BuildNotifyEventsTask(task::ExceptionContext& ctx, NarrowPhase& narrow)
{
    return task::Guard(ctx, [&narrow]
    {
        OPTICK_CATEGORY("NarrowPhase::NotifyEvents", Optick::Category::Physics);
        narrow.NotifyEvents();
    });
}
} // namespace nc::physics
