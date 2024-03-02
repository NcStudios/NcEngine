#include "NcPhysicsImpl.h"
#include "ncengine/Events.h"
#include "ncengine/config/Config.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

namespace
{
using namespace nc::physics;

struct BspTreeStub
{
    BspTreeStub(nc::Registry* registry)
        : onAddConnection{registry->OnAdd<ConcaveCollider>().Connect(this, &BspTreeStub::OnAdd)},
          onRemoveConnection{registry->OnRemove<ConcaveCollider>().Connect(this, &BspTreeStub::OnRemove)}
    {
    }

    void OnAdd(ConcaveCollider&) {}
    void OnRemove(nc::Entity) {}
    nc::Connection<ConcaveCollider&> onAddConnection;
    nc::Connection<nc::Entity> onRemoveConnection;
};

class NcPhysicsStub : public nc::physics::NcPhysics
{
    public:
        NcPhysicsStub(nc::Registry* reg) : m_bspStub{reg} {}
        void AddJoint(nc::Entity, nc::Entity, const nc::Vector3&, const nc::Vector3&, float = 0.2f, float = 0.0f) override {}
        void RemoveJoint(nc::Entity, nc::Entity) override {}
        void RemoveAllJoints(nc::Entity) override {}
        void RegisterClickable(IClickable*) override {}
        void UnregisterClickable(IClickable*) noexcept override {}
        auto RaycastToClickables(LayerMask = LayerMaskAll) -> IClickable* override { return nullptr;}

    private:
        BspTreeStub m_bspStub;
};
} // anonymous namespace

namespace nc::physics
{
auto BuildPhysicsModule(const config::PhysicsSettings& settings, Registry* registry, SystemEvents& events) -> std::unique_ptr<NcPhysics>
{
    if(settings.enabled)
    {
        NC_LOG_TRACE("Building NcPhysics module");
        return std::make_unique<NcPhysicsImpl>(settings, registry, events);
    }

    NC_LOG_TRACE("Physics disabled - building NcPhysics stub");
    return std::make_unique<NcPhysicsStub>(registry);
}

NcPhysicsImpl::NcPhysicsImpl(const config::PhysicsSettings& settings, Registry* registry, SystemEvents& events)
    : m_pipeline{registry, settings.fixedUpdateInterval, events.rebuildStatics},
      m_clickableSystem{},
      m_accumulatedTime{0.0},
      m_currentIterations{0u}
{
}

void NcPhysicsImpl::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
{
    m_pipeline.GetJointSystem()->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
}

void NcPhysicsImpl::RemoveJoint(Entity entityA, Entity entityB)
{
    m_pipeline.GetJointSystem()->RemoveJoint(entityA, entityB);
}

void NcPhysicsImpl::RemoveAllJoints(Entity entity)
{
    m_pipeline.GetJointSystem()->RemoveAllJoints(entity);
}

void NcPhysicsImpl::RegisterClickable(IClickable* clickable)
{
    m_clickableSystem.RegisterClickable(clickable);
}

void NcPhysicsImpl::UnregisterClickable(IClickable* clickable) noexcept
{
    m_clickableSystem.UnregisterClickable(clickable);
}

auto NcPhysicsImpl::RaycastToClickables(LayerMask mask) -> IClickable*
{
    return m_clickableSystem.RaycastToClickables(mask);
}

void NcPhysicsImpl::OnBuildTaskGraph(task::UpdateTasks& graph, task::RenderTasks&)
{
    NC_LOG_TRACE("Building NcPhysics workload");
    const auto fixedStep = config::GetPhysicsSettings().fixedUpdateInterval;
    auto tasks = std::make_unique<tf::Taskflow>();
    auto pipelineModule = [&graph, &tasks, &pipeline = m_pipeline]()
    {
        auto pipelineGraph = pipeline.BuildTaskGraph(graph.GetExceptionContext());
        auto pipelineTask = tasks->composed_of(*pipelineGraph).name("Physics Pipeline");
        graph.StoreGraph(std::move(pipelineGraph));
        return pipelineTask;
    }();

    auto init = tasks->emplace(
        [&iterations = m_currentIterations,
         &fixedDt = m_accumulatedTime]
    {
        iterations = 0u;
        fixedDt += time::DeltaTime();
    }).name("Init");

    auto condition = tasks->emplace(
        [&cur = m_currentIterations,
         &fixedDt = m_accumulatedTime,
         fixedStep]
    {
        constexpr auto maxIterations = physics::MaxPhysicsIterations;
        return (cur < maxIterations && fixedDt > fixedStep) ? 1 : 0;
    }).name("Condition");

    auto update = tasks->emplace(
        [&curIt = m_currentIterations,
         &fixedDt = m_accumulatedTime,
         fixedStep]
    {
        fixedDt -= fixedStep;
        ++curIt;
        return 0;
    }).name("Update Accumulated Time");

    auto finish = tasks->emplace([](){}).name("Finish");

    init.precede(condition);
    condition.precede(finish, pipelineModule);
    pipelineModule.precede(update);
    update.precede(condition);

    graph.Add(task::UpdatePhase::Physics, "NcPhysics", std::move(tasks));
}

void NcPhysicsImpl::Clear() noexcept
{
    /** @todo make sure these are noexcept */

    m_pipeline.Clear();
    m_clickableSystem.Clear();
}
} // namespace nc::physics
