#include "Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

#include <string>

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    void Component::EditorGuiElement()
    {
        std::string str = std::to_string(m_handle);

        ImGui::PushItemWidth(60.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("User Component");
                ImGui::Indent();
                    ImGui::Text("ID: ");  ImGui::SameLine();  ImGui::Text(str.c_str());
                ImGui::Unindent();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
} //end namespace nc