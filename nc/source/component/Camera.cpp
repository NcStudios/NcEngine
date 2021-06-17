#include "component/Camera.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Camera::Camera(Entity entity) noexcept
        : AutoComponent(entity)
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Camera::ComponentGuiElement()
    {
        ImGui::Text("Camera");
    }
    #endif
}