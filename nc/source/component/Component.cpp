#include "Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    void ComponentBase::EditorGuiElement()
    {
        ImGui::PushItemWidth(60.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("User Component");
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
} //end namespace nc