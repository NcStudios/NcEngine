#include "physics/ConcaveCollider.h"
#include "ncutility/NcError.h"

namespace nc::physics
{
ConcaveCollider::ConcaveCollider(Entity entity, std::string assetPath)
    : ComponentBase(entity),
        m_path{std::move(assetPath)}
{
    if(!entity.IsStatic())
        throw NcError("Cannot add ConcaveCollider to a non-static entity");
}
} // namespace nc::physics
