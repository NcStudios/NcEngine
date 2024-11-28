#include "CoreTypes.h"
#include "ncengine/ecs/Hierarchy.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"

namespace nc
{
void RegisterCoreTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<Tag>(
        registry,
        maxEntities,
        TagId,
        "Tag",
        ui::editor::TagUIWidget
    );

    Register<Transform>(
        registry,
        maxEntities,
        TransformId,
        "Transform",
        ui::editor::TransformUIWidget
    );

    Register<Hierarchy>(
        registry,
        maxEntities,
        HierarchyId,
        "Hierarchy"
    );
}
} // namespace nc
