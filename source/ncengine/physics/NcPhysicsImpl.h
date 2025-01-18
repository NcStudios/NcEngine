#pragma once

#include "DeferredPhysicsCreateState.h"
#include "jolt/JoltPhysics.h"
#include "jolt/BodyManager.h"
#include "jolt/CollisionQueryManager.h"
#include "jolt/ConstraintFactory.h"
#include "jolt/ConstraintManager.h"
#include "jolt/ShapeFactory.h"
#include "jolt/VehicleManager.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/task/TaskGraph.h"

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
class NcPhysicsImpl final : public NcPhysics
{
    public:
        NcPhysicsImpl(const config::MemorySettings& memorySettings,
                      const config::PhysicsSettings& physicsSettings,
                      ecs::Ecs world,
                      asset::NcAsset& ncAsset,
                      const task::AsyncDispatcher& dispatcher,
                      SystemEvents& events,
                      std::unique_ptr<DeferredPhysicsCreateState> deferredState);

        // Module Interface
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&) override;
        void OnBeforeSceneLoad() override;
        void OnBeforeSceneFragmentLoad() override;
        void OnAfterSceneFragmentLoad() override;
        void Clear() noexcept override;

        // NcPhysics Interface
        auto GetTick() const -> size_t override;
        void ResetTick(size_t step = 0) override;
        void Tick(size_t steps = 0) override;
        void SyncTransforms() override;
        void DispatchAccumulatedEvents() override;
        void SaveSnapshot(PhysicsSnapshot& snapshot) override;
        auto RestoreSnapshot(PhysicsSnapshot& snapshot) -> bool override;
        auto IsUpdateEnabled() const -> bool override { return m_updateEnabled; }
        void EnableUpdate(bool enable) override { m_updateEnabled = enable; }
        void BeginRigidBodyBatch(size_t bodyCountHint = 0ull) override;
        void EndRigidBodyBatch() override;

        void Run();

    private:
        ecs::Ecs m_ecs;
        JoltPhysics m_jolt;
        ShapeFactory m_shapeFactory;
        ConstraintFactory m_constraintFactory;
        ConstraintManager m_constraintManager;
        VehicleManager m_vehicleManager;
        BodyManager m_bodyManager;
        CollisionQueryManager m_queryManager;
        std::unique_ptr<DeferredPhysicsCreateState> m_deferredState;
        bool m_updateEnabled = true; // todo: could combine these
        bool m_manualTick;
};
} // namespace physics
} // namespace nc
