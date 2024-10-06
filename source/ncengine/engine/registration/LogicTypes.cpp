#include "LogicTypes.h"
#include "ncengine/ecs/FrameLogic.h"

namespace nc
{
void RegisterLogicTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<FrameLogic>(
        registry,
        maxEntities,
        FrameLogicId,
        "FrameLogic",
        ui::editor::FrameLogicUIWidget,
        CreateFrameLogic
    );
}
} // namespace nc
