/**
 * @file NcPhysics.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "IClickable.h"
#include "ncengine/module/Module.h"
#include "ncengine/type/EngineId.h"

#include <memory>

namespace nc
{
struct SystemEvents;
class Registry;

namespace config
{
struct MemorySettings;
struct PhysicsSettings;
} // namespace config

namespace physics
{
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

    /**
     * @brief A a joint between two entities.
     * 
     * Both entities must have a PhysicsBody. Joints must be explicitly removed with
     * RemoveJoint() or RemoveAllJoints().
     * 
     * @param entityA The first body.
     * @param entityB The second body.
     * @param anchorA The offset from the first body's center where the joint is attached.
     * @param anchorB The offset from the second body's center where the joint is attached.
     * @param bias A float in the range [0, 1] for position stabilization.
     * @param softness A float in the range [0, 1] for softening the final impulse response.
     */
    virtual void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) = 0;

    /**
     * @brief Remove a joint between two entities.
     * @param entityA The first body.
     * @param entityB The second body.
     */
    virtual void RemoveJoint(Entity entityA, Entity entityB) = 0;

    /**
     * @brief Remove all joints attached to an Entity.
     * @param entity The body whose joints will be cleared.
     */
    virtual void RemoveAllJoints(Entity entity) = 0;

    /** @todo The whole system behind these is broken. */
    virtual void RegisterClickable(IClickable* clickable) = 0;
    virtual void UnregisterClickable(IClickable* clickable) noexcept = 0;
    virtual auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* = 0;
};

/** @brief Build an NcPhysics module instance. */
auto BuildPhysicsModule(const config::MemorySettings& memorySettings,
                        const config::PhysicsSettings& physicsSettings,
                        Registry* registry,
                        const task::AsyncDispatcher& dispatcher,
                        SystemEvents& events) -> std::unique_ptr<NcPhysics>;
} // namespace physics
} // namespace nc
