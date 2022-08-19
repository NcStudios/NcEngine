#include "physics/ConcaveCollider.h"
#include "debug/NcError.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

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

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<physics::ConcaveCollider>(physics::ConcaveCollider* collider)
{
    const auto& path = collider->GetPath();
    ImGui::Text("ConcaveCollider");
    ImGui::Text("  Path: %s", path.c_str());
}
#endif
} // namespace nc