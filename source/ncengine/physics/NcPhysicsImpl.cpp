#include "NcPhysicsImpl.h"
#include "EventDispatch.h"
#include "jolt/Conversion.h"
#include "jolt/ShapeFactory.h"
#include "jolt/VehicleAnimator.h"

#include "ncengine/asset/NcAsset.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/config/Config.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

namespace
{
[[maybe_unused]]
auto RegisterDeferredCreateState(nc::ecs::Ecs world) -> std::unique_ptr<nc::physics::DeferredPhysicsCreateState>
{
    auto state = std::make_unique<nc::physics::DeferredPhysicsCreateState>();
    auto& userData = world.GetPool<nc::RigidBody>().Handler().userData;
    NC_ASSERT(!userData.has_value(), "Attempting to initialize RigidBody user data, but it already has a value");
    userData = std::any{state.get()};
    return state;
}

class NcPhysicsStub : public nc::NcPhysics
{
    public:
        NcPhysicsStub(std::unique_ptr<nc::physics::DeferredPhysicsCreateState> deferredState)
            : m_deferredState{std::move(deferredState)}
        {
        }

        auto GetTick() const -> nc::PhysicsTick override { return nc::PhysicsTick::Null(); }
        void ResetTick(nc::PhysicsTick) override {}
        void Tick(uint32_t) override {}
        void SyncTransforms() override {}
        void DispatchAccumulatedEvents() override {}
        void SaveSnapshot(nc::PhysicsSnapshot&) {}
        auto RestoreSnapshot(nc::PhysicsSnapshot&) -> bool { return false; }
        void BeginRigidBodyBatch(size_t) override {}
        void EndRigidBodyBatch() override {}
        void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks&)
        {
            update.Add(
                nc::update_task_id::PhysicsPipeline,
                "PhysicsPipeline(stub)",
                []{},
                {nc::update_task_id::CommitStagedChanges}
            );
        }

    private:
        std::unique_ptr<nc::physics::DeferredPhysicsCreateState> m_deferredState;
};
} // anonymous namespace

namespace nc
{
auto BuildPhysicsModule(const config::MemorySettings& memorySettings,
                        const config::PhysicsSettings& physicsSettings,
                        ecs::Ecs world,
                        asset::NcAsset& ncAsset,
                        const task::AsyncDispatcher& dispatcher,
                        SystemEvents& events) -> std::unique_ptr<NcPhysics>
{
    auto deferredState = RegisterDeferredCreateState(world);
    if(physicsSettings.enabled)
    {
        NC_LOG_TRACE("Building NcPhysics module");
        return std::make_unique<physics::NcPhysicsImpl>(
            memorySettings,
            physicsSettings,
            world,
            ncAsset,
            dispatcher,
            events,
            std::move(deferredState)
        );
    }

    NC_LOG_TRACE("Physics disabled - building NcPhysics stub");
    return std::make_unique<NcPhysicsStub>(std::move(deferredState));
}

namespace physics
{
NcPhysicsImpl::NcPhysicsImpl(const config::MemorySettings& memorySettings,
                             const config::PhysicsSettings& physicsSettings,
                             ecs::Ecs world,
                             asset::NcAsset& ncAsset,
                             const task::AsyncDispatcher& dispatcher,
                             SystemEvents&,
                             std::unique_ptr<DeferredPhysicsCreateState> deferredState)
    : m_ecs{world},
      m_jolt{memorySettings, physicsSettings, dispatcher},
      m_shapeFactory{
        ncAsset.OnConvexHullUpdate(),
        ncAsset.OnMeshColliderUpdate()
      },
      m_constraintFactory{m_jolt.physicsSystem},
      m_constraintManager{
        m_jolt.physicsSystem,
        m_constraintFactory,
        memorySettings.maxTransforms
      },
      m_vehicleManager{
        m_jolt.physicsSystem,
        m_constraintFactory,
        memorySettings.maxTransforms
      },
      m_bodyManager{
        world.GetPool<Transform>(),
        world.GetPool<RigidBody>(),
        memorySettings.maxTransforms,
        m_jolt.physicsSystem,
        m_shapeFactory,
        m_constraintManager,
        m_vehicleManager
      },
      m_queryManager{
        m_jolt.physicsSystem.GetNarrowPhaseQuery(),
        m_jolt.physicsSystem.GetBodyLockInterfaceNoLock(),
        m_shapeFactory
      },
      m_deferredState{std::move(deferredState)},
      m_networkModeEnabled{physicsSettings.enableNetworkRollback}
{
}

auto NcPhysicsImpl::GetTick() const -> PhysicsTick
{
    return m_jolt.currentTick;
}

void NcPhysicsImpl::ResetTick(PhysicsTick tick)
{
    m_jolt.currentTick = tick;
}

void NcPhysicsImpl::Tick(uint32_t steps)
{
    NC_PROFILE_SCOPE("NcPhysics::Tick", ProfileCategory::Physics);
    if (!m_updateEnabled)
    {
        return;
    }

    m_jolt.Tick(time::DeltaTime(), steps);
}

void NcPhysicsImpl::SyncTransforms()
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

