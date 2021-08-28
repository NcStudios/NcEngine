#include "component/MeshCollider.h"

#include <stdexcept>

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    MeshCollider::MeshCollider(Entity entity, std::string assetPath)
        : ComponentBase(entity),
          m_path{std::move(assetPath)}
    {
        if(!EntityUtils::IsStatic(entity))
            throw std::runtime_error("MeshCollider - Cannot be added to a non-static entity");
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshCollider>(MeshCollider* collider)
    {
        const auto& path = collider->GetPath();
        ImGui::Text("MeshCollider");
        ImGui::Text("  Path: %s", path.c_str());
    }
    #endif
}