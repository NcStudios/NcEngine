#include "NcPhysicsImpl2.h"
#include "jolt/Conversion.h"
#include "jolt/Shape.h"

#include "ncengine/ecs/Registry.h"
#include "ncengine/config/Config.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"

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
} // anonymous namespace

namespace nc::physics
{
#ifdef NC_USE_JOLT
auto BuildPhysicsModule(const config::PhysicsSettings& settings, Registry* registry, SystemEvents& events) -> std::unique_ptr<NcPhysics>
{
    if(settings.enabled)
    {
        NC_LOG_TRACE("Building NcPhysics module");
        return std::make_unique<NcPhysicsImpl2>(settings, registry, events);
    }

    NC_LOG_TRACE("Physics disabled - building NcPhysics stub");
    return std::make_unique<NcPhysicsStub2>();
}
#endif

NcPhysicsImpl2::NcPhysicsImpl2(const config::PhysicsSettings&, Registry* registry, SystemEvents&)
    : m_ecs{registry->GetEcs()},
      m_jolt{JoltApi::Initialize()},
      m_onAddRigidBodyConnection{registry->OnAdd<physics::RigidBody>().Connect(this, &NcPhysicsImpl2::OnAddRigidBody)}
{
}

void NcPhysicsImpl2::Run()
{
    // @todo: 689 need to update api bodies for all dirty non-static objects
    m_jolt.Update(time::DeltaTime());

    for (auto& body : m_ecs.GetAll<RigidBody>())
    {
        // @todo: 691 not sure how kinematic behave yet, assuming they won't get updated
        if (body.GetBodyType() != BodyType::Dynamic)
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

void NcPhysicsImpl2::OnAddRigidBody(RigidBody& body)
{
    const auto& transform = m_ecs.Get<Transform>(body.GetEntity());
    const auto [scale, rotation, position] = DecomposeMatrix(transform.TransformationMatrix());
    const auto bodyType = body.GetBodyType();
    const auto bodySettings = JPH::BodyCreationSettings{
        MakeShape(body.GetShape(), ToJoltVec3(scale)),
        ToJoltVec3(position),
        ToJoltQuaternion(rotation),
        ToMotionType(bodyType),
        ToObjectLayer(bodyType)
    };

    auto& iBody = m_jolt.physicsSystem.GetBodyInterface(); // @todo: 697 try non-locking interface
    auto apiBody = iBody.CreateBody(bodySettings);
    iBody.AddBody(apiBody->GetID(), JPH::EActivation::Activate);
    body.Init(apiBody, &iBody);
}

void NcPhysicsImpl2::Clear() noexcept
{
}
} // namespace nc::physics
