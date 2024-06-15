#include "LogicTypes.h"
#include "ncengine/ecs/Logic.h"

namespace nc
{
void RegisterLogicTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<FrameLogic>(registry, maxEntities, FrameLogicId, "FrameLogic", ui::editor::FrameLogicUIWidget, CreateFrameLogic);
    Register<FixedLogic>(registry, maxEntities, FixedLogicId, "FixedLogic", ui::editor::FixedLogicUIWidget, CreateFixedLogic);
    Register<CollisionLogic>(registry, maxEntities, CollisionLogicId, "CollisionLogic", ui::editor::CollisionLogicUIWidget, CreateCollisionLogic);
}
} // namespace nc
