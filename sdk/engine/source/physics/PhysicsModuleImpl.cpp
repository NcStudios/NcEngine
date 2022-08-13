#include "PhysicsModuleImpl.h"
#include "config/Config.h"
#include "graphics/DebugRenderer.h"

namespace
{

using namespace nc::physics;

struct BspTreeStub
{
    BspTreeStub(nc::Registry* registry)
        : onAddConnection{registry->OnAdd<nc::ConcaveCollider>().Connect(this, &BspTreeStub::OnAdd)},
            onRemoveConnection{registry->OnRemove<nc::ConcaveCollider>().Connect(this, &BspTreeStub::OnRemove)}
    {
    }

    void OnAdd(nc::ConcaveCollider&) {}
    void OnRemove(nc::Entity) {}
    nc::Connection<nc::ConcaveCollider&> onAddConnection;
    nc::Connection<nc::Entity> onRemoveConnection;
};

class PhysicsModuleStub : public nc::physics::PhysicsModule
{
    public:
        PhysicsModuleStub(nc::Registry* reg, nc::time::Time*) : m_tasks{}, m_bspStub{reg} {}
        void AddJoint(nc::Entity, nc::Entity, const nc::Vector3&, const nc::Vector3&, float = 0.2f, float = 0.0f) override {}
        void RemoveJoint(nc::Entity, nc::Entity) override {}
        void RemoveAllJoints(nc::Entity) override {}
        void RegisterClickable(IClickable*) override {}
        void UnregisterClickable(IClickable*) noexcept override {}
        auto RaycastToClickables(LayerMask = LayerMaskAll) -> IClickable* override { return nullptr;}
        auto BuildWorkload() -> std::vector<nc::task::Job> override { return {}; }
        void Clear() noexcept override {}

    private:
        nc::task::TaskGraph m_tasks;
        BspTreeStub m_bspStub;
};

} // anonymous namespace

namespace nc::physics
{

auto BuildPhysicsModule(bool enableModule, Registry* registry, time::Time* time) -> std::unique_ptr<PhysicsModule>
{
    if(enableModule)
    {
        return std::make_unique<PhysicsModuleImpl>(registry, time);
    }
    else
    {
        return std::make_unique<PhysicsModuleStub>(registry, time);
    }
}

PhysicsModuleImpl::PhysicsModuleImpl(Registry* registry, time::Time* time)
    : m_pipeline{registry, config::GetPhysicsSettings().fixedUpdateInterval},
        m_clickableSystem{},
        m_time{time},
        m_currentIterations{0u}
{
}

void PhysicsModuleImpl::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
{
    m_pipeline.GetJointSystem()->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
}

void PhysicsModuleImpl::RemoveJoint(Entity entityA, Entity entityB)
{
    m_pipeline.GetJointSystem()->RemoveJoint(entityA, entityB);
}

void PhysicsModuleImpl::RemoveAllJoints(Entity entity)
{
    m_pipeline.GetJointSystem()->RemoveAllJoints(entity);
}

void PhysicsModuleImpl::RegisterClickable(IClickable* clickable)
{
    m_clickableSystem.RegisterClickable(clickable);
}

void PhysicsModuleImpl::UnregisterClickable(IClickable* clickable) noexcept
{
    m_clickableSystem.UnregisterClickable(clickable);
}

auto PhysicsModuleImpl::RaycastToClickables(LayerMask mask) -> IClickable*
{
    return m_clickableSystem.RaycastToClickables(mask);
}

auto PhysicsModuleImpl::BuildWorkload() -> std::vector<task::Job>
{
    const auto fixedStep = config::GetPhysicsSettings().fixedUpdateInterval;

    auto* tf = m_pipeline.GetTasks().GetTaskFlow();
    auto pipelineModule = m_tasks.composed_of(*tf).name("Physics Pipeline");

    auto init = m_tasks.emplace([&it = m_currentIterations]
    {
        it = 0u;
    }).name("Init");

    auto condition = m_tasks.emplace(
        [&cur = m_currentIterations,
            time = m_time,
            fixedStep]
    {
        constexpr auto maxIterations = physics::MaxPhysicsIterations;
        return (cur < maxIterations && time->GetAccumulatedTime() > fixedStep) ? 1 : 0;
    }).name("Condition");

    auto update = m_tasks.emplace(
        [&curIt = m_currentIterations,
            time = m_time,
            fixedStep]
    {
        time->DecrementAccumulatedTime(fixedStep);
        ++curIt;
        return 0;
    }).name("Update Accumulated Time");

    auto finish = m_tasks.emplace([](){}).name("Finish");

    init.precede(condition);
    condition.precede(finish, pipelineModule);
    pipelineModule.precede(update);
    update.precede(condition);

    return std::vector<task::Job>
    {
        task::Job{ &m_tasks, "Physics Module", task::ExecutionPhase::Physics }
    };
}

void PhysicsModuleImpl::Clear() noexcept
{
    /** @todo make sure these are noexcept */

    m_pipeline.Clear();
    m_clickableSystem.Clear();
}

} // namespace nc::physics
