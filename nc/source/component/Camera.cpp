#include "Camera.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

#include <string>

namespace nc
{
    Camera::Camera(EntityHandle handle) noexcept
        : Component(handle)
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Camera::EditorGuiElement()
    {
        ImGui::PushItemWidth(60.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("Camera");
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
}