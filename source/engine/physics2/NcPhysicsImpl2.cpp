#include "NcPhysicsImpl2.h"
#include "EventDispatch.h"
#include "jolt/Conversion.h"
#include "jolt/ShapeFactory.h"

#include "ncengine/ecs/Registry.h"
#include "ncengine/config/Config.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

namespace
{
class NcPhysicsStub2 : public nc::physics::NcPhysics
{
    public:
        NcPhysicsStub2() {}
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
};

auto GetDeferredCreateState(nc::Registry* registry) -> nc::physics::DeferredPhysicsCreateState*
{
    auto& userData = registry->GetEcs().GetPool<nc::physics::RigidBody>().Handler().userData;
    auto state = std::any_cast<nc::physics::DeferredPhysicsCreateState>(&userData);
    NC_ASSERT(state, "RigidBody user data did not contain DeferredPhysicsCreateState");
    return state;
}
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
    if(physicsSettings.enabled)
    {
        NC_LOG_TRACE("Building NcPhysics module");
        return std::make_unique<NcPhysicsImpl2>(memorySettings, physicsSettings, registry, dispatcher, events);
    }

    NC_LOG_TRACE("Physics disabled - building NcPhysics stub");
    return std::make_unique<NcPhysicsStub2>();
}
#endif

NcPhysicsImpl2::NcPhysicsImpl2(const config::MemorySettings& memorySettings,
                               const config::PhysicsSettings& physicsSettings,
                               Registry* registry,
                               const task::AsyncDispatcher& dispatcher,
                               SystemEvents&)
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
      m_deferredState{GetDeferredCreateState(registry)}
{
}

void NcPhysicsImpl2::Run()
{
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
    m_deferredState->bodyBatchIndex = m_bodyManager.BeginBatchAdd();
    m_deferredState->constraintBatchIndex = m_constraintManager.BeginBatchAdd();
}

void NcPhysicsImpl2::OnAfterSceneFragmentLoad()
{
    m_bodyManager.EndBatchAdd(std::exchange(m_deferredState->bodyBatchIndex, 0ull));

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

    m_constraintManager.EndBatchAdd(std::exchange(m_deferredState->constraintBatchIndex, 0ull));
}

void NcPhysicsImpl2::Clear() noexcept
{
    m_jolt.contactListener.Clear();
    m_constraintManager.Clear();
    m_bodyManager.Clear();
    m_bodyManager.DeferCleanup(true);
}
} // namespace nc::physics
