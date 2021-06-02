#include "component/Camera.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

#include <string>

namespace nc
{
    Camera::Camera(Entity entity) noexcept
        : AutoComponent(entity)
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Camera::EditorGuiElement()
    {
        ImGui::Text("Camera");
    }
    #endif
}