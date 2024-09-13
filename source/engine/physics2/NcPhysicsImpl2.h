#pragma once

#include "jolt/JoltApi.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Signal.h"

namespace nc
{
struct SystemEvents;

namespace config
{
struct MemorySettings;
struct PhysicsSettings;
} // namespace config

namespace physics
{
class NcPhysicsImpl2 final : public NcPhysics
{
    public:
        NcPhysicsImpl2(const config::MemorySettings& memorySettings,
                       const config::PhysicsSettings& physicsSettings,
                       Registry* registry,
                       const task::AsyncDispatcher& dispatcher,
                       SystemEvents& events);

        void Run();
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&) override;
        void Clear() noexcept override;

        void AddJoint(Entity , Entity, const Vector3&, const Vector3&, float = 0.2f, float = 0.0f) override {}
        void RemoveJoint(Entity, Entity ) override {}
        void RemoveAllJoints(Entity) override {}
        void RegisterClickable(IClickable*) override {}
        void UnregisterClickable(IClickable*) noexcept override {}
        auto RaycastToClickables(LayerMask = LayerMaskAll) -> IClickable* override { return nullptr; }

    private:
        ecs::Ecs m_ecs;
        JoltApi m_jolt;
        Connection<RigidBody&> m_onAddRigidBodyConnection;

        void OnAddRigidBody(RigidBody& body);
        void SyncTransforms();
};
} // namespace physics
} // namespace nc
