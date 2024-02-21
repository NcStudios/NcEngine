#include "PhysicsTypes.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc
{
void RegisterPhysicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<physics::Collider>(registry, maxEntities, ColliderId, "Collider", editor::ColliderUIWidget, CreateCollider, SerializeCollider, DeserializeCollider);
    Register<physics::ConcaveCollider>(registry, maxEntities, ConcaveColliderId, "ConcaveCollider", editor::ConcaveColliderUIWidget, CreateConcaveCollider, SerializeConcaveCollider, DeserializeConcaveCollider);
    auto colliderPool = static_cast<void*>(&registry.GetPool<physics::Collider>());
    Register<physics::PhysicsBody>(registry, maxEntities, PhysicsBodyId, "PhysicsBody", editor::PhysicsBodyUIWidget, CreatePhysicsBody, SerializePhysicsBody, DeserializePhysicsBody, colliderPool);
}
} // namespace nc
