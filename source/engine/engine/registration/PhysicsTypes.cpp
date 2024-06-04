#include "PhysicsTypes.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"

namespace nc
{
void RegisterPhysicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<physics::Collider>(registry, maxEntities, ColliderId, "Collider", editor::ColliderUIWidget, CreateCollider, SerializeCollider, DeserializeCollider);
    Register<physics::ConcaveCollider>(registry, maxEntities, ConcaveColliderId, "ConcaveCollider", editor::ConcaveColliderUIWidget, nullptr, SerializeConcaveCollider, DeserializeConcaveCollider);
    Register<physics::PhysicsBody>(registry, maxEntities, PhysicsBodyId, "PhysicsBody", editor::PhysicsBodyUIWidget, CreatePhysicsBody, SerializePhysicsBody, DeserializePhysicsBody, &registry);
    Register<physics::PhysicsMaterial>(registry, maxEntities, PhysicsMaterialId, "PhysicsMaterial", editor::PhysicsMaterialUIWidget, CreatePhysicsMaterial, SerializePhysicsMaterial, DeserializePhysicsMaterial);
    Register<physics::VelocityRestriction>(registry, maxEntities, VelocityRestrictionId, "VelocityRestriction", editor::VelocityRestrictionUIWidget, CreateVelocityRestriction, SerializeVelocityRestriction, DeserializeVelocityRestriction);
    Register<physics::PositionClamp>(registry, maxEntities, PositionClampId, "PositionClamp", editor::PositionClampUIWidget, CreatePositionClamp, SerializePositionClamp, DeserializePositionClamp);
    Register<physics::OrientationClamp>(registry, maxEntities, OrientationClampId, "OrientationClamp", editor::OrientationClampUIWidget, CreateOrientationClamp, SerializeOrientationClamp, DeserializeOrientationClamp);
}
} // namespace nc
