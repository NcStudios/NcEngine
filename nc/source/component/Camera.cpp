#include "Camera.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

#include <string>

namespace nc
{
    Camera::Camera(ComponentHandle handle, EntityHandle parentHandle) noexcept
        : Component(handle, parentHandle)
    {
    }

    #ifdef NC_EDITOR_ENABLED
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