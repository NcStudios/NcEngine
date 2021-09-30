#include "component/ConcaveCollider.h"

#include <stdexcept>

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    ConcaveCollider::ConcaveCollider(Entity entity, std::string assetPath)
        : ComponentBase(entity),
          m_path{std::move(assetPath)}
    {
        if(!entity.IsStatic())
            throw std::runtime_error("ConcaveCollider - Cannot be added to a non-static entity");
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ConcaveCollider>(ConcaveCollider* collider)
    {
        const auto& path = collider->GetPath();
        ImGui::Text("ConcaveCollider");
        ImGui::Text("  Path: %s", path.c_str());
    }
    #endif
}