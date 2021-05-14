#include "graphics/vulkan/PhongMaterial.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::graphics::vulkan
{
    #ifdef NC_EDITOR_ENABLED
    void PhongMaterial::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("Phong Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:");
        ImGui::Text(baseColor.c_str());
        ImGui::Text("Normal:");
        ImGui::Text(normal.c_str());
        ImGui::Text("Roughness:");
        ImGui::Text(roughness.c_str());
    }
    #endif
}