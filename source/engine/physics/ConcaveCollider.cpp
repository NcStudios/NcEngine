#include "physics/ConcaveCollider.h"
#include "ncutility/NcError.h"

#include "ncengine/ecs/Registry.h"

namespace nc::physics
{
ConcaveCollider::ConcaveCollider(Entity entity, std::string assetPath)
    : ComponentBase(entity),
        m_path{std::move(assetPath)}
{
    if(!entity.IsStatic())
        throw NcError("Cannot add ConcaveCollider to a non-static entity");
}

void ConcaveCollider::SetAsset(std::string assetPath)
{
    // workaround for updating asset through editor
    m_path = std::move(assetPath);
    auto registry = ActiveRegistry();
    registry->OnRemove<ConcaveCollider>().Emit(ParentEntity());
    registry->OnAdd<ConcaveCollider>().Emit(*this);
}
} // namespace nc::physics
