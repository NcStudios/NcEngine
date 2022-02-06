#include "ecs/component/Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"

namespace nc
{
    void StateAttachment::ComponentGuiElement()
    {
        ImGui::Text("Unknown StateAttachment");
    }

    namespace internal
    {
        void DefaultComponentGuiElement()
        {
            ImGui::Text("User Component");
        }
    }
}

#endif