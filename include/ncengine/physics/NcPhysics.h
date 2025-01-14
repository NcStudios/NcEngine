/**
 * @file NcPhysics.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/module/Module.h"
#include "ncengine/physics/PhysicsSnapshot.h"
#include "ncengine/type/EngineId.h"

#include <any>
#include <memory>

namespace nc
{
struct SystemEvents;

namespace asset
{
class NcAsset;
} // namespace asset

namespace config
{
struct MemorySettings;
struct PhysicsSettings;
} // namespace config


// api notes
// - prob want ptr or something b/c any must be moveable
// - gets eaten on restore

// struct PhysicsSnapshot
// {
//     std::any state;
//     size_t frame = 0;
// };

/** @brief Physics module interface
 * 
 * Tasks
 *   PhysicsPipeline
 *     Depends On: FrameLogicUpdate
 *     Component Access:
 *       Write: Collider, PhysicsBody, Transform
 *       Read: ConcaveCollider, PhysicsMaterial, PositionClamp, VelocityRestriction
 */
struct NcPhysics : public Module
{
    explicit NcPhysics() noexcept
        : Module{NcPhysicsId} {}

    /** @brief Check if the physics update step is enabled. */
    virtual auto IsUpdateEnabled() const -> bool { return true; }

    /** @brief Toggle physics update step on or off. */
    virtual void EnableUpdate(bool) {}

    virtual void SaveSnapshot(PhysicsSnapshot& snapshot) = 0;
    virtual void RestoreSnapshot(PhysicsSnapshot& snapshot) = 0;

    /**
     * @name RigidBody Batching Operations
     *
     * A RigidBody batch enables more efficient bulk initialization of RigidBody and Constraint instances. Batching is
     * automatically performed during scene fragment deserialization. The Begin/End batch functions can be used to wrap
     * other scopes where many objects are created, e.g. a Scene::Load() function that hard codes object creation. An
     * optional count hint can be provided to also reserve space in the RigidBody pool upfront.
     * 
     * While a batch is in progress, RigidBody and Constraint objects cannot be deleted, and new instances should be
     * considered read-only. Only one batch may be in progress at a time.
     */
    virtual void BeginRigidBodyBatch(size_t bodyCountHint = 0ull) = 0;
    virtual void EndRigidBodyBatch() = 0;
};

/** @brief Build an NcPhysics module instance. */
auto BuildPhysicsModule(const config::MemorySettings& memorySettings,
                        const config::PhysicsSettings& physicsSettings,
                        ecs::Ecs world,
                        asset::NcAsset& ncAsset,
                        const task::AsyncDispatcher& dispatcher,
                        SystemEvents& events) -> std::unique_ptr<NcPhysics>;
} // namespace nc
