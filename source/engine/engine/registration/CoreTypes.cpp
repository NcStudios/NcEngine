#include "CoreTypes.h"
#include "ncengine/ecs/Hierarchy.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"

namespace nc
{
void RegisterCoreTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<ecs::detail::FreeComponentGroup>(registry, maxEntities, FreeComponentGroupId, "");
    Register<Tag>(registry, maxEntities, TagId, "Tag", editor::TagUIWidget);
    Register<Transform>(registry, maxEntities, TransformId, "Transform", editor::TransformUIWidget);
    Register<Hierarchy>(registry, maxEntities, HierarchyId, "Hierarchy");
}
} // namespace nc
