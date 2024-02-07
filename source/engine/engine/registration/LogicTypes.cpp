#include "LogicTypes.h"
#include "ncengine/ecs/Logic.h"

namespace nc
{
void RegisterLogicTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<FrameLogic>(registry, maxEntities, FrameLogicId, "FrameLogic", editor::FrameLogicUIWidget, CreateFrameLogic);
    Register<FixedLogic>(registry, maxEntities, FixedLogicId, "FixedLogic", editor::FixedLogicUIWidget, CreateFixedLogic);
    Register<CollisionLogic>(registry, maxEntities, CollisionLogicId, "CollisionLogic", editor::CollisionLogicUIWidget, CreateCollisionLogic);
}
} // namespace nc
