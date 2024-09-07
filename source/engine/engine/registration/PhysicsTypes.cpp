#include "PhysicsTypes.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/EventListeners.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/physics/RigidBody.h"

namespace nc
{
void RegisterPhysicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<physics::Collider>(
        registry,
        maxEntities,
        ColliderId,
        "Collider",
        ui::editor::ColliderUIWidget,
        CreateCollider,
        SerializeCollider,
        DeserializeCollider
    );

    Register<physics::ConcaveCollider>(
        registry,
        maxEntities,
        ConcaveColliderId,
        "ConcaveCollider",
        ui::editor::ConcaveColliderUIWidget,
        nullptr,
        SerializeConcaveCollider,
        DeserializeConcaveCollider
    );

    Register<physics::PhysicsBody>(
        registry,
        maxEntities,
        PhysicsBodyId,
        "PhysicsBody",
        ui::editor::PhysicsBodyUIWidget,
        CreatePhysicsBody,
        SerializePhysicsBody,
        DeserializePhysicsBody,
        &registry
    );

    Register<physics::PhysicsMaterial>(
        registry,
        maxEntities,
        PhysicsMaterialId,
        "PhysicsMaterial",
        ui::editor::PhysicsMaterialUIWidget,
        CreatePhysicsMaterial,
        SerializePhysicsMaterial,
        DeserializePhysicsMaterial
    );

    Register<physics::VelocityRestriction>(
        registry,
        maxEntities,
        VelocityRestrictionId,
        "VelocityRestriction",
        ui::editor::VelocityRestrictionUIWidget,
        CreateVelocityRestriction,
        SerializeVelocityRestriction,
        DeserializeVelocityRestriction
    );

    Register<physics::PositionClamp>(
        registry,
        maxEntities,
        PositionClampId,
        "PositionClamp",
        ui::editor::PositionClampUIWidget,
        CreatePositionClamp,
        SerializePositionClamp,
        DeserializePositionClamp
    );

    Register<physics::OrientationClamp>(
        registry,
        maxEntities,
        OrientationClampId,
        "OrientationClamp",
        ui::editor::OrientationClampUIWidget,
        CreateOrientationClamp,
        SerializeOrientationClamp,
        DeserializeOrientationClamp
    );

    Register<physics::RigidBody>(
        registry,
        maxEntities,
        RigidBodyId,
        "RigidBody"
    );

    Register<physics::CollisionListener>(
        registry,
        maxEntities,
        CollisionListenerId,
        "CollisionListener"
    );
}
} // namespace nc
