#include "NcPhysicsImpl2.h"
#include "EventDispatch.h"
#include "jolt/Conversion.h"
#include "jolt/ShapeFactory.h"

#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/config/Config.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

namespace
{
auto RegisterDeferredCreateState(nc::Registry* registry) -> std::unique_ptr<nc::physics::DeferredPhysicsCreateState>
{
    auto state = std::make_unique<nc::physics::DeferredPhysicsCreateState>();
    auto& userData = registry->GetEcs().GetPool<nc::physics::RigidBody>().Handler().userData;
    NC_ASSERT(!userData.has_value(), "Attempting to initialize RigidBody user data, but it already has a value");
    userData = std::any{state.get()};
    return state;
}

class NcPhysicsStub2 : public nc::physics::NcPhysics
{
    public:
        NcPhysicsStub2(std::unique_ptr<nc::physics::DeferredPhysicsCreateState> deferredState)
            : m_deferredState{std::move(deferredState)}
        {
        }

        void BeginRigidBodyBatch(size_t) override {}
        void EndRigidBodyBatch() override {}
        void AddJoint(nc::Entity, nc::Entity, const nc::Vector3&, const nc::Vector3&, float = 0.2f, float = 0.0f) override {}
        void RemoveJoint(nc::Entity, nc::Entity) override {}
        void RemoveAllJoints(nc::Entity) override {}
        void RegisterClickable(nc::physics::IClickable*) override {}
        void UnregisterClickable(nc::physics::IClickable*) noexcept override {}
        auto RaycastToClickables(nc::physics::LayerMask) -> nc::physics::IClickable* override { return nullptr;}

        void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks&)
        {
            update.Add(
                nc::update_task_id::PhysicsPipeline,
                "PhysicsPipeline(stub)",
                []{},
                {nc::update_task_id::FrameLogicUpdate}
            );
        }

    private:
        std::unique_ptr<nc::physics::DeferredPhysicsCreateState> m_deferredState;
};
} // anonymous namespace

namespace nc::physics
{
#ifdef NC_USE_JOLT
auto BuildPhysicsModule(const config::MemorySettings& memorySettings,
                        const config::PhysicsSettings& physicsSettings,
                        Registry* registry,
                        const task::AsyncDispatcher& dispatcher,
                        SystemEvents& events) -> std::unique_ptr<NcPhysics>
{
    auto deferredState = RegisterDeferredCreateState(registry);
    if(physicsSettings.enabled)
    {
        NC_LOG_TRACE("Building NcPhysics module");
        return std::make_unique<NcPhysicsImpl2>(memorySettings, physicsSettings, registry, dispatcher, events, std::move(deferredState));
    }

    NC_LOG_TRACE("Physics disabled - building NcPhysics stub");
    return std::make_unique<NcPhysicsStub2>(std::move(deferredState));
}
#endif

NcPhysicsImpl2::NcPhysicsImpl2(const config::MemorySettings& memorySettings,
                               const config::PhysicsSettings& physicsSettings,
                               Registry* registry,
                               const task::AsyncDispatcher& dispatcher,
                               SystemEvents&,
                               std::unique_ptr<DeferredPhysicsCreateState> deferredState)
    : m_ecs{registry->GetEcs()},
      m_jolt{JoltApi::Initialize(memorySettings, physicsSettings, dispatcher)},
      m_constraintManager{m_jolt.physicsSystem, memorySettings.maxTransforms},
      m_bodyManager{
        registry->GetImpl().GetPool<Transform>(),
        registry->GetImpl().GetPool<RigidBody>(),
        memorySettings.maxTransforms,
        m_jolt.physicsSystem,
        m_shapeFactory,
        m_constraintManager
      },
      m_deferredState{std::move(deferredState)}
{
}

void NcPhysicsImpl2::Run()
{
    NC_PROFILE_TASK("NcPhysics::Run", ProfileCategory::Physics);
    if (!m_updateEnabled)
    {
        return;
    }

    m_jolt.Update(time::DeltaTime());
    SyncTransforms();
    DispatchPhysicsEvents(m_jolt.contactListener, m_ecs);
}

void NcPhysicsImpl2::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&)
{
    NC_LOG_TRACE("Building NcPhysics Tasks");
    update.Add(
        update_task_id::PhysicsPipeline,
        "PhysicsPipeline",
        [this](){ this->Run(); },
        {update_task_id::FrameLogicUpdate}
    );
}

void NcPhysicsImpl2::SyncTransforms()
{
    NC_PROFILE_SCOPE("NcPhysics::SyncTransforms", ProfileCategory::Physics);
    for (auto& body : m_ecs.GetAll<RigidBody>())
    {
        if (body.GetBodyType() == BodyType::Static)
        {
            continue;
        }

        auto* apiBody = reinterpret_cast<JPH::Body*>(body.GetHandle());
        if (!apiBody->IsActive())
        {
            continue;
        }

        const auto position = ToXMVectorHomogeneous(apiBody->GetPosition());
        const auto orientation = ToXMQuaternion(apiBody->GetRotation());
        auto& transform = m_ecs.Get<Transform>(body.GetEntity());
        transform.SetPositionAndRotationXM(position, orientation);
    }
}

void NcPhysicsImpl2::OnBeforeSceneLoad()
{
    m_bodyManager.DeferCleanup(false);
}

void NcPhysicsImpl2::OnBeforeSceneFragmentLoad()
{
    BeginRigidBodyBatch();
}

void NcPhysicsImpl2::OnAfterSceneFragmentLoad()
{
    EndRigidBodyBatch();
}

void NcPhysicsImpl2::Clear() noexcept
{
    m_jolt.contactListener.Clear();
    m_constraintManager.Clear();
    m_bodyManager.Clear();
    m_bodyManager.DeferCleanup(true);
}

void NcPhysicsImpl2::BeginRigidBodyBatch(size_t bodyCountHint)
{
    NC_ASSERT(
        m_deferredState->bodyBatchIndex == DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->constraintBatchIndex == DeferredPhysicsCreateState::NullBatch,
        "RigidBody batch already in progress"
    );

    if (bodyCountHint != 0ull)
    {
        auto& pool = m_ecs.GetPool<RigidBody>();
        pool.Reserve(pool.size() + bodyCountHint);
    }

    m_deferredState->bodyBatchIndex = m_bodyManager.BeginBatchAdd(bodyCountHint);
    m_deferredState->constraintBatchIndex = m_constraintManager.BeginBatchAdd();
}

void NcPhysicsImpl2::EndRigidBodyBatch()
{
    NC_ASSERT(
        m_deferredState->bodyBatchIndex != DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->constraintBatchIndex != DeferredPhysicsCreateState::NullBatch,
        "No RigidBody batch is in progress"
    );

    m_bodyManager.EndBatchAdd(std::exchange(m_deferredState->bodyBatchIndex, DeferredPhysicsCreateState::NullBatch));

    // Deserialization needs to queue constraints until all bodies exist. If the batch isn't from a scene fragment, this
    // will be empty and any constraints will already be in the ConstraintManager, but not yet added to the simulation.
    for (const auto& [ownerEntity, targetEntity, info] : m_deferredState->constraints)
    {
        auto& owner = m_ecs.Get<RigidBody>(ownerEntity);
        if (targetEntity.Valid())
        {
            auto& target = m_ecs.Get<RigidBody>(targetEntity);
            owner.AddConstraint(info, target);
        }
        else
        {
            owner.AddConstraint(info);
        }
    }

    m_constraintManager.EndBatchAdd(std::exchange(m_deferredState->constraintBatchIndex, DeferredPhysicsCreateState::NullBatch));
}
} // namespace nc::physics
