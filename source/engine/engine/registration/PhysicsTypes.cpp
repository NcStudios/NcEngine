#include "PhysicsTypes.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"

namespace nc
{
void RegisterPhysicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<RigidBody>(
        registry,
        maxEntities,
        RigidBodyId,
        "RigidBody",
        ui::editor::RigidBodyUIWidget,
        CreateRigidBody,
        SerializeRigidBody,
        DeserializeRigidBody
        // has user data, but is set during NcPhysics initialization
    );

    Register<CollisionListener>(
        registry,
        maxEntities,
        CollisionListenerId,
        "CollisionListener",
        ui::editor::CollisionListenerUIWidget
    );
}
} // namespace nc