    auto& transformPool = m_ecs.GetPool<Transform>();
    for (const auto& vehicle : m_vehicleManager.GetVehicles())
    {
        if (!vehicle->IsEnabled())
        {
            continue;
        }

        const auto& assemblies = vehicle->GetWheelAssemblies();
        const auto& constraint = *static_cast<const JPH::VehicleConstraint*>(vehicle->GetHandle());
        AnimateVehicle(assemblies, constraint, transformPool);
    }
}

void NcPhysicsImpl::DispatchAccumulatedEvents()
{
    NC_PROFILE_SCOPE("NcPhysics::DispatchAccumulatedEvents()", ProfileCategory::Physics);
    DispatchPhysicsEvents(m_jolt.contactListener, m_ecs);
}

void NcPhysicsImpl::SaveSnapshot(PhysicsSnapshot& snapshot)
{
    return m_jolt.SaveSnapshot(snapshot);
}

auto NcPhysicsImpl::RestoreSnapshot(PhysicsSnapshot& snapshot) -> bool
{
    return m_jolt.RestoreFromSnapshot(snapshot);
}

void NcPhysicsImpl::Run()
{
    NC_PROFILE_TASK("NcPhysics", ProfileCategory::Physics);
    if (!m_updateEnabled)
    {
        return;
    }

    Tick(1);
    SyncTransforms();
    DispatchAccumulatedEvents();
}

void NcPhysicsImpl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&)
{
    if (m_networkModeEnabled)
    {
        NC_LOG_TRACE("Skipping Building NcPhysics Tasks - Network Rollback Enabled");
        return;
    }

    NC_LOG_TRACE("Building NcPhysics Tasks");
    update.Add(
        update_task_id::PhysicsPipeline,
        "PhysicsPipeline",
        [this](){ this->Run(); },
        {update_task_id::CommitStagedChanges}
    );
}

void NcPhysicsImpl::OnBeforeSceneLoad()
{
    m_bodyManager.DeferCleanup(false);
}

void NcPhysicsImpl::OnBeforeSceneFragmentLoad()
{
    BeginRigidBodyBatch();
}

void NcPhysicsImpl::OnAfterSceneFragmentLoad()
{
    EndRigidBodyBatch();
}

void NcPhysicsImpl::Clear() noexcept
{
    m_jolt.contactListener.Clear();
    m_constraintManager.Clear();
    m_vehicleManager.Clear();
    m_bodyManager.Clear();
    m_bodyManager.DeferCleanup(true);
}

void NcPhysicsImpl::BeginRigidBodyBatch(size_t bodyCountHint)
{
    NC_ASSERT(
        m_deferredState->bodyBatchIndex == DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->constraintBatchIndex == DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->vehicleBatchIndex == DeferredPhysicsCreateState::NullBatch,
        "RigidBody batch already in progress"
    );

    if (bodyCountHint != 0ull)
    {
        auto& pool = m_ecs.GetPool<RigidBody>();
        pool.Reserve(pool.size() + bodyCountHint);
    }

    m_deferredState->bodyBatchIndex = m_bodyManager.BeginBatch(bodyCountHint);
    m_deferredState->constraintBatchIndex = m_constraintManager.BeginBatch();
    m_deferredState->vehicleBatchIndex = m_vehicleManager.BeginBatch();
}

void NcPhysicsImpl::EndRigidBodyBatch()
{
    NC_ASSERT(
        m_deferredState->bodyBatchIndex != DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->constraintBatchIndex != DeferredPhysicsCreateState::NullBatch &&
        m_deferredState->vehicleBatchIndex != DeferredPhysicsCreateState::NullBatch,
        "No RigidBody batch is in progress"
    );

    m_bodyManager.EndBatch(std::exchange(m_deferredState->bodyBatchIndex, DeferredPhysicsCreateState::NullBatch));

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

    m_constraintManager.EndBatch(std::exchange(m_deferredState->constraintBatchIndex, DeferredPhysicsCreateState::NullBatch));
    m_vehicleManager.EndBatch(std::exchange(m_deferredState->vehicleBatchIndex, DeferredPhysicsCreateState::NullBatch));
}
} // namespace physics
} // namespace nc
