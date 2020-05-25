#include "Camera.h"

namespace nc
{
    Camera::Camera(ComponentHandle handle, EntityView parentView)
        : Component(handle, parentView)
    {}

    #ifdef NC_DEBUG_BUILD
    void Camera::EditorGuiElement()
    {
        std::string str = std::to_string(GetHandle());

        ImGui::PushItemWidth(60.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("Camera");
                ImGui::Indent();
                    ImGui::Text("ID: ");  ImGui::SameLine();     ImGui::Text(str.c_str());
                ImGui::Unindent();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
}